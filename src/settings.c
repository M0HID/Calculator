#include "settings.h"
#include "ui_common.h"
#include "ui_submenu.h"
#include "input_hal.h"
#include "main_menu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static AngleMode  angle_mode     = ANGLE_RAD;
static int        decimal_places = 0;           
static double     user_vars[SETTINGS_VAR_COUNT] = {0};



AngleMode settings_get_angle_mode(void)        { return angle_mode; }
void      settings_set_angle_mode(AngleMode m)  { angle_mode = m; }

int  settings_get_decimal_places(void)         { return decimal_places; }
void settings_set_decimal_places(int dp)       { decimal_places = (dp < 0) ? 0 : (dp > 10 ? 10 : dp); }

double settings_get_variable(int idx)          { return (idx >= 0 && idx < SETTINGS_VAR_COUNT) ? user_vars[idx] : 0; }
void   settings_set_variable(int idx, double v){ if (idx >= 0 && idx < SETTINGS_VAR_COUNT) user_vars[idx] = v; }
char   settings_get_variable_name(int idx)     { return (idx >= 0 && idx < SETTINGS_VAR_COUNT) ? (char)('A' + idx) : '?'; }

#define COL_ACCENT_SETTINGS   lv_color_hex(0x607D8B)   
#define COL_FOCUS_BG_SETTINGS lv_color_hex(0xECEFF1)

static void show_settings_menu(void);
static void show_angle_page(void);
static void show_decimals_page(void);
static void show_variables_page(void);


static lv_group_t *angle_group  = NULL;
static lv_obj_t   *angle_labels[2] = {NULL};
static int         angle_sel    = 0;

static void angle_update(void) {
    const char *names[2] = { "Radians (RAD)", "Degrees (DEG)" };
    for (int i = 0; i < 2; i++) {
        lv_label_set_text(angle_labels[i], names[i]);
        if (i == angle_sel) {
            lv_obj_set_style_bg_color(angle_labels[i], COL_FOCUS_BG_SETTINGS, 0);
            lv_obj_set_style_bg_opa(angle_labels[i], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(angle_labels[i], LV_OPA_TRANSP, 0);
        }
    }
}

static void angle_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    switch (key) {
    case LV_KEY_UP:
        if (angle_sel > 0) { angle_sel--; angle_update(); }
        break;
    case LV_KEY_DOWN:
        if (angle_sel < 1) { angle_sel++; angle_update(); }
        break;
    case LV_KEY_ENTER:
        angle_mode = (angle_sel == 0) ? ANGLE_RAD : ANGLE_DEG;
        show_settings_menu();
        break;
    case LV_KEY_ESC: case LV_KEY_BACKSPACE:
        show_settings_menu();
        break;
    case 'M':
        main_menu_create();
        break;
    default: break;
    }
}

static void show_angle_page(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    ui_setup_screen(scr);

    angle_sel = (angle_mode == ANGLE_DEG) ? 1 : 0;

    if (angle_group) { lv_group_del(angle_group); angle_group = NULL; }
    angle_group = lv_group_create();

    lv_obj_t *title = ui_label(scr, "Angle Unit", CONTENT_SIDE, CONTENT_TOP);
    lv_obj_set_style_text_color(title, COL_ACCENT_SETTINGS, 0);

    for (int i = 0; i < 2; i++) {
        int y = CONTENT_TOP + 28 + i * 28;
        angle_labels[i] = lv_label_create(scr);
        lv_label_set_text(angle_labels[i], "");
        lv_obj_set_pos(angle_labels[i], CONTENT_SIDE + 4, y);
        lv_obj_set_size(angle_labels[i], LCD_H_RES - 2 * CONTENT_SIDE - 8, 24);
        lv_obj_set_style_text_font(angle_labels[i], FONT_PRIMARY, 0);
        lv_obj_set_style_text_color(angle_labels[i], COL_TEXT, 0);
        lv_obj_set_style_pad_left(angle_labels[i], 6, 0);
        lv_obj_set_style_pad_top(angle_labels[i], 4, 0);
        lv_obj_set_style_radius(angle_labels[i], 3, 0);
    }

    lv_obj_t *key_recv = lv_obj_create(scr);
    lv_obj_set_size(key_recv, 0, 0);
    lv_obj_add_flag(key_recv, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(key_recv, angle_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(angle_group, key_recv);

    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, angle_group);
    lv_group_focus_obj(key_recv);

    angle_update();

    
    lv_obj_t *cur = ui_label(scr, "", CONTENT_SIDE, CONTENT_TOP + 92);
    lv_obj_set_style_text_font(cur, FONT_SECONDARY, 0);
    lv_obj_set_style_text_color(cur, COL_TEXT_SEC, 0);
    char buf[48];
    snprintf(buf, sizeof(buf), "Current: %s", angle_mode == ANGLE_DEG ? "DEG" : "RAD");
    lv_label_set_text(cur, buf);

    ui_create_hint_bar(scr, "[Up/Dn] Select  [=] Confirm  [AC] Back  [M] Menu");
}


static lv_group_t *dec_group  = NULL;
static lv_obj_t   *dec_labels[11] = {NULL};
static int         dec_sel    = 0;

static void dec_update(void) {
    for (int i = 0; i < 11; i++) {
        if (i == dec_sel) {
            lv_obj_set_style_bg_color(dec_labels[i], COL_FOCUS_BG_SETTINGS, 0);
            lv_obj_set_style_bg_opa(dec_labels[i], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(dec_labels[i], LV_OPA_TRANSP, 0);
        }
    }
}

static void dec_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    switch (key) {
    case LV_KEY_UP:
        if (dec_sel > 0) { dec_sel--; dec_update(); }
        break;
    case LV_KEY_DOWN:
        if (dec_sel < 10) { dec_sel++; dec_update(); }
        break;
    case LV_KEY_ENTER:
        decimal_places = dec_sel;
        show_settings_menu();
        break;
    case LV_KEY_ESC: case LV_KEY_BACKSPACE:
        show_settings_menu();
        break;
    case 'M':
        main_menu_create();
        break;
    default: break;
    }
}

static void show_decimals_page(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    ui_setup_screen(scr);

    dec_sel = decimal_places;

    if (dec_group) { lv_group_del(dec_group); dec_group = NULL; }
    dec_group = lv_group_create();

    lv_obj_t *title = ui_label(scr, "Decimal Places", CONTENT_SIDE, CONTENT_TOP);
    lv_obj_set_style_text_color(title, COL_ACCENT_SETTINGS, 0);

    
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, LCD_H_RES - 8, HINT_BAR_Y - CONTENT_TOP - 24);
    lv_obj_set_pos(cont, 4, CONTENT_TOP + 22);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < 11; i++) {
        char buf[32];
        if (i == 0)
            snprintf(buf, sizeof(buf), "Auto (smart)");
        else
            snprintf(buf, sizeof(buf), "%d decimal place%s", i, i == 1 ? "" : "s");

        int y = i * 18;
        dec_labels[i] = lv_label_create(cont);
        lv_label_set_text(dec_labels[i], buf);
        lv_obj_set_pos(dec_labels[i], CONTENT_SIDE, y);
        lv_obj_set_size(dec_labels[i], LCD_H_RES - 2 * CONTENT_SIDE - 16, 16);
        lv_obj_set_style_text_font(dec_labels[i], FONT_SECONDARY, 0);
        lv_obj_set_style_text_color(dec_labels[i], COL_TEXT, 0);
        lv_obj_set_style_pad_left(dec_labels[i], 4, 0);
        lv_obj_set_style_pad_top(dec_labels[i], 2, 0);
        lv_obj_set_style_radius(dec_labels[i], 3, 0);
    }

    lv_obj_t *key_recv = lv_obj_create(scr);
    lv_obj_set_size(key_recv, 0, 0);
    lv_obj_add_flag(key_recv, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(key_recv, dec_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(dec_group, key_recv);

    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, dec_group);
    lv_group_focus_obj(key_recv);

    dec_update();
    ui_create_hint_bar(scr, "[Up/Dn] Select  [=] Confirm  [AC] Back  [M] Menu");
}


static lv_group_t *var_group    = NULL;
static lv_obj_t   *var_labels[SETTINGS_VAR_COUNT]  = {NULL};
static lv_obj_t   *var_fields[SETTINGS_VAR_COUNT]  = {NULL};
static int         var_focused  = 0;

static void var_focus(int idx) {
    if (idx < 0) idx = 0;
    if (idx >= SETTINGS_VAR_COUNT) idx = SETTINGS_VAR_COUNT - 1;
    var_focused = idx;
    lv_group_focus_obj(var_fields[idx]);
}

static void var_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    lv_obj_t *ta = lv_event_get_target(e);

    
    int idx = -1;
    for (int i = 0; i < SETTINGS_VAR_COUNT; i++) {
        if (var_fields[i] == ta) { idx = i; break; }
    }

    switch (key) {
    case LV_KEY_UP:
        if (idx > 0) var_focus(idx - 1);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_DOWN:
        if (idx < SETTINGS_VAR_COUNT - 1) var_focus(idx + 1);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_ENTER:
        
        for (int i = 0; i < SETTINGS_VAR_COUNT; i++) {
            const char *text = lv_textarea_get_text(var_fields[i]);
            if (strlen(text) > 0)
                user_vars[i] = atof(text);
            else
                user_vars[i] = 0;
        }
        show_settings_menu();
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_ESC: case LV_KEY_BACKSPACE:
        show_settings_menu();
        lv_event_stop_bubbling(e);
        return;
    case 'M':
        main_menu_create();
        lv_event_stop_bubbling(e);
        return;
    default: break;
    }
}

static void show_variables_page(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    ui_setup_screen(scr);

    if (var_group) { lv_group_del(var_group); var_group = NULL; }
    var_group = lv_group_create();

    lv_obj_t *title = ui_label(scr, "Variables (A\xe2\x80\x93""F)", CONTENT_SIDE, CONTENT_TOP);
    lv_obj_set_style_text_color(title, COL_ACCENT_SETTINGS, 0);

    for (int i = 0; i < SETTINGS_VAR_COUNT; i++) {
        int y = CONTENT_TOP + 24 + i * ROW_SPACING;

        char lbl[8];
        snprintf(lbl, sizeof(lbl), "%c:", 'A' + i);
        var_labels[i] = ui_label(scr, lbl, CONTENT_SIDE, y + 4);

        var_fields[i] = lv_textarea_create(scr);
        lv_obj_set_size(var_fields[i], 200, FIELD_H);
        lv_obj_set_pos(var_fields[i], LCD_H_RES - CONTENT_SIDE - 200, y);
        lv_textarea_set_one_line(var_fields[i], true);
        lv_textarea_set_max_length(var_fields[i], 15);
        lv_textarea_set_placeholder_text(var_fields[i], "0");
        ui_style_textarea(var_fields[i], COL_ACCENT_SETTINGS, COL_FOCUS_BG_SETTINGS);

        lv_obj_add_event_cb(var_fields[i], var_key_cb, LV_EVENT_KEY, NULL);
        lv_group_add_obj(var_group, var_fields[i]);

        
        if (user_vars[i] != 0) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.6g", user_vars[i]);
            lv_textarea_set_text(var_fields[i], buf);
        }
    }

    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, var_group);
    var_focus(0);

    ui_create_hint_bar(scr, "[Up/Dn] Field  [=] Save  [AC] Back  [M] Menu");
}


static const SubMenuItem settings_items[] = {
    { "Angle Unit (RAD/DEG)",   show_angle_page     },
    { "Decimal Places",         show_decimals_page   },
    { "Variable Editor (A-F)",  show_variables_page  },
};

static void show_settings_menu(void) {
    SubMenuStyle style;
    style.accent_color  = COL_ACCENT_SETTINGS;
    style.focus_bg_color = COL_FOCUS_BG_SETTINGS;
    style.hint_text     = "[Up/Dn] Select  [=] Open  [M] Menu";

    ui_create_submenu(settings_items, 3, &style, main_menu_create);
}

void settings_app_start(void) {
    show_settings_menu();
}
