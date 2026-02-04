#ifndef MCP23017_KEYMAP_H
#define MCP23017_KEYMAP_H

#include <stdint.h>
#include "lvgl.h"

/**
 * @brief Initialize the keymap from configuration file
 */
void mcp23017_keymap_init(void);

/**
 * @brief Get the LVGL key code for a button at row, col
 * 
 * @param row Button row (0-8, corresponding to ROW1-ROW9)
 * @param col Button column (0-5, corresponding to COL1-COL6)
 * @return uint32_t LVGL key code, or 0 if not mapped
 */
uint32_t mcp23017_keymap_get_key(int row, int col);

/**
 * @brief Get a human-readable label for a button
 * 
 * @param row Button row (0-8)
 * @param col Button column (0-5)
 * @return const char* Button label, or NULL if not mapped
 */
const char* mcp23017_keymap_get_label(int row, int col);

#endif // MCP23017_KEYMAP_H
