#include "driver.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"

#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#include "esp_lvgl_port.h"

// Pins and config
#define LCD_HOST SPI2_HOST
#define PIN_NUM_SCLK 7
#define PIN_NUM_MOSI 9
#define PIN_NUM_DC 3
#define PIN_NUM_RST 2
#define PIN_NUM_CS 1
#define PIN_NUM_BL -1

#define LCD_H_RES 320
#define LCD_V_RES 240

static const char *TAG = "DRIVER";

static esp_lcd_panel_handle_t panel_handle = NULL;
static lv_disp_t *disp_handle = NULL;

static void backlight_on(void) {
#if PIN_NUM_BL >= 0
  gpio_config_t io_conf = {
      .pin_bit_mask = 1ULL << PIN_NUM_BL,
      .mode = GPIO_MODE_OUTPUT,
      .pull_down_en = 0,
      .pull_up_en = 0,
      .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&io_conf);
  gpio_set_level(PIN_NUM_BL, 1);
#endif
}

void driver_init(void) {
  ESP_LOGI(TAG, "Initialize SPI bus");

  spi_bus_config_t buscfg = {
      .sclk_io_num = PIN_NUM_SCLK,
      .mosi_io_num = PIN_NUM_MOSI,
      .miso_io_num = -1,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = LCD_H_RES * 40 * sizeof(uint16_t),
  };
  ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

  ESP_LOGI(TAG, "Create panel IO");

  esp_lcd_panel_io_handle_t io_handle = NULL;
  esp_lcd_panel_io_spi_config_t io_config = {
      .cs_gpio_num = PIN_NUM_CS,
      .dc_gpio_num = PIN_NUM_DC,
      .spi_mode = 0,
      .pclk_hz = 40 * 1000 * 1000,  // 40 MHz
      .trans_queue_depth = 10,
      .lcd_cmd_bits = 8,
      .lcd_param_bits = 8,
  };

  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST,
                                           &io_config, &io_handle));

  ESP_LOGI(TAG, "Install ST7789 panel");

  const esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = PIN_NUM_RST,
      .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
      .bits_per_pixel = 16,
  };

  ESP_ERROR_CHECK(
      esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

  backlight_on();

  ESP_LOGI(TAG, "Init LVGL port");

  const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
  ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

  ESP_LOGI(TAG, "Add LVGL display");

  const lvgl_port_display_cfg_t disp_cfg = {.io_handle = io_handle,
                                            .panel_handle = panel_handle,
                                            .buffer_size = LCD_H_RES * 40,
                                            .double_buffer = true,
                                            .hres = LCD_H_RES,
                                            .vres = LCD_V_RES,
                                            .monochrome = false,
                                            .flags = {
                                                .buff_dma = true,
                                                .swap_bytes = true,
                                            }};

  disp_handle = lvgl_port_add_disp(&disp_cfg);

  // Rotate display to landscape mode (320x240)
  ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
  ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));

  lvgl_port_lock(0);
  lv_obj_invalidate(lv_screen_active());
  lvgl_port_unlock();
}

lv_disp_t *driver_get_display(void) { return disp_handle; }
