#include "math.h"
#include "input_hal.h"
#include "expr_eval.h"
#include "lvgl.h"
#include <stdio.h>
#include <string.h>

// Explicitly declare math functions
int isnan(double x);
int isinf(double x);

static lv_obj_t *result_label = NULL;
static lv_obj_t *input_textarea = NULL;
static lv_group_t *calc_group = NULL;

static void textarea_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_VALUE_CHANGED) {
        const char *text = lv_textarea_get_text(input_textarea);
        double result = eval_expression(text);
        
        char result_str[64];
        if (isnan(result)) {
            snprintf(result_str, sizeof(result_str), "Error");
        } else if (isinf(result)) {
            snprintf(result_str, sizeof(result_str), "Infinity");
        } else if (result == (long long)result) {
            snprintf(result_str, sizeof(result_str), "%.0f", result);
        } else {
            snprintf(result_str, sizeof(result_str), "%.6g", result);
        }
        
        lv_label_set_text(result_label, result_str);
    }
}

void math_app_start(void) {
    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    
    // Result label at top (large display)
    result_label = lv_label_create(scr);
    lv_label_set_text(result_label, "0");
    lv_obj_set_style_text_font(result_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(result_label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_pos(result_label, 20, 20);
    lv_obj_set_width(result_label, 280);
    lv_obj_set_height(result_label, 60);
    lv_obj_set_style_text_align(result_label, LV_TEXT_ALIGN_RIGHT, 0);
    
    // Input textarea at bottom
    input_textarea = lv_textarea_create(scr);
    lv_obj_set_size(input_textarea, 300, 130);
    lv_obj_set_pos(input_textarea, 10, 100);
    lv_textarea_set_placeholder_text(input_textarea, "Enter equation...");
    lv_textarea_set_one_line(input_textarea, false);
    lv_obj_set_style_text_font(input_textarea, &lv_font_montserrat_14, 0);
    
    lv_obj_add_event_cb(input_textarea, textarea_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Setup input group
    calc_group = lv_group_create();
    lv_group_add_obj(calc_group, input_textarea);
    
    lv_indev_t *button_indev = get_navigation_indev();
    if (button_indev != NULL) {
        lv_indev_set_group(button_indev, calc_group);
    }
    
    printf("Math calculator started!\n");
}
