#include "input_hal.h"
#include "hal/hal.h"

static lv_indev_t *kb_indev = NULL;

void store_keyboard_indev(lv_indev_t *kb) {
  kb_indev = kb;
}

lv_indev_t *get_navigation_indev(void) {
  return kb_indev;
}

void lvgl_lock(void) {
  // No-op for single-threaded PC application
}

void lvgl_unlock(void) {
  // No-op for single-threaded PC application
}
