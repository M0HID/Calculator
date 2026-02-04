#include "mcp23017_button_driver.h"
#include "button_keymap.h"

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "MCP23017_BTN";

// MCP23017 register addresses
#define MCP23017_IODIRA   0x00  // I/O direction register A
#define MCP23017_IODIRB   0x01  // I/O direction register B
#define MCP23017_GPPUA    0x0C  // Pull-up resistor register A
#define MCP23017_GPPUB    0x0D  // Pull-up resistor register B
#define MCP23017_GPIOA    0x12  // Port A register
#define MCP23017_GPIOB    0x13  // Port B register

// I2C configuration
#define I2C_MASTER_SCL_IO           6       // GPIO for I2C SCL
#define I2C_MASTER_SDA_IO           5       // GPIO for I2C SDA
#define I2C_MASTER_FREQ_HZ          100000  // I2C clock frequency (100kHz for reliability)
#define I2C_TIMEOUT_MS              1000    // I2C operation timeout
#define MCP23017_I2C_ADDR           0x20    // Default I2C address

// Matrix configuration based on your wiring:
// Port B (GPIOB): GPB0-GPB5 = COL6-COL1, GPB6-GPB7 = ROW1-ROW2
// Port A (GPIOA): GPA0-GPA6 = ROW9-ROW3
#define MATRIX_ROWS 9
#define MATRIX_COLS 6

static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static i2c_master_dev_handle_t mcp23017_handle = NULL;
static lv_indev_t *indev_handle = NULL;
static uint32_t last_key = 0;

// Write to MCP23017 register
static esp_err_t mcp23017_write_reg(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    return i2c_master_transmit(mcp23017_handle, data, 2, I2C_TIMEOUT_MS);
}

// Read from MCP23017 register
static esp_err_t mcp23017_read_reg(uint8_t reg, uint8_t *value) {
    esp_err_t ret = i2c_master_transmit(mcp23017_handle, &reg, 1, I2C_TIMEOUT_MS);
    if (ret != ESP_OK) {
        return ret;
    }
    return i2c_master_receive(mcp23017_handle, value, 1, I2C_TIMEOUT_MS);
}

// Scan the button matrix and return pressed row/col
static bool scan_matrix(int *pressed_row, int *pressed_col) {
    // Port B pin mapping:
    // GPB0-GPB5: COL6-COL1 (columns, now inputs with pull-ups)
    // GPB6-GPB7: ROW1-ROW2 (rows, now outputs)
    
    // Port A pin mapping:
    // GPA0-GPA6: ROW9-ROW3 (rows, now outputs)
    // GPA7: unused
    
    // DIODE ORIENTATION: Column-to-Row (cathode on column, anode on row)
    // This means we drive ROWS low and read COLUMNS
    
    // Scan each row
    for (int row = 0; row < MATRIX_ROWS; row++) {
        // Set all rows high, then drive one row low
        uint8_t port_a_val = 0xFF;
        uint8_t port_b_val = 0xFF;
        
        if (row < 2) {
            // ROW1 or ROW2 on Port B (GPB6, GPB7)
            int physical_row_b = 6 + row;  // ROW1 = GPB6, ROW2 = GPB7
            port_b_val &= ~(1 << physical_row_b);  // Set this row low
        } else {
            // ROW3-ROW9 on Port A (GPA6-GPA0)
            int physical_row_a = 8 - row;  // ROW3 = GPA6, ROW4 = GPA5, ..., ROW9 = GPA0
            port_a_val &= ~(1 << physical_row_a);  // Set this row low
        }
        
        // Write to both ports
        if (mcp23017_write_reg(MCP23017_GPIOA, port_a_val) != ESP_OK) {
            return false;
        }
        if (mcp23017_write_reg(MCP23017_GPIOB, port_b_val) != ESP_OK) {
            return false;
        }
        
        // Small delay for signal stabilization
        vTaskDelay(pdMS_TO_TICKS(1));
        
        // Read Port B columns (COL6-COL1: GPB0-GPB5)
        uint8_t port_b;
        if (mcp23017_read_reg(MCP23017_GPIOB, &port_b) != ESP_OK) {
            return false;
        }
        
        // Check each column
        for (int col = 0; col < MATRIX_COLS; col++) {
            // COL1 = GPB5, COL2 = GPB4, ..., COL6 = GPB0
            int physical_col = 5 - col;
            
            if (!(port_b & (1 << physical_col))) {  // Active low
                *pressed_row = row;
                *pressed_col = col;
                return true;
            }
        }
    }
    
    return false;
}

// LVGL read callback
static void mcp23017_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    (void)indev;
    
    static int last_pressed_row = -1;
    static int last_pressed_col = -1;
    static bool was_pressed = false;
    
    int pressed_row = -1;
    int pressed_col = -1;
    bool pressed = scan_matrix(&pressed_row, &pressed_col);
    
    if (pressed) {
        // Look up the key in the keymap
        uint32_t key = button_keymap_get_key(pressed_row, pressed_col);
        if (key != 0) {
            data->key = key;
            data->state = LV_INDEV_STATE_PRESSED;
            last_key = key;
            
            // Only log if this is a new press (not a repeat)
            if (!was_pressed || pressed_row != last_pressed_row || pressed_col != last_pressed_col) {
                const char* label = button_keymap_get_label(pressed_row, pressed_col);
                if (label) {
                    ESP_LOGI(TAG, "Button pressed: [%s] ROW%d COL%d (0x%02lX)", 
                             label, pressed_row + 1, pressed_col + 1, key);
                } else {
                    ESP_LOGI(TAG, "Button pressed: ROW%d COL%d (0x%02lX - unmapped)", 
                             pressed_row + 1, pressed_col + 1, key);
                }
            }
            
            was_pressed = true;
            last_pressed_row = pressed_row;
            last_pressed_col = pressed_col;
        } else {
            data->key = last_key;
            data->state = LV_INDEV_STATE_RELEASED;
            was_pressed = false;
        }
    } else {
        data->key = last_key;
        data->state = LV_INDEV_STATE_RELEASED;
        
        // Log button release
        if (was_pressed) {
            const char* label = button_keymap_get_label(last_pressed_row, last_pressed_col);
            if (label) {
                ESP_LOGI(TAG, "Button released: [%s]", label);
            }
        }
        
        was_pressed = false;
    }
}

lv_indev_t *mcp23017_button_get_indev(void) {
    return indev_handle;
}

esp_err_t mcp23017_button_init(lv_disp_t *disp) {
    if (!disp) {
        ESP_LOGE(TAG, "Null display");
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Initializing MCP23017 button driver");
    
    // Initialize I2C master bus
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    esp_err_t ret = i2c_new_master_bus(&bus_config, &i2c_bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C bus: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "I2C bus initialized on SDA=GPIO%d, SCL=GPIO%d at %d Hz", 
             I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ);
    
    // Add MCP23017 device to the bus
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MCP23017_I2C_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    
    ret = i2c_master_bus_add_device(i2c_bus_handle, &dev_config, &mcp23017_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add MCP23017 device: %s", esp_err_to_name(ret));
        i2c_del_master_bus(i2c_bus_handle);
        return ret;
    }
    
    ESP_LOGI(TAG, "MCP23017 device added at address 0x%02X", MCP23017_I2C_ADDR);
    
    // Small delay to let device stabilize
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // Test communication by reading IODIRA register (should be 0xFF by default)
    uint8_t test_read;
    ret = mcp23017_read_reg(MCP23017_IODIRA, &test_read);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to communicate with MCP23017 at 0x%02X: %s", 
                 MCP23017_I2C_ADDR, esp_err_to_name(ret));
        ESP_LOGE(TAG, "Possible issues:");
        ESP_LOGE(TAG, "  1. Check physical I2C connections (SDA=GPIO%d, SCL=GPIO%d)", 
                 I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
        ESP_LOGE(TAG, "  2. Verify MCP23017 address pins (A0, A1, A2) for 0x%02X", 
                 MCP23017_I2C_ADDR);
        ESP_LOGE(TAG, "  3. Add external 4.7kΩ pull-up resistors on SDA/SCL");
        ESP_LOGE(TAG, "  4. Check MCP23017 power (VDD) and RESET pin (must be HIGH)");
        i2c_master_bus_rm_device(mcp23017_handle);
        i2c_del_master_bus(i2c_bus_handle);
        return ret;
    }
    ESP_LOGI(TAG, "MCP23017 communication test OK (IODIRA = 0x%02X)", test_read);
    
    // Configure MCP23017
    // REVERSED for column-to-row diodes:
    // Port B: GPB0-GPB5 (columns) = inputs with pull-ups, GPB6-GPB7 (rows) = outputs
    // Port A: GPA0-GPA6 (rows) = outputs, GPA7 = unused output
    
    // Set Port A as all outputs (rows)
    ret = mcp23017_write_reg(MCP23017_IODIRA, 0x00);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure Port A direction");
        return ret;
    }
    
    // Set Port B: bits 0-5 as inputs (columns), bits 6-7 as outputs (rows)
    ret = mcp23017_write_reg(MCP23017_IODIRB, 0x3F);  // 00111111
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure Port B direction");
        return ret;
    }
    
    // Enable pull-ups on Port B column inputs (bits 0-5)
    ret = mcp23017_write_reg(MCP23017_GPPUB, 0x3F);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure Port B pull-ups");
        return ret;
    }
    
    // Set all rows high initially
    ret = mcp23017_write_reg(MCP23017_GPIOA, 0xFF);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set initial Port A state");
        return ret;
    }
    ret = mcp23017_write_reg(MCP23017_GPIOB, 0xFF);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set initial Port B state");
        return ret;
    }
    
    // Load keymap configuration
    button_keymap_init();
    
    // Create LVGL input device
    indev_handle = lv_indev_create();
    if (!indev_handle) {
        ESP_LOGE(TAG, "Failed to create LVGL input device");
        return ESP_FAIL;
    }
    
    lv_indev_set_type(indev_handle, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev_handle, mcp23017_read_cb);
    lv_indev_set_display(indev_handle, disp);
    
    ESP_LOGI(TAG, "MCP23017 button driver initialized successfully");
    return ESP_OK;
}
