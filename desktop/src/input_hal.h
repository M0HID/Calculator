#ifndef INPUT_HAL_H
#define INPUT_HAL_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

lv_indev_t *get_navigation_indev(void);
void lvgl_lock(void);
void lvgl_unlock(void);

#ifdef __cplusplus
}
#endif

#endif
