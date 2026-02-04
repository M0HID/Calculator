#include "input_hal.h"
#include "mcp23017_button_driver.h"
#include "esp_lvgl_port.h"

lv_indev_t *get_navigation_indev(void) {
  return mcp23017_button_get_indev();
}

void lvgl_lock(void) {
  lvgl_port_lock(0);
}

void lvgl_unlock(void) {
  lvgl_port_unlock();
}
