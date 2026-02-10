idf_component_register(
    SRCS 
        "button_driver.c" 
        "button_config.c" 
        "driver.c" 
        "main.c" 
        "input_hal.c"
        "mcp23017_button_driver.c"
        "../../src/button_keymap.c"
        "../../src/calc_math.c"
        "../../src/graph.c"
        "../../src/stats.c"
        "../../src/solver.c"
        "../../src/mechanics.c"
        "../../src/expr_eval.c"
        "../../src/settings.c"
        "../../src/main_menu.c"
        "../../src/ui_submenu.c"
        "../../src/font_math_14.c"
        "../../src/font_math_12.c"
    INCLUDE_DIRS 
        "." 
        "../../src"
)

target_compile_definitions(__idf_main PRIVATE LV_LVGL_H_INCLUDE_SIMPLE)
