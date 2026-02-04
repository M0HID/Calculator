#pragma once

#include "driver/gpio.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Button configuration structure
 * Maps a GPIO pin to an LVGL keypad key
 */
typedef struct {
  gpio_num_t gpio;   // GPIO pin number
  uint32_t lvgl_key; // LVGL key code (LV_KEY_UP, LV_KEY_DOWN, etc.)
  const char *name;  // Button name for debugging
  int active_level;  // Active level (0 for active LOW, 1 for active HIGH)
} button_config_t;

// Number of buttons configured
#define BUTTON_CONFIG_COUNT 5

// Button configuration array - define your GPIO pins here
// TODO: Update GPIO pin numbers to match your hardware
extern const button_config_t button_configs[BUTTON_CONFIG_COUNT];

#ifdef __cplusplus
}
#endif
