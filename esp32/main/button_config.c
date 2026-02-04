#include "button_config.h"

const button_config_t button_configs[BUTTON_CONFIG_COUNT] = {
    {GPIO_NUM_4, LV_KEY_UP, "UP", 0},
    {GPIO_NUM_5, LV_KEY_DOWN, "DOWN", 0},
    {GPIO_NUM_6, LV_KEY_LEFT, "LEFT", 0},
    {GPIO_NUM_43, LV_KEY_RIGHT, "RIGHT", 0},
    {GPIO_NUM_44, LV_KEY_ENTER, "SELECT", 0} // new button
};
