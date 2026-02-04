#include "button_keymap.h"
#include <string.h>

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

// Default keymap - 9 rows x 6 columns
static void load_default_keymap(void) {
    keymap_count = 0;
    
    // Row 1: shift | alpha | < | ^ | menu | on
    keymap[keymap_count++] = (button_mapping_t){0, 0, 'S', "SHIFT"};
    keymap[keymap_count++] = (button_mapping_t){0, 1, 'A', "ALPHA"};
    keymap[keymap_count++] = (button_mapping_t){0, 2, LV_KEY_LEFT, "<"};
    keymap[keymap_count++] = (button_mapping_t){0, 3, LV_KEY_UP, "^"};
    keymap[keymap_count++] = (button_mapping_t){0, 4, 'M', "MENU"};
    keymap[keymap_count++] = (button_mapping_t){0, 5, 'O', "ON"};
    
    // Row 2: var | calc | v | > | const | d/dx
    keymap[keymap_count++] = (button_mapping_t){1, 0, 'V', "VAR"};
    keymap[keymap_count++] = (button_mapping_t){1, 1, 'C', "CALC"};
    keymap[keymap_count++] = (button_mapping_t){1, 2, LV_KEY_DOWN, "v"};
    keymap[keymap_count++] = (button_mapping_t){1, 3, LV_KEY_RIGHT, ">"};
    keymap[keymap_count++] = (button_mapping_t){1, 4, 'K', "CONST"};
    keymap[keymap_count++] = (button_mapping_t){1, 5, 'D', "d/dx"};
    
    // Row 3: frac | sqrt | ^2 | ^x | logab | ln
    keymap[keymap_count++] = (button_mapping_t){2, 0, 'f', "FRAC"};
    keymap[keymap_count++] = (button_mapping_t){2, 1, 'r', "√"};
    keymap[keymap_count++] = (button_mapping_t){2, 2, '2', "x²"};
    keymap[keymap_count++] = (button_mapping_t){2, 3, '^', "^"};
    keymap[keymap_count++] = (button_mapping_t){2, 4, 'L', "logₐb"};
    keymap[keymap_count++] = (button_mapping_t){2, 5, 'l', "ln"};
    
    // Row 4: ! | ( | ) | sin | cos | tan
    keymap[keymap_count++] = (button_mapping_t){3, 0, '!', "!"};
    keymap[keymap_count++] = (button_mapping_t){3, 1, '(', "("};
    keymap[keymap_count++] = (button_mapping_t){3, 2, ')', ")"};
    keymap[keymap_count++] = (button_mapping_t){3, 3, 's', "sin"};
    keymap[keymap_count++] = (button_mapping_t){3, 4, 'c', "cos"};
    keymap[keymap_count++] = (button_mapping_t){3, 5, 't', "tan"};
    
    // Row 5: A | B | C | D | E | F
    keymap[keymap_count++] = (button_mapping_t){4, 0, 'A', "A"};
    keymap[keymap_count++] = (button_mapping_t){4, 1, 'B', "B"};
    keymap[keymap_count++] = (button_mapping_t){4, 2, 'C', "C"};
    keymap[keymap_count++] = (button_mapping_t){4, 3, 'D', "D"};
    keymap[keymap_count++] = (button_mapping_t){4, 4, 'E', "E"};
    keymap[keymap_count++] = (button_mapping_t){4, 5, 'F', "F"};
    
    // Row 6: X | 7 | 8 | 9 | DEL | AC
    keymap[keymap_count++] = (button_mapping_t){5, 0, 'X', "X"};
    keymap[keymap_count++] = (button_mapping_t){5, 1, '7', "7"};
    keymap[keymap_count++] = (button_mapping_t){5, 2, '8', "8"};
    keymap[keymap_count++] = (button_mapping_t){5, 3, '9', "9"};
    keymap[keymap_count++] = (button_mapping_t){5, 4, LV_KEY_BACKSPACE, "DEL"};
    keymap[keymap_count++] = (button_mapping_t){5, 5, LV_KEY_DEL, "AC"};
    
    // Row 7: Y | 4 | 5 | 6 | x | /
    keymap[keymap_count++] = (button_mapping_t){6, 0, 'Y', "Y"};
    keymap[keymap_count++] = (button_mapping_t){6, 1, '4', "4"};
    keymap[keymap_count++] = (button_mapping_t){6, 2, '5', "5"};
    keymap[keymap_count++] = (button_mapping_t){6, 3, '6', "6"};
    keymap[keymap_count++] = (button_mapping_t){6, 4, '*', "×"};
    keymap[keymap_count++] = (button_mapping_t){6, 5, '/', "÷"};
    
    // Row 8: Z | 1 | 2 | 3 | + | -
    keymap[keymap_count++] = (button_mapping_t){7, 0, 'Z', "Z"};
    keymap[keymap_count++] = (button_mapping_t){7, 1, '1', "1"};
    keymap[keymap_count++] = (button_mapping_t){7, 2, '2', "2"};
    keymap[keymap_count++] = (button_mapping_t){7, 3, '3', "3"};
    keymap[keymap_count++] = (button_mapping_t){7, 4, '+', "+"};
    keymap[keymap_count++] = (button_mapping_t){7, 5, '-', "-"};
    
    // Row 9: e | 0 | . | pi | Ans | =
    keymap[keymap_count++] = (button_mapping_t){8, 0, 'e', "e"};
    keymap[keymap_count++] = (button_mapping_t){8, 1, '0', "0"};
    keymap[keymap_count++] = (button_mapping_t){8, 2, '.', "."};
    keymap[keymap_count++] = (button_mapping_t){8, 3, 'p', "π"};
    keymap[keymap_count++] = (button_mapping_t){8, 4, 'a', "Ans"};
    keymap[keymap_count++] = (button_mapping_t){8, 5, LV_KEY_ENTER, "="};
}

void button_keymap_init(void) {
    load_default_keymap();
}

uint32_t button_keymap_get_key(int row, int col) {
    for (int i = 0; i < keymap_count; i++) {
        if (keymap[i].row == row && keymap[i].col == col) {
            return keymap[i].lvgl_key;
        }
    }
    return 0; // Not mapped
}

const char* button_keymap_get_label(int row, int col) {
    for (int i = 0; i < keymap_count; i++) {
        if (keymap[i].row == row && keymap[i].col == col) {
            return keymap[i].label;
        }
    }
    return NULL; // Not mapped
}
