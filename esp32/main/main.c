#include "driver.h"
#include "main_menu.h"
#include "mcp23017_button_driver.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

void app_main(void) {
  // Initialize display
  driver_init();
  
  // Initialize MCP23017 button matrix
  lv_disp_t *disp = driver_get_display();
  esp_err_t ret = mcp23017_button_init(disp);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize MCP23017 button driver: %s", esp_err_to_name(ret));
  } else {
    ESP_LOGI(TAG, "MCP23017 button driver initialized successfully");
  }
  
  // Create main menu UI
  main_menu_create();
}
