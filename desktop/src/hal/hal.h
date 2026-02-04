/**
 * @file hal.h
 *
 */

#ifndef LV_VSCODE_HAL_H
#define LV_VSCODE_HAL_H

#include "lvgl/lvgl.h"
#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
lv_display_t * sdl_hal_init(int32_t w, int32_t h);

/**
 * Store keyboard input device for use by input_hal
 */
void store_keyboard_indev(lv_indev_t *kb);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VSCODE_HAL_H*/
