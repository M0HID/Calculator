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

**Purpose:** Maps the 9×6 physical button matrix positions to LVGL key codes and display labels. It acts as a translation layer between the raw hardware button positions and the logical key events consumed by all calculator applications, meaning none of the app code needs to know anything about the physical layout of the keypad. The keymap is populated at startup from a hardcoded default and can optionally be overridden at runtime via the `button_keycodes.txt` config file on the desktop build. This separation makes it straightforward to remap any key without touching the application code.

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
- Purpose: Populates all 54 keymap entries with their row, column, LVGL key code, and display label. Covers all 9 rows of the physical button grid, mapping everything from digits and operators to special keys like sin, sqrt, π, and Ans.

<!-- **Line 83: `button_keymap_init()`**
- Arguments: None
- Returns: void
- Purpose: Public API; calls `load_default_keymap()` -->

**Lines 85–92: `button_keymap_get_key(int row, int col)`**
- Returns: `uint32_t` LVGL key code, or 0 if unmapped
- Purpose: Scans the keymap and returns the LVGL key code for the given row and column. Returns 0 if no entry matches.

**Lines 94–101: `button_keymap_get_label(int row, int col)`**
- Returns: `const char*` label, or NULL if unmapped
- Purpose: Scans the keymap and returns the display label for the given row and column. This label is used in the virtual keypad to show the user what each button does.

---

## 2. calc_math.c — 302 lines

**Purpose:** Implements the main Math Calculator application, which is the core general-purpose calculation screen of the project. It provides a single-line expression input field with a live result preview that updates on every keystroke, so the user can see the answer as they type without having to press equals. Completed calculations are stored in a circular buffer of up to 10 history entries, displayed in two columns above the input box so the user can reference previous results at a glance. Smart backspace is also implemented, which detects when the cursor is immediately after a function token such as `"sin("` and deletes the whole token as a single unit rather than one character at a time, making editing expressions much less tedious.

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
- Purpose: Calls `eval_expression()` on the text in the textarea and formats the result. Values below 1e-10 snap to 0 to avoid floating point noise. Displays "Error" or "Infinity" where appropriate, otherwise formats as an integer or to 6 significant figures.

<!-- **Lines 65–68: `insert_text_at_cursor(const char *text)`**
- Purpose: Calls `lv_textarea_add_text()` then `calculate_and_show_result()` -->

**Lines 70–82: `check_function_at_cursor()`**
- Returns: int — length of the function token if the cursor is directly after one, else 0
- Purpose: Checks whether the text immediately before the cursor ends with a known function name like `sin(` or `sqrt(`. Used to enable smart backspace so the whole token is deleted at once rather than one character at a time.

**Lines 83–91: `delete_function_at_cursor()`**
- Purpose: If the cursor is directly after a function token, deletes the whole token at once. Otherwise deletes a single character. Calls `calculate_and_show_result()` afterwards to keep the live preview up to date.

**Lines 93–113: `handle_custom_key(uint32_t key)`**
- Purpose: Maps special key codes to multi-character insertions. For example, `'s'` inserts `"sin("`, `'r'` inserts `"sqrt("`, and `'N'` inserts the last answer. Passes any unrecognised key through to the textarea directly.

**Lines 115–128: `add_to_history(const char *equation, const char *result)`**
- Purpose: Implements a FIFO circular buffer of up to 10 equations. If the buffer is full, it shifts all entries down by one and discards the oldest. Also updates `last_answer` so it can be recalled with the Ans key.

**Lines 130–151: `update_history_display()`**
- Purpose: Refreshes all 10 history label slots, showing or hiding them based on how many entries are stored. Highlights the currently selected row and sets all others to a transparent background.

<!-- **Lines 153–156: `textarea_event_cb(lv_event_t *e)`**
- Purpose: Fires on `LV_EVENT_VALUE_CHANGED`; calls `calculate_and_show_result()` -->

**Lines 158–225: `math_key_cb(lv_event_t *e)`**
- Purpose: Main keyboard event handler for the calculator. Enter saves the current equation to history and clears the textarea. Up and down navigate the history selection. ESC clears the input. Function keys like `'s'` and `'r'` are forwarded to `handle_custom_key()`, and digits and operators are inserted directly into the textarea.

**Lines 227–301: `math_app_start()`**
- Purpose: Creates the full calculator UI, including the history container with 10 rows, the live result label, and the input textarea. Sets up the key receiver and focus group, then adds a hint bar at the bottom of the screen.

---

## 3. expr_eval.c — 350 lines

**Purpose:** Implements a hand-written recursive descent parser that evaluates mathematical expressions given as strings, enforcing full BIDMAS operator precedence without relying on any external parsing library. The parser is used by every part of the application that needs to evaluate user input — the math app, the graph plotter, the Newton-Raphson solver, and the numerical integrator all call into this module. It supports the full range of mathematical constructs required by the A-Level syllabus, including trig functions, logarithms, square roots, factorial, exponentiation, the constants π and e, user-defined variables A–F and x/y/z, and implicit multiplication such as `2x` or `3sin(x)`. The implementation uses the C call stack to encode operator precedence naturally through the recursive grammar, runs in O(n) time relative to expression length, and performs no heap allocation.

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

<!-- **Lines 31–36: `skip_whitespace(Parser *p)`**
- Purpose: Advances `p->pos` past spaces and tabs -->

**Lines 37–54: `parse_number(Parser *p)`**
- Returns: double
- Purpose: Reads a numeric literal from the current parser position, including an optional decimal point, and converts it to a double using `atof()`.

**Lines 56–68: `factorial(double n)`**
- Returns: double
- Purpose: Computes n! for the postfix `!` operator. Returns NAN for negative or non-integer values, and INFINITY if n exceeds 170 to avoid overflow.

**Lines 70–235: `parse_factor(Parser *p)`**
- Returns: double
- Purpose: Part of the recursive descent parser, handling the highest precedence tokens. Resolves brackets, unary minus, variables (x, y, z, A–F), constants (pi, e), and functions such as sin, cos, tan, sqrt, and log. Applies degree-to-radian conversion for trig functions if the angle mode is set to DEG. Returns NAN for unrecognised identifiers.

**Lines 237–308: `parse_term(Parser *p)`**
- Returns: double
- Purpose: Part of the recursive descent parser, handling multiplication-level operations. Calls `parse_factor()`, then checks for the postfix `!` operator, implicit multiplication (e.g. `2x` or `3sin(x)`), and explicit `*`, `/`, and `^` operators. Returns NAN on division by zero.

**Lines 310–329: `parse_expression(Parser *p)`**
- Returns: double
- Purpose: Part of the recursive descent parser, handling the lowest precedence operators `+` and `-`. Calls `parse_term()` and loops to handle chains of addition and subtraction.

<!-- **Lines 331–340: `eval_expression(const char *expr)`**
- Returns: double
- Purpose: Public API. Sets `current_x_value` from `settings_get_variable(6)` (the stored x variable), then parses.

**Lines 342–349: `eval_expression_x(const char *expr, double x_val)`**
- Returns: double
- Purpose: Public API for graphing and numerical integration. Sets `current_x_value = x_val`, then parses. Used by `do_integration()` and `draw_function()`. -->

---

## 4. graph.c — 477 lines

**Purpose:** Implements the function graphing application, which allows the user to plot up to four mathematical functions simultaneously on a full-screen canvas. The app is split into two screens — a function list editor where the user can enter and toggle equations, and a graph view where the curves are rendered with axes, a grid, and optional trace cursor. Pan and zoom are controlled entirely from the keypad, and the trace mode lets the user move a crosshair along a selected curve and read off the exact coordinates in real time. Each function is plotted in a distinct colour (green, red, blue, orange) so multiple curves can be easily distinguished, which is particularly useful when studying intersections or comparing transformations of the same function.

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
- Purpose: Draws the x and y axes on the graph canvas, but only if zero falls within the current viewport range. Uses adaptive tick spacing depending on the scale, and draws grey grid lines at each tick position.
- Objectives: 5.1 — provides clearly labelled axes on the display

**Lines 109–133: `draw_function(int idx)`**
- Arguments: `int idx` — index into `functions[]` (0–3)
- Purpose: Evaluates the function at every x pixel column using `eval_expression_x()` and draws line segments between consecutive valid points. Skips any segment where the y difference is too large, to avoid drawing false vertical lines at discontinuities.
- Objectives: 5.1 — plots the function visibly on the LCD canvas

**Lines 135–160: `draw_trace_cursor()`**
- Arguments: None (reads globals `trace_enabled`, `trace_func_idx`, `trace_x`)
- Purpose: Evaluates the traced function at the current `trace_x` position and draws a crosshair with a small square marker at that point. The marker is coloured to match the function being traced.
- Objectives: 5.2 — supports cursor tracing across the graph

<!-- **Lines 162–167: `draw_graph()`**
- Arguments: None
- Purpose: Full render pipeline — fills canvas with `COL_BG`, calls `draw_axes()`, `draw_function(0..3)`, `draw_trace_cursor()`.
- Objectives: 5.1, 5.2 — assembles the complete graph display -->

**Lines 169–198: `update_function_list_ui()`**
- Arguments: None (reads globals)
- Purpose: Refreshes all 4 function rows to reflect the current state. Highlights the selected row, and switches between showing the equation label or the editable textarea depending on whether that function is currently being edited.
- Objectives: 2.2 — keeps the LCD display of function list in sync with state

**Lines 217–293: `show_function_list()`**
- Arguments: None
- Purpose: Opens the function list editor screen. Creates a container with 4 rows, each containing a checkbox, a label, an equation label, and a hidden textarea for editing. Sets up the key receiver and calls `update_function_list_ui()` to populate the rows.
- Objectives: 5.1 — enables the user to enter and manage up to 4 functions

**Lines 296–331: `show_graph_view()`**
- Arguments: None
- Purpose: Switches to the graph canvas screen. Creates the info label, coordinates label, and LVGL canvas with a static draw buffer. Sets up the key receiver and renders the graph.
- Objectives: 5.1, 5.2 — renders the interactive graph on the LCD

**Lines 340–353: `textarea_event_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL event (READY or CANCEL on function textarea)
- Purpose: On READY, saves the textarea text back into the function's equation string. On both READY and CANCEL, exits edit mode and returns focus to the key receiver.
- Objectives: 5.1 — saves or discards equation edits

**Lines 355–386: `funclist_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the hidden key receiver
- Purpose: Handles all keypad input on the function list screen. Up and down move the selection between functions. Enter starts editing the selected function. ESC toggles whether that function is enabled or disabled. The graph key switches to the graph view.
- Objectives: 2.3 — all functions controllable from the physical keypad; 5.1 — enables toggling individual functions

**Lines 388–474: `graph_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the graph view receiver
- Purpose: Handles all keypad input on the graph view screen. Left and right pan or move the trace cursor. Up and down pan the y-axis or cycle between traced functions. Plus and minus zoom in and out. The VAR key toggles trace mode. Enter and ESC return to the function list.
- Objectives: 2.3 — all graph interactions driven by the keypad; 5.2 — implements zoom and pan

---

## 5. main_menu.c — 145 lines

**Purpose:** Implements the main menu, which is the central navigation hub of the calculator and the first screen the user sees on startup. It presents six coloured app tiles in a 3×2 grid, each with a distinct accent colour so the user can immediately identify each app without needing icons or lengthy labels. Navigation is handled via the arrow keys and the equals key, and the focused tile is highlighted with a white border to give clear visual feedback. Mouse clicks on the tile buttons are also supported on the desktop build, allowing the app to be used without a keyboard for testing and demonstration purposes.

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
- Purpose: Iterates over the 3×2 button grid to find which button is currently focused, and writes its row and column into the output parameters. Returns true if a focused button is found.
- Objectives: 2.3 — enables keyboard focus tracking for grid navigation

**Lines 31–37: `move(int dr, int dc)`**
- Arguments: `int dr` — row delta (−1/+1), `int dc` — column delta (−1/+1)
- Purpose: Calculates the new focused position using `get_current_position()`, checks it against the grid bounds to prevent moving off-screen, then moves focus to the target button.
- Objectives: 2.3 — arrow key navigation across the 3×2 button grid

**Lines 39–51: `openApp(int row, int col)`**
- Arguments: `int row` (0–1), `int col` (0–2) — grid position of the button to launch
- Purpose: Opens the app specified by the grid position, by converting the row and column into a linear index and running the appropriate app start function.
- Objectives: 2.3 — launches any of the 6 calculator applications from the keypad

**Lines 52–68: `key_event_handler(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event fired on the focused button
- Purpose: Routes arrow keys to `move()`, Enter to `openApp()`, and the settings key to `settings_app_start()`.
- Objectives: 2.3 — routes all keypad inputs to the correct menu action

**Lines 69–75: `click_event_handler(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL clicked event on a button
- Purpose: Identifies which button was clicked by comparing the event target against each button in the grid, then opens the corresponding app.
- Objectives: 2.3 — supports mouse/touch input on the PC build

**Lines 76–144: `main_menu_create()`**
- Arguments: None
- Purpose: Clears the screen and creates the 6 coloured app buttons with white labels, centred on the display. Adds a white border highlight for the focused button. Automatically focuses button 0 and adds the hint bar at the bottom.
- Objectives: 2.2 — displays all 6 apps clearly on the 320×240 LCD; 2.3 — sets up full keyboard focus management

---

## 6. mechanics.c — 287 lines

**Purpose:** Implements the SUVAT kinematics solver, which covers the mechanics topic found in both A-Level Maths and Physics. The user enters any three or more of the five kinematic variables — displacement (s), initial velocity (u), final velocity (v), acceleration (a), and time (t) — and the solver works out all remaining unknowns. Rather than implementing a separate solver for each combination of known variables, the algorithm iteratively applies all 15 rearrangements of the four SUVAT equations in repeated passes until all five values are determined, which keeps the code compact and handles every valid input combination automatically. The result screen also displays the four SUVAT equations as a reference, which is useful for students who want to understand which equation was used to find each unknown.

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

<!-- **Lines 31–33: `get_sv(SUVAT *sv, int i)`**
- Arguments: `SUVAT *sv` — pointer to SUVAT struct; `int i` — variable index (0=s, 1=u, 2=v, 3=a, 4=t)
- Purpose: Returns the value of the i-th SUVAT variable using a switch statement for index-based access.
- Objectives: 8.1 — provides array-like access to the five kinematic variables

**Lines 35–37: `set_sv(SUVAT *sv, int i, double v)`**
- Arguments: `SUVAT *sv` — pointer to SUVAT struct; `int i` — variable index; `double v` — value to assign
- Purpose: Sets the i-th SUVAT variable by index, enabling loop-based population of the struct.
- Objectives: 8.1 — enables programmatic writing of solved SUVAT values -->

**Lines 39–47: `count_known(SUVAT *sv)`**
- Arguments: `SUVAT *sv` — pointer to SUVAT struct
- Purpose: Counts how many of the 5 SUVAT variables are not equal to `SUVAT_UNKNOWN`. Used to check whether the user has entered enough values before attempting to solve.
- Objectives: 8.2 — used to enforce the minimum 3-input validation rule

**Lines 49–89: `solve_suvat(SUVAT *sv)`**
- Arguments: `SUVAT *sv` — pointer to SUVAT struct (modified in-place)
- Purpose: Iteratively applies all 15 rearrangements of the SUVAT equations to fill in any unknown variables. Repeats up to 10 times, stopping early if no new variables are solved in a pass. Returns the number of known variables at the end.
- Objectives: 8.1 — implements iterative constraint propagation to solve all SUVAT equations

**Lines 94–122: `do_calculate()`**
- Arguments: None (reads from `input_fields[]` textareas)
- Purpose: Reads all 5 input fields and treats empty ones as unknown. Validates that at least 3 values are provided, then calls `solve_suvat()`. Displays an appropriate error message if the inputs are insufficient or contradictory, otherwise shows the result page.
- Objectives: 8.2 — enforces the 3-input minimum; 3.2 — displays error messages for invalid inputs

**Lines 131–171: `mech_textarea_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event fired on a SUVAT input textarea
- Purpose: Handles all keypad input on the SUVAT input page. Up and down move focus between the 5 fields. Enter triggers the calculation. ESC clears all fields and resets the solver state.
- Objectives: 2.3 — full keypad control of the SUVAT input page

**Lines 183–233: `show_input_page()`**
- Arguments: None
- Purpose: Creates the SUVAT input screen with 5 labelled textareas for s, u, v, a, and t. Restores any previously entered values. Adds an error label for validation messages and focuses the first field.
- Objectives: 2.2 — displays all 5 SUVAT input fields clearly on the LCD; 8.1 — presents the standard kinematic variable layout

**Lines 235–274: `show_result_page()`**
- Arguments: None
- Purpose: Displays the solved SUVAT values on a results screen, each formatted as e.g. `"s (m) = 12.3456"`. Also shows a compact reference of the four SUVAT equations at the bottom of the screen.
- Objectives: 2.2 — clearly displays all solved values on the LCD; 6.2 — all correct solutions are shown on screen

<!-- **Lines 276–287: `mechanics_app_start()`**
- Arguments: None
- Purpose: Resets all 5 SUVAT fields to `SUVAT_UNKNOWN`. Nulls `mech_group`. Resets `focused_field = 0`. Calls `show_input_page()`.
- Objectives: 8.1 — initialises the SUVAT solver in a clean state -->

---

## 7. numerical_methods.c — 491 lines

**Purpose:** Implements the Numerical Methods application, which groups three distinct numerical analysis tools into a single app accessible from the main menu. The three tools are: Curve Fitting, which performs least-squares linear regression on a user-supplied dataset and reports the line of best fit along with the R² value; Numerical Integration, which evaluates a definite integral of any user-entered function using Simpson's 1/3 rule with up to 10,000 steps; and Newton-Raphson root finding, which iteratively converges on a root of f(x)=0 from a user-supplied initial guess using the central difference method to estimate the derivative. All three tools accept expressions entered via the same keypad function buttons as the rest of the calculator, so the user does not need to learn a different input system.

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

<!-- **Lines 24–30: `cleanup_nm_ui()`**
- Arguments: None
- Purpose: If `nm_group` is non-null, calls `ui_submenu_cleanup(nm_group)` and nulls both `nm_group` and `nm_key_recv`. Prevents LVGL memory leaks when switching between NM sub-screens.
- Objectives: General — ensures clean screen transitions within the NM module -->

<!-- **Lines 32–50: `setup_nm_screen(const char *hint)`**
- Arguments: `const char *hint` — hint bar text string
- Purpose: Calls `cleanup_nm_ui()`, cleans active screen, calls `ui_setup_screen()`. Creates `nm_group`, hidden `nm_key_recv` (0×0 object added to group). Creates hint bar with provided text. Sets indev group. Returns the active screen pointer.
- Objectives: 2.2 — standard screen setup with hint bar for all NM sub-screens -->

**Lines 52–68: `parse_csv(const char *str, double *out, int max)`**
- Arguments: `const char *str` — comma- or space-separated number string; `double *out` — output array; `int max` — maximum values to parse
- Purpose: Parses a string of space- or comma-separated numbers into a double array. Returns the count of successfully parsed values, allowing the caller to validate the input before using it.
- Objectives: 7.3 — enables user to input x/y data lists for curve fitting

**Lines 74–116: `do_curve_fit()`**
- Arguments: None (reads from `cf_x_field` and `cf_y_field` textareas)
- Purpose: Reads the x and y data from both fields and validates that there are at least 2 matching pairs. Performs least-squares linear regression to compute the slope, intercept, and R² value, then displays the result as `"y = mx ± c"`.
- Objectives: 7.3 — performs least-squares linear regression and reports R²

**Lines 118–159: `cf_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a curve fitting textarea
- Purpose: Handles keypad input on the curve fitting screen. Enter triggers the regression calculation. The FRAC key inserts a space to act as a CSV separator between values. ESC returns to the numerical methods menu.
- Objectives: 2.3 — full keypad control of curve fitting input; 7.3 — triggers the regression calculation

**Lines 161–199: `show_curve_fitting()`**
- Arguments: None
- Purpose: Creates the curve fitting input screen with two data fields for x and y values, and a result label. Placeholder text shows the expected format of space-separated numbers.
- Objectives: 7.3 — builds the curve fitting input screen; 2.2 — displays fields clearly on LCD

**Lines 207–245: `do_integration()`**
- Arguments: None (reads from `ni_expr_field`, `ni_a_field`, `ni_b_field`, `ni_n_field`)
- Purpose: Reads f(x), the lower and upper bounds, and the number of steps. Applies Simpson's 1/3 rule to numerically integrate f(x) from a to b, using alternating coefficients of 4 and 2 for the inner terms. Displays the result with the number of steps used, or an error if the result is invalid.
- Objectives: 7.2 — implements Simpson's rule for definite numerical integration; 1.2 — uses sufficient steps (up to 10000) for 6+ significant figure accuracy

**Lines 247–283: `ni_textarea_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on any numerical integration textarea
- Purpose: Handles keypad input on the integration screen. Enter triggers the calculation. Function keys like `'s'` and `'r'` insert the corresponding function string into the textarea. ESC returns to the numerical methods menu.
- Objectives: 2.3 — keypad drives all integration input; 1.1 — supports trig/power functions in the integrand

**Lines 285–346: `show_numerical_integration()`**
- Arguments: None
- Purpose: Creates the numerical integration screen with input fields for the expression, lower bound, upper bound, and step count. All fields share the same key callback.
- Objectives: 7.2 — builds the numerical integration UI; 2.2 — displays fields clearly on LCD

**Lines 351–354: `numerical_derivative(const char *expr, double x)`**
- Arguments: `const char *expr` — expression string; `double x` — point at which to differentiate
- Purpose: Estimates the derivative of f(x) at a given point using the central difference method: `(f(x+h) − f(x−h)) / (2h)` with h = 1e-5.
- Objectives: 7.1 — provides the derivative estimate needed by Newton-Raphson

**Lines 356–372: `newton_raphson(const char *expr, double x0, double *result, int *iters)`**
- Arguments: `const char *expr` — f(x) expression; `double x0` — initial guess; `double *result` — output root; `int *iters` — output iteration count
- Purpose: Iteratively refines an initial guess using the Newton-Raphson formula `x = x − f(x)/f'(x)`. Stops when the change between iterations is below 1e-8, or after 100 iterations. Returns 1 on convergence, 0 otherwise.
- Objectives: 7.1 — core Newton-Raphson iterative root-finding algorithm

**Lines 376–387: `solve_nr_fn(lv_obj_t **f, int n, lv_obj_t *res)`**
- Arguments: `lv_obj_t **f` — array of field pointers (f[0]=expr textarea, f[1]=x0 textarea); `int n` — field count (2); `lv_obj_t *res` — result label
- Purpose: Reads the expression and initial guess from the input fields, then calls `newton_raphson()`. Displays the root and iteration count on success, or a no-convergence message on failure.
- Objectives: 7.1 — shows the Newton-Raphson result clearly; 3.2 — displays failure message when no convergence

**Lines 391–433: `nr_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a Newton-Raphson textarea
- Purpose: Handles keypad input on the Newton-Raphson screen. Works the same as `ni_textarea_key_cb` but Enter calls `solve_nr_fn()` instead. Also supports `x` and `^2` insertion from their respective keys.
- Objectives: 2.3 — keypad drives Newton-Raphson input; 1.1 — supports trig and power notation in f(x)

**Lines 435–473: `show_newton_raphson()`**
- Arguments: None
- Purpose: Creates the Newton-Raphson input screen with a field for f(x) and a field for the initial guess x0, plus a result label below. Placeholder text guides the user with an example expression.
- Objectives: 7.1 — builds the Newton-Raphson input screen; 2.2 — displays input fields clearly on LCD

<!-- **Lines 483–487: `show_nm_menu()`**
- Arguments: None
- Purpose: Calls `cleanup_nm_ui()`. Calls `ui_create_submenu(nm_menu_items, 3, &style, main_menu_create)` where style uses `COL_ACCENT_NM` and `COL_FOCUS_BG_NM`.
- Objectives: General — navigation hub for the three numerical methods tools -->

<!-- **Lines 489–491: `numerical_methods_app_start()`**
- Arguments: None
- Purpose: Calls `show_nm_menu()`. Entry point called by `main_menu.c`.
- Objectives: 7.1, 7.2, 7.3 — launches the full Numerical Methods application -->

---

## 8. settings.c — 338 lines

**Purpose:** Manages all global calculator settings and exposes them through a public API that the other application modules call into. The three configurable settings are the angle mode (RAD or DEG, which controls how trig functions in `expr_eval.c` interpret their arguments), the number of decimal places for displayed results (Auto or 1–10 fixed), and the values of the nine user-defined variables A through F and x, y, z. All state is held in static memory and persists across app switches for the duration of a session, so switching from the graph app to the math app and back does not reset the user's settings. The settings module is accessible from any screen at any time via the CONST key, without having to return to the main menu first.

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

<!-- ### Public API (lines 14–26)

| Function | Line | Arguments | Purpose |
|----------|------|-----------|---------|
| `settings_get_angle_mode()` | 14 | None | Returns current `angle_mode` |
| `settings_set_angle_mode(m)` | 15 | `AngleMode m` | Sets `angle_mode = m` |
| `settings_get_decimal_places()` | 17 | None | Returns `decimal_places` |
| `settings_set_decimal_places(dp)` | 18 | `int dp` | Clamps to 0–10, sets `decimal_places` |
| `settings_get_variable(idx)` | 20 | `int idx` (0–8) | Bounds-checked getter for `user_vars` |
| `settings_set_variable(idx, v)` | 21 | `int idx`, `double v` | Bounds-checked setter for `user_vars` |
| `settings_get_variable_name(idx)` | 22 | `int idx` | Returns `'A'`–`'F'` for 0–5, `'x'`/`'y'`/`'z'` for 6/7/8 | -->

### Functions

**Lines 40–51: `angle_update()`**
- Arguments: None (reads globals `angle_sel`, `angle_labels[]`)
- Purpose: Highlights the currently selected angle mode label and sets all others to a transparent background, providing visual feedback of the current selection.
- Objectives: 1.1 — visual feedback for angle mode selection which directly affects trig functions

**Lines 53–74: `angle_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the angle page hidden receiver
- Purpose: Handles keypad input on the angle mode selection page. Up and down move the selection between RAD and DEG. Enter saves the selection and returns to the settings menu.
- Objectives: 1.1 — allows switching between radians and degrees for trig calculations; 2.3 — keypad-driven selection

**Lines 76–122: `show_angle_page()`**
- Arguments: None
- Purpose: Creates the angle mode selection screen with two labelled options for Radians and Degrees. Pre-selects the current mode and adds a hint bar at the bottom.
- Objectives: 1.1 — presents RAD/DEG selection; 2.2 — LCD display of angle mode page

**Lines 139–160: `dec_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the decimal places page hidden receiver
- Purpose: Handles keypad input on the decimal places page. Up and down scroll through the 11 options (Auto plus 1–10). Enter saves the selection and returns to the settings menu.
- Objectives: 1.2 — controls output precision; 2.3 — keypad navigation

**Lines 162–215: `show_decimals_page()`**
- Arguments: None
- Purpose: Creates the decimal places selection screen with 11 labelled options: Auto and 1–10 fixed decimal places. Pre-selects the current value and adds a hint bar.
- Objectives: 1.2 — allows selecting from Auto or 1–10 decimal places; 2.2 — LCD display of options

**Lines 229–268: `var_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a variable editor textarea
- Purpose: Handles keypad input on the variable editor page. Up and down move focus between the 9 variable fields. Enter saves all non-empty fields into `user_vars[]` and returns to the settings menu. ESC discards changes.
- Objectives: 1.3 — saves user-defined variables to memory for use in expressions

**Lines 270–319: `show_variables_page()`**
- Arguments: None
- Purpose: Creates the variable editor screen with a labelled textarea for each of the 9 variables (A–F, x, y, z). Pre-fills any variables that are already set to a non-zero value.
- Objectives: 1.3 — displays all 9 user variables (A–F, x/y/z) for editing; 2.2 — clear LCD display

**Lines 321–325: `settings_items[]`**
- 3 `SubMenuItem`s: `{"Angle Unit (RAD/DEG)", show_angle_page}`, `{"Decimal Places", show_decimals_page}`, `{"Variable Editor (A-F, x/y/z)", show_variables_page}`
- Objectives: General — defines the settings submenu entries

<!-- **Lines 327–334: `show_settings_menu()`**
- Arguments: None
- Purpose: Creates a `SubMenuStyle` with `COL_ACCENT_SETTINGS` and `COL_FOCUS_BG_SETTINGS`. Calls `ui_create_submenu(settings_items, 3, &style, main_menu_create)`.
- Objectives: General — displays the top-level settings navigation list -->

<!-- **Lines 336–338: `settings_app_start()`**
- Arguments: None
- Purpose: Calls `show_settings_menu()`. Entry point called from any app via `'K'` key or the main menu.
- Objectives: 1.1, 1.2, 1.3 — entry point for all global calculator configuration -->

---

## 9. solver.c — 322 lines

**Purpose:** Implements the equation solver application, providing analytical solvers for linear equations in the form ax+b=0 and quadratic equations in the form ax²+bx+c=0. Both solvers are built on a shared `SolverCtx` infrastructure, which stores the input field pointers and a function pointer to the appropriate solve function, allowing the same key callback to trigger the correct calculation regardless of which solver is active. The quadratic solver correctly handles all three cases — two distinct real roots, one repeated root, and no real solutions — displaying an appropriate message for each. The module uses a submenu as its entry point, which makes it straightforward to add further solver types such as simultaneous equations or cubic equations in future without restructuring the UI.

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
- Purpose: Solves ax + b = 0. Returns 0 if a is effectively zero (no solution), otherwise computes `x = −b/a` and returns 1.
- Objectives: 6.1 — analytically solves the linear equation ax + b = 0

**Lines 34–50: `solve_quadratic(double a, double b, double c, double *x1, double *x2)`**
- Arguments: `double a`, `double b`, `double c` — coefficients; `double *x1`, `double *x2` — output roots
- Purpose: Solves ax² + bx + c = 0. Falls back to `solve_linear` if a is zero. Computes the discriminant and returns 0 for no real roots, 1 for a repeated root, or 2 for two distinct roots.
- Objectives: 6.1 — analytically solves the quadratic equation; 6.2 — correctly handles all three root cases

**Lines 91–100: `show_solver_menu()`**
- Arguments: None
- Purpose: Displays the solver submenu with two options: Linear and Quadratic. Uses the shared `ui_create_submenu` component with the solver's orange accent colour.
- Objectives: 6.1 — entry point presenting linear and quadratic solver options

**Lines 111–133: `sub_solver_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the solver's hidden key receiver
- Purpose: Enter triggers the solve by calling the function pointer stored in `active_ctx`. ESC returns to the solver menu. Uses a function pointer so the same callback works for both the linear and quadratic solver without duplicating logic.
- Objectives: 2.3 — keypad triggers solve via function pointer dispatch; 6.1 — generic solve handler

**Lines 142–180: `solver_handle_custom_key(uint32_t key)`**
- Arguments: `uint32_t key` — single-character LVGL key code
- Purpose: Maps special key codes to multi-character function insertions in the focused solver textarea. For example `'s'` inserts `"sin("` and `'r'` inserts `"sqrt("`. Returns 1 if the key was handled, 0 otherwise.
- Objectives: 1.1 — allows full use of mathematical functions and constants in solver expressions

**Lines 182–225: `solver_textarea_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a solver input textarea
- Purpose: Handles all keypad input within the solver coefficient fields. Up and down move between fields. Enter calls the solve function. Function keys like `'s'` and `'r'` are forwarded to `solver_handle_custom_key()`.
- Objectives: 2.3 — keypad handles all navigation and input within solver screens

**Lines 230–240: `solve_linear_fn(lv_obj_t **f, int n, lv_obj_t *res)`**
- Arguments: `lv_obj_t **f` — array of field pointers (f[0]=a, f[1]=b textareas); `int n` — field count (2); `lv_obj_t *res` — result label
- Purpose: Reads a and b from the input fields and calls `solve_linear()`. Displays the root or a "No solution" message if a equals zero.
- Objectives: 6.1 — solves ax + b = 0; 6.2 — displays solution clearly; 6.3 — shows error for degenerate case

**Lines 242–270: `show_linear_solver()`**
- Arguments: None
- Purpose: Creates the linear solver screen with two input fields for a and b, and a result label. Sets the active context so that Enter triggers `solve_linear_fn`.
- Objectives: 6.1 — builds the linear solver UI; 2.2 — clearly displays input fields on LCD

**Lines 275–289: `solve_quad_fn(lv_obj_t **f, int n, lv_obj_t *res)`**
- Arguments: `lv_obj_t **f` — array of field pointers (f[0]=a, f[1]=b, f[2]=c textareas); `int n` — field count (3); `lv_obj_t *res` — result label
- Purpose: Reads a, b, c from the input fields and calls `solve_quadratic()`. Displays "No real solutions", a single root, or both roots depending on the discriminant.
- Objectives: 6.1 — solves ax² + bx + c = 0; 6.2 — shows all roots; 6.3 — clear no-solution message

**Lines 291–320: `show_quadratic_solver()`**
- Arguments: None
- Purpose: Creates the quadratic solver screen with three input fields for a, b, and c, and a taller result label to accommodate two-line output. Sets the active context so that Enter triggers `solve_quad_fn`.
- Objectives: 6.1 — builds the quadratic solver UI; 2.2 — displays all 3 coefficient fields clearly

<!-- **Line 322: `solver_app_start()`**
- Arguments: None
- Purpose: Calls `show_solver_menu()`. Entry point called by `main_menu.c`.
- Objectives: 6.1, 6.2, 6.3 — launches the equation solver application -->

---

## 10. stats.c — 487 lines

**Purpose:** Implements the statistical distributions application, covering all the probability distributions required by the A-Level Maths and Further Maths syllabus. The seven available calculations are the Binomial PMF and CDF, the Normal PDF, CDF, and Inverse Normal, and the Poisson PMF and CDF. All seven share a common input layout built by a single `create_dist_screen()` function, which takes the title, parameter labels, and a calculation function pointer as arguments — this means adding a new distribution in future only requires writing the calculation function and registering it with the submenu, without duplicating any UI code. The statistical algorithm functions are all implemented from first principles in pure C, including an Abramowitz & Stegun polynomial approximation for the error function used by the normal CDF, since the standard library `erf()` is not reliably available on all embedded targets.

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

<!-- **Lines 79–85: `factorial(int n)`**
- Arguments: `int n` — non-negative integer
- Purpose: Returns NAN for n < 0. Returns 1.0 for n ≤ 1. Iterates product from 2 to n. Used internally for Poisson and binomial calculations.
- Objectives: 4.1, 4.3 — required by binomial coefficient and Poisson PMF -->

**Lines 87–93: `binomial_coeff(int n, int k)`**
- Arguments: `int n` — number of trials; `int k` — number of successes
- Purpose: Computes C(n,k) using an iterative multiply-divide approach to avoid large intermediate values. Returns 0 for out-of-range inputs and 1 for the edge cases k=0 or k=n.
- Objectives: 4.1 — computes C(n,k) for the binomial PMF

**Lines 95–99: `binomial_pmf(int n, int k, double p)`**
- Arguments: `int n` — trials; `int k` — successes; `double p` — probability of success
- Purpose: Validates all inputs and computes P(X=k) using the binomial formula C(n,k) × p^k × (1−p)^(n−k).
- Objectives: 4.1 — calculates P(X=k) for the binomial distribution

**Lines 101–109: `binomial_cdf(int n, int k, double p)`**
- Arguments: `int n` — trials; `int k` — maximum successes; `double p` — probability
- Purpose: Computes cumulative P(X≤k) by summing the PMF from 0 to k. Returns 1.0 immediately if k ≥ n to avoid unnecessary iterations.
- Objectives: 4.1 — calculates cumulative P(X≤k) for the binomial distribution

**Lines 111–115: `normal_pdf(double x, double mu, double sigma)`**
- Arguments: `double x` — value; `double mu` — mean; `double sigma` — standard deviation
- Purpose: Evaluates the normal probability density function at x. Returns NAN if σ is not positive.
- Objectives: 4.2 — evaluates the normal probability density function

**Lines 117–124: `erf_approx(double x)`**
- Arguments: `double x` — input value
- Purpose: Approximates the error function using the Abramowitz & Stegun polynomial method. Used internally by `normal_cdf()` as a substitute for the standard library erf, which is not available on all targets.
- Objectives: 4.2 — provides the error function needed by `normal_cdf()`

**Lines 126–129: `normal_cdf(double x, double mu, double sigma)`**
- Arguments: `double x` — value; `double mu` — mean; `double sigma` — standard deviation
- Purpose: Computes the cumulative normal probability P(X≤x) using the error function approximation. Returns NAN if σ is not positive.
- Objectives: 4.2 — calculates cumulative P(X≤x) for the normal distribution

**Lines 131–138: `erf_inv_approx(double x)`**
- Arguments: `double x` — value in (−1, 1)
- Purpose: Approximates the inverse error function using a closed-form algebraic method. Used internally by `normal_inv()`. Returns NAN for inputs outside (−1, 1).
- Objectives: 4.2 — provides the inverse error function needed by `normal_inv()`

**Lines 140–144: `normal_inv(double p, double mu, double sigma)`**
- Arguments: `double p` — cumulative probability in (0,1); `double mu` — mean; `double sigma` — standard deviation
- Purpose: Computes the inverse normal (quantile), i.e. the value x such that P(X≤x) = p. Returns NAN if the inputs are out of range.
- Objectives: 4.2 — calculates the inverse normal (quantile) function

**Lines 432–435: `poisson_pmf(double lambda, int k)`**
- Arguments: `double lambda` — mean rate λ; `int k` — number of events
- Purpose: Computes P(X=k) for the Poisson distribution using the formula e^(−λ) × λ^k / k!. Returns NAN for invalid inputs.
- Objectives: 4.3 — calculates P(X=k) for the Poisson distribution

**Lines 437–443: `poisson_cdf(double lambda, int k)`**
- Arguments: `double lambda` — mean rate λ; `int k` — maximum events
- Purpose: Computes cumulative P(X≤k) for the Poisson distribution by summing the PMF from 0 to k. Returns NAN or 0 for invalid inputs.
- Objectives: 4.3 — calculates cumulative P(X≤k) for the Poisson distribution

### UI Functions

**Lines 204–238: `stats_textarea_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on a distribution parameter textarea
- Purpose: Handles keypad input on all distribution parameter screens. Up and down move between the 3 input fields. Enter triggers the distribution calculation via the function pointer stored in the active context. ESC returns to the stats menu.
- Objectives: 2.3 — keypad-driven input and calculation trigger for all distribution screens

**Lines 254–265: `show_stats_menu()`**
- Arguments: None
- Purpose: Displays the statistics submenu with all 7 distribution options using the shared submenu component with the stats green accent colour.
- Objectives: 4.1, 4.2, 4.3 — presents all 7 distribution options

**Lines 270–314: `create_dist_screen(...)`**
- Arguments: title string, 3 parameter label strings, calculation function pointer, field output array, context struct pointer
- Purpose: Generic screen builder used by all 7 distributions. Creates a title label, 3 parameter input fields with their labels, and a result label. Stores the field pointers and calc function into the context struct so `stats_textarea_key_cb` can call the right calculation on Enter.
- Objectives: 4.1, 4.2, 4.3 — generic UI builder ensuring consistent 3-parameter layout across all 7 distributions; 2.2 — standard layout displayed clearly on LCD

**Lines 320–337: Binomial PMF screen**
- `calc_binomial_pmf` — reads n, k, p from the input fields and calls `binomial_pmf()`. Displays the result or an error.
- `show_binomial_pmf` — calls `create_dist_screen` to build the Binomial P(X=k) input screen.
- Objectives: 4.1 — displays P(X=k) for the binomial distribution

**Lines 340–360: Binomial CDF screen**
- `calc_binomial_cdf` — reads n, k, p and calls `binomial_cdf()`. Displays the cumulative result.
- `show_binomial_cdf` — calls `create_dist_screen` to build the Binomial CDF screen.
- Objectives: 4.1 — displays cumulative P(X≤k) for the binomial distribution

**Lines 363–383: Normal PDF screen**
- `calc_normal_pdf` — reads x, μ, σ and calls `normal_pdf()`. Displays the probability density.
- `show_normal_pdf` — calls `create_dist_screen` to build the Normal PDF screen.
- Objectives: 4.2 — displays normal probability density

**Lines 386–406: Normal CDF screen**
- `calc_normal_cdf` — reads x, μ, σ and calls `normal_cdf()`. Displays P(X≤x).
- `show_normal_cdf` — calls `create_dist_screen` to build the Normal CDF screen.
- Objectives: 4.2 — displays cumulative normal probability P(X≤x)

**Lines 409–429: Inverse Normal screen**
- `calc_inverse_normal` — reads p, μ, σ and calls `normal_inv()`. Displays the quantile value.
- `show_inverse_normal` — calls `create_dist_screen` to build the Inverse Normal screen.
- Objectives: 4.2 — implements inverse normal (quantile) function

**Lines 432–463: Poisson PMF screen**
- `calc_poisson_pmf` — reads λ and k and calls `poisson_pmf()`. Displays P(X=k).
- `show_poisson_pmf` — calls `create_dist_screen` to build the Poisson P(X=k) screen.
- Objectives: 4.3 — displays P(X=k) for Poisson distribution

**Lines 465–483: Poisson CDF screen**
- `calc_poisson_cdf` — reads λ and k and calls `poisson_cdf()`. Displays cumulative P(X≤k).
- `show_poisson_cdf` — calls `create_dist_screen` to build the Poisson CDF screen.
- Objectives: 4.3 — displays cumulative P(X≤k) for Poisson distribution

<!-- **Lines 485–487: `stats_app_start()`**
- Arguments: None
- Purpose: Calls `show_stats_menu()`. Entry point called by `main_menu.c`.
- Objectives: 4.1, 4.2, 4.3 — launches the full statistics application -->

---

## 11. ui_submenu.c — 129 lines

**Purpose:** Implements a reusable vertical list submenu component that is shared across four applications — Settings, Solver, Stats, and Numerical Methods — to provide a consistent selection UI without duplicating code. Each submenu is created by passing an array of `SubMenuItem` structs (each containing a label string and a callback function pointer), a style struct specifying the accent colour and hint text, and a back-navigation callback. The component handles all the rendering, keyboard navigation, highlight updates, and memory cleanup internally, so each app only needs to define its list of items and the component does the rest. This directly demonstrates the DRY (Don't Repeat Yourself) principle, as without this component the same 100+ lines of list-building and navigation code would need to appear separately in four different source files.

**Includes:** `ui_submenu.h`, `ui_common.h`, `input_hal.h`, `string.h`

**Objectives met:** 2.2 (submenu displays list clearly on LCD), 2.3 (keypad navigation: Up/Down + Enter), General design — eliminates duplicated code across 4 apps

### Data Structures

| Name | Line | Description |
|------|------|-------------|
| `SubMenuCtx` struct | 6–15 | Internal state: `{const SubMenuItem *items, int item_count, int selection, void (*on_menu)(void), lv_obj_t **labels, lv_obj_t *key_recv, lv_group_t *group, lv_color_t focus_bg}` |
| `active_submenu` | 17 | Static pointer — only one submenu active at a time |

### Functions

<!-- **Lines 19–28: `update_highlight(SubMenuCtx *ctx)`**
- Arguments: `SubMenuCtx *ctx` — pointer to the current submenu context
- Purpose: Iterates all `ctx->item_count` labels. Sets background of `ctx->labels[ctx->selection]` to `ctx->focus_bg` with full opacity. Sets all other labels to transparent background.
- Objectives: 2.2 — provides clear visual highlight of the currently selected menu item on the LCD -->

**Lines 30–61: `submenu_key_cb(lv_event_t *e)`**
- Arguments: `lv_event_t *e` — LVGL key event on the hidden key receiver
- Purpose: Handles up/down navigation between menu items and highlights the selected one. Enter calls the callback for the selected item. The menu key returns to the main menu.
- Objectives: 2.3 — provides Up/Down/Enter keypad navigation for all submenus

**Lines 63–118: `ui_create_submenu(...)`**
- Arguments: array of menu items with labels and callbacks, item count, style struct with accent colour and hint text, and an on-menu callback
- Purpose: Generic submenu builder used by Settings, Solver, Stats, and Numerical Methods. Creates a vertically stacked list of labelled items, sets up keyboard navigation, and highlights the first item. Cleans up any previously active submenu before building the new one.
- Objectives: 2.2 — consistent list display across all 4 apps; 2.3 — shared keyboard navigation infrastructure

<!-- **Lines 120–129: `ui_submenu_cleanup(lv_group_t *group)`**
- Arguments: `lv_group_t *group` — the group to clean up
- Purpose: Checks `active_submenu != NULL && active_submenu->group == group`. Frees labels array via `lv_free()`. Calls `lv_group_del(group)`. Frees `active_submenu` context. Sets `active_submenu = NULL`.
- Objectives: General — memory-safe teardown preventing LVGL leaks when switching screens -->

---

## 12. desktop/src/main.c — 509 lines

**Purpose:** Implements the desktop PC entry point for the calculator, setting up the full SDL2 environment that simulates the embedded hardware on a standard Windows or Linux machine. It creates two separate SDL2 windows — a 320×240 display window that renders the calculator UI exactly as it would appear on the physical LCD, and a 320×270 keypad window that shows the 9×6 on-screen button grid which the user can click with a mouse. Both windows share the same LVGL instance, and key events from either the on-screen buttons or the physical keyboard are routed through a unified key dispatch path before being sent to the focused LVGL group. The button layout and keyboard mapping are loaded from `button_keycodes.txt` at runtime, meaning the key bindings can be changed without recompiling the application.

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

**Purpose:** Implements the desktop version of the input hardware abstraction layer defined in `input_hal.h`. It provides the three functions that the rest of the application calls to interact with input hardware — storing and retrieving the LVGL keyboard indev pointer, and acquiring/releasing an LVGL mutex lock. On the desktop build, the lock and unlock functions are no-ops since the application is single-threaded, but on the ESP32 build they wrap FreeRTOS mutex calls to make the LVGL task thread-safe. This file is the only place where the desktop and ESP32 builds differ in terms of input handling, which is the entire point of the abstraction layer.

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
