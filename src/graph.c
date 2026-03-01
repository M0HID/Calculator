#include "graph.h"
#include "ui_common.h"
#include "input_hal.h"
#include "expr_eval.h"
#include "main_menu.h"
#include "settings.h"
#include "lvgl.h"
#include <stdio.h>
#include <string.h>

int isnan(double x);
int isinf(double x);

#define SCREEN_W LCD_H_RES
#define SCREEN_H LCD_V_RES

#define CANVAS_Y 30
#define CANVAS_W SCREEN_W
#define CANVAS_H (HINT_BAR_Y - CANVAS_Y)

#define MAX_FUNCTIONS 4

static const uint32_t func_colors[MAX_FUNCTIONS] = {
    0x00CC00, 0xDD0000, 0x0088EE, 0xEE8800
};

typedef enum { SCREEN_FUNCTION_LIST, SCREEN_GRAPH_VIEW } GraphScreen;

typedef struct { char equation[64]; int enabled; } Function;

static Function functions[MAX_FUNCTIONS] = {
    { "x^2", 1 }, { "sin(x)*3", 1 }, { "", 0 }, { "", 0 }
};

static double x_min = -10.0, x_max = 10.0;
static double y_min = -10.0, y_max = 10.0;

static GraphScreen current_screen = SCREEN_FUNCTION_LIST;
static int selected_function = 0;
static int editing_function = -1;

static int trace_enabled = 0;
static double trace_x = 0.0;
static int trace_func_idx = 0;

static lv_obj_t *func_list_container = NULL;
static lv_obj_t *func_rows[MAX_FUNCTIONS];
static lv_obj_t *func_checkboxes[MAX_FUNCTIONS];
static lv_obj_t *func_prefixes[MAX_FUNCTIONS];
static lv_obj_t *func_labels[MAX_FUNCTIONS];
static lv_obj_t *func_textareas[MAX_FUNCTIONS];
static lv_obj_t *hint_label = NULL;

static lv_obj_t *graph_container = NULL;
static lv_obj_t *canvas = NULL;
static lv_obj_t *info_label = NULL;
static lv_obj_t *coords_label = NULL;

LV_DRAW_BUF_DEFINE_STATIC(canvas_buf, CANVAS_W, CANVAS_H, LV_COLOR_FORMAT_NATIVE);

static lv_group_t *graph_group = NULL;
static lv_obj_t *key_receiver = NULL;

static void show_function_list(void);
static void show_graph_view(void);
static void draw_graph(void);
static void update_function_list_ui(void);
static void update_graph_info(void);
static void textarea_event_cb(lv_event_t *e);
static void funclist_key_cb(lv_event_t *e);
static void graph_key_cb(lv_event_t *e);

static int g2cx(double gx) { return (int)((gx - x_min) / (x_max - x_min) * CANVAS_W); }
static int g2cy(double gy) { return (int)(CANVAS_H - (gy - y_min) / (y_max - y_min) * CANVAS_H); }

static void draw_axes(void) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_draw_line_dsc_t ld;
    lv_draw_line_dsc_init(&ld);
    ld.color = COL_TEXT; ld.width = 1; ld.opa = LV_OPA_COVER;

    if (x_min <= 0 && x_max >= 0) { int ax = g2cx(0); ld.p1.x=ax; ld.p1.y=0; ld.p2.x=ax; ld.p2.y=CANVAS_H; lv_draw_line(&layer,&ld); }
    if (y_min <= 0 && y_max >= 0) { int ay = g2cy(0); ld.p1.x=0; ld.p1.y=ay; ld.p2.x=CANVAS_W; ld.p2.y=ay; lv_draw_line(&layer,&ld); }

    ld.color = lv_color_hex(0xDDDDDD);
    double range = x_max - x_min;
    double tick = 1.0;
    if (range > 100) tick = 20.0;
    else if (range > 50) tick = 10.0;
    else if (range > 20) tick = 5.0;
    else if (range > 10) tick = 2.0;
    else if (range < 2) tick = 0.25;
    else if (range < 5) tick = 0.5;

    for (double gx = ((int)(x_min/tick))*tick; gx <= x_max; gx += tick) {
        if (gx > -0.001 && gx < 0.001) continue;
        int cx = g2cx(gx);
        if (cx >= 0 && cx < CANVAS_W) { ld.p1.x=cx; ld.p1.y=0; ld.p2.x=cx; ld.p2.y=CANVAS_H; lv_draw_line(&layer,&ld); }
    }
    for (double gy = ((int)(y_min/tick))*tick; gy <= y_max; gy += tick) {
        if (gy > -0.001 && gy < 0.001) continue;
        int cy = g2cy(gy);
        if (cy >= 0 && cy < CANVAS_H) { ld.p1.x=0; ld.p1.y=cy; ld.p2.x=CANVAS_W; ld.p2.y=cy; lv_draw_line(&layer,&ld); }
    }
    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_function(int idx) {
    if (idx < 0 || idx >= MAX_FUNCTIONS) return;
    if (!functions[idx].enabled || strlen(functions[idx].equation) == 0) return;

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_draw_line_dsc_t ld;
    lv_draw_line_dsc_init(&ld);
    ld.color = lv_color_hex(func_colors[idx]); ld.width = 2; ld.opa = LV_OPA_COVER;

    double step = (x_max - x_min) / CANVAS_W;
    int pcx = -1, pcy = -1, pv = 0;
    for (double gx = x_min; gx <= x_max; gx += step) {
        double gy = eval_expression_x(functions[idx].equation, gx);
        int cx = g2cx(gx), cy = g2cy(gy);
        int v = !isnan(gy) && !isinf(gy) && cy >= -CANVAS_H && cy <= 2*CANVAS_H;
        if (v && pv) {
            ld.p1.x=pcx; ld.p1.y=pcy; ld.p2.x=cx; ld.p2.y=cy;
            int dy = cy-pcy; if (dy<0) dy=-dy;
            if (dy < CANVAS_H*2) lv_draw_line(&layer, &ld);
        }
        pcx=cx; pcy=cy; pv=v;
    }
    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_trace_cursor(void) {
    if (!trace_enabled || trace_func_idx < 0 || trace_func_idx >= MAX_FUNCTIONS) return;
    if (!functions[trace_func_idx].enabled) return;

    double gy = eval_expression_x(functions[trace_func_idx].equation, trace_x);
    if (isnan(gy) || isinf(gy)) return;
    int cx = g2cx(trace_x), cy = g2cy(gy);
    if (cx < 0 || cx >= CANVAS_W || cy < 0 || cy >= CANVAS_H) return;

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_line_dsc_t ld;
    lv_draw_line_dsc_init(&ld);
    ld.color = lv_color_hex(0x999999); ld.width = 1; ld.opa = LV_OPA_COVER;
    ld.p1.x=cx; ld.p1.y=0; ld.p2.x=cx; ld.p2.y=CANVAS_H; lv_draw_line(&layer,&ld);
    ld.p1.x=0; ld.p1.y=cy; ld.p2.x=CANVAS_W; ld.p2.y=cy; lv_draw_line(&layer,&ld);

    lv_draw_rect_dsc_t rd;
    lv_draw_rect_dsc_init(&rd);
    rd.bg_color = lv_color_hex(func_colors[trace_func_idx]); rd.bg_opa = LV_OPA_COVER;
    lv_area_t dot = { cx-4, cy-4, cx+4, cy+4 };
    lv_draw_rect(&layer, &rd, &dot);

    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_graph(void) {
    lv_canvas_fill_bg(canvas, COL_BG, LV_OPA_COVER);
    draw_axes();
    for (int i = 0; i < MAX_FUNCTIONS; i++) draw_function(i);
    draw_trace_cursor();
}

static void update_function_list_ui(void) {
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        if (functions[i].enabled) lv_obj_add_state(func_checkboxes[i], LV_STATE_CHECKED);
        else lv_obj_remove_state(func_checkboxes[i], LV_STATE_CHECKED);

        if (i == selected_function && editing_function < 0) {
            lv_obj_set_style_bg_color(func_rows[i], COL_FOCUS_BG_GRAPH, 0);
            lv_obj_set_style_bg_opa(func_rows[i], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(func_rows[i], LV_OPA_TRANSP, 0);
        }

        if (editing_function == i) {
            lv_obj_add_flag(func_labels[i], LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(func_textareas[i], LV_OBJ_FLAG_HIDDEN);
            lv_textarea_set_text(func_textareas[i], functions[i].equation);
            lv_textarea_set_cursor_pos(func_textareas[i], LV_TEXTAREA_CURSOR_LAST);
        } else {
            lv_obj_remove_flag(func_labels[i], LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(func_textareas[i], LV_OBJ_FLAG_HIDDEN);
            const char *eq = functions[i].equation;
            lv_label_set_text(func_labels[i], strlen(eq) > 0 ? eq : "(empty)");
        }
    }

    if (editing_function >= 0)
        lv_label_set_text(hint_label, "[=] Save  [AC] Cancel");
    else
        lv_label_set_text(hint_label, "[=] Edit  [AC] Toggle  [CALC] Graph  [M] Menu");
}

static void update_graph_info(void) {
    char info[64];
    snprintf(info, sizeof(info), "x:[%.3g,%.3g] y:[%.3g,%.3g]", x_min, x_max, y_min, y_max);
    lv_label_set_text(info_label, info);

    if (trace_enabled && trace_func_idx >= 0 && trace_func_idx < MAX_FUNCTIONS) {
        double gy = eval_expression_x(functions[trace_func_idx].equation, trace_x);
        char coords[48];
        snprintf(coords, sizeof(coords), "y%d: (%.4g, %.4g)", trace_func_idx+1, trace_x, gy);
        lv_label_set_text(coords_label, coords);
        lv_obj_set_style_text_color(coords_label, lv_color_hex(func_colors[trace_func_idx]), 0);
    } else {
        lv_label_set_text(coords_label, "[V] Trace [+/-] Zoom [=] Edit [AC] Back [M] Menu");
        lv_obj_set_style_text_color(coords_label, COL_TEXT_SEC, 0);
    }
}

static void show_function_list(void) {
    current_screen = SCREEN_FUNCTION_LIST;
    trace_enabled = 0;

    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    ui_setup_screen(scr);

    func_list_container = lv_obj_create(scr);
    lv_obj_set_size(func_list_container, SCREEN_W - 8, HINT_BAR_Y - CONTENT_TOP - 2);
    lv_obj_set_pos(func_list_container, 4, CONTENT_TOP);
    lv_obj_set_style_bg_color(func_list_container, COL_BG, 0);
    lv_obj_set_style_border_width(func_list_container, 1, 0);
    lv_obj_set_style_border_color(func_list_container, COL_BORDER, 0);
    lv_obj_set_style_radius(func_list_container, 3, 0);
    lv_obj_set_style_pad_all(func_list_container, 6, 0);
    lv_obj_clear_flag(func_list_container, LV_OBJ_FLAG_SCROLLABLE);

    int row_height = ROW_H;
    int row_spacing = ROW_H + 4;
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        func_rows[i] = lv_obj_create(func_list_container);
        lv_obj_set_size(func_rows[i], SCREEN_W - 28, row_height);
        lv_obj_set_pos(func_rows[i], 0, i * row_spacing);
        lv_obj_set_style_bg_opa(func_rows[i], LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(func_rows[i], 0, 0);
        lv_obj_set_style_pad_all(func_rows[i], 2, 0);
        lv_obj_set_style_radius(func_rows[i], 3, 0);
        lv_obj_clear_flag(func_rows[i], LV_OBJ_FLAG_SCROLLABLE);

        func_checkboxes[i] = lv_checkbox_create(func_rows[i]);
        lv_checkbox_set_text(func_checkboxes[i], "");
        lv_obj_set_pos(func_checkboxes[i], 0, 2);
        if (functions[i].enabled) lv_obj_add_state(func_checkboxes[i], LV_STATE_CHECKED);

        func_prefixes[i] = lv_label_create(func_rows[i]);
        char prefix[16];
        snprintf(prefix, sizeof(prefix), "y%d =", i + 1);
        lv_label_set_text(func_prefixes[i], prefix);
        lv_obj_set_pos(func_prefixes[i], 30, 4);
        lv_obj_set_style_text_color(func_prefixes[i], lv_color_hex(func_colors[i]), 0);
        lv_obj_set_style_text_font(func_prefixes[i], FONT_PRIMARY, 0);

        func_labels[i] = lv_label_create(func_rows[i]);
        lv_obj_set_pos(func_labels[i], 75, 4);
        lv_obj_set_style_text_color(func_labels[i], COL_TEXT, 0);
        lv_obj_set_style_text_font(func_labels[i], FONT_PRIMARY, 0);

        func_textareas[i] = lv_textarea_create(func_rows[i]);
        lv_obj_set_size(func_textareas[i], SCREEN_W - 120, 24);
        lv_obj_set_pos(func_textareas[i], 75, 1);
        lv_textarea_set_one_line(func_textareas[i], true);
        lv_textarea_set_placeholder_text(func_textareas[i], "");
        lv_obj_set_style_border_width(func_textareas[i], 0, 0);
        lv_obj_set_style_bg_opa(func_textareas[i], LV_OPA_TRANSP, 0);
        lv_obj_set_style_pad_all(func_textareas[i], 0, 0);
        lv_obj_set_style_text_font(func_textareas[i], FONT_PRIMARY, 0);
        lv_obj_add_flag(func_textareas[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_event_cb(func_textareas[i], textarea_event_cb, LV_EVENT_READY, (void*)(intptr_t)i);
        lv_obj_add_event_cb(func_textareas[i], textarea_event_cb, LV_EVENT_CANCEL, (void*)(intptr_t)i);
    }

    hint_label = ui_create_hint_bar(scr, "");

    graph_group = lv_group_create();
    key_receiver = lv_obj_create(scr);
    lv_obj_set_size(key_receiver, 0, 0);
    lv_obj_add_flag(key_receiver, LV_OBJ_FLAG_HIDDEN);
    lv_group_add_obj(graph_group, key_receiver);
    lv_obj_add_event_cb(key_receiver, funclist_key_cb, LV_EVENT_KEY, NULL);
    for (int i = 0; i < MAX_FUNCTIONS; i++) lv_group_add_obj(graph_group, func_textareas[i]);

    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, graph_group);
    lv_group_focus_obj(key_receiver);

    update_function_list_ui();
}

static void show_graph_view(void) {
    current_screen = SCREEN_GRAPH_VIEW;

    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    ui_setup_screen(scr);

    info_label = lv_label_create(scr);
    lv_obj_set_pos(info_label, 4, 2);
    lv_obj_set_style_text_color(info_label, COL_TEXT_SEC, 0);
    lv_obj_set_style_text_font(info_label, FONT_SECONDARY, 0);

    coords_label = lv_label_create(scr);
    lv_obj_set_pos(coords_label, 4, 16);
    lv_obj_set_style_text_color(coords_label, COL_TEXT, 0);
    lv_obj_set_style_text_font(coords_label, FONT_SECONDARY, 0);

    canvas = lv_canvas_create(scr);
    LV_DRAW_BUF_INIT_STATIC(canvas_buf);
    lv_canvas_set_draw_buf(canvas, &canvas_buf);
    lv_obj_set_pos(canvas, 0, CANVAS_Y);

    graph_group = lv_group_create();
    lv_obj_t *recv = lv_obj_create(scr);
    lv_obj_set_size(recv, 0, 0);
    lv_obj_add_flag(recv, LV_OBJ_FLAG_HIDDEN);
    lv_group_add_obj(graph_group, recv);
    lv_obj_add_event_cb(recv, graph_key_cb, LV_EVENT_KEY, NULL);

    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, graph_group);
    lv_group_focus_obj(recv);

    draw_graph();
    update_graph_info();
}

void graph_app_start(void) {
    x_min = -10.0; x_max = 10.0; y_min = -10.0; y_max = 10.0;
    trace_enabled = 0; trace_x = 0;
    selected_function = 0; editing_function = -1;
    show_function_list();
}

static void textarea_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    if (idx < 0 || idx >= MAX_FUNCTIONS) return;

    if (code == LV_EVENT_READY) {
        const char *text = lv_textarea_get_text(func_textareas[idx]);
        strncpy(functions[idx].equation, text, sizeof(functions[idx].equation) - 1);
        functions[idx].equation[sizeof(functions[idx].equation) - 1] = '\0';
    }
    editing_function = -1;
    lv_group_focus_obj(key_receiver);
    update_function_list_ui();
}

static void funclist_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);

    switch (key) {
    case LV_KEY_UP:
        if (selected_function > 0) { selected_function--; update_function_list_ui(); }
        break;
    case LV_KEY_DOWN:
        if (selected_function < MAX_FUNCTIONS - 1) { selected_function++; update_function_list_ui(); }
        break;
    case LV_KEY_ENTER:
        editing_function = selected_function;
        update_function_list_ui();
        lv_group_focus_obj(func_textareas[editing_function]);
        break;
    case LV_KEY_ESC:
        functions[selected_function].enabled = !functions[selected_function].enabled;
        update_function_list_ui();
        break;
    case 'G':
        show_graph_view();
        break;
    case 'K':
        settings_app_start();
        break;
    case 'M':
        main_menu_create();
        break;
    default:
        break;
    }
}

static void graph_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    double x_range = x_max - x_min;
    double y_range = y_max - y_min;

    switch (key) {
    case LV_KEY_LEFT:
        if (trace_enabled) {
            trace_x -= x_range / CANVAS_W * 4;
            draw_graph(); update_graph_info();
        } else {
            double shift = x_range * 0.2;
            x_min -= shift; x_max -= shift;
            draw_graph(); update_graph_info();
        }
        break;
    case LV_KEY_RIGHT:
        if (trace_enabled) {
            trace_x += x_range / CANVAS_W * 4;
            draw_graph(); update_graph_info();
        } else {
            double shift = x_range * 0.2;
            x_min += shift; x_max += shift;
            draw_graph(); update_graph_info();
        }
        break;
    case LV_KEY_UP:
        if (trace_enabled) {
            for (int i = 1; i <= MAX_FUNCTIONS; i++) {
                int next = (trace_func_idx + i) % MAX_FUNCTIONS;
                if (functions[next].enabled) { trace_func_idx = next; break; }
            }
            draw_graph(); update_graph_info();
        } else {
            double shift = y_range * 0.2;
            y_min += shift; y_max += shift;
            draw_graph(); update_graph_info();
        }
        break;
    case LV_KEY_DOWN:
        if (trace_enabled) {
            for (int i = MAX_FUNCTIONS - 1; i >= 0; i--) {
                int prev = (trace_func_idx - 1 + MAX_FUNCTIONS) % MAX_FUNCTIONS;
                if (functions[prev].enabled) { trace_func_idx = prev; break; }
                trace_func_idx = prev;
            }
            draw_graph(); update_graph_info();
        } else {
            double shift = y_range * 0.2;
            y_min -= shift; y_max -= shift;
            draw_graph(); update_graph_info();
        }
        break;
    case '+':
        x_min += x_range * 0.2; x_max -= x_range * 0.2;
        y_min += y_range * 0.2; y_max -= y_range * 0.2;
        draw_graph(); update_graph_info();
        break;
    case '-':
        x_min -= x_range * 0.25; x_max += x_range * 0.25;
        y_min -= y_range * 0.25; y_max += y_range * 0.25;
        draw_graph(); update_graph_info();
        break;
    case 'V':
        trace_enabled = !trace_enabled;
        if (trace_enabled) {
            trace_x = (x_min + x_max) / 2.0;
            trace_func_idx = 0;
            for (int i = 0; i < MAX_FUNCTIONS; i++) {
                if (functions[i].enabled) { trace_func_idx = i; break; }
            }
        }
        draw_graph(); update_graph_info();
        break;
    case LV_KEY_ENTER:
        show_function_list();
        break;
    case LV_KEY_ESC:
        show_function_list();
        break;
    case 'K':
        settings_app_start();
        break;
    case 'M':
        main_menu_create();
        break;
    default:
        break;
    }
}
