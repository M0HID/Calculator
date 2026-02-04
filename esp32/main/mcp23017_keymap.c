#include "mcp23017_keymap.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "MCP23017_KEYMAP";

// Maximum buttons in a 9x6 matrix
#define MAX_BUTTONS 54

typedef struct {
    int row;           // 0-8 (ROW1-ROW9)
    int col;           // 0-5 (COL1-COL6)
    uint32_t lvgl_key; // LVGL key code
    char label[16];    // Button label for display
} button_mapping_t;

static button_mapping_t keymap[MAX_BUTTONS];
static int keymap_count = 0;

// Default keymap - you can customize this!
// This is similar to the Windows button_keycodes.txt format
static void load_default_keymap(void) {
    keymap_count = 0;
    
    // Row 0 (ROW1) - Special keys and arrow diamond
    keymap[keymap_count++] = (button_mapping_t){0, 0, 'S', "SHIFT"};      // Shift key
    keymap[keymap_count++] = (button_mapping_t){0, 1, 'A', "ALPHA"};      // Alpha key
    keymap[keymap_count++] = (button_mapping_t){0, 2, LV_KEY_UP, "^"};    // Up arrow
    keymap[keymap_count++] = (button_mapping_t){0, 3, 'F', "F1"};         // Function key
    keymap[keymap_count++] = (button_mapping_t){0, 4, 'M', "MENU"};       // Menu key
    keymap[keymap_count++] = (button_mapping_t){0, 5, 'O', "ON"};         // On/Off key
    
    // Row 1 (ROW2) - Arrow diamond middle and control keys
    keymap[keymap_count++] = (button_mapping_t){1, 0, 'G', "F2"};
    keymap[keymap_count++] = (button_mapping_t){1, 1, LV_KEY_LEFT, "<"};
    keymap[keymap_count++] = (button_mapping_t){1, 2, LV_KEY_RIGHT, ">"};
    keymap[keymap_count++] = (button_mapping_t){1, 3, LV_KEY_DOWN, "v"};
    keymap[keymap_count++] = (button_mapping_t){1, 4, LV_KEY_ENTER, "ENTER"};
    keymap[keymap_count++] = (button_mapping_t){1, 5, LV_KEY_BACKSPACE, "DEL"};
    
    // Row 2 (ROW3) - Math functions (trigonometry)
    keymap[keymap_count++] = (button_mapping_t){2, 0, 's', "sin"};
    keymap[keymap_count++] = (button_mapping_t){2, 1, 'c', "cos"};
    keymap[keymap_count++] = (button_mapping_t){2, 2, 't', "tan"};
    keymap[keymap_count++] = (button_mapping_t){2, 3, 'a', "asin"};
    keymap[keymap_count++] = (button_mapping_t){2, 4, 'l', "log"};
    keymap[keymap_count++] = (button_mapping_t){2, 5, 'e', "e^x"};
    
    // Row 3 (ROW4) - Numbers 7-9 and operations
    keymap[keymap_count++] = (button_mapping_t){3, 0, '7', "7"};
    keymap[keymap_count++] = (button_mapping_t){3, 1, '8', "8"};
    keymap[keymap_count++] = (button_mapping_t){3, 2, '9', "9"};
    keymap[keymap_count++] = (button_mapping_t){3, 3, '/', "÷"};
    keymap[keymap_count++] = (button_mapping_t){3, 4, '*', "×"};
    keymap[keymap_count++] = (button_mapping_t){3, 5, LV_KEY_DEL, "AC"};
    
    // Row 4 (ROW5) - Numbers 4-6 and operations
    keymap[keymap_count++] = (button_mapping_t){4, 0, '4', "4"};
    keymap[keymap_count++] = (button_mapping_t){4, 1, '5', "5"};
    keymap[keymap_count++] = (button_mapping_t){4, 2, '6', "6"};
    keymap[keymap_count++] = (button_mapping_t){4, 3, '-', "-"};
    keymap[keymap_count++] = (button_mapping_t){4, 4, '+', "+"};
    keymap[keymap_count++] = (button_mapping_t){4, 5, '(', "("};
    
    // Row 5 (ROW6) - Numbers 1-3 and operations
    keymap[keymap_count++] = (button_mapping_t){5, 0, '1', "1"};
    keymap[keymap_count++] = (button_mapping_t){5, 1, '2', "2"};
    keymap[keymap_count++] = (button_mapping_t){5, 2, '3', "3"};
    keymap[keymap_count++] = (button_mapping_t){5, 3, '^', "^"};
    keymap[keymap_count++] = (button_mapping_t){5, 4, 'r', "√"};
    keymap[keymap_count++] = (button_mapping_t){5, 5, ')', ")"};
    
    // Row 6 (ROW7) - Number 0, decimal, and operations
    keymap[keymap_count++] = (button_mapping_t){6, 0, '0', "0"};
    keymap[keymap_count++] = (button_mapping_t){6, 1, '.', "."};
    keymap[keymap_count++] = (button_mapping_t){6, 2, 'n', "(-)"};
    keymap[keymap_count++] = (button_mapping_t){6, 3, 'E', "EXP"};
    keymap[keymap_count++] = (button_mapping_t){6, 4, '=', "="};
    keymap[keymap_count++] = (button_mapping_t){6, 5, 'p', "π"};
    
    // Row 7 (ROW8) - Special functions
    keymap[keymap_count++] = (button_mapping_t){7, 0, 'x', "x"};
    keymap[keymap_count++] = (button_mapping_t){7, 1, 'y', "y"};
    keymap[keymap_count++] = (button_mapping_t){7, 2, 'z', "z"};
    keymap[keymap_count++] = (button_mapping_t){7, 3, 'i', "i"};
    keymap[keymap_count++] = (button_mapping_t){7, 4, 'j', "j"};
    keymap[keymap_count++] = (button_mapping_t){7, 5, 'k', "k"};
    
    // Row 8 (ROW9) - Additional functions
    keymap[keymap_count++] = (button_mapping_t){8, 0, LV_KEY_ESC, "ESC"};
    keymap[keymap_count++] = (button_mapping_t){8, 1, LV_KEY_NEXT, "TAB"};
    keymap[keymap_count++] = (button_mapping_t){8, 2, 'h', "HELP"};
    keymap[keymap_count++] = (button_mapping_t){8, 3, 'v', "VAR"};
    keymap[keymap_count++] = (button_mapping_t){8, 4, LV_KEY_HOME, "HOME"};
    keymap[keymap_count++] = (button_mapping_t){8, 5, LV_KEY_END, "END"};
    
    ESP_LOGI(TAG, "Loaded %d button mappings", keymap_count);
}

void mcp23017_keymap_init(void) {
    // For now, just load the default keymap
    // In the future, this could load from a config file in SPIFFS/LittleFS
    load_default_keymap();
}

uint32_t mcp23017_keymap_get_key(int row, int col) {
    for (int i = 0; i < keymap_count; i++) {
        if (keymap[i].row == row && keymap[i].col == col) {
            return keymap[i].lvgl_key;
        }
    }
    return 0; // Not mapped
}

const char* mcp23017_keymap_get_label(int row, int col) {
    for (int i = 0; i < keymap_count; i++) {
        if (keymap[i].row == row && keymap[i].col == col) {
            return keymap[i].label;
        }
    }
    return NULL; // Not mapped
}
