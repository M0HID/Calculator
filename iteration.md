# ITERATION LOG

Chronological record of calculator design iterations: what was built in `src/`, how it was tested, what bugs were found, how they were fixed, and what was confirmed working after.

---

## Iteration 1 — Math App: Expression Evaluator + Input Textbox

### Built
- `expr_eval.c` — recursive descent parser for mathematical expressions.
  - Operators: `+`, `-`, `*`, `/`, `^`, parentheses.
  - Functions: `sin`, `cos`, `tan`, `sqrt`, `log`, `ln`.
  - Variables: `x`, `y`, `z`.
- `calc_math.c` — Math app UI: LVGL textarea for input, scrollable history list showing previous expressions and results.

### Tested
- Typed expressions via keyboard on desktop build.
- Confirmed results appeared in history: `2+2 = 4`, `sqrt(144) = 12`, `sin(3.14159) ≈ 0`.

### Bug Found 1: `isnan` / `isinf` undeclared
- Build error: `implicit declaration of function 'isnan'` in `expr_eval.c`.
- Cause: `<math.h>` not included.

### Fixed 1 — `expr_eval.c`
- Added `#include <math.h>` at the top of `expr_eval.c`.

### Bug Found 2: `NAN` undeclared
- Follow-up build error: `'NAN' undeclared`.
- Cause: include ordering placed `expr_eval.h` before `math.h`, so `NAN` was not yet defined when referenced.

### Fixed 2 — `expr_eval.c`
- Reordered includes so `math.h` is included before any usage of `NAN`, `INFINITY`, `isnan`, `isinf`.

### Retested
- `sin(3.14159)`, `2^8`, `sqrt(144)`, `log(100)` all evaluated correctly.
- History list scrolled and displayed results cleanly.

---

## Iteration 2 — Virtual Keypad: Button Labels Showing Coordinates

### Built
- On-screen virtual keypad rendered as an LVGL button matrix, mirroring the physical calculator layout.
- `button_keymap.c` loads key labels and keycodes from `button_keycodes.txt` at runtime and maps each button to an LVGL logical key.

### Tested
- Ran the desktop build and opened the calculator.

### Bug Found: All buttons labelled `0,0`, `0,1`, `0,2`…
- Console warning: `button_keycodes.txt not found, using default keymap`.
- Root cause: the binary runs from `desktop/build/bin/` but the config file was only in the project root; the loader searched only the current working directory.

### Fixed — `button_keymap.c`
- Added fallback path search: `./button_keycodes.txt`, `../button_keycodes.txt`, and the binary's own directory.
- CMake `configure_file` step added to copy `button_keycodes.txt` into the build output folder automatically on each build.

### Retested
- Config file found on first path attempt; all buttons showed correct labels and fired correct keycodes.

---

## Iteration 3 — Main Menu: App Tiles Not Responding to Clicks

### Built
- `main_menu.c` — 3×2 grid of app tiles, each launching one of the six apps on click.
- Keypad widget repositioned below the calculator display area; arrow keys resized to half-width to match physical layout.

### Tested
- Ran desktop build and clicked app tiles.

### Bug Found: Clicking app tiles did nothing after keypad repositioning
- All six tiles stopped registering click events.
- Root cause: the keypad container's bounding box overlapped the menu grid in the LVGL widget tree, absorbing click/touch events before they reached the tiles underneath.

### Fixed — `main_menu.c`
- Corrected container sizing and z-order so the keypad occupies a non-overlapping region below the menu.
- Confirmed `lv_obj_add_flag(tile, LV_OBJ_FLAG_CLICKABLE)` was set on each tile after the layout change.

### Retested
- All six tiles clickable; each correctly launched its respective app.

---

## Iteration 4 — Graph App, Solver, Stats, Mechanics

### Built

#### `graph.c` — Graph Plotter
- Function list screen: four formula slots (y1–y4), each editable in a dedicated textarea.
- Graph view: plots all active functions simultaneously with labelled axes and tick marks.
- Coordinate transform: screen pixel ↔ mathematical coordinate (`graph.c:74-75`).
- Trace mode: a cursor tracks along the plotted curve, displaying the current (x, y) value.
- Zoom and pan: `+`/`-` keys scale the axes; arrow keys translate the viewport.

#### `stats.c` — Statistics App
- Binomial distribution: PMF `P(X = k)` and CDF `P(X ≤ k)`.
- Normal distribution: PDF, CDF, and inverse normal (quantile function).
- Separate input/result screen per distribution.

#### `solver.c` — Equation Solver
- Linear solver: `ax + b = 0` → `x = -b/a`.
- Quadratic solver: `ax² + bx + c = 0` with discriminant check for real/complex roots.
- Newton-Raphson iterative solver for arbitrary `f(x) = 0`.
- Numerical derivative via central difference: `f'(x) ≈ (f(x+h) - f(x-h)) / 2h`.

#### `mechanics.c` — SUVAT Solver
- Accepts any 3 of {s, u, v, a, t} as inputs and solves for the remaining unknowns using the five SUVAT equations.

#### `ui_submenu.c` — Reusable Menu Component
- Vertical item list with highlight.
- Navigation: Up/Down to move, Enter to select, ESC to go back, M to return to main menu.
- Used by Solver, Stats, and Settings to avoid duplicated layout code.

### Tested
- Entered `sin(x)`, `x^2`, `1/x` into graph editor → graph rendered all three curves.
- Solver: `x² - 4 = 0` → `x = 2, x = -2`; `2x + 6 = 0` → `x = -3`; Newton-Raphson on `cos(x)` → `x ≈ 1.5708`.
- Stats: binomial PMF `P(X=5 | n=10, p=0.5)` → `0.2461`; normal CDF checked against known table values.
- Mechanics: given `u=0, a=9.8, t=3` → solved `v=29.4`, `s=44.1`.

### Bug Found 1: `undefined reference to stats_app_start`
- `stats.c` was written but not added to `CMakeLists.txt`, so the linker could not find the entry function.

### Fixed 1 — `desktop/CMakeLists.txt`
- Added `src/stats.c` (and `src/solver.c`, `src/mechanics.c`, `src/ui_submenu.c`) to the source list.

### Bug Found 2: `lv_event_send` type mismatch in `calc_math.c`
- `calc_math.c` was dispatching key events using `lv_event_send(obj, code, (void*)keycode)` — casting a `uint32_t` keycode directly to `void*`.
- LVGL v9 tightened the API; this caused type mismatch compiler errors.

### Fixed 2 — `calc_math.c`
- Replaced direct `lv_event_send` calls with the correct LVGL v9 input injection pattern using `lv_indev_t` and the event data structure.

### Bug Found 3: `sin(π)` returning `1.22461e-16` instead of `0`
- Floating-point representation of `M_PI` is not exact, so `sin(M_PI)` does not produce exactly zero.
- Separately: the `π` symbol was rendering as two box characters in the LVGL result label.

### Fixed 3 — `expr_eval.c` + font
- Added post-evaluation snap: if a result is within `1e-10` of an integer, it is rounded to that integer.
- Changed the result label font to `font_math_14` (custom-compiled with Greek Unicode range); confirmed the `π` UTF-8 sequence maps to a single glyph.

### Bug Found 4: `fabs` / `sqrt` implicitly declared in `solver.c`
- `solver.c` used `fabs()` and `sqrt()` without `#include <math.h>`, producing implicit declaration warnings that escalated to errors.

### Fixed 4 — `solver.c`, `mechanics.c`
- Added `#include <math.h>` to both files.
- Deleted the now-redundant stub files `src/math.c` and `src/math.h`; all math functionality lives in `expr_eval.c` and the standard library.

### Retested
- All five apps launched and navigated without crashing.
- `sin(x)`, `x^2`, `1/x` graphed cleanly.
- Solver roots correct for linear, quadratic, and Newton-Raphson cases.
- `sin(π)` now displays `0` instead of scientific notation.
- Stats distributions matched reference values.
- Mechanics SUVAT results correct.

---

## Iteration 5 — AC Key Conflict, `=` Not Logging, ANS Key Mislabelled

### Bug Found 1: AC key reset the graph instead of clearing the formula input
- In the function list editor, pressing AC triggered the graph-reset handler instead of clearing the active textarea.
- Root cause: both the graph view and the formula editor registered handlers for the same AC keycode with no check for which screen was currently active.

### Fixed 1 — `graph.c`
- Added a state guard in the AC handler: checks `graph_state == GRAPH_STATE_EDIT` and clears the textarea if true; otherwise falls through to reset the graph view.

### Bug Found 2: `=` button evaluated the expression but did not add it to history
- The history append logic was inside the `ENTER` key handler only; the `=` button sent a separate keycode that bypassed it.

### Fixed 2 — `calc_math.c`
- Extracted evaluation and history logging into a single `calc_math_evaluate_and_log()` function.
- Both the `=` keycode and the `ENTER` keycode now call this function.

### Bug Found 3: Answer-recall button labelled "END" instead of "ANS"
- Typo in the keypad label mapping: `"END"` was written instead of `"ANS"`.

### Fixed 3 — `button_keycodes.txt`
- Corrected the label string; regenerated the virtual keypad display.

### Retested
- AC clears formula textarea when in edit mode; resets graph when in graph view mode.
- Pressing `=` and pressing Enter both log the result to history.
- ANS button label correct; recalls last result into input.

---

## Iteration 6 — Unicode Math Symbols Rendering as Boxes; Solver UI Inconsistency

### Bug Found 1: Greek and math symbols (Σ, √, π, θ, σ, μ) rendering as empty boxes
- Affected: stats labels, solver screen headings, math app result display.
- Root cause: `lv_font_montserrat_14` only covers basic Latin (U+0020–U+007F); Greek and math Unicode code points were missing from the glyph table.

### Fixed 1 — `ui_common.h`, `font_math_12.c`, `font_math_14.c`
- Generated two custom fonts using the LVGL font converter tool, covering:
  - Greek alphabet: U+0391–U+03C9.
  - Math symbols: `√` (U+221A), `π` (U+03C0), `Σ` (U+03A3), `θ` (U+03B8), `≤`, `≥`.
- Added `UI_FONT_MATH` and `UI_FONT_MATH_SMALL` design tokens to `ui_common.h`; all affected labels updated to use these tokens.

### Bug Found 2: Each solver screen had a bespoke, inconsistent layout
- Linear, quadratic, and Newton-Raphson screens each had different label positions, button arrangements, and key handling.
- No consistent back/menu navigation — some screens had no way to return to the solver menu without restarting.

### Fixed 2 — `ui_submenu.c` / `ui_submenu.h`
- Implemented a reusable vertical menu component: scrollable list, highlighted selection, standardised Up/Down/Enter/ESC/M key handling.
- All three solver entry points, all stats distribution selections, and the settings page now use this component as their top-level navigation.

### Retested
- σ, μ, Σ rendered correctly on stats screens; π rendered as single glyph in math results.
- Solver menu navigated with Up/Down; Enter opened the correct solver screen; ESC returned to the solver submenu; M returned to main menu.
- Same navigation confirmed working in stats and settings submenus.

---

## Iteration 7 — Graph Formula Editor: Double-Character Insertion

### Bug Found 1: Special function buttons inserting doubled/garbled text
- Pressing the `sin(` shortcut button in the graph formula editor inserted `ssin(` or `sin(s` — the raw character `s` from the keydown event plus the full `sin(` string from the button handler. Both ran for the same key event.
- Same issue with `cos(`, `tan(`, `x^2`, `^`, and the variable keys (`x`, `y`, `z`).

### Fixed 1 — `graph.c` (formula editor key handler)
- Registered an `LV_EVENT_PREPROCESS_KEY` handler on the formula editor textarea.
- When a special key is detected (SIN, COS, TAN, SQRT, LOG, x², ^, VAR), the event is consumed before LVGL passes it to the textarea; only the handler-inserted string reaches the buffer.
- All other keys (digits, operators, backspace, etc.) pass through normally.

### Bug Found 2: Variable `x` inserted as uppercase `X`
- The VAR key sent `LV_KEY_X` (ASCII uppercase); the expression evaluator `expr_eval.c` only recognised lowercase `x` as a variable.

### Fixed 2 — `graph.c` (key intercept handler)
- Forced all variable key insertions (`x`, `y`, `z`) to lowercase before writing into the textarea.

### Bug Found 3: Pressing Enter in the formula editor did not exit edit mode
- The formula string was saved correctly, but the UI remained on the editor textarea; the function list screen was never restored.

### Fixed 3 — `graph.c`
- After saving the formula on Enter, explicitly calls `graph_show_function_list()` to transition back to the list view.

### Retested
- Typed `sin(x)+cos(x)^2` using a mix of shortcut buttons and direct key input; no doubled characters.
- `sin(` button → `sin(` only; `x^2` button → `^2` only; variable key → `x` lowercase.
- Enter saved formula and immediately returned to the function list.
- Graph plotted the saved formula correctly on the next CALC press.

---

## Iteration 8 — Settings App: Angle Mode Not Applied; Variable Keys on Expression Editors

### Bug Found 1: Changing angle mode in Settings had no effect on calculations
- Settings app had a DEG/RAD toggle, but `expr_eval.c` always evaluated `sin`, `cos`, `tan` in radians regardless of the setting.
- The settings value was stored locally in `settings.c` but never passed to the evaluator.

### Fixed 1 — `expr_eval.c`, `settings.c`
- Added `angle_mode` global variable to `expr_eval.c` with `expr_eval_set_angle_mode()` setter.
- When evaluating `sin`, `cos`, or `tan`, the evaluator checks `angle_mode`: if `ANGLE_DEG`, converts the argument to radians before calling the C math function.
- Settings app calls `expr_eval_set_angle_mode(ANGLE_DEG)` or `(ANGLE_RAD)` when the toggle changes.

### Bug Found 2: `x`, `y`, `z` variable keys not inserting in the Math app input
- The VAR key on the keypad was not mapped to a handler in `calc_math.c`, so pressing it did nothing in the expression input field.

### Fixed 2 — `calc_math.c`
- Added a VAR key case to the math app key handler; inserts `x` (or cycles `x → y → z`) into the input textarea, matching the behaviour in the graph formula editor.

### Retested
- `sin(90)` in DEG mode → `1.000`; `sin(90)` in RAD mode → `0.894`.
- `cos(0)` → `1.000` in both modes.
- Variable `x` inserted correctly into math app input; expression `2*x` with `x=3` → `6`.

---

## Iteration 9 — Solver App: Double-Character Input from Special Buttons

### Bug Found: Solver input fields producing `^2W^2` instead of `^2`
- The same double-insertion bug from Iteration 7 (graph editor) was present in all solver input textareas: pressing `x²` produced garbled strings like `^2W^2` or `^2^2`.
- Root cause: `solver.c` input screens used plain LVGL textareas with no `LV_EVENT_PREPROCESS_KEY` intercept; both the raw key character and the handler-inserted string wrote into the buffer.

### Fixed — `solver.c` (input screen key handlers)
- Applied the same `LV_EVENT_PREPROCESS_KEY` interception pattern used in `graph.c` to every solver input textarea.
- Special buttons (`^`, `^2`, `sin(`, `cos(`, `tan(`, `sqrt(`) all intercepted and suppressed before the textarea receives them; only the intended string is inserted.

### Retested
- `x²` button in solver coefficient field → `^2` only, no duplication.
- `sin(` button → `sin(` only.
- Tested linear solver: `2x + 4 = 0` → `x = -2.000` ✓
- Tested quadratic: `x² - 5x + 6 = 0` → `x = 3.000, x = 2.000` ✓
- Newton-Raphson: `x³ - x - 2 = 0` converged to `x ≈ 1.5214` ✓
