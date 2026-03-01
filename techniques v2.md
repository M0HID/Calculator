# Programming Techniques v2

All references are to `.c` files in `src/` only. Line numbers verified against the current codebase.

---

## Techniques Used Table

| Technique | Where used (`src/`) |
|-----------|-------------------|
| **OOP — Classes** | Pure C99 — structs with associated functions serve as classes: `Parser` (`expr_eval.c:26`), `SUVAT` (`mechanics.c:16`), `SolverCtx` (`solver.c:102`), `StatsCtx` (`stats.c:194`), `SubMenuCtx` (`ui_submenu.c:6`) |
| **OOP — Methods** | `math_app_start()` (`calc_math.c:227`), `do_integration()` (`numerical_methods.c:207`), `solve_suvat()` (`mechanics.c:49`), `draw_graph()` (`graph.c:162`), `eval_expression_x()` (`expr_eval.c:342`) — and all other `_start()`, `show_*()`, `do_*()` functions throughout |
| **OOP — Attributes** | `HistoryEntry.equation/result` (`calc_math.c:15–18`), `SUVAT.s/u/v/a/t` (`mechanics.c:16`), `Function.equation/enabled` (`graph.c:29`), `button_mapping_t.row/col/lvgl_key/label` (`button_keymap.c:6–11`), `Parser.str/pos` (`expr_eval.c:26–29`) |
| **OOP — Polymorphism** | `SolverCtx.solve_fn` function pointer (`solver.c:106`) dispatched at `solver.c:115`; `StatsCtx.calc_fn` (`stats.c:198`) dispatched at `stats.c:230`; `SubMenuItem.callback` (`ui_submenu.c:13`) dispatched at `ui_submenu.c:50` |
| **OOP — Composition** | `SubMenuCtx` (`ui_submenu.c:6–15`) composes a `SubMenuItem[]` array, `lv_group_t*`, `lv_obj_t*`, and a function pointer. `SolverCtx` (`solver.c:102–107`) composes an `lv_obj_t**` fields array with a `solve_fn*` pointer |
| **Procedures and Functions** | Used extensively throughout every `.c` file — `calc_math.c:45`, `expr_eval.c:31`, `mechanics.c:49`, `numerical_methods.c:207`, `graph.c:76`, `stats.c:95` and many more |
| **Arrays (multi-dimensional / integrated)** | `history[10]` (`calc_math.c:25`), `history_labels[10][2]` 2D widget grid (`calc_math.c:31`), `menu_buttons[2][3]` 2D button grid (`main_menu.c:18`), `functions[4]` (`graph.c:31`), `keymap[54]` (`button_keymap.c:13`), `user_vars[9]` (`settings.c:12`) |
| **Queues** | `history[10]` circular FIFO — `add_to_history()` (`calc_math.c:115–128`): shifts all entries left when full, discarding the oldest |
| **Read / Write to File** | `button_keycodes.txt` read via `fopen` / `fgets` / `fclose` (`desktop/src/main.c:82–156`). Not present in `src/` — settings persist in static memory only |
| **Exception Handling** | `NAN` on invalid expression (`expr_eval.c:332`), division by zero (`expr_eval.c:288`), non-integer factorial (`expr_eval.c:57`), invalid probability (`stats.c:96`), non-positive σ (`stats.c:112`), singular regression matrix (`numerical_methods.c:94`), failed integration (`numerical_methods.c:239`), insufficient SUVAT inputs (`mechanics.c:105`) |
| **Recursion** | Mutual: `parse_factor` → `parse_expression` (`expr_eval.c:85, 130, 141, 153, 163, 174, 187, 200, 210, 221`). Direct: `parse_factor` calls itself for unary signs (`expr_eval.c:75, 80`) |

---

## Complex Algorithms / Models

### Math — Recursive Descent Expression Parser
**`src/expr_eval.c` — lines 70–329**

The parser implements a three-level grammar hierarchy where each level calls the next, and the bottom level can recurse back to the top, giving it full BIDMAS precedence without any explicit operator-precedence table:

```
parse_expression()   →  handles + and −           lines 310–329
  └─ parse_term()    →  handles * / ^ implicit×   lines 237–308
       └─ parse_factor()  →  handles atoms, functions, parentheses   lines 70–235
            └─ calls parse_expression() recursively for sub-expressions
```

A forward declaration on **line 54** enables the mutual recursion:
```c
static double parse_expression(Parser *p);
```
`parse_factor` recurses back into `parse_expression` at **line 85** (for parenthesised groups) and at **lines 130, 141, 153, 163, 174, 187, 200, 210, 221** (for each named function argument such as `sin(...)`, `sqrt(...)`).

Direct recursion on **lines 75 and 80** handles chained unary signs:
```c
if (c == '-') { p->pos++; return -parse_factor(p); }  // line 75
if (c == '+') { p->pos++; return  parse_factor(p); }  // line 80
```
This correctly evaluates chains like `--x` or `+-3` by unwinding the sign stack naturally.

The C call stack itself acts as the operand stack — values are held as `double` return values in stack frames until the calling level combines them, requiring no explicit stack data structure.

Implicit multiplication is detected in `parse_term` at **lines 246–280**: after evaluating a factor, if the next character is `(`, `x`, `X`, `A`–`F`, `pi`, `e`, or any function name, the result is multiplied by the next factor without any explicit `*` token. This supports expressions like `2x`, `3(4+1)`, `2sin(x)`, `4pi`.

---

### Math — Calculation History (Circular Buffer / FIFO)
**`src/calc_math.c` — lines 115–128**

The 10-entry history array acts as a fixed-capacity FIFO queue. When full, all entries shift left by one to discard the oldest, and the new entry occupies the last slot:

```c
if (history_count >= MAX_HISTORY) {               // MAX_HISTORY = 10, line 12
    for (int i = 0; i < MAX_HISTORY - 1; i++) {
        strcpy(history[i].equation, history[i+1].equation);
        strcpy(history[i].result,   history[i+1].result);
    }
    history_count = MAX_HISTORY - 1;
}
strncpy(history[history_count].equation, equation, sizeof(...) - 1);
history_count++;
```

The `HistoryEntry` struct at **lines 15–18** stores each entry as a pair of fixed-length strings: `char equation[128]` and `char result[64]`.

---

### Graph — Pixel-by-Pixel Function Plotting with Discontinuity Detection
**`src/graph.c` — lines 109–133**

Each function is evaluated at every pixel column across the canvas width, and adjacent valid points are connected with a line segment:

```c
for (int cx = 0; cx < CANVAS_W; cx++) {
    double gx = x_min + (double)cx / CANVAS_W * (x_max - x_min);
    double gy = eval_expression_x(functions[idx].equation, gx);
    int cy = g2cy(gy);
    int v = !isnan(gy) && !isinf(gy) && cy >= -CANVAS_H && cy <= 2*CANVAS_H;
    if (v && pv) {
        int dy = cy - pcy; if (dy < 0) dy = -dy;
        if (dy < CANVAS_H * 2) lv_draw_line(&layer, &ld);   // suppress asymptote jump
    }
    pcx = cx; pcy = cy; pv = v;
}
```

Two separate discontinuity guards prevent graphing artifacts from functions like `tan(x)` or `1/x`:
1. **line 124** — the `v` validity flag: if the evaluated point is NaN, Inf, or wildly off-screen, no segment is drawn to or from it
2. **line 128** — even between two valid points, if the vertical gap exceeds twice the canvas height, the connecting line is suppressed, catching near-vertical asymptote jumps

Coordinate transformation is done by two macros at **lines 73–74**:
```c
#define g2cx(gx) (int)((gx - x_min) / (x_max - x_min) * CANVAS_W)
#define g2cy(gy) (int)(CANVAS_H - (gy - y_min) / (y_max - y_min) * CANVAS_H)
```
`g2cy` inverts the y-axis because screen y increases downward while mathematical y increases upward.

---

### Stats — Binomial Distribution
**`src/stats.c` — lines 87–109**

Binomial coefficient is computed iteratively at **lines 87–93** using a multiply-then-divide pattern that avoids factorial overflow:
```c
double r = 1.0;
for (int i = 0; i < k; i++) {
    r *= (n - i);
    r /= (i + 1);
}
```

PMF at **lines 95–99**:  `P(X=k) = C(n,k) · p^k · (1−p)^(n−k)`

CDF at **lines 101–109**: cumulative sum of PMF from i = 0 to k.

---

### Stats — Normal Distribution (PDF, CDF, Inverse)
**`src/stats.c` — lines 111–144**

**PDF** at **lines 111–115**: `(1 / σ√2π) · e^(−½((x−μ)/σ)²)`

**CDF** uses a custom `erf_approx` at **lines 117–124** (Abramowitz & Stegun 5-coefficient polynomial) to avoid depending on a C99 `erf()` that may not be available on the ESP32 toolchain:
```c
double t = 1.0 / (1.0 + p * x);
double y = 1.0 - (((((a5*t+a4)*t)+a3)*t+a2)*t+a1)*t * exp(-x*x);
```
The Horner-form nested multiplication minimises floating-point rounding error. `normal_cdf` at **line 126** then evaluates `0.5 · (1 + erf_approx((x−μ)/(σ√2)))`.

**Inverse normal** at **lines 131–144**: uses `erf_inv_approx` (constant a = 0.147 approximation) to compute quantile `x = μ + σ√2 · erf⁻¹(2p−1)`. Returns NAN if p ∉ (0,1) or σ ≤ 0.

---

### Stats — Poisson Distribution
**`src/stats.c` — lines 432–443**

PMF at **lines 432–435**: `P(X=k) = e^(−λ) · λᵏ / k!`

CDF at **lines 437–443**: cumulative sum of PMF from i = 0 to k. Returns NAN if λ ≤ 0 or k < 0.

---

### Solver — Linear Equation (ax + b = 0)
**`src/solver.c` — lines 27–33**

```c
static int solve_linear(double a, double b, double *x) {
    if (fabs(a) < 1e-12) return 0;   // degenerate: no unique solution
    *x = -b / a;
    return 1;
}
```

Returns 0 (no solution) if the coefficient `a` is effectively zero. Otherwise writes the solution through the output pointer and returns 1. The `fabs(a) < 1e-12` epsilon guard prevents division by a near-zero float.

---

### Solver — Quadratic Equation (ax² + bx + c = 0)
**`src/solver.c` — lines 34–50**

```c
double d = b*b - 4*a*c;                    // discriminant
if (d < 0) return 0;                       // no real roots
if (fabs(d) < 1e-12) {                     // repeated root
    *x1 = *x2 = -b / (2*a); return 1;
}
*x1 = (-b + sqrt(d)) / (2*a);             // two distinct roots
*x2 = (-b - sqrt(d)) / (2*a);
return 2;
```

If |a| < 1e-12 the equation is degenerate and the function delegates to `solve_linear(b, c, x1)` at **line 36**. Three return values (0, 1, 2) tell the caller how many roots were found.

---

### Mechanics — SUVAT Constraint Propagation
**`src/mechanics.c` — lines 49–93**

Given ≥ 3 of the 5 kinematic variables (s, u, v, a, t), the solver applies all 16 rearrangements of the 4 SUVAT equations iteratively until all 5 are determined:

```c
for (int iter = 0; iter < 10 && count_known(sv) < 5; iter++) {
    int before = count_known(sv);
    if (IS_KNOWN(sv->u) && IS_KNOWN(sv->a) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->v))
        sv->v = sv->u + sv->a * sv->t;                          // v = u + at
    if (IS_KNOWN(sv->v) && IS_KNOWN(sv->u) && IS_KNOWN(sv->t) && !IS_KNOWN(sv->a))
        sv->a = (sv->v - sv->u) / sv->t;                        // a = (v-u)/t
    // ... 14 more rearrangements covering all forms of the 4 equations ...
    if (count_known(sv) == before) break;                        // fixed-point exit
}
```

The fixed-point exit at **line 86** terminates as soon as an entire iteration passes with no new variable derived — it never runs needless iterations. The sentinel `SUVAT_UNKNOWN = −999999.0` (**line 13**) marks unknowns; the `IS_KNOWN(x)` macro (**line 14**) makes each equation guard readable.

---

### Numerical Methods — Simpson's 1/3 Rule (Numerical Integration)
**`src/numerical_methods.c` — lines 207–245**

```c
double h   = (b - a) / n;
double sum = eval_expression_x(expr, a) + eval_expression_x(expr, b);
for (int i = 1; i < n; i++) {
    double xi = a + i * h;
    double fi = eval_expression_x(expr, xi);
    sum += (i % 2 == 0) ? 2.0 * fi : 4.0 * fi;   // alternating 4/2 weights
}
double result = (h / 3.0) * sum;
```

The `i % 2` ternary at **line 235** is the defining feature of Simpson's rule — odd-indexed interior points get weight 4, even-indexed get weight 2. `n` is clamped to an even number and a maximum of 10000 at **lines 220–222**. A zero-width interval (`a == b`) short-circuits to result 0 at **line 227**.

---

### Numerical Methods — Newton-Raphson Root Finding
**`src/numerical_methods.c` — lines 356–372**

```c
for (int i = 0; i < NR_MAX_ITERATIONS; i++) {   // NR_MAX_ITERATIONS = 100
    double fx  = eval_expression_x(expr, x);
    double fpx = numerical_derivative(expr, x);  // central difference
    if (fabs(fpx) < 1e-14) break;               // flat derivative guard
    double xn  = x - fx / fpx;                  // Newton step
    if (fabs(xn - x) < NR_TOLERANCE) {          // NR_TOLERANCE = 1e-8
        *result = xn; return 1;                  // converged
    }
    x = xn;
}
```

Convergence is tested on the step size `|xn − x|` rather than `|f(x)|`, which handles cases where the function value is large but the root is still nearby. The derivative is never computed symbolically — it is approximated numerically on every iteration.

---

### Numerical Methods — Numerical Derivative (Central Difference)
**`src/numerical_methods.c` — lines 351–354**

```c
static double numerical_derivative(const char *expr, double x) {
    double h = 1e-5;
    return (eval_expression_x(expr, x + h) - eval_expression_x(expr, x - h)) / (2.0 * h);
}
```

Symmetric two-point central difference, giving O(h²) accuracy compared to O(h) for a one-sided formula. It is called on every Newton-Raphson iteration, making symbolic differentiation unnecessary — any expression the parser can evaluate can be differentiated.

---

### Numerical Methods — Least-Squares Linear Regression
**`src/numerical_methods.c` — lines 74–115**

Sums Σx, Σy, Σx², Σxy are accumulated in a single pass through the data at **lines 86–93**, then the normal equations are solved in closed form:

```c
double m = ((double)nx * Sxy - Sx * Sy) / ((double)nx * Sxx - Sx * Sx);  // slope
double c = (Sy - m * Sx) / (double)nx;                                     // intercept
```

R² is computed at **lines 102–109** as `1 − SS_res / SS_tot` to report goodness of fit alongside the regression line. A near-zero denominator guard at **line 94** (`fabs(denom) < 1e-12`) prevents division by zero for degenerate inputs.

---

## Core Programming Techniques

### Recursion
**`src/expr_eval.c`**

Two forms are used simultaneously:

**Mutual recursion** — `parse_factor` calls `parse_expression` to handle sub-expressions inside parentheses and function arguments (lines 85, 130, 141, 153, 163, 174, 187, 200, 210, 221), while `parse_expression` calls `parse_term` which calls `parse_factor`, completing the cycle.

**Direct recursion** — `parse_factor` calls itself for chained unary signs:
- Line 75: `return -parse_factor(p);`
- Line 80: `return  parse_factor(p);`

---

### Function Pointers Stored in Structs (Polymorphism)
**`src/solver.c` lines 102–107 · `src/stats.c` lines 194–199 · `src/ui_submenu.c` line 13**

`SolverCtx` stores a function pointer as a field, enabling the same key callback to dispatch to different solvers at runtime:
```c
typedef struct {
    lv_obj_t **fields;
    int        field_count;
    lv_obj_t  *result_label;
    void      (*solve_fn)(lv_obj_t **fields, int count, lv_obj_t *result);
} SolverCtx;
```
Dispatch at **solver.c line 115** with no conditional:
```c
active_ctx->solve_fn(active_ctx->fields, active_ctx->field_count, active_ctx->result_label);
```
On the linear screen `solve_fn` points to `solve_linear_fn`; on the quadratic screen it points to `solve_quad_fn`. `StatsCtx` uses the same pattern with `calc_fn` to dispatch across 7 distribution calculators. `SubMenuItem.callback` at **ui_submenu.c line 50** dispatches to any of 13 screen-open functions from a single line.

---

### Structs as Custom Types
**Multiple files in `src/`**

| Struct | File | Lines | Fields |
|--------|------|-------|--------|
| `Parser` | `expr_eval.c` | 26–29 | `const char *str`, `int pos` |
| `HistoryEntry` | `calc_math.c` | 15–18 | `char equation[128]`, `char result[64]` |
| `SUVAT` | `mechanics.c` | 16 | `double s, u, v, a, t` |
| `Function` | `graph.c` | 29 | `char equation[64]`, `int enabled` |
| `SolverCtx` | `solver.c` | 102–107 | fields, count, result label, `solve_fn*` |
| `StatsCtx` | `stats.c` | 194–199 | fields, count, result label, `calc_fn*` |
| `SubMenuCtx` | `ui_submenu.c` | 6–15 | items, count, selection, `on_menu*`, labels, group, focus colour |
| `button_mapping_t` | `button_keymap.c` | 6–11 | `int row`, `int col`, `uint32_t lvgl_key`, `char label[16]` |

---

### Enums for Named States
**Multiple files in `src/`**

| Enum | File | Lines | Values |
|------|------|-------|--------|
| `GraphScreen` | `graph.c` | 27 | `SCREEN_FUNCTION_LIST`, `SCREEN_GRAPH_VIEW` |
| `SolverMode` | `solver.c` | 12–16 | `SOLVER_MENU`, `SOLVER_LINEAR`, `SOLVER_QUADRATIC` |
| `StatsMode` | `stats.c` | 28–37 | `STATS_MENU`, `STATS_BINOMIAL`, `STATS_BINOMIAL_CDF`, `STATS_NORMAL`, `STATS_NORMAL_CDF`, `STATS_NORMAL_INV`, `STATS_POISSON`, `STATS_POISSON_CDF` |
| `AngleMode` | `settings.c` | 63 | `ANGLE_RAD`, `ANGLE_DEG` |

---

### Static Variables for Persistent State
**Multiple files in `src/`**

| Variable | File | Line | What it persists |
|----------|------|------|-----------------|
| `current_x_value` | `expr_eval.c` | 24 | Value of `x` across an entire parse call |
| `history[10]`, `history_count`, `last_answer` | `calc_math.c` | 25–28 | Calculation history and Ans across keystrokes |
| `x_min`, `x_max`, `y_min`, `y_max`, `trace_x` | `graph.c` | 35–43 | Graph viewport and trace position across redraws |
| `current_suvat` | `mechanics.c` | 29 | SUVAT values between the input and result pages |
| `angle_mode`, `decimal_places`, `user_vars[9]` | `settings.c` | 10–12 | App-wide settings for the whole session |
| `active_submenu` | `ui_submenu.c` | 17 | Pointer to the one active submenu, enforcing single-instance |

---

### Output Parameters (Pointer-Based Return Values)
**`src/solver.c` · `src/numerical_methods.c`**

Functions return an integer status code while writing computed results through pointer arguments, allowing both a success/failure signal and the actual values from one call:

- `solve_linear(a, b, double *x)` — **solver.c:27**: writes `*x = -b/a`, returns 1 or 0
- `solve_quadratic(a, b, c, double *x1, double *x2)` — **solver.c:34**: writes up to two roots, returns 0/1/2
- `newton_raphson(expr, x0, double *result, int *iters)` — **numerical_methods.c:356**: writes the converged root and iteration count

---

### String Parsing
**`src/expr_eval.c` · `src/numerical_methods.c`**

The expression parser identifies function names using `strncmp` prefix matching at **expr_eval.c lines 115, 125, 136, 147, 158, 169, 181, 193, 205, 216**, then reads numeric literals using `isdigit` + `atof` at **lines 43–51**.

The CSV parser in `numerical_methods.c` uses `strtod` with an advancing pointer at **lines 55–66** — the `end` pointer returned by `strtod` is reused as the next start position, correctly handling any mix of space and comma delimiters without `strtok`:
```c
char *end;
double v = strtod(tok, &end);
if (end == tok) break;   // nothing consumed — stop
out[count++] = v;
tok = end;               // advance past what was consumed
```

---

### Validation and Error Handling
**Multiple files in `src/`**

All invalid inputs return `NAN` or display an error label — there are no crashes or undefined results:

| File | Lines | Check |
|------|-------|-------|
| `expr_eval.c` | 332–334 | Return NAN if expression is null or empty |
| `expr_eval.c` | 57–58 | Return NAN for factorial of non-integer or negative |
| `expr_eval.c` | 288–292 | Return NAN on division by zero |
| `stats.c` | 96 | Return NAN if p ∉ [0,1] |
| `stats.c` | 112 | Return NAN if σ ≤ 0 |
| `stats.c` | 131–132 | Return NAN if erf_inv argument ∉ (−1,1) |
| `numerical_methods.c` | 80–83 | Reject curve fit if fewer than 2 points or nx ≠ ny |
| `numerical_methods.c` | 94–98 | Guard against singular matrix in regression |
| `numerical_methods.c` | 239–242 | Check integration result for NaN/Inf before display |
| `mechanics.c` | 105–111 | Require at least 3 known SUVAT values |
| `solver.c` | 27–29 | Return 0 (no solution) if coefficient `a` ≈ 0 |

---

### Modular Design
**`src/` directory**

Each application is a self-contained translation unit. No app references another's internals — they communicate only through the public entry-point functions (`math_app_start()`, etc.) and the shared APIs (`settings_*`, `eval_expression_x`):

| File | Responsibility |
|------|---------------|
| `calc_math.c` | Math calculator |
| `graph.c` | Function plotter |
| `stats.c` | Statistical distributions |
| `solver.c` | Equation solver |
| `mechanics.c` | SUVAT kinematics |
| `numerical_methods.c` | Numerical integration, root-finding, regression |
| `settings.c` | Global settings |
| `expr_eval.c` | Expression parsing — shared library used by calc, graph, and numerical methods |
| `ui_submenu.c` | Reusable submenu component — used by stats, solver, numerical methods, settings |
| `button_keymap.c` | Hardware button matrix abstraction |
| `main_menu.c` | Navigation hub only |

---

## Coding Style Techniques

### Macros for Constants and Predicates
**Multiple files in `src/`**

| Macro | File | Line | Purpose |
|-------|------|------|---------|
| `SUVAT_UNKNOWN` | `mechanics.c` | 13 | Sentinel value for an unsolved SUVAT variable |
| `IS_KNOWN(x)` | `mechanics.c` | 14 | Predicate macro — reads as natural language in equation guards |
| `NAN`, `INFINITY`, `M_PI`, `M_E` | `expr_eval.c` | 8–22 | Portability fallbacks if not provided by the toolchain |
| `MAX_HISTORY` | `calc_math.c` | 12 | Capacity of history circular buffer |
| `NR_MAX_ITERATIONS`, `NR_TOLERANCE` | `numerical_methods.c` | 348–349 | Newton-Raphson tuning constants — easily adjustable |
| `g2cx`, `g2cy` | `graph.c` | 73–74 | Coordinate transform — macro avoids function-call overhead per pixel |
| `CONTENT_TOP`, `COL_BG`, `FONT_PRIMARY`, etc. | `ui_common.h` | 14–48 | Shared design token system used by every screen |

---

### Inline Helper Functions
**`src/ui_common.h` — lines 54–122**

Four `static inline` functions give every screen access to consistent UI construction without code duplication:

| Function | Line | Purpose |
|----------|------|---------|
| `ui_create_hint_bar(parent, text)` | 54 | Creates the bottom hint bar, returns its label |
| `ui_setup_screen(scr)` | 80 | Applies white background and removes scrollable flag |
| `ui_style_textarea(ta, accent, focus_bg)` | 91 | Applies themed border and background on focus |
| `ui_label(parent, text, x, y)` | 114 | Creates a label with consistent font and colour |

`static inline` in a header compiles into each translation unit with zero call overhead and no linker conflicts.

---

### Sentinel Values
**`src/mechanics.c` — lines 13–14**

```c
#define SUVAT_UNKNOWN -999999.0
#define IS_KNOWN(x)   ((x) != SUVAT_UNKNOWN)
```

`−999999.0` is physically impossible as a kinematic quantity, so it safely represents "not yet entered." Every equation guard in `solve_suvat()` (**lines 52–85**) reads `IS_KNOWN(sv->v)` rather than a raw numeric comparison, making the constraint logic self-documenting.

---

### Guard Clauses / Early Returns
**Multiple files in `src/`**

Rather than deeply nested `if/else`, functions return early when preconditions are not met:

| File | Lines | Guard |
|------|-------|-------|
| `expr_eval.c` | 332–334 | Return NAN if expression is null or empty |
| `numerical_methods.c` | 224–227 | Return 0 immediately if integral bounds are equal |
| `mechanics.c` | 105–111 | Show error and return if fewer than 3 SUVAT values given |
| `solver.c` | 27–29 | Return 0 immediately if leading coefficient ≈ 0 |
| `graph.c` | 110–111 | Return early if function is disabled or equation is empty |

---

### State Machines (Enum + Screen Transitions)
**`src/graph.c` · `src/solver.c` · `src/stats.c`**

Each multi-screen app uses an enum to name its states and a static variable to track the current one. Screen-switch functions set the state on entry; key callbacks read it to decide behaviour:

- **`graph.c`** — `GraphScreen` enum (line 27): `SCREEN_FUNCTION_LIST`, `SCREEN_GRAPH_VIEW`. `funclist_key_cb` line 374 transitions to graph view; `graph_key_cb` line 462 transitions back.
- **`solver.c`** — `SolverMode` enum (lines 12–16): `SOLVER_MENU`, `SOLVER_LINEAR`, `SOLVER_QUADRATIC`. Each `show_*` function sets `current_mode` on entry (line 21).
- **`stats.c`** — `StatsMode` enum (lines 28–37): 8 values covering every distribution screen.

---

### Event-Driven Architecture
**All apps in `src/`**

No app polls input. All logic runs only when LVGL fires an event on a focused widget via `lv_obj_add_event_cb`:

| File | Line | Event | Callback |
|------|------|-------|----------|
| `calc_math.c` | 287 | `LV_EVENT_VALUE_CHANGED` | `textarea_event_cb` — recalculates on every keystroke |
| `calc_math.c` | 292 | `LV_EVENT_KEY` | `math_key_cb` |
| `graph.c` | 275–276 | `LV_EVENT_READY`, `LV_EVENT_CANCEL` | `textarea_event_cb` — saves/discards equation edit |
| `graph.c` | 286 | `LV_EVENT_KEY` | `funclist_key_cb` |
| `graph.c` | 323 | `LV_EVENT_KEY` | `graph_key_cb` |
| `main_menu.c` | 122–123 | `LV_EVENT_KEY`, `LV_EVENT_CLICKED` | Keyboard and mouse/touch support |
| `ui_submenu.c` | 90 | `LV_EVENT_KEY` | `submenu_key_cb` — shared by 4 apps |
| `numerical_methods.c` | 176, 187 | `LV_EVENT_KEY` | `cf_key_cb`, `ni_textarea_key_cb` |
| `stats.c` | 291 | `LV_EVENT_KEY` | `stats_textarea_key_cb` |

---

### Reusable Component
**`src/ui_submenu.c` — `ui_create_submenu()` lines 63–118**

The same function is called unmodified by four separate apps, each passing its own `SubMenuItem[]` array:

| Caller | File | Line |
|--------|------|------|
| Stats | `stats.c` | 264 |
| Solver | `solver.c` | 98 |
| Numerical Methods | `numerical_methods.c` | 486 |
| Settings | `settings.c` | 333 |

Rendering, keyboard navigation, highlight logic, and memory management are all implemented once inside `ui_submenu.c`.

---

### Single-Responsibility Functions
**Multiple files in `src/`**

| Function | File | Lines | Does exactly |
|----------|------|-------|-------------|
| `skip_whitespace()` | `expr_eval.c` | 31–36 | Advance parser past spaces |
| `parse_number()` | `expr_eval.c` | 37–54 | Read and convert one numeric literal |
| `factorial()` | `expr_eval.c` | 56–68 | Compute n! |
| `numerical_derivative()` | `numerical_methods.c` | 351–354 | Central-difference derivative |
| `parse_csv()` | `numerical_methods.c` | 52–68 | CSV string → double array |
| `count_known()` | `mechanics.c` | 39–47 | Count known SUVAT fields |
| `draw_axes()` | `graph.c` | 76–107 | Draw axes and grid |
| `draw_function()` | `graph.c` | 109–133 | Plot one function curve |
| `update_highlight()` | `ui_submenu.c` | 19–28 | Update selection highlight |

---

## Data Structures

| Structure | File | Lines | Used for |
|-----------|------|-------|----------|
| `HistoryEntry history[10]` — fixed array as FIFO | `calc_math.c` | 25 | Calculation history |
| `lv_obj_t *history_labels[10][2]` — 2D array | `calc_math.c` | 31 | Widget grid for history rows |
| `lv_obj_t *menu_buttons[2][3]` — 2D array | `main_menu.c` | 18 | 3×2 menu button grid |
| `Function functions[4]` — array of structs | `graph.c` | 31–33 | Up to 4 plottable functions |
| `button_mapping_t keymap[54]` — static lookup table | `button_keymap.c` | 13 | Button matrix mapping |
| `double user_vars[9]` — named variable store | `settings.c` | 12 | Variables A–F, x, y, z |
| `double xs[30]`, `ys[30]` — local arrays | `numerical_methods.c` | 76–77 | Curve-fit data points |
| Implicit call stack — C call stack holds parser operands | `expr_eval.c` | 70–329 | Recursive descent operand storage |
