#include "mechanics.h"
#include "ui_common.h"
#include "input_hal.h"
#include "main_menu.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define NAN_VAL (0.0/0.0)
#define SUVAT_UNKNOWN -999999.0
#define IS_KNOWN(x) ((x) != SUVAT_UNKNOWN)

typedef struct { double s, u, v, a, t; } SUVAT;

static lv_obj_t *input_labels[5] = {NULL};
static lv_obj_t *input_fields[5] = {NULL};
static lv_obj_t *result_labels[5] = {NULL};
static lv_obj_t *error_label = NULL;
static lv_group_t *mech_group = NULL;
static lv_obj_t *hint_lbl = NULL;
static int current_page = 0;
static int focused_field = 0;

static const char *suvat_names[] = {"s (m)", "u (m/s)", "v (m/s)", "a (m/s\xC2\xB2)", "t (s)"};

static SUVAT current_suvat = {SUVAT_UNKNOWN, SUVAT_UNKNOWN, SUVAT_UNKNOWN, SUVAT_UNKNOWN, SUVAT_UNKNOWN};

static double get_sv(SUVAT *sv, int i) {
  switch(i) { case 0: return sv->s; case 1: return sv->u; case 2: return sv->v; case 3: return sv->a; case 4: return sv->t; default: return SUVAT_UNKNOWN; }
}

static void set_sv(SUVAT *sv, int i, double v) {
  switch(i) { case 0: sv->s=v; break; case 1: sv->u=v; break; case 2: sv->v=v; break; case 3: sv->a=v; break; case 4: sv->t=v; break; }
}

static int count_known(SUVAT *sv) {
  int c = 0;
  if (IS_KNOWN(sv->s)) c++;
  if (IS_KNOWN(sv->u)) c++;
  if (IS_KNOWN(sv->v)) c++;
  if (IS_KNOWN(sv->a)) c++;
  if (IS_KNOWN(sv->t)) c++;
  return c;
}

static int solve_suvat(SUVAT *sv) {
  for (int iter = 0; iter < 10 && count_known(sv) < 5; iter++) {
    int before = count_known(sv);
    if (IS_KNOWN(sv->u) && IS_KNOWN(sv->a) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->v))
      sv->v = sv->u + sv->a * sv->t;
    if (IS_KNOWN(sv->v) && IS_KNOWN(sv->a) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->u))
      sv->u = sv->v - sv->a * sv->t;
    if (IS_KNOWN(sv->u) && IS_KNOWN(sv->v) && IS_KNOWN(sv->a) && !IS_KNOWN(sv->t) && fabs(sv->a) > 1e-10)
      sv->t = (sv->v - sv->u) / sv->a;
    if (IS_KNOWN(sv->u) && IS_KNOWN(sv->v) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->a) && fabs(sv->t) > 1e-10)
      sv->a = (sv->v - sv->u) / sv->t;
    if (IS_KNOWN(sv->u) && IS_KNOWN(sv->a) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->s))
      sv->s = sv->u * sv->t + 0.5 * sv->a * sv->t * sv->t;
    if (IS_KNOWN(sv->u) && IS_KNOWN(sv->v) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->s))
      sv->s = (sv->u + sv->v) * sv->t / 2.0;
    if (IS_KNOWN(sv->u) && IS_KNOWN(sv->a) && IS_KNOWN(sv->s) && !IS_KNOWN(sv->v)) {
      double v2 = sv->u*sv->u + 2.0*sv->a*sv->s;
      if (v2 >= 0) sv->v = sqrt(v2);
    }
    if (IS_KNOWN(sv->v) && IS_KNOWN(sv->a) && IS_KNOWN(sv->s) && !IS_KNOWN(sv->u)) {
      double u2 = sv->v*sv->v - 2.0*sv->a*sv->s;
      if (u2 >= 0) sv->u = sqrt(u2);
    }
    if (IS_KNOWN(sv->u) && IS_KNOWN(sv->v) && IS_KNOWN(sv->s) && !IS_KNOWN(sv->a) && fabs(sv->s) > 1e-10)
      sv->a = (sv->v*sv->v - sv->u*sv->u) / (2.0*sv->s);
    if (IS_KNOWN(sv->u) && IS_KNOWN(sv->v) && IS_KNOWN(sv->a) && !IS_KNOWN(sv->s) && fabs(sv->a) > 1e-10)
      sv->s = (sv->v*sv->v - sv->u*sv->u) / (2.0*sv->a);
    if (IS_KNOWN(sv->s) && IS_KNOWN(sv->u) && IS_KNOWN(sv->v) && !IS_KNOWN(sv->t) && fabs(sv->u+sv->v) > 1e-10)
      sv->t = 2.0*sv->s / (sv->u+sv->v);
    if (IS_KNOWN(sv->s) && IS_KNOWN(sv->v) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->u) && fabs(sv->t) > 1e-10)
      sv->u = 2.0*sv->s / sv->t - sv->v;
    if (IS_KNOWN(sv->s) && IS_KNOWN(sv->u) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->v) && fabs(sv->t) > 1e-10)
      sv->v = 2.0*sv->s / sv->t - sv->u;
    if (IS_KNOWN(sv->s) && IS_KNOWN(sv->v) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->a) && fabs(sv->t) > 1e-10)
      sv->a = 2.0*(sv->v * sv->t - sv->s) / (sv->t * sv->t);
    if (IS_KNOWN(sv->s) && IS_KNOWN(sv->a) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->u) && fabs(sv->t) > 1e-10)
      sv->u = (sv->s - 0.5 * sv->a * sv->t * sv->t) / sv->t;
    if (count_known(sv) == before) break;
  }
  return count_known(sv);
}

static void show_result_page(void);
static void show_input_page(void);

static void do_calculate(void) {
  if (error_label) lv_label_set_text(error_label, "");

  for (int i = 0; i < 5; i++) {
    const char *text = lv_textarea_get_text(input_fields[i]);
    if (strlen(text) > 0)
      set_sv(&current_suvat, i, atof(text));
    else
      set_sv(&current_suvat, i, SUVAT_UNKNOWN);
  }

  if (count_known(&current_suvat) < 3) {
    if (error_label) {
      lv_label_set_text(error_label, "Enter at least 3 values");
      lv_obj_set_style_text_color(error_label, COL_ERROR, 0);
    }
    return;
  }

  int fc = solve_suvat(&current_suvat);
  if (fc < 5) {
    if (error_label) {
      lv_label_set_text(error_label, "Cannot solve - check inputs");
      lv_obj_set_style_text_color(error_label, COL_ERROR, 0);
    }
    return;
  }
  show_result_page();
}

static void focus_input(int idx) {
  if (idx < 0) idx = 0;
  if (idx > 4) idx = 4;
  focused_field = idx;
  lv_group_focus_obj(input_fields[idx]);
}

static void mech_textarea_key_cb(lv_event_t *e) {
  uint32_t key = lv_event_get_key(e);
  lv_obj_t *ta = lv_event_get_target(e);

  int idx = -1;
  for (int i = 0; i < 5; i++) {
    if (input_fields[i] == ta) { idx = i; break; }
  }

  switch (key) {
  case LV_KEY_UP:
    if (idx > 0) focus_input(idx - 1);
    lv_event_stop_bubbling(e);
    return;
  case LV_KEY_DOWN:
    if (idx < 4) focus_input(idx + 1);
    lv_event_stop_bubbling(e);
    return;
  case LV_KEY_ENTER:
    do_calculate();
    lv_event_stop_bubbling(e);
    return;
  case LV_KEY_ESC:
    for (int i = 0; i < 5; i++) lv_textarea_set_text(input_fields[i], "");
    current_suvat.s = SUVAT_UNKNOWN; current_suvat.u = SUVAT_UNKNOWN;
    current_suvat.v = SUVAT_UNKNOWN; current_suvat.a = SUVAT_UNKNOWN;
    current_suvat.t = SUVAT_UNKNOWN;
    lv_event_stop_bubbling(e);
    return;
  case 'K':
    settings_app_start();
    lv_event_stop_bubbling(e);
    return;
  case 'M':
    main_menu_create();
    lv_event_stop_bubbling(e);
    return;
  default:
    break;
  }
}

static void mech_result_key_cb(lv_event_t *e) {
  uint32_t key = lv_event_get_key(e);
  if (key == 'K') { settings_app_start(); return; }
  if (key == 'M') { main_menu_create(); return; }
  if (key == LV_KEY_ESC) {
    show_input_page();
    return;
  }
}

static void show_input_page(void) {
  current_page = 0;

  lv_obj_t *scr = lv_scr_act();
  lv_obj_clean(scr);
  ui_setup_screen(scr);

  if (mech_group) { lv_group_del(mech_group); mech_group = NULL; }
  mech_group = lv_group_create();

  for (int i = 0; i < 5; i++) {
    int y = CONTENT_TOP + 4 + i * ROW_SPACING;

    input_labels[i] = ui_label(scr, "", CONTENT_SIDE, y + 4);
    char lbl[32];
    snprintf(lbl, sizeof(lbl), "%s:", suvat_names[i]);
    lv_label_set_text(input_labels[i], lbl);

    input_fields[i] = lv_textarea_create(scr);
    lv_obj_set_size(input_fields[i], 180, FIELD_H);
    lv_obj_set_pos(input_fields[i], LCD_H_RES - CONTENT_SIDE - 180, y);
    lv_textarea_set_one_line(input_fields[i], true);
    lv_textarea_set_max_length(input_fields[i], 15);
    lv_textarea_set_placeholder_text(input_fields[i], "?");
    ui_style_textarea(input_fields[i], COL_ACCENT_MECH, COL_FOCUS_BG_MECH);

    lv_obj_add_event_cb(input_fields[i], mech_textarea_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(mech_group, input_fields[i]);

    double val = get_sv(&current_suvat, i);
    if (IS_KNOWN(val)) {
      char buf[32];
      snprintf(buf, sizeof(buf), "%.6g", val);
      lv_textarea_set_text(input_fields[i], buf);
    }
  }

  error_label = lv_label_create(scr);
  lv_label_set_text(error_label, "");
  lv_obj_set_pos(error_label, CONTENT_SIDE, CONTENT_TOP + 4 + 5 * ROW_SPACING + 2);
  lv_obj_set_size(error_label, LCD_H_RES - 2 * CONTENT_SIDE, 18);
  lv_obj_set_style_text_color(error_label, COL_ERROR, 0);
  lv_obj_set_style_text_font(error_label, FONT_SECONDARY, 0);

  lv_indev_t *indev = get_navigation_indev();
  if (indev) lv_indev_set_group(indev, mech_group);

  focus_input(0);

  hint_lbl = ui_create_hint_bar(scr, "[Up/Dn] Field  [=] Calculate  [AC] Clear  [M] Menu");
}

static void show_result_page(void) {
  current_page = 1;

  lv_obj_t *scr = lv_scr_act();
  lv_obj_clean(scr);
  ui_setup_screen(scr);

  if (mech_group) { lv_group_del(mech_group); mech_group = NULL; }
  mech_group = lv_group_create();

  lv_obj_t *key_receiver = lv_obj_create(scr);
  lv_obj_set_size(key_receiver, 0, 0);
  lv_obj_add_flag(key_receiver, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(key_receiver, mech_result_key_cb, LV_EVENT_KEY, NULL);
  lv_group_add_obj(mech_group, key_receiver);

  for (int i = 0; i < 5; i++) {
    int y = CONTENT_TOP + 4 + i * ROW_SPACING;
    result_labels[i] = lv_label_create(scr);
    char buf[64];
    double val = get_sv(&current_suvat, i);
    snprintf(buf, sizeof(buf), "%s = %.6g", suvat_names[i], val);
    lv_label_set_text(result_labels[i], buf);
    lv_obj_set_pos(result_labels[i], CONTENT_SIDE, y);
    lv_obj_set_style_text_color(result_labels[i], COL_TEXT, 0);
    lv_obj_set_style_text_font(result_labels[i], FONT_PRIMARY, 0);
  }

  lv_obj_t *eq = lv_label_create(scr);
  lv_label_set_text(eq, "v=u+at  s=ut+0.5at\xC2\xB2\nv\xC2\xB2=u\xC2\xB2+2as  s=(u+v)t/2");
  lv_obj_set_pos(eq, CONTENT_SIDE, CONTENT_TOP + 4 + 5 * ROW_SPACING + 4);
  lv_obj_set_style_text_color(eq, COL_TEXT_SEC, 0);
  lv_obj_set_style_text_font(eq, FONT_SECONDARY, 0);

  lv_indev_t *indev = get_navigation_indev();
  if (indev) lv_indev_set_group(indev, mech_group);
  lv_group_focus_obj(key_receiver);

  hint_lbl = ui_create_hint_bar(scr, "[AC] Back  [M] Menu");
}

void mechanics_app_start(void) {
  current_suvat.s = SUVAT_UNKNOWN;
  current_suvat.u = SUVAT_UNKNOWN;
  current_suvat.v = SUVAT_UNKNOWN;
  current_suvat.a = SUVAT_UNKNOWN;
  current_suvat.t = SUVAT_UNKNOWN;

  mech_group = NULL;
  focused_field = 0;

  show_input_page();
}
