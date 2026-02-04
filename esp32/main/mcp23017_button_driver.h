#ifndef MCP23017_BUTTON_DRIVER_H
#define MCP23017_BUTTON_DRIVER_H

#include "esp_err.h"
#include "lvgl.h"

/**
 * @brief Initialize MCP23017 button matrix driver
 * 
 * @param disp LVGL display handle
 * @return esp_err_t ESP_OK on success
 */
esp_err_t mcp23017_button_init(lv_disp_t *disp);

/**
 * @brief Get the LVGL input device handle
 * 
 * @return lv_indev_t* Input device handle
 */
lv_indev_t *mcp23017_button_get_indev(void);

#endif // MCP23017_BUTTON_DRIVER_H
