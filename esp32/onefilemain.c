#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include <stdio.h>

#include "esp_lcd_ili9341.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#include "esp_lvgl_port.h"
#include "lvgl.h"

// ----- Adjust these to your wiring -----
#define LCD_HOST SPI2_HOST

// XIAO ESP32S3 default SPI pins (per Seeed docs)
#define PIN_NUM_SCLK 7 // D8
#define PIN_NUM_MOSI 9 // D10
// MISO not used by ILI9341
// #define PIN_NUM_MISO        8   // D9 (if needed, wire it & add to bus
// config)

// Control pins you chose above
#define PIN_NUM_DC 6  // D1
#define PIN_NUM_RST 5 // D4; use -1 if your board ties reset
#define PIN_NUM_CS 4  // D0
#define PIN_NUM_BL -1 // D3; set to -1 if backlight hardwired

// Panel resolution (portrait default for most 2.8” ILI9341)
#define LCD_H_RES 320
#define LCD_V_RES 240

static const char *TAG = "APP";

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
  gpio_set_level(PIN_NUM_BL, 1); // Full brightness (or drive PWM if needed)
#endif
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    ESP_LOGI("LVGL", "Button clicked!");
    lv_label_set_text(obj, "Clicked!");
  }
}

void app_main(void) {
  ESP_LOGI(TAG, "Initialize SPI bus");
  spi_bus_config_t buscfg = ILI9341_PANEL_BUS_SPI_CONFIG(
      PIN_NUM_SCLK,                     // SCLK
      PIN_NUM_MOSI,                     // MOSI
      LCD_H_RES * 40 * sizeof(uint16_t) // Max transfer bytes (tune if needed)
  );
  ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

  ESP_LOGI(TAG, "Create panel IO (SPI)");
  esp_lcd_panel_io_handle_t io_handle = NULL;
  const esp_lcd_panel_io_spi_config_t io_config = ILI9341_PANEL_IO_SPI_CONFIG(
      PIN_NUM_CS, // CS
      PIN_NUM_DC, // DC
      NULL,       // no on_color_trans_done callback (LVGL port handles sync)
      NULL);

  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST,
                                           &io_config, &io_handle));

  ESP_LOGI(TAG, "Install ILI9341 panel driver");
  esp_lcd_panel_handle_t panel_handle = NULL;
  const esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = PIN_NUM_RST, // -1 if not used
      .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
      .bits_per_pixel = 16, // RGB565
  };
  ESP_ERROR_CHECK(
      esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));

  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

  // Rotate to landscape (ILI9341 is rotated by swapping X/Y and mirroring Y)
  ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
  ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));

  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

  backlight_on();

  ESP_LOGI(TAG, "Init LVGL port");
  const lvgl_port_cfg_t lvgl_cfg =
      ESP_LVGL_PORT_INIT_CONFIG(); // creates LVGL task/timer for you
  ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

  ESP_LOGI(TAG, "Add LVGL display");
  const lvgl_port_display_cfg_t disp_cfg = {
      .io_handle = io_handle,
      .panel_handle = panel_handle,
      .buffer_size = LCD_H_RES * 40, // tune as needed
      .double_buffer = true,
      .hres = LCD_H_RES,
      .vres = LCD_V_RES,
      .monochrome = false,
      // .color_format = LV_COLOR_FORMAT_RGB565,  // keep if available in your
      // version; otherwise comment out
      .flags = {
          .buff_dma = true,
          .swap_bytes = true,
      }};

  lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);
  (void)disp;

  // Now rotate panel (this order matters)
  ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
  ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true,
                                       true)); // if mirrored, try (false, true)

  lvgl_port_lock(0);
  lv_obj_invalidate(lv_screen_active()); // force redraw
  lvgl_port_unlock();

  ESP_LOGI(TAG, "Create app grid");
  lvgl_port_lock(0);

  // Screen container
  lv_obj_t *scr = lv_screen_active();

  // Basic layout parameters
  int cols = 3;
  int rows = 2;
  int icon_w = 90;
  int icon_h = 90;
  int spacing_x = 15;
  int spacing_y = 15;
  int start_x = (LCD_H_RES - (cols * icon_w + (cols - 1) * spacing_x)) / 2;
  int start_y = (LCD_V_RES - (rows * icon_h + (rows - 1) * spacing_y)) / 2;

  // App names and colors
  const char *names[6] = {
      "Math", "Graph", "Stats", "Numerical\n Methods", "Mechanics", "Matrices"};
  lv_color_t colors[6] = {
      lv_palette_main(LV_PALETTE_BLUE),   lv_palette_main(LV_PALETTE_RED),
      lv_palette_main(LV_PALETTE_GREEN),  lv_palette_main(LV_PALETTE_ORANGE),
      lv_palette_main(LV_PALETTE_PURPLE), lv_palette_main(LV_PALETTE_RED),
  };

  // Create icons
  for (int i = 0; i < 6; i++) {
    int col = i % cols;
    int row = i / cols;

    lv_obj_t *icon = lv_obj_create(scr);
    lv_obj_set_size(icon, icon_w, icon_h);
    lv_obj_set_pos(icon, start_x + col * (icon_w + spacing_x),
                   start_y + row * (icon_h + spacing_y));

    lv_obj_set_style_radius(icon, 15, LV_PART_MAIN);
    lv_obj_set_style_bg_color(icon, colors[i], LV_PART_MAIN);
    lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(icon);
    lv_label_set_text(label, names[i]);
    lv_obj_center(label);
  }

  lvgl_port_unlock();

  ESP_LOGI(TAG,
           "Done. LVGL task runs in background. Nothing to do in app_main.");
}
