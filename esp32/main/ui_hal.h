#ifndef UI_HAL_H
#define UI_HAL_H

#include "lvgl.h"

// Platform-agnostic locking
void ui_lock(void);
void ui_unlock(void);

// Platform-agnostic input device discovery
lv_indev_t *ui_get_indev(void);

#endif