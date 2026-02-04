#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include "esp_err.h"
#include "lvgl.h"

// -----------------------------------------------------------------------------
// Initializes the button driver
// - Configures all GPIOs from button_config.c
// - Creates an LVGL keypad input device
// -----------------------------------------------------------------------------
esp_err_t button_driver_init(lv_disp_t *disp);

// -----------------------------------------------------------------------------
// LVGL calls this function to read button states
// Do not call manually
// -----------------------------------------------------------------------------
void button_driver_read_cb(lv_indev_t *indev, lv_indev_data_t *data);

// -----------------------------------------------------------------------------
// Returns the LVGL input device handle
// Use this in main_menu.c to attach the device to a group
// -----------------------------------------------------------------------------
lv_indev_t *button_driver_get_indev(void);

#endif
