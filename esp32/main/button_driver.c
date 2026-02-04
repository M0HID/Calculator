#include "button_driver.h"
#include "button_config.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"

static const char *TAG = "BTN_DRV";

// Store the LVGL input device object
static lv_indev_t *indev_handle = NULL;

// Optional: last key pressed, to satisfy LVGL when released
static uint32_t last_key = 0;

void button_driver_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
  (void)indev;

  bool any_pressed = false;
  uint32_t pressed_key = 0;

  for (int i = 0; i < BUTTON_CONFIG_COUNT; i++) {
    int lvl = gpio_get_level(button_configs[i].gpio);
    bool pressed = (lvl == button_configs[i].active_level);
    if (pressed) {
      any_pressed = true;
      pressed_key = button_configs[i].lvgl_key;
      last_key = pressed_key;
      break;
    }
  }

  if (any_pressed) {
    data->key = pressed_key;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->key = last_key;
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

lv_indev_t *button_driver_get_indev(void) { return indev_handle; }

esp_err_t button_driver_init(lv_disp_t *disp) {
  if (!disp) {
    ESP_LOGE(TAG, "Null display");
    return ESP_ERR_INVALID_ARG;
  }

  // Configure GPIO pins
  uint64_t mask = 0;
  for (int i = 0; i < BUTTON_CONFIG_COUNT; i++) {
    mask |= (1ULL << button_configs[i].gpio);
  }
  gpio_config_t io = {.pin_bit_mask = mask,
                      .mode = GPIO_MODE_INPUT,
                      .pull_up_en = GPIO_PULLUP_ENABLE,
                      .pull_down_en = GPIO_PULLDOWN_DISABLE,
                      .intr_type = GPIO_INTR_DISABLE};
  if (gpio_config(&io) != ESP_OK) {
    ESP_LOGE(TAG, "GPIO config failed");
    return ESP_FAIL;
  }

  // Create and configure LVGL input device
  indev_handle = lv_indev_create();
  if (!indev_handle) {
    ESP_LOGE(TAG, "lv_indev_create failed");
    return ESP_FAIL;
  }

  lv_indev_set_type(indev_handle, LV_INDEV_TYPE_KEYPAD);
  lv_indev_set_read_cb(indev_handle, button_driver_read_cb);
  lv_indev_set_group(indev_handle,
                     NULL); // you can set group later in main menu

  ESP_LOGI(TAG, "Button driver initialized");
  return ESP_OK;
}
