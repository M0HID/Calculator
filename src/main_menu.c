#include "main_menu.h"
#include "input_hal.h"
#include "lvgl.h"
#include "math.h"
#include "graph.h"
#include <string.h>

// Grid dimensions
#define MENU_COLS 3
#define MENU_ROWS 2
#define MENU_BUTTON_COUNT (MENU_COLS * MENU_ROWS)

// Static storage for buttons and group
static lv_obj_t *menu_buttons[MENU_ROWS][MENU_COLS];
static lv_group_t *menu_group = NULL;

// Navigation functions
static void moveUp(void);
static void moveDown(void);
static void moveLeft(void);
static void moveRight(void);

// Get current focused button position
static bool get_current_position(int *row, int *col) {
  if (menu_group == NULL) {
    return false;
  }

  lv_obj_t *focused = lv_group_get_focused(menu_group);
  if (focused == NULL) {
    return false;
  }

  // Find which button is focused
  for (int r = 0; r < MENU_ROWS; r++) {
    for (int c = 0; c < MENU_COLS; c++) {
      if (menu_buttons[r][c] == focused) {
        *row = r;
        *col = c;
        return true;
      }
    }
  }
  return false;
}

static void moveUp(void) {
  int row, col;
  if (!get_current_position(&row, &col)) {
    return;
  }

  if (row > 0) {
    lv_group_focus_obj(menu_buttons[row - 1][col]);
  }
}

static void moveDown(void) {
  int row, col;
  if (!get_current_position(&row, &col)) {
    return;
  }

  if (row < MENU_ROWS - 1) {
    lv_group_focus_obj(menu_buttons[row + 1][col]);
  }
}

static void moveLeft(void) {
  int row, col;
  if (!get_current_position(&row, &col)) {
    return;
  }

  if (col > 0) {
    lv_group_focus_obj(menu_buttons[row][col - 1]);
  }
}

static void moveRight(void) {
  int row, col;
  if (!get_current_position(&row, &col)) {
    return;
  }

  if (col < MENU_COLS - 1) {
    lv_group_focus_obj(menu_buttons[row][col + 1]);
  }
}

static void openApp(int row, int col) {
  int index = row * MENU_COLS + col;
  switch (index) {
    case 0: // Math
      math_app_start();
      break;
    case 1: // Graph
      graph_app_start();
      break;
    // TODO: Add other apps
    default:
      break;
  }
}

// Key event handler for grid navigation
static void key_event_handler(lv_event_t *e) {
  uint32_t key = lv_event_get_key(e);

  switch (key) {
  case LV_KEY_UP:
    moveUp();
    break;
  case LV_KEY_DOWN:
    moveDown();
    break;
  case LV_KEY_LEFT:
    moveLeft();
    break;
  case LV_KEY_RIGHT:
    moveRight();
    break;
  case LV_KEY_ENTER: {
    int row, col;
    if (get_current_position(&row, &col)) {
      openApp(row, col);
    }
    break;
  }
  default:
    // Let other keys be handled normally
    break;
  }
}

void main_menu_create(void) {
  lvgl_lock();

  lv_obj_t *scr = lv_screen_active();
  lv_obj_clean(scr);  // Clear any existing content

  int cols = MENU_COLS;
  int rows = MENU_ROWS;
  int icon_w = 75;
  int icon_h = 75;
  int spacing_x = 30;
  int spacing_y = 30;

  int start_x = (LCD_H_RES - (cols * icon_w + (cols - 1) * spacing_x)) / 2;
  int start_y = (LCD_V_RES - (rows * icon_h + (rows - 1) * spacing_y)) / 2;

  const char *names[6] = {
      "Math", "Graph", "Stats", "Numerical\n Methods", "Mechanics", "Matrices"};

  lv_color_t colors[6] = {
      lv_palette_main(LV_PALETTE_BLUE),   lv_palette_main(LV_PALETTE_RED),
      lv_palette_main(LV_PALETTE_GREEN),  lv_palette_main(LV_PALETTE_ORANGE),
      lv_palette_main(LV_PALETTE_PURPLE), lv_palette_main(LV_PALETTE_RED)};

  int selected = 0;

  // Create group for navigation
  menu_group = lv_group_create();

  // Get button input device and set group
  lv_indev_t *button_indev = get_navigation_indev();
  if (button_indev != NULL) {
    lv_indev_set_group(button_indev, menu_group);
  }

  for (int i = 0; i < MENU_BUTTON_COUNT; i++) {
    int col = i % cols;
    int row = i / cols;

    lv_obj_t *btn = lv_button_create(scr);
    lv_obj_set_size(btn, icon_w, icon_h);
    lv_obj_set_pos(btn, start_x + col * (icon_w + spacing_x),
                   start_y + row * (icon_h + spacing_y));
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    // Store button reference in 2D array
    menu_buttons[row][col] = btn;

    // Normal color
    lv_obj_set_style_bg_color(btn, colors[i], LV_PART_MAIN | LV_STATE_DEFAULT);

    // Focused color
    lv_obj_set_style_bg_color(btn, lv_color_darken(colors[i], 40),
                              LV_PART_MAIN | LV_STATE_FOCUSED);

    // Focused transform: slightly bigger (105%) from center
    lv_obj_set_style_transform_zoom(btn, 256 * 105 / 100,
                                    LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_transform_pivot_x(btn, icon_w / 2,
                                       LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_transform_pivot_y(btn, icon_h / 2,
                                       LV_PART_MAIN | LV_STATE_FOCUSED);

    // Add key event handler for grid navigation
    lv_obj_add_event_cb(btn, key_event_handler, LV_EVENT_KEY, NULL);

    // Add button to group
    lv_group_add_obj(menu_group, btn);

    // Add focus state to selected button
    if (i == selected) {
      lv_group_focus_obj(btn);
    }

    // Label
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, names[i]);
    lv_obj_center(label);
  }

  lvgl_unlock();
}