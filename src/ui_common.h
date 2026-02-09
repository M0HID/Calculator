#pragma once

#include "lvgl.h"
#include "main_menu.h"

/*──────────────────────────────────────────────────────────────
 *  Shared design tokens – every screen must use these.
 *──────────────────────────────────────────────────────────────*/

/* ── Custom math fonts (Segoe UI with Greek/math glyphs) ── */
LV_FONT_DECLARE(font_math_14);
LV_FONT_DECLARE(font_math_12);

#define FONT_PRIMARY   &font_math_14   /* labels, values, inputs   */
#define FONT_SECONDARY &font_math_12   /* hints, small annotations */

/* ── Layout constants ── */
#define CONTENT_TOP       4            /* top padding              */
#define CONTENT_SIDE      6            /* left/right padding       */
#define HINT_BAR_H       16            /* height of bottom bar     */
#define HINT_BAR_Y       (LCD_V_RES - HINT_BAR_H)   /* y of bar  */
#define CONTENT_H        (LCD_V_RES - HINT_BAR_H - CONTENT_TOP)
#define ROW_H            28            /* standard input row       */
#define ROW_SPACING      30            /* row-to-row distance      */
#define FIELD_H          26            /* textarea height          */

/* ── Color palette ── */
#define COL_BG           lv_color_hex(0xFFFFFF)
#define COL_TEXT         lv_color_hex(0x333333)
#define COL_TEXT_SEC     lv_color_hex(0x888888)
#define COL_HINT_BG      lv_color_hex(0xF0F0F0)
#define COL_BORDER       lv_color_hex(0xCCCCCC)
#define COL_RESULT       lv_color_hex(0x009900)
#define COL_ERROR        lv_color_hex(0xCC0000)

/* Per-app accent colours (focus highlight only) */
#define COL_ACCENT_MATH      lv_color_hex(0x2196F3)   /* blue   */
#define COL_ACCENT_GRAPH     lv_color_hex(0xF44336)   /* red    */
#define COL_ACCENT_STATS     lv_color_hex(0x4CAF50)   /* green  */
#define COL_ACCENT_SOLVER    lv_color_hex(0xFF9800)   /* orange */
#define COL_ACCENT_MECH      lv_color_hex(0x9C27B0)   /* purple */

/* Light tint versions for focused background */
#define COL_FOCUS_BG_MATH    lv_color_hex(0xE3F2FD)
#define COL_FOCUS_BG_GRAPH   lv_color_hex(0xFFEBEE)
#define COL_FOCUS_BG_STATS   lv_color_hex(0xE8F5E9)
#define COL_FOCUS_BG_SOLVER  lv_color_hex(0xFFF3E0)
#define COL_FOCUS_BG_MECH    lv_color_hex(0xF3E5F5)

/*──────────────────────────────────────────────────────────────
 *  Helper: create the bottom hint bar on any screen.
 *  Returns the label so the caller can update text later.
 *──────────────────────────────────────────────────────────────*/
static inline lv_obj_t *ui_create_hint_bar(lv_obj_t *parent, const char *text) {
    /* Background strip */
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_set_size(bar, LCD_H_RES, HINT_BAR_H);
    lv_obj_set_pos(bar, 0, HINT_BAR_Y);
    lv_obj_set_style_bg_color(bar, COL_HINT_BG, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_radius(bar, 0, 0);
    lv_obj_set_style_pad_all(bar, 0, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_CLICKABLE);

    /* Text label */
    lv_obj_t *lbl = lv_label_create(bar);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_font(lbl, FONT_SECONDARY, 0);
    lv_obj_set_style_text_color(lbl, COL_TEXT_SEC, 0);
    lv_obj_set_pos(lbl, 6, 1);

    return lbl;
}

/*──────────────────────────────────────────────────────────────
 *  Helper: style a screen background consistently.
 *──────────────────────────────────────────────────────────────*/
static inline void ui_setup_screen(lv_obj_t *scr) {
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
}

/*──────────────────────────────────────────────────────────────
 *  Helper: apply focus styles to a textarea.
 *  call once per textarea, passing the app's accent colour.
 *──────────────────────────────────────────────────────────────*/
static inline void ui_style_textarea(lv_obj_t *ta, lv_color_t accent, lv_color_t focus_bg) {
    /* Default state */
    lv_obj_set_style_text_font(ta, FONT_PRIMARY, 0);
    lv_obj_set_style_text_color(ta, COL_TEXT, 0);
    lv_obj_set_style_bg_color(ta, COL_BG, 0);
    lv_obj_set_style_border_color(ta, COL_BORDER, 0);
    lv_obj_set_style_border_width(ta, 1, 0);
    lv_obj_set_style_radius(ta, 3, 0);
    lv_obj_set_style_pad_left(ta, 4, 0);
    lv_obj_set_style_pad_right(ta, 4, 0);

    /* Hide scrollbar */
    lv_obj_set_scrollbar_mode(ta, LV_SCROLLBAR_MODE_OFF);

    /* Focused state */
    lv_obj_set_style_border_color(ta, accent, LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ta, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ta, focus_bg, LV_STATE_FOCUSED);
}

/*──────────────────────────────────────────────────────────────
 *  Helper: create a standard label.
 *──────────────────────────────────────────────────────────────*/
static inline lv_obj_t *ui_label(lv_obj_t *parent, const char *text,
                                  int x, int y) {
    lv_obj_t *lbl = lv_label_create(parent);
    lv_label_set_text(lbl, text);
    lv_obj_set_pos(lbl, x, y);
    lv_obj_set_style_text_font(lbl, FONT_PRIMARY, 0);
    lv_obj_set_style_text_color(lbl, COL_TEXT, 0);
    return lbl;
}
