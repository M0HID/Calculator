# Technical Solution v2 - Source Code Documentation

This document provides detailed technical explanations of each source file in the calculator application, including accurate function line references, data structures, and algorithmic details. All line numbers are verified against the current codebase.

---

## Project Structure

```
lv_pc_calculator/
├── src/                        ← Shared application source (all platforms)
│   ├── button_keymap.c / .h    ← Hardware button matrix → LVGL key mapping
│   ├── calc_math.c / .h        ← Math calculator app
│   ├── expr_eval.c / .h        ← Recursive descent expression parser
│   ├── font_math_12.c          ← Custom math font (12pt)
│   ├── font_math_14.c          ← Custom math font (14pt)
│   ├── graph.c / .h            ← Function graphing app
│   ├── input_hal.h             ← Input hardware abstraction interface
│   ├── main_menu.c / .h        ← Main navigation hub
│   ├── mechanics.c / .h        ← SUVAT kinematics solver app
│   ├── numerical_methods.c / .h← Numerical Integration, Newton-Raphson, Curve Fitting
│   ├── settings.c / .h         ← Global settings (angle mode, decimals, variables)
│   ├── solver.c / .h           ← Linear and Quadratic equation solver
│   ├── stats.c / .h            ← Statistical distributions app
│   ├── test_eval.c             ← Standalone expression evaluator test harness
│   ├── ui_common.h             ← Shared design tokens, layout constants, helpers
│   └── ui_submenu.c / .h       ← Reusable vertical list submenu component
├── desktop/src/
│   ├── main.c                  ← PC/SDL2 entry point, two-window setup, keymap loader
│   ├── input_hal.c             ← Desktop implementation of input HAL
│   ├── input_hal.h             ← (shared with src/)
│   └── mouse_cursor_icon.c     ← SDL2 cursor bitmap data
├── button_keycodes.txt         ← Runtime-editable keyboard → button panel config
├── pseudocodes/                ← AQA pseudocode algorithm references
└── esp32/                      ← ESP32 embedded build files
```

---

## Key Design Constants (src/ui_common.h)

| Constant | Value | Description |
|----------|-------|-------------|
| `LCD_H_RES` | 320 | Display width in pixels |
| `LCD_V_RES` | 240 | Display height in pixels |
| `CONTENT_TOP` | 4 | Top content padding |
| `CONTENT_SIDE` | 6 | Left/right content padding |
| `HINT_BAR_H` | 16 | Bottom hint bar height |
| `HINT_BAR_Y` | 224 | Y position of hint bar (LCD_V_RES - HINT_BAR_H) |
| `ROW_SPACING` | 30 | Vertical distance between input rows |
| `FIELD_H` | 26 | Standard textarea height |

### Color Palette (src/ui_common.h)

| Constant | Hex | Usage |
|----------|-----|-------|
| `COL_BG` | `0xFFFFFF` | Screen background |
| `COL_TEXT` | `0x333333` | Primary text |
| `COL_RESULT` | `0x009900` | Green result values |
| `COL_ERROR` | `0xCC0000` | Red error messages |
| `COL_ACCENT_MATH` | `0x2196F3` | Math app blue |
| `COL_ACCENT_GRAPH` | `0xF44336` | Graph app red |
| `COL_ACCENT_STATS` | `0x4CAF50` | Stats app green |
| `COL_ACCENT_SOLVER` | `0xFF9800` | Solver app orange |
| `COL_ACCENT_MECH` | `0x9C27B0` | Mechanics app purple |
| `COL_ACCENT_NM` | `0x00897B` | Numerical Methods teal |

### Inline Helper Functions (src/ui_common.h)

| Function | Line | Description |
|----------|------|-------------|
| `ui_create_hint_bar` | 54 | Creates 320×16px bar at y=224; returns inner text label |
| `ui_setup_screen` | 80 | Sets bg to `COL_BG`, hides scrollbar, removes scrollable flag |
| `ui_style_textarea` | 91 | Applies accent colour on focus border and background |
| `ui_label` | 114 | Creates label with primary font and `COL_TEXT` at x,y |

---

## System-Wide Key Code Mapping

All apps share a unified key code vocabulary. Special calculator keys are encoded as single `uint32_t` values that flow through LVGL's event system.

| Key Code | Meaning | Sent by |
|----------|---------|---------|
| `'s'` | sin( | F11 / cos button |
| `'c'` | cos( | F12 |
| `'t'` | tan( | TAB |
| `'r'` | sqrt( | F9 |
| `'l'` | ln( | F10 |
| `'L'` | log( | F4 |
| `'P'` | pi | HOME |
| `'E'` | e constant | ESCAPE |
| `'N'` | Ans | END |
| `'W'` | ^2 | F2 |
| `'X'` | ^ | F3 |
| `'V'` | VAR / insert x | F5 / backtick |
| `'M'` | Return to main menu | PAGEUP |
| `'K'` | Settings | F6 |
| `'G'` | CALC / graph toggle | F1 |
| `'f'` | FRAC / space | F8 |
| `LV_KEY_ESC` | AC / back | DELETE |
| `LV_KEY_BACKSPACE` | Delete one char | BACKSPACE |
| `LV_KEY_ENTER` | Execute / confirm | RETURN |

---

## 1. button_keymap.c — 101 lines

**Purpose:** Maps the 9×6 physical button matrix positions to LVGL key codes and display labels. Provides a translation layer between hardware button presses and logical key events used by the calculator applications.

**Includes:** `button_keymap.h`, `string.h`

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `MAX_BUTTONS` | 4 | Constant: 54 (9×6 grid) |
| `button_mapping_t` | 6–11 | Struct: `{int row, int col, uint32_t lvgl_key, char label[16]}` |
| `keymap[]` | 13 | Static array of 54 `button_mapping_t` entries |
| `keymap_count` | 14 | Tracks how many entries are loaded |

### Functions

**Lines 16–81: `load_default_keymap()`**
- Arguments: None
- Returns: void
- Purpose: Populates all 54 keymap entries. Row 0: SHIFT (`'S'`), ALPHA (`'A'`), LEFT, UP, MENU (`'M'`), ON (`'O'`). Row 1: VAR (`'V'`), CALC (`'G'`), DOWN, RIGHT, CONST (`'K'`), EXP (`'Q'`). Row 2: FRAC (`'f'`), sqrt (`'r'`), x² (`'W'`), ^ (`'X'`), log (`'L'`), ln (`'l'`). Row 3: `!`, `(`, `)`, sin (`'s'`), cos (`'c'`), tan (`'t'`). Row 4: A–F (literal `'A'`–`'F'`). Row 5: x (`'x'`), 7–9, DEL (`LV_KEY_BACKSPACE`), AC (`LV_KEY_ESC`). Row 6: y (`'y'`), 4–6, `*`, `/`. Row 7: z (`'z'`), 1–3, `+`, `-`. Row 8: e const (`'E'`), 0, `.`, π (`'P'`), Ans (`'N'`), = (`LV_KEY_ENTER`).

**Line 83: `button_keymap_init()`**
- Arguments: None
- Returns: void
- Purpose: Public API; calls `load_default_keymap()`

**Lines 85–92: `button_keymap_get_key(int row, int col)`**
- Returns: `uint32_t` LVGL key code, or 0 if unmapped
- Purpose: Linear scan of keymap; returns key code for given row/col

**Lines 94–101: `button_keymap_get_label(int row, int col)`**
- Returns: `const char*` label, or NULL if unmapped
- Purpose: Linear scan; returns display label (e.g., `"sin"`, `"\xCF\x80"` for π)

---

## 2. calc_math.c — 302 lines

**Purpose:** Main Math Calculator application. Single-line expression input with live result preview and a circular buffer of up to 10 calculation history entries. Smart backspace deletes entire function tokens (e.g., `"sin("` as one unit).

**Includes:** `calc_math.h`, `ui_common.h`, `expr_eval.h`, `input_hal.h`, `main_menu.h`, `settings.h`, `lvgl.h`, `stdio.h`, `string.h`, `math.h`

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `MAX_HISTORY` | 12 | 10 — maximum history entries |
| `LINE_HEIGHT` | 13 | 18 — pixels per history row |
| `HistoryEntry` | 15–18 | Struct: `{char equation[128], char result[64]}` |
| `function_names[]` | 20–23 | NULL-terminated array of recognised function strings: `"sin("`, `"cos("`, `"tan("`, `"sqrt("`, `"ln("`, `"log("`, `"abs("`, `"floor("`, `"ceil("`, `"pi"` |
| `history[10]` | 25 | Circular buffer of `HistoryEntry` |
| `history_count` | 26 | Number of entries currently in buffer |
| `selected_line` | 27 | -1 = none, 0–9 = highlighted history row |
| `last_answer` | 28 | char[64]; initialised to `"0"`, updated on each calculation |
| `input_textarea` | 33 | Single-line expression input field |
| `calc_group` | 34 | LVGL focus group |
| `key_receiver` | 35 | Hidden 0×0 object receiving all key events |

### Functions

**Lines 45–62: `calculate_and_show_result()`**
- Arguments: None (reads from `input_textarea`)
- Returns: void
- Purpose: Calls `eval_expression()`, formats result. Values below 1e-10 snap to 0. Formats as "Error", "Infinity", `"%.0f"` for integers, or `"%.6g"` for decimals. Updates `current_result_label`.

**Lines 65–68: `insert_text_at_cursor(const char *text)`**
- Purpose: Calls `lv_textarea_add_text()` then `calculate_and_show_result()`

**Lines 70–82: `check_function_at_cursor()`**
- Returns: int — length of the function token if the cursor is directly after one, else 0
- Purpose: Enables smart backspace — detects whether the preceding text ends with a known function name

**Lines 83–91: `delete_function_at_cursor()`**
- Purpose: If `check_function_at_cursor()` > 0, deletes that many characters at once; otherwise deletes one character. Then calls `calculate_and_show_result()`

**Lines 93–113: `handle_custom_key(uint32_t key)`**
- Purpose: Maps special key codes to multi-character insertions. Handled cases:

| Key | Action |
|-----|--------|
| `'s'` | insert `"sin("` |
| `'c'` | insert `"cos("` |
| `'t'` | insert `"tan("` |
| `'r'` | insert `"sqrt("` |
| `'l'` | insert `"ln("` |
| `'L'` | insert `"log("` |
| `'N'` | insert `last_answer` |
| `'P'` | insert `"pi"` |
| `'E'` | insert `"e"` |
| `'V'` | insert `"x"` |
| `'W'` | insert `"^2"` |
| `'X'` | insert `"^"` |
| `LV_KEY_BACKSPACE` | `delete_function_at_cursor()` |
| default | `lv_group_send_data(calc_group, key)` — passes character to textarea |

**Lines 115–128: `add_to_history(const char *equation, const char *result)`**
- Purpose: Appends to circular buffer. If full (10 entries), shifts all entries down by one (discarding oldest). Updates `last_answer`, increments `history_count`, calls `update_history_display()`

**Lines 130–151: `update_history_display()`**
- Purpose: Iterates all 10 label slots, shows/hides based on `history_count`. Highlights the `selected_line` row with `COL_FOCUS_BG_MATH`, others transparent.

**Lines 153–156: `textarea_event_cb(lv_event_t *e)`**
- Purpose: Fires on `LV_EVENT_VALUE_CHANGED`; calls `calculate_and_show_result()`

**Lines 158–225: `math_key_cb(lv_event_t *e)`**
- Purpose: Main keyboard event handler. Attached to `key_receiver`.
- ENTER: saves equation + result to history if textarea non-empty, then clears textarea
- UP/DOWN: navigate history selection (`selected_line ±1`)
- ESC: clears textarea and resets history selection
- `'M'`: `main_menu_create()`
- `'K'`: `settings_app_start()`
- `'s','c','t','r','l','L','N','P','E','V','W','X'`: `handle_custom_key()`
- `'x'`: inserts `"x"` directly
- `'A'`–`'F'`, `'y'`, `'z'`, `'e'`: inserts variable letter
- BACKSPACE: `delete_function_at_cursor()`
- Digits, operators: insert single character

**Lines 227–301: `math_app_start()`**
- Purpose: Creates full calculator UI. History container at (4, CONTENT_TOP), 10 history rows with equation + result labels. Live result label at (8, 148). Input textarea at (6, 172), size (LCD_H_RES−12) × 34, one-line. Creates `key_receiver`, `calc_group`, focuses key receiver. Hint bar.

---

## 3. expr_eval.c — 350 lines

**Purpose:** Recursive descent parser evaluating mathematical expressions with full BIDMAS precedence. Supports functions, constants, user variables (A–F, x, y, z), implicit multiplication, and factorial.

**Includes:** `expr_eval.h`, `settings.h`, `stdlib.h`, `string.h`, `ctype.h`, `math.h`

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `NAN` | 8–10 | `(0.0/0.0)` — defined if not already available |
| `INFINITY` | 12–14 | `(1.0/0.0)` — defined if not already available |
| `M_PI` | 16–18 | `3.14159265358979323846` |
| `M_E` | 20–22 | `2.71828182845904523536` |
| `current_x_value` | 24 | Static double; set by `eval_expression_x()` before parsing; provides the value of variable `x` throughout the parse |
| `Parser` struct | 26–29 | `{const char *str, int pos}` — tracks position through the input string |

### Grammar

```
expression  := term (('+' | '-') term)*
term        := factor '!'?  (implicit_mult | '*' | '/' | '^')* factor '!'?
factor      := '-' factor | '+' factor
             | '(' expression ')'
             | 'x' | 'X' | 'y' | 'Y' | 'z' | 'Z'
             | 'A'..'F'
             | 'pi' | 'e'
             | 'sqrt'|'floor'|'ceil'|'abs'|'sin'|'cos'|'tan'|'ln'|'log'
               '(' expression ')'
             | number
number      := digit* ('.' digit*)?
implicit_mult := (next token is '(' | x | X | A-F | pi | e | known_function)
```

**Implicit multiplication examples:** `2x`, `2(3+4)`, `3sin(x)`, `2pi`

### Functions

**Lines 31–36: `skip_whitespace(Parser *p)`**
- Purpose: Advances `p->pos` past spaces and tabs

**Lines 37–54: `parse_number(Parser *p)`**
- Returns: double
- Purpose: Reads optional sign, integer digits, optional decimal point. Copies to 64-byte buffer and calls `atof()`

**Lines 56–68: `factorial(double n)`**
- Returns: double
- Purpose: Returns NAN for n < 0 or non-integer n, INFINITY for n > 170, otherwise iterates i = 2..n multiplying. Used for the postfix `!` operator in `parse_term`

**Lines 70–235: `parse_factor(Parser *p)`**
- Returns: double
- Purpose: Handles the highest-precedence tokens
- Details: Unary `-` / `+` (recursive call), parenthesised expression `(...)`, variable `x`/`X` → `current_x_value`, `y`/`Y` → `settings_get_variable(7)`, `z`/`Z` → `settings_get_variable(8)`, `A`–`F` → `settings_get_variable(0–5)`, constant `pi`, constant `e` (when not followed by alpha), functions `sqrt`, `floor`, `ceil`, `abs`, `sin`, `cos`, `tan`, `ln`, `log` (each parses a parenthesised argument; trig functions apply degree→radian conversion if `settings_get_angle_mode() == ANGLE_DEG`). Unknown alpha sequences return NAN.

**Lines 237–308: `parse_term(Parser *p)`**
- Returns: double
- Purpose: Handles multiplication-level operators
- Details: Calls `parse_factor()`, checks for postfix `!`. Then loops detecting implicit multiplication (when next char is `(`, `x`, `X`, `A`–`F`, `pi`, `e`, or a known function name) and explicit `*`, `/`, `^`. Division by zero returns NAN.

**Lines 310–329: `parse_expression(Parser *p)`**
- Returns: double
- Purpose: Handles `+` and `-` (lowest precedence). Calls `parse_term()`, loops for multiple operators.

**Lines 331–340: `eval_expression(const char *expr)`**
- Returns: double
- Purpose: Public API. Sets `current_x_value` from `settings_get_variable(6)` (the stored x variable), then parses.

**Lines 342–349: `eval_expression_x(const char *expr, double x_val)`**
- Returns: double
- Purpose: Public API for graphing and numerical integration. Sets `current_x_value = x_val`, then parses. Used by `do_integration()` and `draw_function()`.

---

## 4. graph.c — 477 lines

**Purpose:** Function graphing application supporting up to 4 simultaneous functions, zoom/pan controls, and a trace cursor. Two-screen: function list editor and graph canvas view.

**Includes:** `graph.h`, `ui_common.h`, `input_hal.h`, `expr_eval.h`, `main_menu.h`, `settings.h`, `lvgl.h`, `stdio.h`, `string.h`

**Objectives met:** 5.1 (plot single and multiple functions with labelled axes), 5.2 (zoom and pan), 1.1 (supports trig/exponent functions in expressions)

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `CANVAS_Y` | 17 | 30 — canvas starts below info labels |
| `MAX_FUNCTIONS` | 21 | 4 |
| `func_colors[]` | 23–25 | `{0x00CC00, 0xDD0000, 0x0088EE, 0xEE8800}` — green, red, blue, orange |
| `GraphScreen` enum | 27 | `SCREEN_FUNCTION_LIST`, `SCREEN_GRAPH_VIEW` |
| `Function` struct | 29 | `{char equation[64], int enabled}` |
| `functions[]` | 31–33 | Default: `{"x^2",1}`, `{"sin(x)*3",1}`, `{"",0}`, `{"",0}` |
| `x_min, x_max` | 35 | Default: −10.0, 10.0 |
| `y_min, y_max` | 36 | Default: −10.0, 10.0 |
| `canvas_buf` | 59 | Static LVGL draw buffer: `LV_DRAW_BUF_DEFINE_STATIC(canvas_buf, CANVAS_W, CANVAS_H, LV_COLOR_FORMAT_NATIVE)` |

### Coordinate Transformation Macros

| Macro | Line | Formula |
|-------|------|---------|
| `g2cx(gx)` | 73 | `(int)((gx - x_min) / (x_max - x_min) * CANVAS_W)` |
| `g2cy(gy)` | 74 | `(int)(CANVAS_H - (gy - y_min) / (y_max - y_min) * CANVAS_H)` (Y-axis inverted) |

### Functions

**Lines 76–107: `draw_axes()`**
- Arguments: None (reads global `x_min`, `x_max`, `y_min`, `y_max`, `canvas`)
- Purpose: Opens canvas layer. Draws Y-axis if x=0 is in range, X-axis if y=0 is in range, both in `COL_TEXT`. Calculates adaptive tick spacing (range > 100 → 20, > 50 → 10, > 20 → 5, > 10 → 2, < 2 → 0.25, < 5 → 0.5, else 1.0). Draws grey grid lines at each tick.
- Objectives: 5.1 — provides clearly labelled axes on the display

**Lines 109–133: `draw_function(int idx)`**
- Arguments: `int idx` — index into `functions[]` (0–3)
- Purpose: Evaluates `functions[idx].equation` at every pixel column using `eval_expression_x()`. Draws line segments between consecutive valid points. Skips segments where |Δy| ≥ CANVAS_H × 2 (discontinuity detection). Uses `func_colors[idx]`, line width 2.
- Objectives: 5.1 — plots the function visibly on the LCD canvas

**Lines 135–160: `draw_trace_cursor()`**
- Arguments: None (reads globals `trace_enabled`, `trace_func_idx`, `trace_x`)
- Purpose: Evaluates traced function at `trace_x`. Draws grey vertical + horizontal crosshair. Draws 9×9 filled square at traced point in the function's colour.
- Objectives: 5.2 — supports cursor tracing across the graph

**Lines 162–167: `draw_graph()`**
- Arguments: None
- Purpose: Full render pipeline — fills canvas with `COL_BG`, calls `draw_axes()`, `draw_function(0..3)`, `draw_trace_cursor()`.
- Objectives: 5.1, 5.2 — assembles the complete graph display

**Lines 169–198: `update_function_list_ui()`**
- Arguments: None (reads globals)
- Purpose: Updates all 4 rows — checkbox states, selected row highlight (`COL_FOCUS_BG_GRAPH`), shows textarea (edit mode) or equation label (view mode). In edit mode, loads equation and moves cursor to end.
- Objectives: 2.2 — keeps the LCD display of function list in sync with state

**Lines 200–215: `update_graph_info()`**
- Arguments: None
- Purpose: Updates `info_label` with `"x:[min,max] y:[min,max]"`. If trace is active, shows `"y1: (x, y)"` in the function's colour on `coords_label`.
- Objectives: 5.1 — displays axis range information clearly on screen

**Lines 217–293: `show_function_list()`**
- Arguments: None
- Purpose: Sets `current_screen = SCREEN_FUNCTION_LIST`. Creates container with 4 rows (checkbox, y#= label, equation label, hidden textarea). Creates `graph_group` + `key_receiver` with `funclist_key_cb`. Adds 4 textareas to group. Calls `update_function_list_ui()`.
- Objectives: 5.1 — enables the user to enter and manage up to 4 functions

**Lines 296–331: `show_graph_view()`**
- Arguments: None
- Purpose: Sets `current_screen = SCREEN_GRAPH_VIEW`. Creates `info_label`, `coords_label`, canvas at (0, CANVAS_Y) with static draw buffer. Creates new `graph_group` + receiver with `graph_key_cb`. Calls `draw_graph()` and `update_graph_info()`.
- Objectives: 5.1, 5.2 — renders the interactive graph on the LCD

**Lines 333–338: `graph_app_start()`**
- Arguments: None
- Purpose: Resets viewport to ±10, disables trace, resets selection. Calls `show_function_list()`.
- Objectives: 5.1 — entry point to the graphing application

**Lines 340–353: `textarea_event_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL event (READY or CANCEL on function textarea)
- Purpose: READY → copies textarea text into `functions[idx].equation`. Both READY and CANCEL → sets `editing_function = -1`, refocuses `key_receiver`, calls `update_function_list_ui()`.
- Objectives: 5.1 — saves or discards equation edits

**Lines 355–386: `funclist_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the hidden key receiver
- Purpose: UP/DOWN: move `selected_function`. ENTER: start editing (`editing_function = selected`, focus textarea). ESC: toggle `functions[selected].enabled`. `'G'`: `show_graph_view()`. `'K'`: settings. `'M'`: main menu.
- Objectives: 2.3 — all functions controllable from the physical keypad; 5.1 — enables toggling individual functions

**Lines 388–474: `graph_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the graph view receiver
- Purpose: LEFT/RIGHT: in trace mode, moves `trace_x` by 4px steps; otherwise pans x-axis ±20%. UP/DOWN: in trace mode, cycles through enabled functions; otherwise pans y-axis ±20%. `'+'`: zoom in (shrinks range by 20% each side). `'-'`: zoom out (expands by 25%). `'V'`: toggles trace, initialises `trace_x` to midpoint. ENTER/ESC: `show_function_list()`. `'K'`: settings. `'M'`: menu.
- Objectives: 2.3 — all graph interactions driven by the keypad; 5.2 — implements zoom and pan

---

## 5. main_menu.c — 145 lines

**Purpose:** Central navigation hub. 3×2 grid of 6 app-launcher buttons with keyboard (arrow + enter) and mouse click support.

**Includes:** `main_menu.h`, `ui_common.h`, `input_hal.h`, `lvgl.h`, all app headers, `string.h`

**Objectives met:** 2.2 (LCD display shows app grid clearly), 2.3 (keypad navigates and launches all apps)

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `MENU_COLS` | 14 | 3 |
| `MENU_ROWS` | 15 | 2 |
| `menu_buttons[2][3]` | 18 | 2D array of LVGL button objects |
| `menu_group` | 19 | LVGL group for keyboard navigation |

### App Index Mapping

| Index | Position | App | Colour |
|-------|----------|-----|--------|
| 0 | Row 0, Col 0 | Math | `COL_ACCENT_MATH` (blue) |
| 1 | Row 0, Col 1 | Graph | `COL_ACCENT_GRAPH` (red) |
| 2 | Row 0, Col 2 | Stats | `COL_ACCENT_STATS` (green) |
| 3 | Row 1, Col 0 | Solver | `COL_ACCENT_SOLVER` (orange) |
| 4 | Row 1, Col 1 | Mechanics | `COL_ACCENT_MECH` (purple) |
| 5 | Row 1, Col 2 | Numerical Methods | `0x00897B` (teal) |

### Functions

**Lines 21–29: `get_current_position(int *row, int *col)`**
- Arguments: `int *row`, `int *col` — output parameters filled with the focused button's grid position
- Purpose: Queries `lv_group_get_focused(menu_group)`, iterates `menu_buttons[2][3]` to find which button is focused. Returns `true` if a focused button is found.
- Objectives: 2.3 — enables keyboard focus tracking for grid navigation

**Lines 31–37: `move(int dr, int dc)`**
- Arguments: `int dr` — row delta (−1/+1), `int dc` — column delta (−1/+1)
- Purpose: Computes new position from `get_current_position()`, bounds-checks against MENU_ROWS/COLS, calls `lv_group_focus_obj()` on the target button.
- Objectives: 2.3 — arrow key navigation across the 3×2 button grid

**Lines 39–51: `openApp(int row, int col)`**
- Arguments: `int row` (0–1), `int col` (0–2) — grid position of the button to launch
- Purpose: Converts 2D position to linear index. Switch: 0→`math_app_start()`, 1→`graph_app_start()`, 2→`stats_app_start()`, 3→`solver_app_start()`, 4→`mechanics_app_start()`, 5→`numerical_methods_app_start()`.
- Objectives: 2.3 — launches any of the 6 calculator applications from the keypad

**Lines 52–68: `key_event_handler(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event fired on the focused button
- Purpose: UP→`move(-1,0)`, DOWN→`move(1,0)`, LEFT→`move(0,-1)`, RIGHT→`move(0,1)`, ENTER→`openApp()`, `'K'`→`settings_app_start()`.
- Objectives: 2.3 — routes all keypad inputs to the correct menu action

**Lines 69–75: `click_event_handler(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL clicked event on a button
- Purpose: Finds which button was clicked by comparing `lv_event_get_target()` against `menu_buttons[r][c]`, then calls `openApp(r, c)`.
- Objectives: 2.3 — supports mouse/touch input on the PC build

**Lines 76–144: `main_menu_create()`**
- Arguments: None
- Purpose: Acquires `lvgl_lock()`. Cleans screen. Calculates centred button positions (80×65px buttons, 20×18px spacing). Creates 6 coloured buttons with white labels. Focus style: darkened bg + 3px white border. Attaches key and click handlers. Creates group, focuses button 0. Hint bar. Releases `lvgl_unlock()`.
- Objectives: 2.2 — displays all 6 apps clearly on the 320×240 LCD; 2.3 — sets up full keyboard focus management

---

## 6. mechanics.c — 287 lines

**Purpose:** SUVAT kinematics solver. Given ≥ 3 of 5 values (s, u, v, a, t), iteratively solves for all 5 using all rearrangements of the SUVAT equations.

**Includes:** `mechanics.h`, `ui_common.h`, `input_hal.h`, `main_menu.h`, `settings.h`, `stdio.h`, `stdlib.h`, `string.h`, `math.h`, `ctype.h`

**Objectives met:** 8.1 (SUVAT kinematics for A-level), 8.2 (validates ≥3 inputs before solving), 3.2 (shows clear error messages for invalid inputs), 2.2 (results displayed clearly on LCD)

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `SUVAT_UNKNOWN` | 13 | `−999999.0` — sentinel for an unsolved variable |
| `IS_KNOWN(x)` | 14 | Macro: `((x) != SUVAT_UNKNOWN)` |
| `SUVAT` struct | 16 | `{double s, u, v, a, t}` |
| `suvat_names[]` | 27 | `{"s (m)","u (m/s)","v (m/s)","a (m/s²)","t (s)"}` |

### Functions

**Lines 31–33: `get_sv(SUVAT *sv, int i)`**
- Arguments: `SUVAT *sv` — pointer to SUVAT struct; `int i` — variable index (0=s, 1=u, 2=v, 3=a, 4=t)
- Purpose: Returns the value of the i-th SUVAT variable using a switch statement for index-based access.
- Objectives: 8.1 — provides array-like access to the five kinematic variables

**Lines 35–37: `set_sv(SUVAT *sv, int i, double v)`**
- Arguments: `SUVAT *sv` — pointer to SUVAT struct; `int i` — variable index; `double v` — value to assign
- Purpose: Sets the i-th SUVAT variable by index, enabling loop-based population of the struct.
- Objectives: 8.1 — enables programmatic writing of solved SUVAT values

**Lines 39–47: `count_known(SUVAT *sv)`**
- Arguments: `SUVAT *sv` — pointer to SUVAT struct
- Purpose: Counts how many of the 5 SUVAT fields are not equal to `SUVAT_UNKNOWN`. Returns an int 0–5.
- Objectives: 8.2 — used to enforce the minimum 3-input validation rule

**Lines 49–89: `solve_suvat(SUVAT *sv)`**
- Arguments: `SUVAT *sv` — pointer to SUVAT struct (modified in-place)
- Purpose: Iterates up to 10 times. Each iteration applies 15 rearranged SUVAT equations: `v=u+at`, `u=v−at`, `t=(v−u)/a`, `a=(v−u)/t`, `s=ut+½at²`, `s=(u+v)t/2`, `v²=u²+2as` (→v), (→u), `a=(v²−u²)/(2s)`, `s=(v²−u²)/(2a)`, `t=2s/(u+v)`, `u=2s/t−v`, `v=2s/t−u`, `a=2(vt−s)/t²`, `u=(s−½at²)/t`. Breaks early if `count_known` does not increase. Returns final `count_known()`.
- Objectives: 8.1 — implements iterative constraint propagation to solve all SUVAT equations

**Lines 94–122: `do_calculate()`**
- Arguments: None (reads from `input_fields[]` textareas)
- Purpose: Reads all 5 input textareas using `atof()` (empty fields become `SUVAT_UNKNOWN`). If `count_known < 3`, shows "Enter at least 3 values" error. Calls `solve_suvat()`. If solved count < 5, shows "Cannot solve - check inputs". Otherwise calls `show_result_page()`.
- Objectives: 8.2 — enforces the 3-input minimum; 3.2 — displays error messages for invalid inputs

**Lines 124–129: `focus_input(int idx)`**
- Arguments: `int idx` — index of the input field to focus (0–4)
- Purpose: Clamps idx to valid range 0–4, updates `focused_field`, calls `lv_group_focus_obj(input_fields[idx])`.
- Objectives: 2.3 — enables keypad-driven focus movement between input fields

**Lines 131–171: `mech_textarea_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event fired on a SUVAT input textarea
- Purpose: UP: `focus_input(idx−1)`, DOWN: `focus_input(idx+1)`, ENTER: `do_calculate()`, ESC: clears all 5 fields and resets SUVAT struct to UNKNOWN, `'K'`: `settings_app_start()`, `'M'`: `main_menu_create()`.
- Objectives: 2.3 — full keypad control of the SUVAT input page

**Lines 173–181: `mech_result_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the result page hidden receiver
- Purpose: `'K'`: settings, `'M'`: main menu, ESC: `show_input_page()` (return to inputs).
- Objectives: 2.3 — keypad navigation from the result page back to inputs

**Lines 183–233: `show_input_page()`**
- Arguments: None
- Purpose: Clears screen, creates new `mech_group`. Creates 5 rows each with a label (`"s (m):"` etc.) and a textarea. Restores previous values if `IS_KNOWN`. Adds error label. Registers `mech_textarea_key_cb`. Calls `focus_input(0)`. Creates hint bar.
- Objectives: 2.2 — displays all 5 SUVAT input fields clearly on the LCD; 8.1 — presents the standard kinematic variable layout

**Lines 235–274: `show_result_page()`**
- Arguments: None
- Purpose: Clears screen, creates new group with hidden key receiver using `mech_result_key_cb`. Displays 5 result labels formatted as `"s (m) = 12.3456"`. Shows SUVAT equation reference at bottom: `"v=u+at  s=ut+0.5at²\nv²=u²+2as  s=(u+v)t/2"`.
- Objectives: 2.2 — clearly displays all solved values on the LCD; 6.2 — all correct solutions are shown on screen

**Lines 276–287: `mechanics_app_start()`**
- Arguments: None
- Purpose: Resets all 5 SUVAT fields to `SUVAT_UNKNOWN`. Nulls `mech_group`. Resets `focused_field = 0`. Calls `show_input_page()`.
- Objectives: 8.1 — initialises the SUVAT solver in a clean state

---

## 7. numerical_methods.c — 491 lines

**Purpose:** Numerical Methods application with three tools: Curve Fitting (least-squares linear regression), Numerical Integration (Simpson's 1/3 rule), and Newton-Raphson root finding.

**Includes:** `numerical_methods.h`, `ui_common.h`, `ui_submenu.h`, `input_hal.h`, `main_menu.h`, `settings.h`, `expr_eval.h`, `lvgl.h`, `stdio.h`, `stdlib.h`, `string.h`, `math.h`

**Objectives met:** 7.1 (Newton-Raphson root finding), 7.2 (Simpson's rule numerical integration), 7.3 (linear curve fitting with R², plotted via graph app)

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `COL_ACCENT_NM` | 14 | `lv_color_hex(0x00897B)` — teal |
| `COL_FOCUS_BG_NM` | 15 | `lv_color_hex(0xE0F2F1)` — light teal |
| `NM_MAX_POINTS` | 17 | 30 — max data points for curve fitting |
| `NR_MAX_ITERATIONS` | 348 | 100 |
| `NR_TOLERANCE` | 349 | 1e-8 |
| `nm_menu_items[]` | 475–479 | 3 `SubMenuItem`s: "Curve Fitting", "Numerical Integration", "Newton-Raphson" |

### Functions

**Lines 24–30: `cleanup_nm_ui()`**
- Arguments: None
- Purpose: If `nm_group` is non-null, calls `ui_submenu_cleanup(nm_group)` and nulls both `nm_group` and `nm_key_recv`. Prevents LVGL memory leaks when switching between NM sub-screens.
- Objectives: General — ensures clean screen transitions within the NM module

**Lines 32–50: `setup_nm_screen(const char *hint)`**
- Arguments: `const char *hint` — hint bar text string
- Purpose: Calls `cleanup_nm_ui()`, cleans active screen, calls `ui_setup_screen()`. Creates `nm_group`, hidden `nm_key_recv` (0×0 object added to group). Creates hint bar with provided text. Sets indev group. Returns the active screen pointer.
- Objectives: 2.2 — standard screen setup with hint bar for all NM sub-screens

**Lines 52–68: `parse_csv(const char *str, double *out, int max)`**
- Arguments: `const char *str` — comma- or space-separated number string; `double *out` — output array; `int max` — maximum values to parse
- Purpose: Copies `str` to a local buffer. Uses `strtod()` to extract numbers one by one, skipping spaces and commas. Returns the count of successfully parsed values.
- Objectives: 7.3 — enables user to input x/y data lists for curve fitting

**Lines 74–116: `do_curve_fit()`**
- Arguments: None (reads from `cf_x_field` and `cf_y_field` textareas)
- Purpose: Calls `parse_csv()` on both fields. Validates n ≥ 2 and nx == ny. Computes Σx, Σy, Σx², Σxy. Slope: `m = (n·Σxy − Σx·Σy) / (n·Σx² − Σx²)`. Intercept: `c = (Σy − m·Σx) / n`. R²: `1 − SS_res / SS_tot`. Displays `"y = mx ± c\nR² = value  (n=N)"` in `cf_result_lbl`.
- Objectives: 7.3 — performs least-squares linear regression and reports R²

**Lines 118–159: `cf_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a curve fitting textarea
- Purpose: UP/DOWN: `lv_group_focus_prev/next()`. ENTER: `do_curve_fit()`. `'f'` (FRAC key): deletes previous char and inserts a space (CSV separator). ESC: `show_nm_menu()`. `'K'`: settings. `'M'`: main menu.
- Objectives: 2.3 — full keypad control of curve fitting input; 7.3 — triggers the regression calculation

**Lines 161–199: `show_curve_fitting()`**
- Arguments: None
- Purpose: Calls `setup_nm_screen()`. Creates title label "Curve Fitting  y = mx + c". Creates `cf_x_field` (x vals, CSV placeholder "1 2 3 4") and `cf_y_field` (y vals, placeholder "2 4 5 4"), both full-width minus label. Creates `cf_result_lbl`. Focuses `cf_x_field`.
- Objectives: 7.3 — builds the curve fitting input screen; 2.2 — displays fields clearly on LCD

**Lines 207–245: `do_integration()`**
- Arguments: None (reads from `ni_expr_field`, `ni_a_field`, `ni_b_field`, `ni_n_field`)
- Purpose: Reads expr, a, b, n (defaults n to 100, clamps to even, max 10000). If a == b, result is 0. Otherwise applies Simpson's 1/3 rule: `h = (b−a)/n`, sum = f(a)+f(b), inner terms get coefficient 4 (odd i) or 2 (even i), `result = (h/3) × sum`. Displays `"= value  (n=N steps)"` or error if NaN/Inf.
- Objectives: 7.2 — implements Simpson's rule for definite numerical integration; 1.2 — uses sufficient steps (up to 10000) for 6+ significant figure accuracy

**Lines 247–283: `ni_textarea_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on any numerical integration textarea
- Purpose: UP/DOWN: navigate fields. ENTER: `do_integration()`. ESC: `show_nm_menu()`. `'M'`: main menu. Special keys delete the preceding auto-inserted character then insert the function string:

| Key | Inserted |
|-----|----------|
| `'s'` | `"sin("` |
| `'c'` | `"cos("` |
| `'t'` | `"tan("` |
| `'r'` | `"sqrt("` |
| `'l'` | `"ln("` |
| `'L'` | `"log("` |
| `'P'` | `"pi"` |
| `'X'` | `"^"` |
| `'V'` | `"x"` |

- Objectives: 2.3 — keypad drives all integration input; 1.1 — supports trig/power functions in the integrand

**Lines 285–346: `show_numerical_integration()`**
- Arguments: None
- Purpose: Calls `setup_nm_screen()`. Creates title "Numerical Integration (Simpson)". Creates 4 fields: `ni_expr_field` (f(x)=, full width), `ni_a_field` (a=, half width), `ni_b_field` (b=, half width), `ni_n_field` (steps, 80px). Creates `ni_result_lbl`. All fields register `ni_textarea_key_cb`. Focuses `ni_expr_field`.
- Objectives: 7.2 — builds the numerical integration UI; 2.2 — displays fields clearly on LCD

**Lines 351–354: `numerical_derivative(const char *expr, double x)`**
- Arguments: `const char *expr` — expression string; `double x` — point at which to differentiate
- Purpose: Computes central difference approximation `(f(x+h) − f(x−h)) / (2h)` with h = 1e-5 using `eval_expression_x()`.
- Objectives: 7.1 — provides the derivative estimate needed by Newton-Raphson

**Lines 356–372: `newton_raphson(const char *expr, double x0, double *result, int *iters)`**
- Arguments: `const char *expr` — f(x) expression; `double x0` — initial guess; `double *result` — output root; `int *iters` — output iteration count
- Purpose: Iterates up to `NR_MAX_ITERATIONS` (100). Each step: computes `fx = eval_expression_x(expr, x)`, `fpx = numerical_derivative(expr, x)`. Breaks if |fpx| < 1e-14 (near-zero derivative). Computes `xn = x − fx/fpx`. If `|xn − x| < NR_TOLERANCE` (1e-8): stores `*result = xn`, returns 1 (converged). Otherwise updates `x = xn`. Returns 0 if no convergence.
- Objectives: 7.1 — core Newton-Raphson iterative root-finding algorithm

**Lines 376–387: `solve_nr_fn(lv_obj_t **f, int n, lv_obj_t *res)`**
- Arguments: `lv_obj_t **f` — array of field pointers (f[0]=expr textarea, f[1]=x0 textarea); `int n` — field count (2); `lv_obj_t *res` — result label
- Purpose: Reads expr and x0 from textareas. Calls `newton_raphson()`. Displays `"x = value  (N iters)"` on success or `"No convergence (x~value)"` on failure.
- Objectives: 7.1 — shows the Newton-Raphson result clearly; 3.2 — displays failure message when no convergence

**Lines 391–433: `nr_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a Newton-Raphson textarea
- Purpose: Same key handling as `ni_textarea_key_cb` with the addition of: `'V'` → insert `"x"`, `'W'` → insert `"^2"`, `'K'` → `settings_app_start()`. ENTER calls `solve_nr_fn()`. ESC returns to `show_nm_menu()`.
- Objectives: 2.3 — keypad drives Newton-Raphson input; 1.1 — supports trig and power notation in f(x)

**Lines 435–473: `show_newton_raphson()`**
- Arguments: None
- Purpose: Calls `setup_nm_screen()`. Creates title "Newton-Raphson  f(x)=0". Creates `nr_fields[0]` (f(x)= textarea, full width, placeholder "e.g. x^2-4") and `nr_fields[1]` (x0= textarea, 100px wide, placeholder "1"). Creates `nr_fields[2]` as result label. Registers `nr_key_cb` on both textareas. Focuses `nr_fields[0]`.
- Objectives: 7.1 — builds the Newton-Raphson input screen; 2.2 — displays input fields clearly on LCD

**Lines 483–487: `show_nm_menu()`**
- Arguments: None
- Purpose: Calls `cleanup_nm_ui()`. Calls `ui_create_submenu(nm_menu_items, 3, &style, main_menu_create)` where style uses `COL_ACCENT_NM` and `COL_FOCUS_BG_NM`.
- Objectives: General — navigation hub for the three numerical methods tools

**Lines 489–491: `numerical_methods_app_start()`**
- Arguments: None
- Purpose: Calls `show_nm_menu()`. Entry point called by `main_menu.c`.
- Objectives: 7.1, 7.2, 7.3 — launches the full Numerical Methods application

---

## 8. settings.c — 338 lines

**Purpose:** Global calculator settings — angle mode (RAD/DEG), decimal places (0–10), and 9 user variables (A–F, x, y, z). State persists across app switches in static memory.

**Includes:** `settings.h`, `ui_common.h`, `ui_submenu.h`, `input_hal.h`, `main_menu.h`, `stdio.h`, `string.h`, `stdlib.h`

**Objectives met:** 1.3 (variables A–F, x, y, z stored in memory), 1.1 (angle mode controls trig function behaviour), 1.2 (decimal places setting controls output precision)

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `angle_mode` | 10 | Static `AngleMode`, default `ANGLE_RAD` |
| `decimal_places` | 11 | Static int, default 0 (auto) |
| `user_vars[9]` | 12 | Static double array, all 0. Indices: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=x, 7=y, 8=z |
| `SETTINGS_VAR_COUNT` | (settings.h:17) | 9 |
| `AngleMode` enum | (settings.h:7) | `ANGLE_RAD = 0`, `ANGLE_DEG` |

### Public API (lines 14–26)

| Function | Line | Arguments | Purpose |
|----------|------|-----------|---------|
| `settings_get_angle_mode()` | 14 | None | Returns current `angle_mode` |
| `settings_set_angle_mode(m)` | 15 | `AngleMode m` | Sets `angle_mode = m` |
| `settings_get_decimal_places()` | 17 | None | Returns `decimal_places` |
| `settings_set_decimal_places(dp)` | 18 | `int dp` | Clamps to 0–10, sets `decimal_places` |
| `settings_get_variable(idx)` | 20 | `int idx` (0–8) | Bounds-checked getter for `user_vars` |
| `settings_set_variable(idx, v)` | 21 | `int idx`, `double v` | Bounds-checked setter for `user_vars` |
| `settings_get_variable_name(idx)` | 22 | `int idx` | Returns `'A'`–`'F'` for 0–5, `'x'`/`'y'`/`'z'` for 6/7/8 |

### Functions

**Lines 40–51: `angle_update()`**
- Arguments: None (reads globals `angle_sel`, `angle_labels[]`)
- Purpose: Sets text of both `angle_labels[]` to "Radians (RAD)" and "Degrees (DEG)". Highlights `angle_labels[angle_sel]` with `COL_FOCUS_BG_SETTINGS` background, sets others to transparent.
- Objectives: 1.1 — visual feedback for angle mode selection which directly affects trig functions

**Lines 53–74: `angle_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the angle page hidden receiver
- Purpose: UP: `angle_sel--` (min 0), DOWN: `angle_sel++` (max 1), calls `angle_update()`. ENTER: sets `angle_mode = ANGLE_RAD or ANGLE_DEG`, calls `show_settings_menu()`. ESC: `show_settings_menu()`. `'M'`: `main_menu_create()`.
- Objectives: 1.1 — allows switching between radians and degrees for trig calculations; 2.3 — keypad-driven selection

**Lines 76–122: `show_angle_page()`**
- Arguments: None
- Purpose: Cleans screen, calls `ui_setup_screen()`. Sets `angle_sel` from current `angle_mode`. Creates `angle_group`. Creates title "Angle Unit". Creates 2 labels with left-padding and rounded corners. Creates hidden key receiver with `angle_key_cb`. Sets indev group and focuses receiver. Calls `angle_update()`. Shows current mode label. Hint bar.
- Objectives: 1.1 — presents RAD/DEG selection; 2.2 — LCD display of angle mode page

**Lines 128–137: `dec_update()`**
- Arguments: None (reads `dec_sel`, `dec_labels[]`)
- Purpose: Highlights `dec_labels[dec_sel]` with `COL_FOCUS_BG_SETTINGS`, sets all others to transparent background.
- Objectives: 1.2 — visual selection of decimal precision setting

**Lines 139–160: `dec_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the decimal places page hidden receiver
- Purpose: UP: `dec_sel--` (min 0), DOWN: `dec_sel++` (max 10), calls `dec_update()`. ENTER: `decimal_places = dec_sel`, calls `show_settings_menu()`. ESC: back. `'M'`: main menu.
- Objectives: 1.2 — controls output precision; 2.3 — keypad navigation

**Lines 162–215: `show_decimals_page()`**
- Arguments: None
- Purpose: Cleans screen. Sets `dec_sel` from `decimal_places`. Creates `dec_group`. Creates title "Decimal Places". Creates a non-scrollable container with 11 labels (i=0: "Auto (smart)", i=1–10: "N decimal place(s)"), each 18px tall. Creates hidden key receiver with `dec_key_cb`. Calls `dec_update()`. Hint bar.
- Objectives: 1.2 — allows selecting from Auto or 1–10 decimal places; 2.2 — LCD display of options

**Lines 222–227: `var_focus(int idx)`**
- Arguments: `int idx` — variable index to focus (0–8)
- Purpose: Clamps idx to valid range 0..(SETTINGS_VAR_COUNT−1), updates `var_focused`, calls `lv_group_focus_obj(var_fields[idx])`.
- Objectives: 2.3 — keypad-driven focus movement between variable fields

**Lines 229–268: `var_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a variable editor textarea
- Purpose: UP: `var_focus(idx−1)`, DOWN: `var_focus(idx+1)`. ENTER: iterates all 9 textareas, saves non-empty values as `atof()` or 0 into `user_vars[]`, calls `show_settings_menu()`. ESC: `show_settings_menu()` without saving. `'M'`: main menu.
- Objectives: 1.3 — saves user-defined variables to memory for use in expressions

**Lines 270–319: `show_variables_page()`**
- Arguments: None
- Purpose: Cleans screen. Creates `var_group`. Creates title "Variables (A-F, x/y/z)". Creates a scrollable container. For each i=0..8: creates label using `settings_get_variable_name(i)` + ":", and a 200px-wide textarea. Loads non-zero current values. Registers `var_key_cb`. Calls `var_focus(0)`. Hint bar.
- Objectives: 1.3 — displays all 9 user variables (A–F, x/y/z) for editing; 2.2 — clear LCD display

**Lines 321–325: `settings_items[]`**
- 3 `SubMenuItem`s: `{"Angle Unit (RAD/DEG)", show_angle_page}`, `{"Decimal Places", show_decimals_page}`, `{"Variable Editor (A-F, x/y/z)", show_variables_page}`
- Objectives: General — defines the settings submenu entries

**Lines 327–334: `show_settings_menu()`**
- Arguments: None
- Purpose: Creates a `SubMenuStyle` with `COL_ACCENT_SETTINGS` and `COL_FOCUS_BG_SETTINGS`. Calls `ui_create_submenu(settings_items, 3, &style, main_menu_create)`.
- Objectives: General — displays the top-level settings navigation list

**Lines 336–338: `settings_app_start()`**
- Arguments: None
- Purpose: Calls `show_settings_menu()`. Entry point called from any app via `'K'` key or the main menu.
- Objectives: 1.1, 1.2, 1.3 — entry point for all global calculator configuration

---

## 9. solver.c — 322 lines

**Purpose:** Equation solver with Linear (ax+b=0) and Quadratic (ax²+bx+c=0) solvers using analytical methods. Submenu navigation with shared `SolverCtx` infrastructure.

**Includes:** `solver.h`, `input_hal.h`, `main_menu.h`, `settings.h`, `ui_common.h`, `ui_submenu.h`, `math.h`, `stdio.h`, `stdlib.h`, `string.h`

**Objectives met:** 6.1 (linear and quadratic solvers), 6.2 (all solutions displayed clearly), 6.3 (error messages for no-solution and invalid input cases), 3.2 (clear error messages on the display)

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `SolverMode` enum | 12–16 | `SOLVER_MENU`, `SOLVER_LINEAR`, `SOLVER_QUADRATIC` |
| `SolverCtx` struct | 102–107 | `{lv_obj_t **fields, int field_count, lv_obj_t *result_label, void (*solve_fn)(...)}` |
| `active_ctx` | 109 | Pointer to current solver's context |
| `solver_menu_items[]` | 84–87 | 2 `SubMenuItem`s: "Linear (ax+b=0)", "Quadratic (ax²+bx+c=0)" |

### Functions

**Lines 27–32: `solve_linear(double a, double b, double *x)`**
- Arguments: `double a`, `double b` — coefficients; `double *x` — output root
- Purpose: Returns 0 (no solution) if |a| < 1e-12. Otherwise computes `*x = −b/a` and returns 1.
- Objectives: 6.1 — analytically solves the linear equation ax + b = 0

**Lines 34–50: `solve_quadratic(double a, double b, double c, double *x1, double *x2)`**
- Arguments: `double a`, `double b`, `double c` — coefficients; `double *x1`, `double *x2` — output roots
- Purpose: If |a| < 1e-12, delegates to `solve_linear(b, c, x1)`. Computes discriminant `d = b²−4ac`. Returns 0 if d < 0 (no real roots). If |d| < 1e-12: repeated root `*x1 = *x2 = −b/(2a)`, returns 1. Else: `*x1 = (−b+√d)/(2a)`, `*x2 = (−b−√d)/(2a)`, returns 2.
- Objectives: 6.1 — analytically solves the quadratic equation; 6.2 — correctly handles all three root cases

**Lines 52–59: `cleanup_solver_ui()`**
- Arguments: None
- Purpose: If `solver_group` non-null, calls `ui_submenu_cleanup(solver_group)`. Nulls `solver_group`, `solver_key_recv`, `solver_hint_lbl`.
- Objectives: General — clean screen teardown preventing LVGL memory leaks

**Lines 61–82: `setup_solver_screen(const char *hint_text)`**
- Arguments: `const char *hint_text` — hint bar string
- Purpose: Calls `cleanup_solver_ui()`. Cleans screen. Creates `solver_group`. Creates hidden `solver_key_recv` (0×0), adds to group. Creates hint bar. Sets indev group. Returns the screen pointer.
- Objectives: 2.2 — standard solver screen setup with hint bar

**Lines 91–100: `show_solver_menu()`**
- Arguments: None
- Purpose: Sets `current_mode = SOLVER_MENU`. Calls `cleanup_solver_ui()`. Creates `SubMenuStyle` with `COL_ACCENT_SOLVER`. Calls `ui_create_submenu(solver_menu_items, 2, &style, main_menu_create)`.
- Objectives: 6.1 — entry point presenting linear and quadratic solver options

**Lines 111–133: `sub_solver_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the solver's hidden key receiver
- Purpose: ENTER: calls `active_ctx->solve_fn(fields, count, result_label)`. ESC: `show_solver_menu()`. `'K'`: `cleanup_solver_ui()` + `settings_app_start()`. `'M'`: `cleanup_solver_ui()` + `main_menu_create()`.
- Objectives: 2.3 — keypad triggers solve via function pointer dispatch; 6.1 — generic solve handler

**Lines 135–140: `solver_insert_text_at_cursor(const char *text)`**
- Arguments: `const char *text` — string to insert
- Purpose: Gets currently focused object from `solver_group`. If it is a textarea, calls `lv_textarea_add_text()` to append text at cursor.
- Objectives: 1.1 — enables entry of math functions (sin, sqrt, etc.) from keypad into solver fields

**Lines 142–180: `solver_handle_custom_key(uint32_t key)`**
- Arguments: `uint32_t key` — single-character LVGL key code
- Purpose: Maps special key codes to `solver_insert_text_at_cursor()` calls. Returns 1 if handled, 0 otherwise. Mappings: `'s'`→`"sin("`, `'c'`→`"cos("`, `'t'`→`"tan("`, `'r'`→`"sqrt("`, `'l'`→`"ln("`, `'L'`→`"log("`, `'P'`→`"pi"`, `'E'`→`"e"`, `'V'`→`"x"`, `'W'`→`"^2"`, `'X'`→`"^"`.
- Objectives: 1.1 — allows full use of mathematical functions and constants in solver expressions

**Lines 182–225: `solver_textarea_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a solver input textarea
- Purpose: UP: `lv_group_focus_prev()`, DOWN: `lv_group_focus_next()`, ENTER: `active_ctx->solve_fn()`, ESC: `show_solver_menu()`, `'K'`: settings, `'M'`: main menu. For function key codes (`'s'`, `'c'`, `'t'`, `'r'`, `'l'`, `'L'`, `'P'`, `'E'`, `'V'`, `'W'`, `'X'`): deletes preceding auto-inserted character, then calls `solver_handle_custom_key()`.
- Objectives: 2.3 — keypad handles all navigation and input within solver screens

**Lines 230–240: `solve_linear_fn(lv_obj_t **f, int n, lv_obj_t *res)`**
- Arguments: `lv_obj_t **f` — array of field pointers (f[0]=a, f[1]=b textareas); `int n` — field count (2); `lv_obj_t *res` — result label
- Purpose: Reads a and b as `atof()` from textareas. Calls `solve_linear(a, b, &x)`. Displays `"x = value"` or `"No solution (a=0)"`.
- Objectives: 6.1 — solves ax + b = 0; 6.2 — displays solution clearly; 6.3 — shows error for degenerate case

**Lines 242–270: `show_linear_solver()`**
- Arguments: None
- Purpose: Sets `current_mode = SOLVER_LINEAR`. Calls `setup_solver_screen("[=] Solve  [AC] Back  [M] Menu")`. Registers `sub_solver_key_cb` on `solver_key_recv`. Creates title label "ax + b = 0". Creates 2 textareas (a:, b:) with `solver_textarea_key_cb`. Creates result label. Sets `linear_ctx` and `active_ctx = &linear_ctx`. Focuses `linear_fields[0]`.
- Objectives: 6.1 — builds the linear solver UI; 2.2 — clearly displays input fields on LCD

**Lines 275–289: `solve_quad_fn(lv_obj_t **f, int n, lv_obj_t *res)`**
- Arguments: `lv_obj_t **f` — array of field pointers (f[0]=a, f[1]=b, f[2]=c textareas); `int n` — field count (3); `lv_obj_t *res` — result label
- Purpose: Reads a, b, c from textareas. Calls `solve_quadratic(a, b, c, &x1, &x2)`. Displays "No real solutions", `"x = value"` (repeated root), or `"x1 = value\nx2 = value"` (two roots).
- Objectives: 6.1 — solves ax² + bx + c = 0; 6.2 — shows all roots; 6.3 — clear no-solution message

**Lines 291–320: `show_quadratic_solver()`**
- Arguments: None
- Purpose: Sets `current_mode = SOLVER_QUADRATIC`. Calls `setup_solver_screen()`. Registers `sub_solver_key_cb`. Creates title "ax² + bx + c = 0". Creates 3 textareas (a:, b:, c:). Creates result label with 40px height (for two-line output). Sets `quad_ctx` and `active_ctx`. Focuses `quad_fields[0]`.
- Objectives: 6.1 — builds the quadratic solver UI; 2.2 — displays all 3 coefficient fields clearly

**Line 322: `solver_app_start()`**
- Arguments: None
- Purpose: Calls `show_solver_menu()`. Entry point called by `main_menu.c`.
- Objectives: 6.1, 6.2, 6.3 — launches the equation solver application

---

## 10. stats.c — 487 lines

**Purpose:** Statistical distribution calculator — Binomial PMF/CDF, Normal PDF/CDF/Inverse, Poisson PMF/CDF. Seven distribution screens sharing a common 3-parameter input layout.

**Includes:** `stats.h`, `ui_common.h`, `ui_submenu.h`, `input_hal.h`, `main_menu.h`, `settings.h`, `lvgl.h`, `stdio.h`, `string.h`, `stdlib.h`

**Objectives met:** 4.1 (binomial distribution PMF and CDF), 4.2 (normal distribution PDF, CDF, and inverse normal), 4.3 (Poisson PMF and CDF), 6.2 (probability values displayed clearly), 6.3 (error messages for invalid inputs)

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `StatsMode` enum | 28–37 | `STATS_MENU`, `STATS_BINOMIAL`, `STATS_BINOMIAL_CDF`, `STATS_NORMAL`, `STATS_NORMAL_CDF`, `STATS_NORMAL_INV`, `STATS_POISSON`, `STATS_POISSON_CDF` |
| `param_names[][3]` | 67–75 | Parameter label strings per distribution. Binomial: "n (trials)", "k (successes)", "p (prob)". Normal: "x (value)", "μ (mean)", "σ (std dev)". Poisson: "λ (mean)", "k (value/max)", "" |
| `StatsCtx` struct | 194–199 | `{lv_obj_t **fields, int field_count, lv_obj_t *result_label, void (*calc_fn)(...)}` |
| `stats_menu_items[]` | 242–250 | 7 `SubMenuItem`s |

### Statistical Algorithm Functions

**Lines 79–85: `factorial(int n)`**
- Arguments: `int n` — non-negative integer
- Purpose: Returns NAN for n < 0. Returns 1.0 for n ≤ 1. Iterates product from 2 to n. Used internally for Poisson and binomial calculations.
- Objectives: 4.1, 4.3 — required by binomial coefficient and Poisson PMF

**Lines 87–93: `binomial_coeff(int n, int k)`**
- Arguments: `int n` — number of trials; `int k` — number of successes
- Purpose: Returns 0 for k < 0 or k > n. Returns 1 for k=0 or k=n. Otherwise uses iterative multiply-divide: `r *= (n−i); r /= (i+1)` for i = 0..k−1. Avoids large intermediate values.
- Objectives: 4.1 — computes C(n,k) for the binomial PMF

**Lines 95–99: `binomial_pmf(int n, int k, double p)`**
- Arguments: `int n` — trials; `int k` — successes; `double p` — probability of success
- Purpose: Validates p ∈ [0,1], n ≥ 0, k ∈ [0,n]. Returns `C(n,k) × p^k × (1−p)^(n−k)`.
- Objectives: 4.1 — calculates P(X=k) for the binomial distribution

**Lines 101–109: `binomial_cdf(int n, int k, double p)`**
- Arguments: `int n` — trials; `int k` — maximum successes; `double p` — probability
- Purpose: Validates inputs. Returns 1.0 early if k ≥ n. Otherwise sums `binomial_pmf(n, i, p)` for i = 0 to k.
- Objectives: 4.1 — calculates cumulative P(X≤k) for the binomial distribution

**Lines 111–115: `normal_pdf(double x, double mu, double sigma)`**
- Arguments: `double x` — value; `double mu` — mean; `double sigma` — standard deviation
- Purpose: Returns NAN if σ ≤ 0. Computes `(1/(σ√(2π))) × e^(−½((x−μ)/σ)²)`.
- Objectives: 4.2 — evaluates the normal probability density function

**Lines 117–124: `erf_approx(double x)`**
- Arguments: `double x` — input value
- Purpose: Abramowitz & Stegun approximation. Computes `t = 1/(1 + 0.3275911|x|)`. Polynomial: `y = 1 − (a1t + a2t² + a3t³ + a4t⁴ + a5t⁵) × e^(−x²)`. Returns signed result.
- Objectives: 4.2 — provides the error function needed by `normal_cdf()`

**Lines 126–129: `normal_cdf(double x, double mu, double sigma)`**
- Arguments: `double x` — value; `double mu` — mean; `double sigma` — standard deviation
- Purpose: Returns NAN if σ ≤ 0. Returns `0.5 × (1 + erf_approx((x−mu)/(sigma×√2)))`.
- Objectives: 4.2 — calculates cumulative P(X≤x) for the normal distribution

**Lines 131–138: `erf_inv_approx(double x)`**
- Arguments: `double x` — value in (−1, 1)
- Purpose: Returns NAN if x ∉ (−1, 1). Uses constant a = 0.147. Computes `b = 2/(π×a) + ln(1−x²)/2`, `c = ln(1−x²)/a`. Returns `sign × √(−b + √(b²−c))`.
- Objectives: 4.2 — provides the inverse error function needed by `normal_inv()`

**Lines 140–144: `normal_inv(double p, double mu, double sigma)`**
- Arguments: `double p` — cumulative probability in (0,1); `double mu` — mean; `double sigma` — standard deviation
- Purpose: Returns NAN if p ∉ (0,1) or σ ≤ 0. Returns `mu + sigma × √2 × erf_inv_approx(2p − 1)`.
- Objectives: 4.2 — calculates the inverse normal (quantile) function

**Lines 432–435: `poisson_pmf(double lambda, int k)`**
- Arguments: `double lambda` — mean rate λ; `int k` — number of events
- Purpose: Returns NAN if λ ≤ 0 or k < 0. Returns `e^(−λ) × λ^k / k!`.
- Objectives: 4.3 — calculates P(X=k) for the Poisson distribution

**Lines 437–443: `poisson_cdf(double lambda, int k)`**
- Arguments: `double lambda` — mean rate λ; `int k` — maximum events
- Purpose: Returns NAN if λ ≤ 0. Returns 0 if k < 0. Sums `poisson_pmf(lambda, i)` for i = 0 to k.
- Objectives: 4.3 — calculates cumulative P(X≤k) for the Poisson distribution

### UI Functions

**Lines 158–171: `cleanup_stats_ui()`**
- Arguments: None
- Purpose: If `stats_group` non-null, calls `ui_submenu_cleanup(stats_group)`. Nulls `stats_group`, `dist_label`, `result_label`, `hint_lbl`, all `param_labels[]` and `param_textareas[]`.
- Objectives: General — clean teardown of stats screens

**Lines 174–190: `setup_stats_screen(const char *hint_text)`**
- Arguments: `const char *hint_text` — hint bar string
- Purpose: Calls `cleanup_stats_ui()`. Cleans screen. Creates `stats_group`. Creates hint bar. Sets indev group. Returns the screen pointer.
- Objectives: 2.2 — standard screen setup with hint bar for distribution screens

**Lines 204–238: `stats_textarea_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a distribution parameter textarea
- Purpose: UP/DOWN: `lv_group_focus_prev/next()`. ENTER: calls `active_stats_ctx->calc_fn(fields, count, result_label)`. ESC: `show_stats_menu()`. `'K'`: settings. `'M'`: main menu.
- Objectives: 2.3 — keypad-driven input and calculation trigger for all distribution screens

**Lines 254–265: `show_stats_menu()`**
- Arguments: None
- Purpose: Sets `current_mode = STATS_MENU`. Calls `cleanup_stats_ui()`. Creates `SubMenuStyle` with `COL_ACCENT_STATS`. Calls `ui_create_submenu(stats_menu_items, 7, &style, main_menu_create)`.
- Objectives: 4.1, 4.2, 4.3 — presents all 7 distribution options

**Lines 270–314: `create_dist_screen(const char *title, const char **param_labels_text, void (*calc_fn)(...), lv_obj_t **fields_out, StatsCtx *ctx)`**
- Arguments: `const char *title` — distribution name; `const char **param_labels_text` — 3-element array of parameter labels; `calc_fn` — calculation function pointer; `lv_obj_t **fields_out` — receives the 3 textarea pointers; `StatsCtx *ctx` — receives the configured context
- Purpose: Calls `setup_stats_screen()`. Creates title label in `COL_ACCENT_STATS`. For each i=0..2: creates a parameter label (secondary font) and a 170px textarea with `stats_textarea_key_cb`. Creates result label (primary font, green). Populates `ctx` with fields, field_count=3, result_label, and calc_fn. Sets `active_stats_ctx = ctx`. Focuses first textarea.
- Objectives: 4.1, 4.2, 4.3 — generic UI builder ensuring consistent 3-parameter layout across all 7 distributions; 2.2 — standard layout displayed clearly on LCD

**Lines 320–337: Binomial PMF screen**
- `calc_binomial_pmf(lv_obj_t **f, int n, lv_obj_t *res)` — Arguments: field array, count, result label. Reads n, k, p as `atof()`. Calls `binomial_pmf()`. Displays `"= value"` or error/infinity.
- `show_binomial_pmf()` — Arguments: None. Calls `create_dist_screen("Binomial P(X=k)", ...)`.
- Objectives: 4.1 — displays P(X=k) for the binomial distribution

**Lines 340–360: Binomial CDF screen**
- `calc_binomial_cdf(lv_obj_t **f, int n, lv_obj_t *res)` — Arguments: field array, count, result label. Calls `binomial_cdf()`. Displays result.
- `show_binomial_cdf()` — Arguments: None. Calls `create_dist_screen("Binomial CDF P(X<=k)", ...)`.
- Objectives: 4.1 — displays cumulative P(X≤k) for the binomial distribution

**Lines 363–383: Normal PDF screen**
- `calc_normal_pdf(lv_obj_t **f, int n, lv_obj_t *res)` — Arguments: field array, count, result label. Reads x, μ, σ. Calls `normal_pdf()`. Displays result.
- `show_normal_pdf()` — Arguments: None. Calls `create_dist_screen("Normal PDF", ...)`.
- Objectives: 4.2 — displays normal probability density

**Lines 386–406: Normal CDF screen**
- `calc_normal_cdf(lv_obj_t **f, int n, lv_obj_t *res)` — Arguments: field array, count, result label. Reads x, μ, σ. Calls `normal_cdf()`. Displays result.
- `show_normal_cdf()` — Arguments: None. Calls `create_dist_screen("Normal CDF", ...)`.
- Objectives: 4.2 — displays cumulative normal probability P(X≤x)

**Lines 409–429: Inverse Normal screen**
- `calc_inverse_normal(lv_obj_t **f, int n, lv_obj_t *res)` — Arguments: field array, count, result label. Reads p, μ, σ. Calls `normal_inv()`. Displays result.
- `show_inverse_normal()` — Arguments: None. Calls `create_dist_screen("Inverse Normal", ...)`.
- Objectives: 4.2 — implements inverse normal (quantile) function

**Lines 432–463: Poisson PMF screen**
- `poisson_pmf(double lambda, int k)` (line 432) — Arguments: λ, k. Returns P(X=k).
- `calc_poisson_pmf(lv_obj_t **f, int n, lv_obj_t *res)` — Arguments: field array, count, result label. Reads λ and k. Calls `poisson_pmf()`. Displays result.
- `show_poisson_pmf()` — Arguments: None. Calls `create_dist_screen("Poisson P(X=k)", ...)`.
- Objectives: 4.3 — displays P(X=k) for Poisson distribution

**Lines 465–483: Poisson CDF screen**
- `poisson_cdf(double lambda, int k)` (line 437) — Arguments: λ, k. Returns Σ P(X=i) for i=0..k.
- `calc_poisson_cdf(lv_obj_t **f, int n, lv_obj_t *res)` — Arguments: field array, count, result label. Reads λ and k. Calls `poisson_cdf()`. Displays result.
- `show_poisson_cdf()` — Arguments: None. Calls `create_dist_screen("Poisson CDF P(X<=k)", ...)`.
- Objectives: 4.3 — displays cumulative P(X≤k) for Poisson distribution

**Lines 485–487: `stats_app_start()`**
- Arguments: None
- Purpose: Calls `show_stats_menu()`. Entry point called by `main_menu.c`.
- Objectives: 4.1, 4.2, 4.3 — launches the full statistics application

---

## 11. ui_submenu.c — 129 lines

**Purpose:** Reusable vertical list submenu component. Used by Settings, Solver, Stats, and Numerical Methods to provide consistent navigation without code duplication.

**Includes:** `ui_submenu.h`, `ui_common.h`, `input_hal.h`, `string.h`

**Objectives met:** 2.2 (submenu displays list clearly on LCD), 2.3 (keypad navigation: Up/Down + Enter), General design — eliminates duplicated code across 4 apps

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `SubMenuCtx` struct | 6–15 | Internal state: `{const SubMenuItem *items, int item_count, int selection, void (*on_menu)(void), lv_obj_t **labels, lv_obj_t *key_recv, lv_group_t *group, lv_color_t focus_bg}` |
| `active_submenu` | 17 | Static pointer — only one submenu active at a time |

### Functions

**Lines 19–28: `update_highlight(SubMenuCtx *ctx)`**
- Arguments: `SubMenuCtx *ctx` — pointer to the current submenu context
- Purpose: Iterates all `ctx->item_count` labels. Sets background of `ctx->labels[ctx->selection]` to `ctx->focus_bg` with full opacity. Sets all other labels to transparent background.
- Objectives: 2.2 — provides clear visual highlight of the currently selected menu item on the LCD

**Lines 30–61: `submenu_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the hidden key receiver
- Purpose: Reads `active_submenu`. UP: `selection--` (min 0), calls `update_highlight()`. DOWN: `selection++` (max item_count−1), calls `update_highlight()`. ENTER: calls `ctx->items[ctx->selection].callback()`. `'M'`: calls `ctx->on_menu()` (usually `main_menu_create`).
- Objectives: 2.3 — provides Up/Down/Enter keypad navigation for all submenus

**Lines 63–118: `ui_create_submenu(const SubMenuItem *items, int item_count, const SubMenuStyle *style, void (*on_menu)(void))`**
- Arguments: `const SubMenuItem *items` — array of menu items (text + callback); `int item_count` — number of items; `const SubMenuStyle *style` — accent colour, focus bg colour, hint text; `void (*on_menu)(void)` — callback for `'M'` key (return to main menu)
- Purpose: If a previous submenu is active, calls `ui_submenu_cleanup()` on it. Cleans screen. Allocates `SubMenuCtx` and labels array via `lv_malloc()`. Creates LVGL group. Creates hidden `key_recv` with `submenu_key_cb`. For each item (y starts at `CONTENT_TOP`, increments 26px): creates a label with primary font, `COL_TEXT`, 6px left pad, 4px top pad, 3px radius. Creates hint bar. Sets indev group. Focuses `key_recv`. Calls `update_highlight()`. Sets `active_submenu`. Returns `ctx->group`.
- Objectives: 2.2 — consistent list display across all 4 apps; 2.3 — shared keyboard navigation infrastructure

**Lines 120–129: `ui_submenu_cleanup(lv_group_t *group)`**
- Arguments: `lv_group_t *group` — the group to clean up
- Purpose: Checks `active_submenu != NULL && active_submenu->group == group`. Frees labels array via `lv_free()`. Calls `lv_group_del(group)`. Frees `active_submenu` context. Sets `active_submenu = NULL`.
- Objectives: General — memory-safe teardown preventing LVGL leaks when switching screens

---

## 12. desktop/src/main.c — 509 lines

**Purpose:** PC application entry point. Sets up two SDL2 windows (320×240 display + 320×270 keypad panel), loads the keyboard configuration file, builds the on-screen button grid, and starts the main menu.

**Includes:** LVGL, SDL2, `input_hal.h`, `button_keymap.h`, all app headers

### Two-Window Architecture

The PC build runs two separate SDL2 windows side by side:
- **Window 1 (display):** 320×240 — the calculator screen, rendered by LVGL
- **Window 2 (keypad):** 320×270 — the on-screen button grid (6 columns × 9 rows)

Both windows share the same LVGL instance. Key events from either window are routed to the LVGL focus group via `send_key_to_main_app()`.

### Button Configuration (button_keycodes.txt)

A runtime-editable plain text file (format: `ROW,COL=KEYCODE`) mapping physical keyboard keys to the 6×9 button panel. Loaded at startup by `load_keymap_config()` and copied to `desktop/bin/` by CMake post-build. Key rows:

| Row | Buttons | SDL2 Keys |
|-----|---------|-----------|
| 0 | SHIFT, ALPHA, ←, ↑, MENU, ON | LSHIFT, LALT, LEFT, UP, PAGEUP, PAGEDOWN |
| 1 | VAR, CALC, ↓, →, CONST, EXP | F5, F1, DOWN, RIGHT, F6, F7 |
| 2 | FRAC, sqrt, x², ^, log, ln | F8, F9, F2, F3, F4, F10 |
| 3 | !, (, ), sin, cos, tan | !, (, ), F11, F12, TAB |
| 4 | A, B, C, D, E, F | A, B, C, D, e, F |
| 5 | x, 7, 8, 9, DEL, AC | ` (backtick), 7, 8, 9, BACKSPACE, DELETE |
| 6 | y, 4, 5, 6, ×, ÷ | y, 4, 5, 6, *, / |
| 7 | z, 1, 2, 3, +, − | z, 1, 2, 3, +, - |
| 8 | e, 0, ., π, Ans, = | ESCAPE, 0, ., HOME, END, RETURN |

### Functions

**Lines 82–156: `load_keymap_config()`**
- Purpose: Opens `button_keycodes.txt`, reads `ROW,COL=KEYCODE` lines, populates local `keymap[]` array. Ignores comment lines (`#`).

**Lines 157–235: `keycode_string_to_lvgl_key(const char *keycode)`**
- Returns: `uint32_t` LVGL key code
- Purpose: Central SDL2-name → LVGL key translation. Full mapping:

| SDL2 Name | LVGL Key Sent |
|-----------|--------------|
| UP / DOWN / LEFT / RIGHT | `LV_KEY_UP/DOWN/LEFT/RIGHT` |
| RETURN / ENTER | `LV_KEY_ENTER` |
| BACKSPACE | `LV_KEY_BACKSPACE` |
| DELETE | `LV_KEY_ESC` (AC button) |
| ESCAPE / ESC | `'E'` (e constant) |
| HOME | `'P'` (π) |
| END | `'N'` (Ans) |
| LSHIFT | `'S'` (Shift) |
| LALT | `'A'` (Alpha) |
| PAGEUP | `'M'` (Menu) |
| PAGEDOWN | `'O'` (On) |
| F1 | `'G'` (CALC/graph) |
| F2 | `'W'` (x²) |
| F3 | `'X'` (^) |
| F4 | `'L'` (log) |
| F5 | `'V'` (VAR/x) |
| F6 | `'K'` (Settings) |
| F7 | `'Q'` (EXP) |
| F8 | `'f'` (FRAC) |
| F9 | `'r'` (sqrt) |
| F10 | `'l'` (ln) |
| F11 | `'s'` (sin) |
| F12 | `'c'` (cos) |
| TAB | `'t'` (tan) |
| `` ` `` (backtick) | `'V'` (VAR/x insert) |
| Single printable char | itself (as-is) |

**Lines 238–253: `send_key_to_main_app(uint32_t key)`**
- Purpose: Special-cases `'M'` (returns to main menu directly). Otherwise calls `lv_group_send_data()` on the group attached to `main_keyboard_indev`.

**Lines 255–293: `button_flash_timer_cb()` / `button_event_cb()`** (lines 255, 262)
- `button_flash_timer_cb`: Restores button background colour after 100ms flash
- `button_event_cb`: Finds clicked button by iterating `button_objects[r][c]`, flashes it, looks up keycode in `keymap[]`, calls `keycode_string_to_lvgl_key()`, then `send_key_to_main_app()`

**Lines 296–387: `create_button_panel(lv_obj_t *parent)`**
- Purpose: Renders 6×9 button grid on the keypad window. Button size: (320−10)/6−2 × 28px. Labels come from `button_keymap_get_label()`. Arrow keys (UP, DOWN, LEFT, RIGHT) are repositioned in a diamond layout by adjusting their x/y offsets. All buttons register `button_event_cb`.

**Lines 390–424: `global_key_handler(lv_event_t *e)`**
- Purpose: Catches physical keyboard presses on the keypad window. Maps SDL2 key names via `find_button_by_key()` + `keycode_string_to_lvgl_key()`, sends to `send_key_to_main_app()`.

**Lines 426–509: `main()`**
- Purpose: Full application bootstrap:
  1. Initialises LVGL
  2. Creates SDL2 display driver for both windows
  3. Creates keyboard indev, stores via `store_keyboard_indev()`
  4. Loads `button_keycodes.txt` via `load_keymap_config()`
  5. Calls `button_keymap_init()`
  6. Calls `create_button_panel()` on keypad window
  7. Calls `main_menu_create()`
  8. Enters LVGL main loop with `lv_timer_handler()`

### Complete Key Flow (Desktop)

```
Physical keyboard key press
        ↓ SDL2 key event → global_key_handler()
        ↓ find_button_by_key() → keycode_string_to_lvgl_key()
        ↓ send_key_to_main_app()
        ↓ lv_group_send_data(main_group, key)
        ↓ LV_EVENT_KEY fires on focused object
        ↓ App-specific key callback (ni_textarea_key_cb / math_key_cb / etc.)
        ↓ Action (insert text, navigate, calculate, etc.)

On-screen button click
        ↓ button_event_cb()
        ↓ keymap[row][col] → keycode_string_to_lvgl_key()
        ↓ send_key_to_main_app()
        ↓ (same as above)
```

---

## 13. desktop/src/input_hal.c — ~20 lines

**Purpose:** Desktop implementation of the input hardware abstraction layer.

| Function | Line | Description |
|----------|------|-------------|
| `store_keyboard_indev(lv_indev_t *indev)` | 6 | Saves the LVGL keyboard indev pointer |
| `get_navigation_indev()` | 10 | Returns the stored indev; called by every app to attach its focus group |
| `lvgl_lock()` | 14 | No-op on PC (single-threaded) |
| `lvgl_unlock()` | 18 | No-op on PC (single-threaded) |

---

## Summary

This technical solution covers all source files in the `lv_pc_calculator` codebase, implementing a comprehensive graphical scientific calculator for both PC (SDL2) and ESP32 embedded targets.

### Application Inventory

| App | Source File | Lines |
|-----|------------|-------|
| Math Calculator | `calc_math.c` | 302 |
| Function Grapher | `graph.c` | 477 |
| Statistical Distributions | `stats.c` | 487 |
| Equation Solver | `solver.c` | 322 |
| SUVAT Mechanics | `mechanics.c` | 287 |
| Numerical Methods | `numerical_methods.c` | 491 |
| Main Menu | `main_menu.c` | 145 |
| Settings | `settings.c` | 338 |

### Supporting Modules

| Module | Source File | Lines |
|--------|------------|-------|
| Expression Evaluator | `expr_eval.c` | 350 |
| Button Matrix Keymap | `button_keymap.c` | 101 |
| Reusable Submenu | `ui_submenu.c` | 129 |
| Shared UI Constants | `ui_common.h` | 122 |
| Desktop Entry Point | `desktop/src/main.c` | 509 |
| Input HAL (desktop) | `desktop/src/input_hal.c` | ~20 |

### Key Algorithms

| Algorithm | Location | Method |
|-----------|----------|--------|
| Expression parsing | `expr_eval.c:70` | Recursive descent (BIDMAS) with implicit multiplication |
| Numerical integration | `numerical_methods.c:207` | Simpson's 1/3 rule |
| Root finding | `numerical_methods.c:356` | Newton-Raphson with central-difference derivative |
| Numerical derivative | `numerical_methods.c:351` | Central difference `(f(x+h)−f(x−h))/(2h)` |
| Linear regression | `numerical_methods.c:74` | Least-squares (Σxy method) |
| Kinematics solver | `mechanics.c:49` | Iterative SUVAT constraint propagation (16 equations, 10 iterations max) |
| Graph plotting | `graph.c:109` | Pixel-column evaluation with discontinuity detection |
| Coordinate transform | `graph.c:73` | Linear mapping with Y-axis inversion macros |
| Normal CDF | `stats.c:126` | Error function with Abramowitz & Stegun approximation |
| Inverse normal | `stats.c:140` | Inverse error function (constant-a approximation) |
| Calculation history | `calc_math.c:115` | Circular FIFO buffer (10 entries) |
| Smart backspace | `calc_math.c:70` | Function-token detection before cursor |

### Design Patterns

| Pattern | Where Used |
|---------|-----------|
| Event-driven callbacks (LVGL) | All apps — `LV_EVENT_KEY` handlers |
| Finite state machine | `graph.c` (GraphScreen), `solver.c` (SolverMode), `stats.c` (StatsMode) |
| Polymorphism via function pointers | `SolverCtx.solve_fn`, `StatsCtx.calc_fn` |
| Generic component / reuse | `ui_submenu.c` — shared by 4 apps |
| Hardware abstraction layer | `input_hal.h` — desktop vs ESP32 implementations |
| Runtime configuration | `button_keycodes.txt` — editable without recompiling |
