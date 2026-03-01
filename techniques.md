# Programming Techniques

## OOP Programming Techniques (implemented by me)

### Classes

Not applicable — project is written in pure C99. Structs with associated functions serve as the equivalent of classes.

### Class Structure Diagram

```
SubMenuCtx ──uses──> SubMenuItem[]
                 └──> SubMenuStyle
StatsCtx   ──uses──> lv_obj_t* param_labels[]
SolverCtx  ──uses──> lv_obj_t* input_fields[]
Parser     ──used by──> expr_eval functions
SUVAT      ──used by──> mechanics solver
```

### Methods

| Technique | File : Line |
|---|---|
| `eval_expression()` — evaluates math string via parser | `src/expr_eval.c:29` |
| `newton_raphson()` — iterative root finder | `src/solver.c:44` |
| `solve_quadratic()` — quadratic formula | `src/solver.c:76` |
| `numerical_derivative()` — central difference | `src/solver.c:35` |
| `suvat_solve()` — kinematics constraint propagation | `src/mechanics.c:49` |
| `binomial_pmf/cdf()` — probability distributions | `src/stats.c:87` |
| `normal_pdf/cdf()` — normal distribution | `src/stats.c:103` |
| `inverse_normal()` — quantile approximation | `src/stats.c:123` |
| `graph_plot()` — pixel-by-pixel canvas drawing | `src/graph.c:105` |

### Attributes

| Technique | File : Line |
|---|---|
| `Parser` struct — pos + input string | `src/expr_eval.c:29` |
| `HistoryEntry` struct — equation + result strings | `src/calc_math.c:14` |
| `Function` struct — equation string + enabled flag | `src/graph.c:28` |
| `SUVAT` struct — s, u, v, a, t fields | `src/mechanics.c:16` |
| `SubMenuItem` struct — label text + callback pointer | `src/ui_submenu.h:13` |
| `SubMenuStyle` struct — colours + hint text | `src/ui_submenu.h:19` |
| `SubMenuCtx` struct — full submenu runtime state | `src/ui_submenu.c:7` |
| `StatsCtx` struct — fields array + calc function pointer | `src/stats.c:184` |
| `SolverCtx` struct — fields array + solve function pointer | `src/solver.c:159` |
| `button_mapping_t` struct — row/col/key/label | `src/button_keymap.c:6` |
| `AngleMode` enum — RAD=0, DEG=1 | `src/settings.h:7` |
| `StatsMode` enum — 6 navigation states | `src/stats.c:27` |
| `SolverMode` enum — 6 navigation states | `src/solver.c:15` |

### Inheritance

Not applicable — project is pure C. Platform abstraction is achieved via a shared HAL interface:

| Technique | File : Line |
|---|---|
| HAL interface definition (platform-agnostic) | `src/input_hal.h` |
| Desktop HAL implementation (SDL2, no-op locks) | `desktop/src/input_hal.c` |
| ESP32 HAL implementation (FreeRTOS mutex) | `esp32/main/input_hal.c` |

### Polymorphism

Simulated via function pointers stored inside structs:

| Technique | File : Line |
|---|---|
| `SubMenuItem.callback` — menu item action dispatch | `src/ui_submenu.h:15` |
| `StatsCtx.calc_fn` — selects binomial/normal/inverse calc | `src/stats.c:188` |
| `SolverCtx.solve_fn` — selects linear/quadratic/Newton solver | `src/solver.c:163` |

---

## Procedures and Functions (used extensively to organise program code)

| Technique | File : Line |
|---|---|
| `main_menu_create()` — top-level app launcher | `src/main_menu.c:1` |
| `math_app_start()` — initialises calculator UI | `src/calc_math.c:1` |
| `graph_app_start()` — initialises graph plotter | `src/graph.c:1` |
| `stats_app_start()` — initialises stats UI | `src/stats.c:1` |
| `solver_app_start()` — initialises solver UI | `src/solver.c:1` |
| `mechanics_app_start()` — initialises SUVAT UI | `src/mechanics.c:1` |
| `button_keymap_init/get_key/get_label()` — keymap API | `src/button_keymap.h` |
| `sdl_hal_init()` — SDL2 display + input setup | `desktop/src/hal/hal.c:1` |
| `driver_init()` — ESP32 ILI9341 display init | `esp32/main/driver.c` |
| `mcp23017_button_init()` — I2C GPIO expander init | `esp32/main/mcp23017_button_driver.c:1` |

---

## Database

| Technique | File : Line |
|---|---|
| Language used | N/A — no database used in this project |
| Multiple tables / Queries / Reports | N/A |
| Multi-user | N/A |
| Website front-end | N/A |
| All CRUD features | N/A |

---

## 3D or 2D Simulation using Unity

| Technique | File : Line |
|---|---|
| Unity simulation | N/A — no Unity used in this project |
| 2D graph plotting simulation | `src/graph.c:105` |

---

## Artificial Intelligence

| Technique | File : Line |
|---|---|
| Neural network | N/A — no neural network used in this project |
| Newton-Raphson iterative solver (AI-adjacent numerical method) | `src/solver.c:44` |
| Abramowitz & Stegun erf approximation (numerical AI method) | `src/stats.c:103` |

---

## Data Structures

| Technique | File : Line |
|---|---|
| **Arrays** — `HistoryEntry history[10]` circular buffer | `src/calc_math.c:24` |
| **Arrays (2D)** — `lv_obj_t *history_labels[10][2]` widget grid | `src/calc_math.c:30` |
| **Arrays (2D)** — `lv_obj_t *menu_buttons[2][3]` menu grid | `src/main_menu.c:18` |
| **Array** — `Function functions[4]` plottable functions | `src/graph.c:30` |
| **Array** — `button_mapping_t keymap[54]` button lookup table | `src/button_keymap.c:13` |
| **Array** — `double user_vars[6]` variables A–F | `src/settings.c:13` |
| **Array** — `lv_obj_t *param_labels[3]` stats input fields | `src/stats.c:53` |
| **Array** — `lv_obj_t *input_fields[5]` SUVAT fields | `src/mechanics.c:19` |
| **Array** — `const char *alt_paths[4]` fallback file paths | `desktop/src/main.c:87` |
| **Graphs/Trees** | N/A — not used |
| **Queues** | N/A — not used |
| **Stacks** — implicit call stack in recursive-descent parser | `src/expr_eval.c:29` |
| **Lists** | N/A — not used |
| **Sets** | N/A — not used |
| **Vectors** | N/A — not used |
| **Dictionary** | N/A — not used |
| **Struct** — `Parser` | `src/expr_eval.c:29` |
| **Struct** — `HistoryEntry` | `src/calc_math.c:14` |
| **Struct** — `Function` | `src/graph.c:28` |
| **Struct** — `SUVAT` | `src/mechanics.c:16` |
| **Struct** — `SubMenuItem` | `src/ui_submenu.h:13` |
| **Struct** — `SubMenuCtx` | `src/ui_submenu.c:7` |
| **Struct** — `StatsCtx` | `src/stats.c:184` |
| **Struct** — `SolverCtx` | `src/solver.c:159` |
| **Struct** — `button_mapping_t` | `src/button_keymap.c:6` |
| **Records** — `HistoryEntry` stores equation/result pairs as records | `src/calc_math.c:14` |
| **Records** — `button_mapping_t` stores per-button row/col/key/label | `src/button_keymap.c:6` |

---

## Read/Write to File

| Technique | File : Line |
|---|---|
| `fopen()` — opens `button_keycodes.txt` config file | `desktop/src/main.c:83` |
| `fgets()` — reads lines from config file | `desktop/src/main.c:110` |
| `fclose()` — closes config file handle | `desktop/src/main.c:144` |
| Fallback path search across 4 locations | `desktop/src/main.c:87` |

### Exception Handling

| Technique | File : Line |
|---|---|
| `NAN` returned on invalid expression / parse error | `src/expr_eval.c:29` |
| NULL-check guard on `fopen()` failure | `desktop/src/main.c:95` |
| `esp_err_t` return codes propagated through ESP32 drivers | `esp32/main/mcp23017_button_driver.c` |
| File not found fallback: tries 4 alternate paths before failing | `desktop/src/main.c:87` |

---

## Algorithms of Note

### Sorts

| Technique | File : Line |
|---|---|
| Sorts | N/A — no explicit sort algorithm implemented |

### Other Notable Algorithms

| Technique | File : Line |
|---|---|
| Recursive-descent expression parser | `src/expr_eval.c:29` |
| Newton-Raphson root finder (up to 100 iterations, 1e-8 tolerance) | `src/solver.c:44` |
| Quadratic formula solver | `src/solver.c:76` |
| Linear equation solver | `src/solver.c:69` |
| SUVAT multi-pass constraint propagation (up to 10 iterations) | `src/mechanics.c:49` |
| Central difference numerical derivative (h=1e-6) | `src/solver.c:35` |
| Binomial PMF/CDF direct combinatorial computation | `src/stats.c:87` |
| Normal PDF/CDF with Abramowitz & Stegun erf approximation | `src/stats.c:103` |
| Inverse normal quantile approximation via inverse erf | `src/stats.c:123` |
| Linear search through 54-entry button keymap (O(n)) | `src/button_keymap.c:85` |
| Pixel-by-pixel function graph plotter | `src/graph.c:105` |

---

## User-defined Data Structures

### Techniques Used Table

| Technique | File : Line |
|---|---|
| `Parser` struct | `src/expr_eval.c:29` |
| `HistoryEntry` struct | `src/calc_math.c:14` |
| `Function` struct | `src/graph.c:28` |
| `SUVAT` struct | `src/mechanics.c:16` |
| `SubMenuItem` struct | `src/ui_submenu.h:13` |
| `SubMenuStyle` struct | `src/ui_submenu.h:19` |
| `SubMenuCtx` struct | `src/ui_submenu.c:7` |
| `StatsCtx` struct | `src/stats.c:184` |
| `SolverCtx` struct | `src/solver.c:159` |
| `button_mapping_t` struct | `src/button_keymap.c:6` |
| `button_keymap_t` struct | `desktop/src/main.c:46` |
| `AngleMode` enum | `src/settings.h:7` |
| `StatsMode` enum | `src/stats.c:27` |
| `SolverMode` enum | `src/solver.c:15` |
| `GraphScreen` enum | `src/graph.c:26` |
| `HistoryEntry history[10]` circular buffer array | `src/calc_math.c:24` |
| `lv_obj_t *history_labels[10][2]` 2D widget array | `src/calc_math.c:30` |
| `lv_obj_t *menu_buttons[2][3]` 2D menu grid array | `src/main_menu.c:18` |
| `Function functions[4]` function list array | `src/graph.c:30` |
| `button_mapping_t keymap[54]` static lookup array | `src/button_keymap.c:13` |
| `double user_vars[6]` variable store array | `src/settings.c:13` |
| Implicit call stack (recursive-descent parser) | `src/expr_eval.c:29` |
| Function pointer dispatch (simulated polymorphism) | `src/ui_submenu.h:15` |

### Complex Formulas Table

| Formula / Algorithm | File : Line |
|---|---|
| Recursive-descent expression parser with operator precedence | `src/expr_eval.c:29` |
| Newton-Raphson: `x1 = x0 - f(x0)/f'(x0)` (100 iter, tol=1e-8) | `src/solver.c:44` |
| Central difference derivative: `(f(x+h) - f(x-h)) / 2h`, h=1e-6 | `src/solver.c:35` |
| Quadratic formula: `x = (-b ± sqrt(b²-4ac)) / 2a` | `src/solver.c:76` |
| SUVAT multi-pass constraint propagation (10 passes) | `src/mechanics.c:49` |
| Binomial PMF: `P(X=k) = C(n,k) * p^k * (1-p)^(n-k)` | `src/stats.c:87` |
| Normal PDF: `(1/σ√2π) * e^(-(x-μ)²/2σ²)` | `src/stats.c:103` |
| Normal CDF via Abramowitz & Stegun erf approximation | `src/stats.c:103` |
| Inverse normal quantile via inverse erf | `src/stats.c:123` |
| Pixel-by-pixel graph plot: map x∈[xmin,xmax] to canvas pixels | `src/graph.c:105` |
| Iterative factorial: `n! = 1 * 2 * ... * n` | `src/expr_eval.c:64` |
| Linear search keymap lookup O(n), n=54 | `src/button_keymap.c:85` |
