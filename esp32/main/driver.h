#pragma once
#include "esp_lcd_panel_interface.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void driver_init(void);
lv_disp_t *driver_get_display(void);

#ifdef __cplusplus
}
#endif
