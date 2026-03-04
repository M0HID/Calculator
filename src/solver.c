#include "solver.h"
#include "input_hal.h"
#include "main_menu.h"
#include "settings.h"
#include "ui_common.h"
#include "ui_submenu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  SOLVER_MENU,
  SOLVER_LINEAR,
  SOLVER_QUADRATIC,
} SolverMode;

static lv_group_t *solver_group = NULL;
static lv_obj_t *solver_key_recv = NULL;
static lv_obj_t *solver_hint_lbl = NULL;
static SolverMode current_mode = SOLVER_MENU;

static void show_solver_menu(void);
static void show_linear_solver(void);
static void show_quadratic_solver(void);

static int solve_linear(double a, double b, double *x) {
  if (fabs(a) < 1e-12)
    return 0;
  *x = -b / a;
  return 1;
}

static int solve_quadratic(double a, double b, double c, double *x1,
                           double *x2) {
  if (fabs(a) < 1e-12)
    return solve_linear(b, c, x1) ? 1 : 0;
  double d = b * b - 4 * a * c;
  if (d < 0)
    return 0;
  if (fabs(d) < 1e-12) {
    *x1 = -b / (2 * a);
    *x2 = *x1;
    return 1;
  }
  double sd = sqrt(d);
  *x1 = (-b + sd) / (2 * a);
  *x2 = (-b - sd) / (2 * a);
  return 2;
}

static void cleanup_solver_ui(void) {
  if (solver_group) {
    ui_submenu_cleanup(solver_group);
    solver_group = NULL;
  }
  solver_key_recv = NULL;
  solver_hint_lbl = NULL;
}

static lv_obj_t *setup_solver_screen(const char *hint_text) {
  cleanup_solver_ui();

  lv_obj_t *scr = lv_scr_act();
  lv_obj_clean(scr);
  ui_setup_screen(scr);

  solver_group = lv_group_create();

  solver_key_recv = lv_obj_create(scr);
  lv_obj_set_size(solver_key_recv, 0, 0);
  lv_obj_add_flag(solver_key_recv, LV_OBJ_FLAG_HIDDEN);
  lv_group_add_obj(solver_group, solver_key_recv);

  solver_hint_lbl = ui_create_hint_bar(scr, hint_text);

  lv_indev_t *indev = get_navigation_indev();
  if (indev)
    lv_indev_set_group(indev, solver_group);

  return scr;
}

static const SubMenuItem solver_menu_items[] = {
    {"Linear (ax+b=0)", show_linear_solver},
    {"Quadratic (ax\xC2\xB2+bx+c=0)", show_quadratic_solver},
};

#define SOLVER_MENU_ITEMS 2

static void show_solver_menu(void) {
  current_mode = SOLVER_MENU;
  cleanup_solver_ui();

  SubMenuStyle style = {COL_ACCENT_SOLVER, COL_FOCUS_BG_SOLVER,
                        "[=] Select  [M] Menu"};

  solver_group = ui_create_submenu(solver_menu_items, SOLVER_MENU_ITEMS, &style,
                                   main_menu_create);
}

typedef struct {
  lv_obj_t **fields;
  int field_count;
  lv_obj_t *result_label;
  void (*solve_fn)(lv_obj_t **fields, int count, lv_obj_t *result);
} SolverCtx;

static SolverCtx *active_ctx = NULL;

static void sub_solver_key_cb(lv_event_t *e) {
  uint32_t key = lv_event_get_key(e);
  switch (key) {
  case LV_KEY_ENTER:
    if (active_ctx && active_ctx->solve_fn)
      active_ctx->solve_fn(active_ctx->fields, active_ctx->field_count,
                           active_ctx->result_label);
    break;
  case LV_KEY_ESC:
    show_solver_menu();
    break;
  case 'K':
    cleanup_solver_ui();
    settings_app_start();
    break;
  case 'M':
    cleanup_solver_ui();
    main_menu_create();
    break;
  default:
    break;
  }
}

static void solver_insert_text_at_cursor(const char *text) {
  lv_obj_t *focused = lv_group_get_focused(solver_group);
  if (focused && lv_obj_check_type(focused, &lv_textarea_class)) {
    lv_textarea_add_text(focused, text);
  }
}

static int solver_handle_custom_key(uint32_t key) {
  switch (key) {
  case 's':
    solver_insert_text_at_cursor("sin(");
    return 1;
  case 'c':
    solver_insert_text_at_cursor("cos(");
    return 1;
  case 't':
    solver_insert_text_at_cursor("tan(");
    return 1;
  case 'r':
    solver_insert_text_at_cursor("sqrt(");
    return 1;
  case 'l':
    solver_insert_text_at_cursor("ln(");
    return 1;
  case 'L':
    solver_insert_text_at_cursor("log(");
    return 1;
  case 'P':
    solver_insert_text_at_cursor("pi");
    return 1;
  case '$':
    solver_insert_text_at_cursor("e");
    return 1;
  case 'V':
    solver_insert_text_at_cursor("x");
    return 1;
  case 'W':
    solver_insert_text_at_cursor("^2");
    return 1;
  case 'X':
    solver_insert_text_at_cursor("^");
    return 1;
  default:
    return 0;
  }
}

static void solver_textarea_key_cb(lv_event_t *e) {
  uint32_t key = lv_event_get_key(e);

  switch (key) {
  case LV_KEY_UP:
    if (solver_group) lv_group_focus_prev(solver_group);
    lv_event_stop_bubbling(e);
    return;
  case LV_KEY_DOWN:
    if (solver_group) lv_group_focus_next(solver_group);
    lv_event_stop_bubbling(e);
    return;
  case LV_KEY_ENTER:
    if (active_ctx && active_ctx->solve_fn)
      active_ctx->solve_fn(active_ctx->fields, active_ctx->field_count, active_ctx->result_label);
    lv_event_stop_bubbling(e);
    return;
  case LV_KEY_ESC:
    show_solver_menu();
    lv_event_stop_bubbling(e);
    return;
  case 'K':
    cleanup_solver_ui();
    settings_app_start();
    lv_event_stop_bubbling(e);
    return;
  case 'M':
    cleanup_solver_ui();
    main_menu_create();
    lv_event_stop_bubbling(e);
    return;
  case 's': case 'c': case 't': case 'r': case 'l': case 'L':
  case 'P': case '$': case 'V': case 'W': case 'X':
    lv_obj_t *focused = lv_group_get_focused(solver_group);
    if (focused && lv_obj_check_type(focused, &lv_textarea_class)) {
      lv_textarea_delete_char(focused);
      solver_handle_custom_key(key);
    }
    lv_event_stop_bubbling(e);
    return;
  default:
    break;
  }
}

static lv_obj_t *linear_fields[3];
static SolverCtx linear_ctx;

static void solve_linear_fn(lv_obj_t **f, int n, lv_obj_t *res) {
  double a = atof(lv_textarea_get_text(f[0]));
  double b = atof(lv_textarea_get_text(f[1]));
  double x;
  char buf[128];
  if (solve_linear(a, b, &x))
    snprintf(buf, sizeof(buf), "x = %.6g", x);
  else
    snprintf(buf, sizeof(buf), "No solution (a=0)");
  lv_label_set_text(res, buf);
}

static void show_linear_solver(void) {
  current_mode = SOLVER_LINEAR;
  lv_obj_t *scr = setup_solver_screen("[=] Solve  [AC] Back  [M] Menu");
  lv_obj_add_event_cb(solver_key_recv, sub_solver_key_cb, LV_EVENT_KEY, NULL);

  ui_label(scr, "ax + b = 0", CONTENT_SIDE, CONTENT_TOP);

  int y = CONTENT_TOP + 22;
  const char *labels[] = {"a:", "b:"};
  for (int i = 0; i < 2; i++) {
    ui_label(scr, labels[i], CONTENT_SIDE, y + 4);
    linear_fields[i] = lv_textarea_create(scr);
    lv_obj_set_size(linear_fields[i], 200, FIELD_H);
    lv_obj_set_pos(linear_fields[i], LCD_H_RES - CONTENT_SIDE - 200, y);
    lv_textarea_set_one_line(linear_fields[i], true);
    ui_style_textarea(linear_fields[i], COL_ACCENT_SOLVER, COL_FOCUS_BG_SOLVER);
    lv_obj_add_event_cb(linear_fields[i], solver_textarea_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(solver_group, linear_fields[i]);
    y += ROW_SPACING;
  }

  linear_fields[2] = ui_label(scr, "", CONTENT_SIDE, y + 8);
  lv_obj_set_style_text_color(linear_fields[2], COL_RESULT, 0);

  linear_ctx = (SolverCtx){linear_fields, 2, linear_fields[2], solve_linear_fn};
  active_ctx = &linear_ctx;

  lv_group_focus_obj(linear_fields[0]);
}

static lv_obj_t *quad_fields[4];
static SolverCtx quad_ctx;

static void solve_quad_fn(lv_obj_t **f, int n, lv_obj_t *res) {
  double a = atof(lv_textarea_get_text(f[0]));
  double b = atof(lv_textarea_get_text(f[1]));
  double c = atof(lv_textarea_get_text(f[2]));
  double x1, x2;
  char buf[256];
  int ns = solve_quadratic(a, b, c, &x1, &x2);
  if (ns == 0)
    snprintf(buf, sizeof(buf), "No real solutions");
  else if (ns == 1)
    snprintf(buf, sizeof(buf), "x = %.6g", x1);
  else
    snprintf(buf, sizeof(buf), "x1 = %.6g\nx2 = %.6g", x1, x2);
  lv_label_set_text(res, buf);
}

static void show_quadratic_solver(void) {
  current_mode = SOLVER_QUADRATIC;
  lv_obj_t *scr = setup_solver_screen("[=] Solve  [AC] Back  [M] Menu");
  lv_obj_add_event_cb(solver_key_recv, sub_solver_key_cb, LV_EVENT_KEY, NULL);

  ui_label(scr, "ax\xC2\xB2 + bx + c = 0", CONTENT_SIDE, CONTENT_TOP);

  int y = CONTENT_TOP + 22;
  const char *labels[] = {"a:", "b:", "c:"};
  for (int i = 0; i < 3; i++) {
    ui_label(scr, labels[i], CONTENT_SIDE, y + 4);
    quad_fields[i] = lv_textarea_create(scr);
    lv_obj_set_size(quad_fields[i], 200, FIELD_H);
    lv_obj_set_pos(quad_fields[i], LCD_H_RES - CONTENT_SIDE - 200, y);
    lv_textarea_set_one_line(quad_fields[i], true);
    ui_style_textarea(quad_fields[i], COL_ACCENT_SOLVER, COL_FOCUS_BG_SOLVER);
    lv_obj_add_event_cb(quad_fields[i], solver_textarea_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(solver_group, quad_fields[i]);
    y += ROW_SPACING;
  }

  quad_fields[3] = ui_label(scr, "", CONTENT_SIDE, y + 8);
  lv_obj_set_size(quad_fields[3], LCD_H_RES - 2 * CONTENT_SIDE, 40);
  lv_obj_set_style_text_color(quad_fields[3], COL_RESULT, 0);

  quad_ctx = (SolverCtx){quad_fields, 3, quad_fields[3], solve_quad_fn};
  active_ctx = &quad_ctx;

  lv_group_focus_obj(quad_fields[0]);
}

void solver_app_start(void) { show_solver_menu(); }
