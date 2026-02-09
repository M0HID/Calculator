#include "main_menu.h"
#include "ui_common.h"
#include "input_hal.h"
#include "lvgl.h"
#include "math.h"
#include "graph.h"
#include "stats.h"
#include "solver.h"
#include "mechanics.h"
#include <string.h>

/* Grid dimensions */
#define MENU_COLS 3
#define MENU_ROWS 2
#define MENU_BUTTON_COUNT (MENU_COLS * MENU_ROWS)

static lv_obj_t *menu_buttons[MENU_ROWS][MENU_COLS];
static lv_group_t *menu_group = NULL;

/* ── Navigation helpers ── */

static bool get_current_position(int *row, int *col) {
  if (menu_group == NULL) return false;
  lv_obj_t *focused = lv_group_get_focused(menu_group);
  if (focused == NULL) return false;
  for (int r = 0; r < MENU_ROWS; r++)
    for (int c = 0; c < MENU_COLS; c++)
      if (menu_buttons[r][c] == focused) { *row = r; *col = c; return true; }
  return false;
}

static void move(int dr, int dc) {
  int r, c;
  if (!get_current_position(&r, &c)) return;
  int nr = r + dr, nc = c + dc;
  if (nr >= 0 && nr < MENU_ROWS && nc >= 0 && nc < MENU_COLS)
    lv_group_focus_obj(menu_buttons[nr][nc]);
}

static void openApp(int row, int col) {
  int index = row * MENU_COLS + col;
  switch (index) {
    case 0: math_app_start();      break;
    case 1: graph_app_start();     break;
    case 2: stats_app_start();     break;
    case 3: solver_app_start();    break;
    case 4: mechanics_app_start(); break;
    default: break;
  }
}

/* ── Event handlers ── */

static void key_event_handler(lv_event_t *e) {
  uint32_t key = lv_event_get_key(e);
  switch (key) {
    case LV_KEY_UP:    move(-1,  0); break;
    case LV_KEY_DOWN:  move( 1,  0); break;
    case LV_KEY_LEFT:  move( 0, -1); break;
    case LV_KEY_RIGHT: move( 0,  1); break;
    case LV_KEY_ENTER: {
      int r, c;
      if (get_current_position(&r, &c)) openApp(r, c);
      break;
    }
    default: break;
  }
}

static void click_event_handler(lv_event_t *e) {
  lv_obj_t *btn = lv_event_get_target(e);
  for (int r = 0; r < MENU_ROWS; r++)
    for (int c = 0; c < MENU_COLS; c++)
      if (menu_buttons[r][c] == btn) { openApp(r, c); return; }
}

/* ── Main menu creation ── */

void main_menu_create(void) {
  lvgl_lock();

  lv_obj_t *scr = lv_screen_active();
  lv_obj_clean(scr);
  ui_setup_screen(scr);

  const int icon_w = 80, icon_h = 65;
  const int spacing_x = 20, spacing_y = 18;
  int start_x = (LCD_H_RES - (MENU_COLS * icon_w + (MENU_COLS - 1) * spacing_x)) / 2;
  int start_y = (HINT_BAR_Y - (MENU_ROWS * icon_h + (MENU_ROWS - 1) * spacing_y)) / 2;

  const char *names[MENU_BUTTON_COUNT] = {
      "Math", "Graph", "Stats", "Solver", "Mech.", "---"};

  /* Accent colors used as the button's main colour */
  lv_color_t colors[MENU_BUTTON_COUNT] = {
      COL_ACCENT_MATH, COL_ACCENT_GRAPH, COL_ACCENT_STATS,
      COL_ACCENT_SOLVER, COL_ACCENT_MECH, lv_color_hex(0xBDBDBD)};

  menu_group = lv_group_create();
  lv_indev_t *indev = get_navigation_indev();
  if (indev) lv_indev_set_group(indev, menu_group);

  for (int i = 0; i < MENU_BUTTON_COUNT; i++) {
    int col = i % MENU_COLS;
    int row = i / MENU_COLS;

    lv_obj_t *btn = lv_button_create(scr);
    lv_obj_set_size(btn, icon_w, icon_h);
    lv_obj_set_pos(btn, start_x + col * (icon_w + spacing_x),
                   start_y + row * (icon_h + spacing_y));
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    menu_buttons[row][col] = btn;

    /* Normal: colored bg */
    lv_obj_set_style_bg_color(btn, colors[i], LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn, 6, 0);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Focused: white border ring, slightly darkened bg – NO zoom */
    lv_obj_set_style_bg_color(btn, lv_color_darken(colors[i], 30),
                              LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(btn, lv_color_hex(0xFFFFFF),
                                  LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(btn, 3, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(btn, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_FOCUSED);

    lv_obj_add_event_cb(btn, key_event_handler, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(btn, click_event_handler, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(menu_group, btn);

    if (i == 0) lv_group_focus_obj(btn);

    /* Label inside button */
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, names[i]);
    lv_obj_set_style_text_font(label, FONT_PRIMARY, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(label);
  }

  /* Hint bar */
  ui_create_hint_bar(scr, "[Enter] Open  [Arrows] Navigate");

  lvgl_unlock();
}
