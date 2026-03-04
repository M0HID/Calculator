#include "numerical_methods.h"
#include "ui_common.h"
#include "ui_submenu.h"
#include "input_hal.h"
#include "main_menu.h"
#include "settings.h"
#include "expr_eval.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define COL_ACCENT_NM    lv_color_hex(0x00897B)
#define COL_FOCUS_BG_NM  lv_color_hex(0xE0F2F1)

#define NM_MAX_POINTS 30

static lv_group_t *nm_group     = NULL;
static lv_obj_t   *nm_key_recv  = NULL;

static void show_nm_menu(void);

static void cleanup_nm_ui(void) {
    if (nm_group) {
        ui_submenu_cleanup(nm_group);
        nm_group    = NULL;
        nm_key_recv = NULL;
    }
}

static lv_obj_t *setup_nm_screen(const char *hint) {
    cleanup_nm_ui();
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    ui_setup_screen(scr);

    nm_group    = lv_group_create();
    nm_key_recv = lv_obj_create(scr);
    lv_obj_set_size(nm_key_recv, 0, 0);
    lv_obj_add_flag(nm_key_recv, LV_OBJ_FLAG_HIDDEN);
    lv_group_add_obj(nm_group, nm_key_recv);

    ui_create_hint_bar(scr, hint);

    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, nm_group);

    return scr;
}

static int parse_csv(const char *str, double *out, int max) {
    int count = 0;
    char buf[512];
    strncpy(buf, str, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    char *tok = buf;
    while (*tok && count < max) {
        while (*tok == ' ' || *tok == ',') tok++;
        if (*tok == '\0') break;
        char *end;
        double v = strtod(tok, &end);
        if (end == tok) break;
        out[count++] = v;
        tok = end;
    }
    return count;
}

static lv_obj_t *cf_x_field    = NULL;
static lv_obj_t *cf_y_field    = NULL;
static lv_obj_t *cf_result_lbl = NULL;

static void do_curve_fit(void) {
    double xs[NM_MAX_POINTS], ys[NM_MAX_POINTS];
    int nx = parse_csv(lv_textarea_get_text(cf_x_field), xs, NM_MAX_POINTS);
    int ny = parse_csv(lv_textarea_get_text(cf_y_field), ys, NM_MAX_POINTS);

    char buf[256];
    if (nx < 2 || nx != ny) {
        snprintf(buf, sizeof(buf), "Need equal x,y lists (min 2 pts)");
        lv_label_set_text(cf_result_lbl, buf);
        return;
    }

    double Sx = 0, Sy = 0, Sxx = 0, Sxy = 0;
    for (int i = 0; i < nx; i++) {
        Sx  += xs[i];
        Sy  += ys[i];
        Sxx += xs[i] * xs[i];
        Sxy += xs[i] * ys[i];
    }
    double denom = (double)nx * Sxx - Sx * Sx;
    if (fabs(denom) < 1e-12) {
        snprintf(buf, sizeof(buf), "No unique solution (vertical data)");
        lv_label_set_text(cf_result_lbl, buf);
        return;
    }
    double m = ((double)nx * Sxy - Sx * Sy) / denom;
    double c = (Sy - m * Sx) / (double)nx;

    double y_mean = Sy / nx;
    double ss_res = 0, ss_tot = 0;
    for (int i = 0; i < nx; i++) {
        double pred = m * xs[i] + c;
        ss_res += (ys[i] - pred) * (ys[i] - pred);
        ss_tot += (ys[i] - y_mean) * (ys[i] - y_mean);
    }
    double r2 = (ss_tot < 1e-12) ? 1.0 : 1.0 - ss_res / ss_tot;

    char sign = (c >= 0) ? '+' : '-';
    snprintf(buf, sizeof(buf),
             "y = %.5gx %c %.5g\nR\xC2\xB2 = %.6f  (n=%d)",
             m, sign, fabs(c), r2, nx);
    lv_label_set_text(cf_result_lbl, buf);
}

static void cf_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    switch (key) {
    case LV_KEY_UP:
        if (nm_group) lv_group_focus_prev(nm_group);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_DOWN:
        if (nm_group) lv_group_focus_next(nm_group);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_ENTER:
        do_curve_fit();
        lv_event_stop_bubbling(e);
        return;
    case 'f': {
        lv_obj_t *focused = lv_group_get_focused(nm_group);
        if (focused && lv_obj_check_type(focused, &lv_textarea_class)) {
            lv_textarea_delete_char(focused);
            lv_textarea_add_text(focused, " ");
        }
        lv_event_stop_bubbling(e);
        return;
    }
    case LV_KEY_ESC:
        show_nm_menu();
        lv_event_stop_bubbling(e);
        return;
    case 'K':
        cleanup_nm_ui();
        settings_app_start();
        lv_event_stop_bubbling(e);
        return;
    case 'M':
        cleanup_nm_ui();
        main_menu_create();
        lv_event_stop_bubbling(e);
        return;
    case '$':
        lv_event_stop_bubbling(e);
        return;
    default:
        break;
    }
}

static void show_curve_fitting(void) {
    lv_obj_t *scr = setup_nm_screen("[=] Fit  [FRAC] Space  [AC] Back  [M] Menu");

    lv_obj_t *title = ui_label(scr, "Curve Fitting  y = mx + c", CONTENT_SIDE, CONTENT_TOP);
    lv_obj_set_style_text_color(title, COL_ACCENT_NM, 0);

    int y = CONTENT_TOP + 22;

    ui_label(scr, "x vals:", CONTENT_SIDE, y + 5);
    cf_x_field = lv_textarea_create(scr);
    lv_obj_set_size(cf_x_field, LCD_H_RES - CONTENT_SIDE * 2 - 52, FIELD_H);
    lv_obj_set_pos(cf_x_field, CONTENT_SIDE + 52, y);
    lv_textarea_set_one_line(cf_x_field, true);
    lv_textarea_set_placeholder_text(cf_x_field, "1 2 3 4");
    ui_style_textarea(cf_x_field, COL_ACCENT_NM, COL_FOCUS_BG_NM);
    lv_obj_add_event_cb(cf_x_field, cf_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(nm_group, cf_x_field);
    y += ROW_SPACING;

    ui_label(scr, "y vals:", CONTENT_SIDE, y + 5);
    cf_y_field = lv_textarea_create(scr);
    lv_obj_set_size(cf_y_field, LCD_H_RES - CONTENT_SIDE * 2 - 52, FIELD_H);
    lv_obj_set_pos(cf_y_field, CONTENT_SIDE + 52, y);
    lv_textarea_set_one_line(cf_y_field, true);
    lv_textarea_set_placeholder_text(cf_y_field, "2 4 5 4");
    ui_style_textarea(cf_y_field, COL_ACCENT_NM, COL_FOCUS_BG_NM);
    lv_obj_add_event_cb(cf_y_field, cf_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(nm_group, cf_y_field);
    y += ROW_SPACING;

    cf_result_lbl = lv_label_create(scr);
    lv_obj_set_pos(cf_result_lbl, CONTENT_SIDE, y + 4);
    lv_obj_set_size(cf_result_lbl, LCD_H_RES - 2 * CONTENT_SIDE, 44);
    lv_obj_set_style_text_color(cf_result_lbl, COL_RESULT, 0);
    lv_obj_set_style_text_font(cf_result_lbl, FONT_PRIMARY, 0);
    lv_label_set_text(cf_result_lbl, "");

    lv_group_focus_obj(cf_x_field);
}

static lv_obj_t *ni_expr_field = NULL;
static lv_obj_t *ni_a_field    = NULL;
static lv_obj_t *ni_b_field    = NULL;
static lv_obj_t *ni_n_field    = NULL;
static lv_obj_t *ni_result_lbl = NULL;

static void do_integration(void) {
    const char *expr = lv_textarea_get_text(ni_expr_field);
    double a = atof(lv_textarea_get_text(ni_a_field));
    double b = atof(lv_textarea_get_text(ni_b_field));
    int    n = atoi(lv_textarea_get_text(ni_n_field));

    char buf[256];

    if (strlen(expr) == 0) {
        snprintf(buf, sizeof(buf), "Enter an expression");
        lv_label_set_text(ni_result_lbl, buf);
        return;
    }
    if (n < 2) n = 100;
    if (n % 2 != 0) n++;
    if (n > 10000) n = 10000;

    if (fabs(b - a) < 1e-14) {
        lv_label_set_text(ni_result_lbl, "= 0 (a = b)");
        return;
    }

    double h   = (b - a) / n;
    double sum = eval_expression_x(expr, a) + eval_expression_x(expr, b);

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;
        double fi = eval_expression_x(expr, xi);
        sum += (i % 2 == 0) ? 2.0 * fi : 4.0 * fi;
    }
    double result = (h / 3.0) * sum;

    if (isnan(result) || isinf(result))
        snprintf(buf, sizeof(buf), "Error: function undefined in range");
    else
        snprintf(buf, sizeof(buf), "= %.8g  (n=%d steps)", result, n);

    lv_label_set_text(ni_result_lbl, buf);
}

static void ni_textarea_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    switch (key) {
    case LV_KEY_UP:
        if (nm_group) lv_group_focus_prev(nm_group);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_DOWN:
        if (nm_group) lv_group_focus_next(nm_group);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_ENTER:
        do_integration();
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_ESC:
        show_nm_menu();
        lv_event_stop_bubbling(e);
        return;
    case 'M':
        cleanup_nm_ui();
        main_menu_create();
        lv_event_stop_bubbling(e);
        return;
    case 's': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "sin("); } lv_event_stop_bubbling(e); return; }
    case 'c': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "cos("); } lv_event_stop_bubbling(e); return; }
    case 't': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "tan("); } lv_event_stop_bubbling(e); return; }
    case 'r': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "sqrt("); } lv_event_stop_bubbling(e); return; }
    case 'l': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "ln("); } lv_event_stop_bubbling(e); return; }
    case 'L': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "log("); } lv_event_stop_bubbling(e); return; }
    case 'P': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "pi"); } lv_event_stop_bubbling(e); return; }
    case 'X': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "^"); } lv_event_stop_bubbling(e); return; }
    case 'V': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "x"); } lv_event_stop_bubbling(e); return; }
    case '$': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "e"); } lv_event_stop_bubbling(e); return; }
    default:
        break;
    }
}

static void show_numerical_integration(void) {
    lv_obj_t *scr = setup_nm_screen("[=] Integrate  [AC] Back  [M] Menu");

    lv_obj_t *title = ui_label(scr, "Numerical Integration (Simpson)", CONTENT_SIDE, CONTENT_TOP);
    lv_obj_set_style_text_color(title, COL_ACCENT_NM, 0);

    int y = CONTENT_TOP + 20;

    ui_label(scr, "f(x)=", CONTENT_SIDE, y + 5);
    ni_expr_field = lv_textarea_create(scr);
    lv_obj_set_size(ni_expr_field, LCD_H_RES - CONTENT_SIDE * 2 - 44, FIELD_H);
    lv_obj_set_pos(ni_expr_field, CONTENT_SIDE + 44, y);
    lv_textarea_set_one_line(ni_expr_field, true);
    lv_textarea_set_placeholder_text(ni_expr_field, "e.g. x^2");
    ui_style_textarea(ni_expr_field, COL_ACCENT_NM, COL_FOCUS_BG_NM);
    lv_obj_add_event_cb(ni_expr_field, ni_textarea_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(nm_group, ni_expr_field);
    y += ROW_SPACING;

    int half = (LCD_H_RES - CONTENT_SIDE * 2 - 10) / 2;
    ui_label(scr, "a=", CONTENT_SIDE, y + 5);
    ni_a_field = lv_textarea_create(scr);
    lv_obj_set_size(ni_a_field, half - 20, FIELD_H);
    lv_obj_set_pos(ni_a_field, CONTENT_SIDE + 20, y);
    lv_textarea_set_one_line(ni_a_field, true);
    lv_textarea_set_placeholder_text(ni_a_field, "0");
    ui_style_textarea(ni_a_field, COL_ACCENT_NM, COL_FOCUS_BG_NM);
    lv_obj_add_event_cb(ni_a_field, ni_textarea_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(nm_group, ni_a_field);

    int bx = CONTENT_SIDE + half + 10;
    ui_label(scr, "b=", bx, y + 5);
    ni_b_field = lv_textarea_create(scr);
    lv_obj_set_size(ni_b_field, half - 20, FIELD_H);
    lv_obj_set_pos(ni_b_field, bx + 20, y);
    lv_textarea_set_one_line(ni_b_field, true);
    lv_textarea_set_placeholder_text(ni_b_field, "1");
    ui_style_textarea(ni_b_field, COL_ACCENT_NM, COL_FOCUS_BG_NM);
    lv_obj_add_event_cb(ni_b_field, ni_textarea_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(nm_group, ni_b_field);
    y += ROW_SPACING;

    ui_label(scr, "steps:", CONTENT_SIDE, y + 5);
    ni_n_field = lv_textarea_create(scr);
    lv_obj_set_size(ni_n_field, 80, FIELD_H);
    lv_obj_set_pos(ni_n_field, CONTENT_SIDE + 46, y);
    lv_textarea_set_one_line(ni_n_field, true);
    lv_textarea_set_placeholder_text(ni_n_field, "100");
    ui_style_textarea(ni_n_field, COL_ACCENT_NM, COL_FOCUS_BG_NM);
    lv_obj_add_event_cb(ni_n_field, ni_textarea_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(nm_group, ni_n_field);
    y += ROW_SPACING;

    ni_result_lbl = lv_label_create(scr);
    lv_obj_set_pos(ni_result_lbl, CONTENT_SIDE, y + 2);
    lv_obj_set_size(ni_result_lbl, LCD_H_RES - 2 * CONTENT_SIDE, 30);
    lv_obj_set_style_text_color(ni_result_lbl, COL_RESULT, 0);
    lv_obj_set_style_text_font(ni_result_lbl, FONT_PRIMARY, 0);
    lv_label_set_text(ni_result_lbl, "");

    lv_group_focus_obj(ni_expr_field);
}

#define NR_MAX_ITERATIONS 100
#define NR_TOLERANCE      1e-8

static double numerical_derivative(const char *expr, double x) {
    double h = 1e-5;
    return (eval_expression_x(expr, x + h) - eval_expression_x(expr, x - h)) / (2.0 * h);
}

static int newton_raphson(const char *expr, double x0, double *result, int *iters) {
    double x = x0;
    for (int i = 0; i < NR_MAX_ITERATIONS; i++) {
        double fx  = eval_expression_x(expr, x);
        double fpx = numerical_derivative(expr, x);
        if (fabs(fpx) < 1e-14) break;
        double xn = x - fx / fpx;
        *iters = i + 1;
        if (fabs(xn - x) < NR_TOLERANCE) {
            *result = xn;
            return 1;
        }
        x = xn;
    }
    *result = x;
    return 0;
}

static lv_obj_t *nr_fields[3];

static void solve_nr_fn(lv_obj_t **f, int n, lv_obj_t *res) {
    const char *expr = lv_textarea_get_text(f[0]);
    double x0 = atof(lv_textarea_get_text(f[1]));
    double result;
    int iters = 0;
    char buf[256];
    if (newton_raphson(expr, x0, &result, &iters))
        snprintf(buf, sizeof(buf), "x = %.8g  (%d iters)", result, iters);
    else
        snprintf(buf, sizeof(buf), "No convergence (x~%.6g)", result);
    lv_label_set_text(res, buf);
}

static void show_newton_raphson(void);

static void nr_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    switch (key) {
    case LV_KEY_UP:
        if (nm_group) lv_group_focus_prev(nm_group);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_DOWN:
        if (nm_group) lv_group_focus_next(nm_group);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_ENTER:
        solve_nr_fn(nr_fields, 2, nr_fields[2]);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_ESC:
        show_nm_menu();
        lv_event_stop_bubbling(e);
        return;
    case 'K':
        cleanup_nm_ui();
        settings_app_start();
        lv_event_stop_bubbling(e);
        return;
    case 'M':
        cleanup_nm_ui();
        main_menu_create();
        lv_event_stop_bubbling(e);
        return;
    case 's': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "sin("); } lv_event_stop_bubbling(e); return; }
    case 'c': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "cos("); } lv_event_stop_bubbling(e); return; }
    case 't': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "tan("); } lv_event_stop_bubbling(e); return; }
    case 'r': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "sqrt("); } lv_event_stop_bubbling(e); return; }
    case 'l': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "ln("); } lv_event_stop_bubbling(e); return; }
    case 'L': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "log("); } lv_event_stop_bubbling(e); return; }
    case 'P': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "pi"); } lv_event_stop_bubbling(e); return; }
    case 'X': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "^"); } lv_event_stop_bubbling(e); return; }
    case 'V': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "x"); } lv_event_stop_bubbling(e); return; }
    case 'W': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "^2"); } lv_event_stop_bubbling(e); return; }
    case '$': { lv_obj_t *f = lv_group_get_focused(nm_group); if (f && lv_obj_check_type(f, &lv_textarea_class)) { lv_textarea_delete_char(f); lv_textarea_add_text(f, "e"); } lv_event_stop_bubbling(e); return; }
    default:
        break;
    }
}

static void show_newton_raphson(void) {
    lv_obj_t *scr = setup_nm_screen("[=] Solve  [AC] Back  [M] Menu");

    lv_obj_t *title = ui_label(scr, "Newton-Raphson  f(x)=0", CONTENT_SIDE, CONTENT_TOP);
    lv_obj_set_style_text_color(title, COL_ACCENT_NM, 0);

    int y = CONTENT_TOP + 22;

    ui_label(scr, "f(x)=", CONTENT_SIDE, y + 4);
    nr_fields[0] = lv_textarea_create(scr);
    lv_obj_set_size(nr_fields[0], LCD_H_RES - CONTENT_SIDE * 2 - 44, FIELD_H);
    lv_obj_set_pos(nr_fields[0], CONTENT_SIDE + 44, y);
    lv_textarea_set_one_line(nr_fields[0], true);
    lv_textarea_set_placeholder_text(nr_fields[0], "e.g. x^2-4");
    ui_style_textarea(nr_fields[0], COL_ACCENT_NM, COL_FOCUS_BG_NM);
    lv_obj_add_event_cb(nr_fields[0], nr_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(nm_group, nr_fields[0]);
    y += ROW_SPACING;

    ui_label(scr, "x0:", CONTENT_SIDE, y + 4);
    nr_fields[1] = lv_textarea_create(scr);
    lv_obj_set_size(nr_fields[1], 100, FIELD_H);
    lv_obj_set_pos(nr_fields[1], LCD_H_RES - CONTENT_SIDE - 100, y);
    lv_textarea_set_one_line(nr_fields[1], true);
    lv_textarea_set_placeholder_text(nr_fields[1], "1");
    ui_style_textarea(nr_fields[1], COL_ACCENT_NM, COL_FOCUS_BG_NM);
    lv_obj_add_event_cb(nr_fields[1], nr_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(nm_group, nr_fields[1]);
    y += ROW_SPACING;

    nr_fields[2] = lv_label_create(scr);
    lv_obj_set_pos(nr_fields[2], CONTENT_SIDE, y + 4);
    lv_obj_set_size(nr_fields[2], LCD_H_RES - 2 * CONTENT_SIDE, 30);
    lv_obj_set_style_text_color(nr_fields[2], COL_RESULT, 0);
    lv_obj_set_style_text_font(nr_fields[2], FONT_PRIMARY, 0);
    lv_label_set_text(nr_fields[2], "");

    lv_group_focus_obj(nr_fields[0]);
}

static const SubMenuItem nm_menu_items[] = {
    {"Curve Fitting",          show_curve_fitting},
    {"Numerical Integration",  show_numerical_integration},
    {"Newton-Raphson",         show_newton_raphson},
};

#define NM_MENU_ITEMS 3

static void show_nm_menu(void) {
    cleanup_nm_ui();
    SubMenuStyle style = {COL_ACCENT_NM, COL_FOCUS_BG_NM, "[=] Select  [M] Menu"};
    nm_group = ui_create_submenu(nm_menu_items, NM_MENU_ITEMS, &style, main_menu_create);
}

void numerical_methods_app_start(void) {
    show_nm_menu();
}
