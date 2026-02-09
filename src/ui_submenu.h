#pragma once

#include "lvgl.h"

/*──────────────────────────────────────────────────────────────
 *  Reusable submenu UI component
 *  
 *  Provides a vertical list-based menu for app submodes.
 *  Used by solver, stats, and potentially other apps.
 *──────────────────────────────────────────────────────────────*/

/* Menu item definition */
typedef struct {
    const char *text;         /* Display text for the menu item */
    void (*callback)(void);   /* Function to call when selected */
} SubMenuItem;

/* Styling configuration for the submenu */
typedef struct {
    lv_color_t accent_color;   /* Accent color for the app */
    lv_color_t focus_bg_color; /* Background color when focused */
    const char *hint_text;     /* Text to show in hint bar */
} SubMenuStyle;

/*──────────────────────────────────────────────────────────────
 *  Creates and displays a submenu screen
 *  
 *  Parameters:
 *    items       - Array of menu items
 *    item_count  - Number of items in the array
 *    style       - Style configuration (colors, hints)
 *    on_menu     - Callback when 'M' key is pressed
 *  
 *  Returns:
 *    The lv_group_t* for the submenu (caller owns, must clean up)
 *  
 *  Navigation:
 *    Up/Down - Navigate between items
 *    Enter   - Select highlighted item
 *    M       - Call on_menu callback
 *──────────────────────────────────────────────────────────────*/
lv_group_t* ui_create_submenu(
    const SubMenuItem *items,
    int item_count,
    const SubMenuStyle *style,
    void (*on_menu)(void)
);

/*──────────────────────────────────────────────────────────────
 *  Cleanup function - deletes the submenu group
 *  Call this before transitioning to another screen
 *──────────────────────────────────────────────────────────────*/
void ui_submenu_cleanup(lv_group_t *group);
