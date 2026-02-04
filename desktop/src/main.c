/**
 * @file main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE /* needed for usleep() */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifdef _MSC_VER
#include <Windows.h>
#include <direct.h>  // For _getcwd
#else
#include <pthread.h>
#include <unistd.h>  // For getcwd

#endif
#include "lvgl/lvgl.h"
#include <SDL.h>


#include "hal/hal.h"
#include "main_menu.h"
#include "input_hal.h"

/*********************
 *      DEFINES
 *********************/
#define BUTTON_COLS 6
#define BUTTON_ROWS 9
#define BUTTON_COUNT (BUTTON_COLS * BUTTON_ROWS)

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
  int row;
  int col;
  char keycode[20];
} button_keymap_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_button_panel(lv_obj_t *parent);
static void load_keymap_config(const char *filename);
static void button_event_cb(lv_event_t *e);
static int find_button_by_key(const char *key);
static void button_flash_timer_cb(lv_timer_t *timer);
static void global_key_handler(lv_event_t *e);
static uint32_t keycode_string_to_lvgl_key(const char *keycode);
static void send_key_to_main_app(uint32_t key);
static const char* get_button_label(const char *keycode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *button_objects[BUTTON_ROWS][BUTTON_COLS];
static button_keymap_t keymap[BUTTON_COUNT];
static int keymap_count = 0;
static lv_indev_t *main_keyboard_indev = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_USE_OS != LV_OS_FREERTOS

static void load_keymap_config(const char *filename) {
  FILE *file = fopen(filename, "r");
  
  if (!file) {
    // Try alternate paths if file not found in current directory
    const char *alt_paths[] = {
      "../button_keycodes.txt",           // One level up
      "../../button_keycodes.txt",        // Two levels up
      "button_keycodes.txt",              // Current directory again
      "windows/bin/button_keycodes.txt"  // From project root
    };
    
    for (int i = 0; i < 4 && !file; i++) {
      file = fopen(alt_paths[i], "r");
      if (file) {
        printf("Loaded keymap from: %s\n", alt_paths[i]);
        break;
      }
    }
    
    if (!file) {
      printf("Warning: Could not open %s, using default keymap\n", filename);
      return;
    }
  } else {
    printf("Loaded keymap from: %s\n", filename);
  }

  char line[256];
  keymap_count = 0;

  while (fgets(line, sizeof(line), file) && keymap_count < BUTTON_COUNT) {
    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
      continue;
    }

    int row, col;
    char keycode[20];
    
    // Parse format: ROW,COL=KEYCODE
    if (sscanf(line, "%d,%d=%19s", &row, &col, keycode) == 3) {
      if (row >= 0 && row < BUTTON_ROWS && col >= 0 && col < BUTTON_COLS) {
        keymap[keymap_count].row = row;
        keymap[keymap_count].col = col;
        
        // Remove trailing whitespace/newline
        char *end = keycode + strlen(keycode) - 1;
        while (end > keycode && (*end == '\n' || *end == '\r' || *end == ' ')) {
          *end = '\0';
          end--;
        }
        
        strncpy(keymap[keymap_count].keycode, keycode, sizeof(keymap[keymap_count].keycode) - 1);
        keymap[keymap_count].keycode[19] = '\0';
        keymap_count++;
      }
    }
  }

  fclose(file);
  printf("Loaded %d button keymaps\n", keymap_count);
}

static int find_button_by_key(const char *key) {
  for (int i = 0; i < keymap_count; i++) {
    if (strcasecmp(keymap[i].keycode, key) == 0) {
      return i;
    }
  }
  return -1;
}

// Get user-friendly button label
static const char* get_button_label(const char *keycode) {
  if (strcasecmp(keycode, "LSHIFT") == 0) return "SHIFT";
  if (strcasecmp(keycode, "LALT") == 0) return "ALPHA";
  if (strcasecmp(keycode, "PAGEUP") == 0) return "MENU";
  if (strcasecmp(keycode, "PAGEDOWN") == 0) return "ON";
  if (strcasecmp(keycode, "RETURN") == 0) return "ENTER";
  if (strcasecmp(keycode, "BACKSPACE") == 0) return "DEL";
  if (strcasecmp(keycode, "DELETE") == 0) return "AC";
  if (strcasecmp(keycode, "ESCAPE") == 0) return "ESC";
  if (strcasecmp(keycode, "SPACE") == 0) return "SPC";
  
  // For arrow keys, use symbols
  if (strcasecmp(keycode, "UP") == 0) return "^";
  if (strcasecmp(keycode, "DOWN") == 0) return "v";
  if (strcasecmp(keycode, "LEFT") == 0) return "<";
  if (strcasecmp(keycode, "RIGHT") == 0) return ">";
  
  // Return as-is for everything else
  return keycode;
}

// Convert keycode string to LVGL key code
static uint32_t keycode_string_to_lvgl_key(const char *keycode) {
  // Navigation keys
  if (strcasecmp(keycode, "UP") == 0) return LV_KEY_UP;
  if (strcasecmp(keycode, "DOWN") == 0) return LV_KEY_DOWN;
  if (strcasecmp(keycode, "LEFT") == 0) return LV_KEY_LEFT;
  if (strcasecmp(keycode, "RIGHT") == 0) return LV_KEY_RIGHT;
  if (strcasecmp(keycode, "RETURN") == 0 || strcasecmp(keycode, "ENTER") == 0) return LV_KEY_ENTER;
  if (strcasecmp(keycode, "BACKSPACE") == 0) return LV_KEY_BACKSPACE;
  if (strcasecmp(keycode, "DELETE") == 0) return LV_KEY_DEL;
  if (strcasecmp(keycode, "ESCAPE") == 0 || strcasecmp(keycode, "ESC") == 0) return LV_KEY_ESC;
  if (strcasecmp(keycode, "TAB") == 0) return LV_KEY_NEXT;
  if (strcasecmp(keycode, "HOME") == 0) return LV_KEY_HOME;
  if (strcasecmp(keycode, "END") == 0) return LV_KEY_END;
  
  // Special calculator keys (mapped to function keys internally)
  if (strcasecmp(keycode, "LSHIFT") == 0) return 'S';  // Shift key
  if (strcasecmp(keycode, "LALT") == 0) return 'A';    // Alpha key
  if (strcasecmp(keycode, "PAGEUP") == 0) return 'M';  // Menu key
  if (strcasecmp(keycode, "PAGEDOWN") == 0) return 'O'; // On key
  
  // Single character keys - return the uppercase ASCII code
  if (strlen(keycode) == 1) {
    return (uint32_t)toupper(keycode[0]);
  }
  
  // For multi-char like "SPACE", return first char
  if (strcasecmp(keycode, "SPACE") == 0) return ' ';
  
  // Default to first character
  return (uint32_t)keycode[0];
}

// Send key event to main app
static void send_key_to_main_app(uint32_t key) {
  // Special handling for Menu key - return to main menu
  if (key == 'M') {
    printf("Menu key pressed - returning to main menu\n");
    main_menu_create();
    return;
  }
  
  if (main_keyboard_indev) {
    lv_group_t *main_group = lv_indev_get_group(main_keyboard_indev);
    if (main_group) {
      // Send the key press to the group
      lv_group_send_data(main_group, key);
    }
  }
}

static void button_flash_timer_cb(lv_timer_t *timer) {
  lv_obj_t *btn = (lv_obj_t *)lv_timer_get_user_data(timer);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_timer_delete(timer);
}

static void button_event_cb(lv_event_t *e) {
  lv_obj_t *btn = lv_event_get_target(e);
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    // Find which button was clicked
    for (int r = 0; r < BUTTON_ROWS; r++) {
      for (int c = 0; c < BUTTON_COLS; c++) {
        if (button_objects[r][c] == btn) {
          printf("Button pressed: Row %d, Col %d\n", r, c);
          
          // Flash the button
          lv_obj_set_style_bg_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_timer_create(button_flash_timer_cb, 100, btn);
          
          // Find the keycode for this button and send it to main app
          for (int i = 0; i < keymap_count; i++) {
            if (keymap[i].row == r && keymap[i].col == c) {
              uint32_t key = keycode_string_to_lvgl_key(keymap[i].keycode);
              printf("Sending key: %s (0x%x) to main app\n", keymap[i].keycode, key);
              send_key_to_main_app(key);
              break;
            }
          }
          
          return;
        }
      }
    }
  }
}

static void create_button_panel(lv_obj_t *parent) {
  // Calculate button dimensions
  int panel_width = 320; // Same as display width
  int panel_height = 270; // 9 rows * 30px per button
  int btn_width = (panel_width - 10) / BUTTON_COLS - 2; // Small margins
  int btn_height = 28;
  int margin = 2;
  int start_x = 5;
  int start_y = 5;

  for (int r = 0; r < BUTTON_ROWS; r++) {
    for (int c = 0; c < BUTTON_COLS; c++) {
      lv_obj_t *btn = lv_button_create(parent);
      lv_obj_set_size(btn, btn_width, btn_height);
      
      // Calculate base position
      int pos_x = start_x + c * (btn_width + margin);
      int pos_y = start_y + r * (btn_height + margin);
      
      // Find keymap for this button to check if it's an arrow key
      char key_text[20] = "";
      bool is_arrow = false;
      for (int i = 0; i < keymap_count; i++) {
        if (keymap[i].row == r && keymap[i].col == c) {
          const char *display_label = get_button_label(keymap[i].keycode);
          strncpy(key_text, display_label, sizeof(key_text) - 1);
          
          // Check if this is an arrow key and adjust position for diamond layout
          if (strcasecmp(keymap[i].keycode, "UP") == 0) {
            // UP key - shift half-position up from row 0
            pos_y -= (btn_height + margin) / 2;
            is_arrow = true;
          } else if (strcasecmp(keymap[i].keycode, "DOWN") == 0) {
            // DOWN key - shift half-position down from row 1
            pos_y += (btn_height + margin) / 2;
            is_arrow = true;
          } else if (strcasecmp(keymap[i].keycode, "LEFT") == 0) {
            // LEFT key - shift half-position left
            pos_x -= (btn_width + margin) / 2;
            is_arrow = true;
          } else if (strcasecmp(keymap[i].keycode, "RIGHT") == 0) {
            // RIGHT key - shift half-position right
            pos_x += (btn_width + margin) / 2;
            is_arrow = true;
          }
          break;
        }
      }
      
      lv_obj_set_pos(btn, pos_x, pos_y);
      
      // Style the button
      lv_obj_set_style_bg_color(btn, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_border_color(btn, lv_color_hex(0x606060), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_radius(btn, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
      
      // Add label
      lv_obj_t *label = lv_label_create(btn);
      
      if (strlen(key_text) == 0) {
        snprintf(key_text, sizeof(key_text), "%d,%d", r, c);
      }
      
      lv_label_set_text(label, key_text);
      lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
      lv_obj_center(label);
      
      // Add click event
      lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_CLICKED, NULL);
      
      // Store reference
      button_objects[r][c] = btn;
    }
  }
}

// Global keyboard handler
static void global_key_handler(lv_event_t *e) {
  uint32_t key = lv_event_get_key(e);
  char key_str[20];
  
  // Convert SDL keycode to string
  SDL_Keycode sdl_key = (SDL_Keycode)key;
  const char *key_name = SDL_GetKeyName(sdl_key);
  
  if (key_name && strlen(key_name) > 0) {
    strncpy(key_str, key_name, sizeof(key_str) - 1);
    key_str[19] = '\0';
    
    // Find and trigger corresponding button
    int idx = find_button_by_key(key_str);
    if (idx >= 0) {
      int row = keymap[idx].row;
      int col = keymap[idx].col;
      
      printf("Key '%s' pressed -> Button[%d][%d]\n", key_str, row, col);
      
      // Trigger button press visually
      lv_obj_t *btn = button_objects[row][col];
      if (btn) {
        // Flash the button
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_timer_create(button_flash_timer_cb, 100, btn);
        
        // Send key to main app
        uint32_t lvgl_key = keycode_string_to_lvgl_key(keymap[idx].keycode);
        send_key_to_main_app(lvgl_key);
      }
    }
  }
}

int main(int argc, char **argv) {
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  // Create two displays: main app (320x240) and button panel (320x270)
  lv_display_t *main_disp = sdl_hal_init(320, 240);
  
  // Get the keyboard input device from main display (stored by hal.c)
  main_keyboard_indev = get_navigation_indev();
  
  // Create second SDL window for button panel
  lv_display_t *button_disp = lv_sdl_window_create(320, 270);
  
  // Set up mouse for button panel
  lv_indev_t *button_mouse = lv_sdl_mouse_create();
  lv_indev_set_display(button_mouse, button_disp);
  
  // Set up keyboard for button panel
  lv_indev_t *button_kb = lv_sdl_keyboard_create();
  lv_indev_set_display(button_kb, button_disp);
  
  // Load keymap configuration
  load_keymap_config("button_keycodes.txt");
  
  // Create button panel on second display
  lv_display_set_default(button_disp);
  lv_obj_t *button_screen = lv_screen_active();
  lv_obj_set_style_bg_color(button_screen, lv_color_hex(0x202020), LV_PART_MAIN | LV_STATE_DEFAULT);
  create_button_panel(button_screen);
  
  // Add global keyboard handler to button screen
  lv_obj_add_event_cb(button_screen, global_key_handler, LV_EVENT_KEY, NULL);
  lv_group_t *btn_group = lv_group_create();
  lv_group_add_obj(btn_group, button_screen);
  lv_indev_set_group(button_kb, btn_group);
  
  // Switch back to main display for app
  lv_display_set_default(main_disp);

  /* Run the calculator main menu */
  main_menu_create();

  while (1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    uint32_t sleep_time_ms = lv_timer_handler();
    if (sleep_time_ms == LV_NO_TIMER_READY) {
      sleep_time_ms = LV_DEF_REFR_PERIOD;
    }
#ifdef _MSC_VER
    Sleep(sleep_time_ms);
#else
    usleep(sleep_time_ms * 1000);
#endif
  }

  return 0;
}

#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/
