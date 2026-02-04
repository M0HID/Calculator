#include "graph.h"
#include "input_hal.h"
#include "expr_eval.h"
#include "main_menu.h"
#include "lvgl.h"
#include <stdio.h>
#include <string.h>

// Math function declarations
int isnan(double x);
int isinf(double x);

// Screen dimensions
#define SCREEN_W LCD_H_RES
#define SCREEN_H LCD_V_RES

// Graph canvas dimensions (for graph view)
#define CANVAS_Y 35
#define CANVAS_W SCREEN_W
#define CANVAS_H (SCREEN_H - CANVAS_Y)

// Maximum number of functions
#define MAX_FUNCTIONS 4

// Function colors
static const uint32_t func_colors[MAX_FUNCTIONS] = {
    0x00FF00,  // Green
    0xFF0000,  // Red  
    0x00AAFF,  // Blue
    0xFFAA00   // Orange
};

// Screens
typedef enum {
    SCREEN_FUNCTION_LIST,
    SCREEN_GRAPH_VIEW
} GraphScreen;

// Function data
typedef struct {
    char equation[64];
    int enabled;
} Function;

static Function functions[MAX_FUNCTIONS] = {
    { "x^2", 1 },
    { "sin(x)*3", 1 },
    { "", 0 },
    { "", 0 }
};

// View window
static double x_min = -10.0;
static double x_max = 10.0;
static double y_min = -10.0;
static double y_max = 10.0;

// State
static GraphScreen current_screen = SCREEN_FUNCTION_LIST;
static int selected_function = 0;
static int editing_function = -1;  // -1 = not editing

// Trace state
static int trace_enabled = 0;
static double trace_x = 0.0;
static int trace_func_idx = 0;

// UI elements - Function List screen
static lv_obj_t *func_list_container = NULL;
static lv_obj_t *func_rows[MAX_FUNCTIONS];
static lv_obj_t *func_checkboxes[MAX_FUNCTIONS];
static lv_obj_t *func_labels[MAX_FUNCTIONS];
static lv_obj_t *func_textareas[MAX_FUNCTIONS];
static lv_obj_t *hint_label = NULL;

// UI elements - Graph View screen
static lv_obj_t *graph_container = NULL;
static lv_obj_t *canvas = NULL;
static lv_obj_t *info_label = NULL;
static lv_obj_t *coords_label = NULL;

// Canvas buffer
LV_DRAW_BUF_DEFINE_STATIC(canvas_buf, CANVAS_W, CANVAS_H, LV_COLOR_FORMAT_NATIVE);

// Input group
static lv_group_t *graph_group = NULL;

// Forward declarations
static void show_function_list(void);
static void show_graph_view(void);
static void draw_graph(void);
static void update_function_list_ui(void);
static void update_graph_info(void);

// ============== Coordinate transforms ==============

static int graph_to_canvas_x(double gx) {
    return (int)((gx - x_min) / (x_max - x_min) * CANVAS_W);
}

static int graph_to_canvas_y(double gy) {
    return (int)(CANVAS_H - (gy - y_min) / (y_max - y_min) * CANVAS_H);
}

// ============== Drawing functions ==============

static void draw_axes(void) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(0x555555);
    line_dsc.width = 1;
    line_dsc.opa = LV_OPA_COVER;
    
    // Y axis
    if (x_min <= 0 && x_max >= 0) {
        int ax = graph_to_canvas_x(0);
        line_dsc.p1.x = ax; line_dsc.p1.y = 0;
        line_dsc.p2.x = ax; line_dsc.p2.y = CANVAS_H;
        lv_draw_line(&layer, &line_dsc);
    }
    
    // X axis
    if (y_min <= 0 && y_max >= 0) {
        int ay = graph_to_canvas_y(0);
        line_dsc.p1.x = 0; line_dsc.p1.y = ay;
        line_dsc.p2.x = CANVAS_W; line_dsc.p2.y = ay;
        lv_draw_line(&layer, &line_dsc);
    }
    
    // Grid
    line_dsc.color = lv_color_hex(0x333333);
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
        int cx = graph_to_canvas_x(gx);
        if (cx >= 0 && cx < CANVAS_W) {
            line_dsc.p1.x = cx; line_dsc.p1.y = 0;
            line_dsc.p2.x = cx; line_dsc.p2.y = CANVAS_H;
            lv_draw_line(&layer, &line_dsc);
        }
    }
    for (double gy = ((int)(y_min/tick))*tick; gy <= y_max; gy += tick) {
        if (gy > -0.001 && gy < 0.001) continue;
        int cy = graph_to_canvas_y(gy);
        if (cy >= 0 && cy < CANVAS_H) {
            line_dsc.p1.x = 0; line_dsc.p1.y = cy;
            line_dsc.p2.x = CANVAS_W; line_dsc.p2.y = cy;
            lv_draw_line(&layer, &line_dsc);
        }
    }
    
    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_function(int idx) {
    if (idx < 0 || idx >= MAX_FUNCTIONS) return;
    if (!functions[idx].enabled || strlen(functions[idx].equation) == 0) return;
    
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(func_colors[idx]);
    line_dsc.width = 2;
    line_dsc.opa = LV_OPA_COVER;
    
    double step = (x_max - x_min) / CANVAS_W;
    int prev_cx = -1, prev_cy = -1, prev_valid = 0;
    
    for (double gx = x_min; gx <= x_max; gx += step) {
        double gy = eval_expression_x(functions[idx].equation, gx);
        int cx = graph_to_canvas_x(gx);
        int cy = graph_to_canvas_y(gy);
        int valid = !isnan(gy) && !isinf(gy) && cy >= -CANVAS_H && cy <= 2*CANVAS_H;
        
        if (valid && prev_valid) {
            line_dsc.p1.x = prev_cx; line_dsc.p1.y = prev_cy;
            line_dsc.p2.x = cx; line_dsc.p2.y = cy;
            int dy = cy - prev_cy;
            if (dy < 0) dy = -dy;
            if (dy < CANVAS_H * 2) {
                lv_draw_line(&layer, &line_dsc);
            }
        }
        prev_cx = cx; prev_cy = cy; prev_valid = valid;
    }
    
    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_trace_cursor(void) {
    if (!trace_enabled) return;
    if (trace_func_idx < 0 || trace_func_idx >= MAX_FUNCTIONS) return;
    if (!functions[trace_func_idx].enabled) return;
    
    double gy = eval_expression_x(functions[trace_func_idx].equation, trace_x);
    if (isnan(gy) || isinf(gy)) return;
    
    int cx = graph_to_canvas_x(trace_x);
    int cy = graph_to_canvas_y(gy);
    if (cx < 0 || cx >= CANVAS_W || cy < 0 || cy >= CANVAS_H) return;
    
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    
    // Crosshair
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(0xFFFF00);
    line_dsc.width = 1;
    line_dsc.opa = LV_OPA_70;
    
    line_dsc.p1.x = cx; line_dsc.p1.y = 0;
    line_dsc.p2.x = cx; line_dsc.p2.y = CANVAS_H;
    lv_draw_line(&layer, &line_dsc);
    
    line_dsc.p1.x = 0; line_dsc.p1.y = cy;
    line_dsc.p2.x = CANVAS_W; line_dsc.p2.y = cy;
    lv_draw_line(&layer, &line_dsc);
    
    // Dot
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(func_colors[trace_func_idx]);
    rect_dsc.bg_opa = LV_OPA_COVER;
    lv_area_t dot = { cx-4, cy-4, cx+4, cy+4 };
    lv_draw_rect(&layer, &rect_dsc, &dot);
    
    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_graph(void) {
    lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), LV_OPA_COVER);
    draw_axes();
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        draw_function(i);
    }
    draw_trace_cursor();
}

// ============== UI Update functions ==============

static void update_function_list_ui(void) {
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        // Update checkbox state
        if (functions[i].enabled) {
            lv_obj_add_state(func_checkboxes[i], LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(func_checkboxes[i], LV_STATE_CHECKED);
        }
        
        // Update label color for selected item
        if (i == selected_function && editing_function < 0) {
            lv_obj_set_style_bg_color(func_rows[i], lv_color_hex(0x333355), 0);
            lv_obj_set_style_bg_opa(func_rows[i], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(func_rows[i], LV_OPA_TRANSP, 0);
        }
        
        // Show/hide textarea vs label
        if (editing_function == i) {
            lv_obj_add_flag(func_labels[i], LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(func_textareas[i], LV_OBJ_FLAG_HIDDEN);
            lv_textarea_set_text(func_textareas[i], functions[i].equation);
            lv_group_focus_obj(func_textareas[i]);
        } else {
            lv_obj_remove_flag(func_labels[i], LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(func_textareas[i], LV_OBJ_FLAG_HIDDEN);
            
            char label_text[80];
            snprintf(label_text, sizeof(label_text), "y%d = %s", i+1, 
                     strlen(functions[i].equation) > 0 ? functions[i].equation : "(empty)");
            lv_label_set_text(func_labels[i], label_text);
            
            // Color code the label
            lv_obj_set_style_text_color(func_labels[i], lv_color_hex(func_colors[i]), 0);
        }
    }
    
    // Update hint
    if (editing_function >= 0) {
        lv_label_set_text(hint_label, "[Enter] Save  [Esc] Cancel");
    } else {
        lv_label_set_text(hint_label, "[Enter] Edit  [Space] Toggle  [G] Graph");
    }
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
        lv_label_set_text(coords_label, "[T]race [+/-]Zoom [0]Reset");
        lv_obj_set_style_text_color(coords_label, lv_color_hex(0x888888), 0);
    }
}

// ============== Event handlers ==============

static void textarea_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    
    if (code == LV_EVENT_READY || code == LV_EVENT_DEFOCUSED) {
        // Save and exit edit mode
        if (editing_function == idx) {
            const char *text = lv_textarea_get_text(func_textareas[idx]);
            strncpy(functions[idx].equation, text, sizeof(functions[idx].equation) - 1);
            editing_function = -1;
            update_function_list_ui();
        }
    } else if (code == LV_EVENT_CANCEL) {
        // Cancel edit
        editing_function = -1;
        update_function_list_ui();
    }
}

static void funclist_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    
    // If editing, let textarea handle most keys
    if (editing_function >= 0) {
        if (key == LV_KEY_ESC) {
            editing_function = -1;
            update_function_list_ui();
        }
        return;
    }
    
    switch (key) {
    case LV_KEY_UP:
        if (selected_function > 0) selected_function--;
        update_function_list_ui();
        break;
        
    case LV_KEY_DOWN:
        if (selected_function < MAX_FUNCTIONS - 1) selected_function++;
        update_function_list_ui();
        break;
        
    case LV_KEY_ENTER:
        // Start editing selected function
        editing_function = selected_function;
        update_function_list_ui();
        break;
        
    case ' ':  // Space to toggle
        functions[selected_function].enabled = !functions[selected_function].enabled;
        update_function_list_ui();
        break;
        
    case 'g':
    case 'G':
    case LV_KEY_RIGHT:
        // Go to graph view
        show_graph_view();
        break;
    }
}

static void graph_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    
    double pan_amt = (x_max - x_min) * 0.1;
    
    switch (key) {
    case LV_KEY_LEFT:
        if (trace_enabled) {
            trace_x -= (x_max - x_min) * 0.02;
            if (trace_x < x_min) trace_x = x_min;
        } else {
            x_min -= pan_amt; x_max -= pan_amt;
        }
        break;
        
    case LV_KEY_RIGHT:
        if (trace_enabled) {
            trace_x += (x_max - x_min) * 0.02;
            if (trace_x > x_max) trace_x = x_max;
        } else {
            x_min += pan_amt; x_max += pan_amt;
        }
        break;
        
    case LV_KEY_UP:
        if (trace_enabled) {
            // Switch to next enabled function
            for (int i = 1; i <= MAX_FUNCTIONS; i++) {
                int next = (trace_func_idx + i) % MAX_FUNCTIONS;
                if (functions[next].enabled && strlen(functions[next].equation) > 0) {
                    trace_func_idx = next;
                    break;
                }
            }
        } else {
            y_min += pan_amt; y_max += pan_amt;
        }
        break;
        
    case LV_KEY_DOWN:
        if (trace_enabled) {
            // Switch to prev enabled function
            for (int i = 1; i <= MAX_FUNCTIONS; i++) {
                int prev = (trace_func_idx - i + MAX_FUNCTIONS) % MAX_FUNCTIONS;
                if (functions[prev].enabled && strlen(functions[prev].equation) > 0) {
                    trace_func_idx = prev;
                    break;
                }
            }
        } else {
            y_min -= pan_amt; y_max -= pan_amt;
        }
        break;
        
    case '+':
    case '=':
        { double cx = (x_min+x_max)/2, cy = (y_min+y_max)/2;
          double rx = (x_max-x_min)/3, ry = (y_max-y_min)/3;
          x_min = cx-rx; x_max = cx+rx; y_min = cy-ry; y_max = cy+ry; }
        break;
        
    case '-':
    case '_':
        { double cx = (x_min+x_max)/2, cy = (y_min+y_max)/2;
          double rx = (x_max-x_min)*0.75, ry = (y_max-y_min)*0.75;
          x_min = cx-rx; x_max = cx+rx; y_min = cy-ry; y_max = cy+ry; }
        break;
        
    case 't':
    case 'T':
        trace_enabled = !trace_enabled;
        if (trace_enabled) {
            trace_x = (x_min + x_max) / 2;
            // Find first enabled function
            trace_func_idx = -1;
            for (int i = 0; i < MAX_FUNCTIONS; i++) {
                if (functions[i].enabled && strlen(functions[i].equation) > 0) {
                    trace_func_idx = i;
                    break;
                }
            }
            if (trace_func_idx < 0) trace_enabled = 0;
        }
        break;
        
    case '0':
        x_min = -10; x_max = 10; y_min = -10; y_max = 10;
        trace_x = 0;
        break;
        
    case LV_KEY_ESC:
    case LV_KEY_BACKSPACE:
        show_function_list();
        return;
    }
    
    draw_graph();
    update_graph_info();
}

// ============== Screen management ==============

static void show_function_list(void) {
    current_screen = SCREEN_FUNCTION_LIST;
    trace_enabled = 0;
    
    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    
    // Title
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Functions");
    lv_obj_set_pos(title, 10, 5);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    
    // Function list container
    func_list_container = lv_obj_create(scr);
    lv_obj_set_size(func_list_container, SCREEN_W - 10, SCREEN_H - 55);
    lv_obj_set_pos(func_list_container, 5, 25);
    lv_obj_set_style_bg_color(func_list_container, lv_color_hex(0x111111), 0);
    lv_obj_set_style_border_width(func_list_container, 0, 0);
    lv_obj_set_style_pad_all(func_list_container, 5, 0);
    lv_obj_clear_flag(func_list_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create rows for each function
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        func_rows[i] = lv_obj_create(func_list_container);
        lv_obj_set_size(func_rows[i], SCREEN_W - 30, 38);
        lv_obj_set_pos(func_rows[i], 0, i * 42);
        lv_obj_set_style_bg_opa(func_rows[i], LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(func_rows[i], 0, 0);
        lv_obj_set_style_pad_all(func_rows[i], 2, 0);
        lv_obj_clear_flag(func_rows[i], LV_OBJ_FLAG_SCROLLABLE);
        
        // Checkbox
        func_checkboxes[i] = lv_checkbox_create(func_rows[i]);
        lv_checkbox_set_text(func_checkboxes[i], "");
        lv_obj_set_pos(func_checkboxes[i], 0, 5);
        if (functions[i].enabled) {
            lv_obj_add_state(func_checkboxes[i], LV_STATE_CHECKED);
        }
        
        // Label (shown when not editing)
        func_labels[i] = lv_label_create(func_rows[i]);
        lv_obj_set_pos(func_labels[i], 35, 8);
        lv_obj_set_style_text_color(func_labels[i], lv_color_hex(func_colors[i]), 0);
        
        // Textarea (shown when editing)
        func_textareas[i] = lv_textarea_create(func_rows[i]);
        lv_obj_set_size(func_textareas[i], SCREEN_W - 80, 30);
        lv_obj_set_pos(func_textareas[i], 35, 2);
        lv_textarea_set_one_line(func_textareas[i], true);
        lv_textarea_set_placeholder_text(func_textareas[i], "Enter f(x)...");
        lv_obj_add_flag(func_textareas[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_event_cb(func_textareas[i], textarea_event_cb, LV_EVENT_ALL, (void*)(intptr_t)i);
    }
    
    // Hint label at bottom
    hint_label = lv_label_create(scr);
    lv_obj_set_pos(hint_label, 10, SCREEN_H - 25);
    lv_obj_set_style_text_color(hint_label, lv_color_hex(0x888888), 0);
    
    // Setup navigation
    graph_group = lv_group_create();
    
    // Add a dummy object for key handling
    lv_obj_t *key_receiver = lv_obj_create(scr);
    lv_obj_set_size(key_receiver, 0, 0);
    lv_obj_add_flag(key_receiver, LV_OBJ_FLAG_HIDDEN);
    lv_group_add_obj(graph_group, key_receiver);
    lv_obj_add_event_cb(key_receiver, funclist_key_cb, LV_EVENT_KEY, NULL);
    
    // Add textareas to group
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        lv_group_add_obj(graph_group, func_textareas[i]);
    }
    
    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, graph_group);
    
    lv_group_focus_obj(key_receiver);
    
    update_function_list_ui();
}

static void show_graph_view(void) {
    current_screen = SCREEN_GRAPH_VIEW;
    
    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    
    // Info bar at top
    info_label = lv_label_create(scr);
    lv_obj_set_pos(info_label, 5, 3);
    lv_obj_set_style_text_color(info_label, lv_color_hex(0x888888), 0);
    
    coords_label = lv_label_create(scr);
    lv_obj_set_pos(coords_label, 5, 18);
    
    // Canvas
    canvas = lv_canvas_create(scr);
    LV_DRAW_BUF_INIT_STATIC(canvas_buf);
    lv_canvas_set_draw_buf(canvas, &canvas_buf);
    lv_obj_set_pos(canvas, 0, CANVAS_Y);
    
    // Key handler
    graph_group = lv_group_create();
    lv_obj_t *key_receiver = lv_obj_create(scr);
    lv_obj_set_size(key_receiver, 0, 0);
    lv_obj_add_flag(key_receiver, LV_OBJ_FLAG_HIDDEN);
    lv_group_add_obj(graph_group, key_receiver);
    lv_obj_add_event_cb(key_receiver, graph_key_cb, LV_EVENT_KEY, NULL);
    
    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, graph_group);
    
    lv_group_focus_obj(key_receiver);
    
    draw_graph();
    update_graph_info();
}

// ============== Entry point ==============

void graph_app_start(void) {
    // Reset view
    x_min = -10.0; x_max = 10.0;
    y_min = -10.0; y_max = 10.0;
    trace_enabled = 0;
    trace_x = 0;
    selected_function = 0;
    editing_function = -1;
    
    show_function_list();
    
    printf("Graph app started!\n");
}
