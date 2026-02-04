# MCP23017 Button Matrix Configuration
# This file documents the button keymap for the ESP32 calculator
# Edit mcp23017_keymap.c to change the button mappings

## Hardware Configuration

### MCP23017 I2C Address: 0x20

### Pin Connections:
- **I2C SDA**: GPIO 10
- **I2C SCL**: GPIO 8
- **I2C Speed**: 400 kHz

### MCP23017 Pin Mapping:

#### Port B (GPIOB):
- GPB0 (Pin 1)  → COL6
- GPB1 (Pin 2)  → COL5
- GPB2 (Pin 3)  → COL4
- GPB3 (Pin 4)  → COL3
- GPB4 (Pin 5)  → COL2
- GPB5 (Pin 6)  → COL1
- GPB6 (Pin 7)  → ROW1
- GPB7 (Pin 8)  → ROW2

#### Port A (GPIOA):
- GPA0 (Pin 21) → ROW9
- GPA1 (Pin 22) → ROW8
- GPA2 (Pin 23) → ROW7
- GPA3 (Pin 24) → ROW6
- GPA4 (Pin 25) → ROW5
- GPA5 (Pin 26) → ROW4
- GPA6 (Pin 27) → ROW3
- GPA7 (Pin 28) → Not connected

## Button Matrix Layout (9 rows × 6 columns)

### Default Keymap:

```
ROW1: [SHIFT] [ALPHA] [  ↑  ] [ F1  ] [MENU ] [ ON  ]
ROW2: [ F2  ] [  ←  ] [  →  ] [  ↓  ] [ENTER] [ DEL ]
ROW3: [ sin ] [ cos ] [ tan ] [asin ] [ log ] [ e^x ]
ROW4: [  7  ] [  8  ] [  9  ] [  ÷  ] [  ×  ] [ AC  ]
ROW5: [  4  ] [  5  ] [  6  ] [  -  ] [  +  ] [  (  ]
ROW6: [  1  ] [  2  ] [  3  ] [  ^  ] [  √  ] [  )  ]
ROW7: [  0  ] [  .  ] [ (-) ] [ EXP ] [  =  ] [  π  ]
ROW8: [  x  ] [  y  ] [  z  ] [  i  ] [  j  ] [  k  ]
ROW9: [ ESC ] [ TAB ] [HELP ] [ VAR ] [HOME ] [ END ]
```

## Customizing the Keymap

To customize button assignments, edit the `load_default_keymap()` function in `mcp23017_keymap.c`.

Each button is defined as:
```c
keymap[keymap_count++] = (button_mapping_t){
    row,        // 0-8 (ROW1-ROW9)
    col,        // 0-5 (COL1-COL6)
    lvgl_key,   // LVGL key code (e.g., LV_KEY_ENTER, '7', '+')
    "label"     // Display label (max 15 chars)
};
```

### Available LVGL Key Codes:
- **Navigation**: `LV_KEY_UP`, `LV_KEY_DOWN`, `LV_KEY_LEFT`, `LV_KEY_RIGHT`
- **Control**: `LV_KEY_ENTER`, `LV_KEY_ESC`, `LV_KEY_BACKSPACE`, `LV_KEY_DEL`
- **Special**: `LV_KEY_HOME`, `LV_KEY_END`, `LV_KEY_NEXT` (Tab)
- **Characters**: `'0'-'9'`, `'a'-'z'`, `'A'-'Z'`, `'+'`, `'-'`, `'*'`, `'/'`, etc.

## Usage

1. Initialize the button driver in your main.c:
   ```c
   #include "mcp23017_button_driver.h"
   
   esp_err_t ret = mcp23017_button_init(disp_handle);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to initialize MCP23017 buttons");
   }
   ```

2. Get the input device handle:
   ```c
   lv_indev_t *button_indev = mcp23017_button_get_indev();
   ```

3. Assign to LVGL group:
   ```c
   lv_group_t *group = lv_group_create();
   lv_indev_set_group(button_indev, group);
   ```

## Troubleshooting

### No buttons responding:
- Check I2C connections (SDA=GPIO10, SCL=GPIO8)
- Verify MCP23017 address is 0x20
- Check power supply to MCP23017 (3.3V)
- Use `i2cdetect` to verify device is detected

### Wrong buttons triggering:
- Check row/column wiring matches the pin mapping above
- Verify matrix connections are correct
- Edit keymap in `mcp23017_keymap.c` if physical layout differs

### Multiple buttons detected:
- Add delay between column scans (already 1ms by default)
- Check for shorts in button matrix wiring
- Verify pull-up resistors are enabled (done in driver)

## Technical Details

### Scanning Method:
- Active-low matrix scanning
- Columns are outputs (driven low one at a time)
- Rows are inputs with pull-ups enabled
- Scan rate: ~1ms per column = ~6ms full scan
- Debouncing: Handled by LVGL input device driver

### Power Consumption:
- Idle: ~1mA (pull-ups active)
- Active scan: ~2-3mA
- Per button press: <1mA additional
