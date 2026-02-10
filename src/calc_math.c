#include "calc_math.h"
#include "ui_common.h"
#include "expr_eval.h"
#include "input_hal.h"
#include "main_menu.h"
#include "lvgl.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_HISTORY 10
#define LINE_HEIGHT 18

typedef struct {
  char equation[128];
  char result[64];
} HistoryEntry;

static const char *function_names[] = {
  "sin(", "cos(", "tan(", "sqrt(", "ln(", "log(",
  "abs(", "floor(", "ceil(", "pi", NULL
};

static HistoryEntry history[MAX_HISTORY];
static int history_count = 0;
static int selected_line = -1;
static char last_answer[64] = "0";

static lv_obj_t *history_container = NULL;
static lv_obj_t *history_labels[MAX_HISTORY][2];
static lv_obj_t *current_result_label = NULL;
static lv_obj_t *input_textarea = NULL;
static lv_group_t *calc_group = NULL;
static lv_obj_t *key_receiver = NULL;
static lv_obj_t *hint_lbl = NULL;

static void update_history_display(void);
static void calculate_and_show_result(void);
static void handle_custom_key(uint32_t key);
static void insert_text_at_cursor(const char *text);
static int check_function_at_cursor(void);
static void delete_function_at_cursor(void);

static void calculate_and_show_result(void) {
  const char *text = lv_textarea_get_text(input_textarea);
  if (strlen(text) == 0) { lv_label_set_text(current_result_label, ""); return; }

  double result = eval_expression(text);
  if (fabs(result) < 1e-10) result = 0.0;

  char result_str[64];
  if (isnan(result))
    snprintf(result_str, sizeof(result_str), "Error");
  else if (isinf(result))
    snprintf(result_str, sizeof(result_str), "Infinity");
  else if (result == (long long)result)
    snprintf(result_str, sizeof(result_str), "%.0f", result);
  else
    snprintf(result_str, sizeof(result_str), "%.6g", result);

  lv_label_set_text(current_result_label, result_str);
}

static void insert_text_at_cursor(const char *text) {
  lv_textarea_add_text(input_textarea, text);
  calculate_and_show_result();
}

static int check_function_at_cursor(void) {
  const char *text = lv_textarea_get_text(input_textarea);
  uint32_t cursor_pos = lv_textarea_get_cursor_pos(input_textarea);
  if (cursor_pos == 0) return 0;
  for (int i = 0; function_names[i] != NULL; i++) {
    int len = strlen(function_names[i]);
    if (cursor_pos >= (uint32_t)len)
      if (strncmp(&text[cursor_pos - len], function_names[i], len) == 0)
        return len;
  }
  return 0;
}

static void delete_function_at_cursor(void) {
  int func_len = check_function_at_cursor();
  if (func_len > 0) {
    for (int i = 0; i < func_len; i++) lv_textarea_delete_char(input_textarea);
  } else {
    lv_textarea_delete_char(input_textarea);
  }
  calculate_and_show_result();
}

static void handle_custom_key(uint32_t key) {
  switch (key) {
    case 's': insert_text_at_cursor("sin(");  break;
    case 'c': insert_text_at_cursor("cos(");  break;
    case 't': insert_text_at_cursor("tan(");  break;
    case 'r': insert_text_at_cursor("sqrt("); break;
    case 'l': insert_text_at_cursor("ln(");   break;
    case 'L': insert_text_at_cursor("log(");  break;
    case 'N': insert_text_at_cursor(last_answer); break;
    case 'P': insert_text_at_cursor("pi");    break;
    case 'E': insert_text_at_cursor("e");     break;
    case 'V': insert_text_at_cursor("x");     break;
    case 'W': insert_text_at_cursor("^2");    break;  
    case 'X': insert_text_at_cursor("^");     break;  
    case LV_KEY_BACKSPACE: delete_function_at_cursor(); break;
    default:
      lv_group_send_data(calc_group, key);
      calculate_and_show_result();
      break;
  }
}

static void add_to_history(const char *equation, const char *result) {
  if (history_count >= MAX_HISTORY) {
    for (int i = 0; i < MAX_HISTORY - 1; i++) {
      strcpy(history[i].equation, history[i + 1].equation);
      strcpy(history[i].result, history[i + 1].result);
    }
    history_count = MAX_HISTORY - 1;
  }
  strncpy(history[history_count].equation, equation, sizeof(history[history_count].equation) - 1);
  strncpy(history[history_count].result, result, sizeof(history[history_count].result) - 1);
  strncpy(last_answer, result, sizeof(last_answer) - 1);
  history_count++;
  update_history_display();
}

static void update_history_display(void) {
  for (int i = 0; i < MAX_HISTORY; i++) {
    if (i < history_count) {
      lv_label_set_text(history_labels[i][0], history[i].equation);
      lv_label_set_text(history_labels[i][1], history[i].result);
      lv_obj_remove_flag(history_labels[i][0], LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(history_labels[i][1], LV_OBJ_FLAG_HIDDEN);
      if (i == selected_line) {
        lv_obj_set_style_bg_color(history_labels[i][0], COL_FOCUS_BG_MATH, 0);
        lv_obj_set_style_bg_opa(history_labels[i][0], LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(history_labels[i][1], COL_FOCUS_BG_MATH, 0);
        lv_obj_set_style_bg_opa(history_labels[i][1], LV_OPA_COVER, 0);
      } else {
        lv_obj_set_style_bg_opa(history_labels[i][0], LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(history_labels[i][1], LV_OPA_TRANSP, 0);
      }
    } else {
      lv_obj_add_flag(history_labels[i][0], LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(history_labels[i][1], LV_OBJ_FLAG_HIDDEN);
    }
  }
}

static void textarea_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_VALUE_CHANGED) calculate_and_show_result();
}

static void math_key_cb(lv_event_t *e) {
  uint32_t key = lv_event_get_key(e);
  switch (key) {
  case LV_KEY_ENTER: {
    const char *text = lv_textarea_get_text(input_textarea);
    if (strlen(text) > 0) {
      const char *result_text = lv_label_get_text(current_result_label);
      add_to_history(text, result_text);
      lv_textarea_set_text(input_textarea, "");
      lv_label_set_text(current_result_label, "");
      selected_line = -1;
    }
    return;
  }
  case LV_KEY_UP:
    if (selected_line < history_count - 1) { selected_line++; update_history_display(); }
    return;
  case LV_KEY_DOWN:
    if (selected_line >= 0) { selected_line--; update_history_display(); }
    return;
  case LV_KEY_ESC:
    lv_textarea_set_text(input_textarea, "");
    lv_label_set_text(current_result_label, "");
    selected_line = -1;
    update_history_display();
    return;
  case 'M':
    main_menu_create();
    return;
  case 's': case 'c': case 't': case 'r': case 'l': case 'L':
  case 'N': case 'P': case 'E': case 'V': case 'W': case 'X':
    handle_custom_key(key);
    return;
  case 'x':  
    insert_text_at_cursor("x");
    return;
  case 'A': insert_text_at_cursor("A"); return;  
  case 'B': insert_text_at_cursor("B"); return;
  case 'C': insert_text_at_cursor("C"); return;
  case 'D': insert_text_at_cursor("D"); return;
  case 'F': insert_text_at_cursor("F"); return;
  case 'G': case 'K': case 'Q': case 'f':  
  case 'y': case 'z': case 'S': case 'O':  
    return;
  case LV_KEY_BACKSPACE:
    delete_function_at_cursor();
    return;
  case LV_KEY_LEFT:
    lv_textarea_cursor_left(input_textarea);
    return;
  case LV_KEY_RIGHT:
    lv_textarea_cursor_right(input_textarea);
    return;
  default:
    
    if ((key >= '0' && key <= '9') || key == '+' || key == '-' ||
        key == '*' || key == '/' || key == '.' || key == '(' ||
        key == ')' || key == '^' || key == '!') {
      char str[2] = {(char)key, '\0'};
      lv_textarea_add_text(input_textarea, str);
      calculate_and_show_result();
    }
    break;
  }
}

void math_app_start(void) {
  lv_obj_clean(lv_scr_act());
  lv_obj_t *scr = lv_scr_act();
  ui_setup_screen(scr);

  history_count = 0;
  selected_line = -1;

  
  history_container = lv_obj_create(scr);
  lv_obj_set_size(history_container, LCD_H_RES - 8, 140);
  lv_obj_set_pos(history_container, 4, CONTENT_TOP);
  lv_obj_set_style_bg_color(history_container, COL_BG, 0);
  lv_obj_set_style_border_width(history_container, 1, 0);
  lv_obj_set_style_border_color(history_container, COL_BORDER, 0);
  lv_obj_set_style_radius(history_container, 3, 0);
  lv_obj_set_style_pad_all(history_container, 4, 0);
  lv_obj_set_flex_flow(history_container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(history_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

  for (int i = 0; i < MAX_HISTORY; i++) {
    lv_obj_t *line_container = lv_obj_create(history_container);
    lv_obj_set_size(line_container, LCD_H_RES - 24, LINE_HEIGHT);
    lv_obj_set_style_bg_opa(line_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(line_container, 0, 0);
    lv_obj_set_style_pad_all(line_container, 0, 0);

    history_labels[i][0] = lv_label_create(line_container);
    lv_obj_set_pos(history_labels[i][0], 4, 0);
    lv_obj_set_size(history_labels[i][0], (LCD_H_RES - 24) / 2, LINE_HEIGHT);
    lv_obj_set_style_text_color(history_labels[i][0], COL_TEXT, 0);
    lv_obj_set_style_text_font(history_labels[i][0], FONT_SECONDARY, 0);
    lv_obj_set_style_pad_all(history_labels[i][0], 2, 0);
    lv_label_set_text(history_labels[i][0], "");
    lv_obj_add_flag(history_labels[i][0], LV_OBJ_FLAG_HIDDEN);

    history_labels[i][1] = lv_label_create(line_container);
    lv_obj_set_pos(history_labels[i][1], (LCD_H_RES - 24) / 2 + 4, 0);
    lv_obj_set_size(history_labels[i][1], (LCD_H_RES - 24) / 2 - 8, LINE_HEIGHT);
    lv_obj_set_style_text_align(history_labels[i][1], LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_color(history_labels[i][1], COL_RESULT, 0);
    lv_obj_set_style_text_font(history_labels[i][1], FONT_SECONDARY, 0);
    lv_obj_set_style_pad_all(history_labels[i][1], 2, 0);
    lv_label_set_text(history_labels[i][1], "");
    lv_obj_add_flag(history_labels[i][1], LV_OBJ_FLAG_HIDDEN);
  }

  
  current_result_label = lv_label_create(scr);
  lv_label_set_text(current_result_label, "");
  lv_obj_set_pos(current_result_label, 8, 148);
  lv_obj_set_size(current_result_label, LCD_H_RES - 16, 22);
  lv_obj_set_style_text_align(current_result_label, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_set_style_text_color(current_result_label, COL_RESULT, 0);
  lv_obj_set_style_text_font(current_result_label, FONT_PRIMARY, 0);

  
  input_textarea = lv_textarea_create(scr);
  lv_obj_set_size(input_textarea, LCD_H_RES - 12, 34);
  lv_obj_set_pos(input_textarea, 6, 172);
  lv_textarea_set_placeholder_text(input_textarea, "Enter expression...");
  lv_textarea_set_one_line(input_textarea, true);
  ui_style_textarea(input_textarea, COL_ACCENT_MATH, COL_FOCUS_BG_MATH);
  lv_obj_add_event_cb(input_textarea, textarea_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

  
  key_receiver = lv_obj_create(scr);
  lv_obj_set_size(key_receiver, 0, 0);
  lv_obj_add_flag(key_receiver, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(key_receiver, math_key_cb, LV_EVENT_KEY, NULL);

  calc_group = lv_group_create();
  lv_group_add_obj(calc_group, key_receiver);

  lv_indev_t *indev = get_navigation_indev();
  if (indev) lv_indev_set_group(indev, calc_group);
  lv_group_focus_obj(key_receiver);

  
  hint_lbl = ui_create_hint_bar(scr, "[=] Enter  [AC] Clear  [M] Menu");
}
