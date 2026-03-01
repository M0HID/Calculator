# SYSTEM INFORMATION: DATA STRUCTURES AND ALGORITHMS

## AQA Computer Science Concepts Demonstrated

This document details the computational thinking, data structures, algorithms, and software engineering principles demonstrated in the scientific calculator system, using terminology aligned with the AQA Computer Science A-Level specification.

---

## 1. System Architecture and Abstraction Layers

### 1.1 Layered Architecture (Abstraction)

The system demonstrates **abstraction** through a multi-layered architecture that separates concerns and hides implementation details:

```
┌─────────────────────────────────────────┐
│   Application Layer (High-level logic)  │  ← User-facing applications
├─────────────────────────────────────────┤
│   UI Framework (Reusable components)    │  ← Abstracted UI primitives
├─────────────────────────────────────────┤
│   Computation Layer (Algorithms)        │  ← Mathematical processing
├─────────────────────────────────────────┤
│   HAL - Hardware Abstraction Layer      │  ← Platform independence
├─────────────────────────────────────────┤
│   Physical Hardware (SDL2 / ESP32)      │  ← Concrete implementations
└─────────────────────────────────────────┘
```

**Key Abstraction Principle**: Higher layers depend on lower layer interfaces, not implementations. For example, the Math app calls `eval_expression(expr)` without knowing whether it runs on desktop (SDL2) or embedded (ESP32) hardware.

### 1.2 Hardware Abstraction Layer (HAL)

**File**: `src/input_hal.h`

**Purpose**: Provides a platform-independent interface for input handling and thread synchronization.

**Abstract Data Type (ADT)**: The HAL defines three abstract operations:
- `get_navigation_indev()` → Returns input device pointer
- `lvgl_lock()` → Acquires mutual exclusion lock
- `lvgl_unlock()` → Releases mutual exclusion lock

**Polymorphism**: The same function signatures have different implementations:
- **Desktop** (`desktop/src/input_hal.c`): No-op locks (single-threaded), returns SDL keyboard device
- **ESP32** (`esp32/main/input_hal.c`): FreeRTOS mutex locks, returns I²C button matrix device

**Benefit**: Demonstrates **encapsulation** - the implementation is hidden; only the interface is exposed.

---

## 2. Data Structures Implementation

### 2.1 Stack (LIFO - Last In, First Out)

**Location**: Call stack during recursive descent parsing (`expr_eval.c:79-346`)

**Structure**: Parser state stored on the function call stack

```c
typedef struct {
    const char *str;  // Pointer to expression string
    int pos;          // Current position (index)
} Parser;
```

**Stack Operations**:
1. **Push**: Each recursive function call (`parse_expression`, `parse_term`, `parse_factor`) pushes a new Parser state onto the call stack
2. **Pop**: Function return pops the state from the stack
3. **Peek**: Reading parser position without modifying

**Example Stack Trace** for expression `"(2+3)*5"`:

```
Call Stack (grows downward):
┌─────────────────────────────────┐
│ parse_expression("(2+3)*5", 0) │ ← Bottom (first call)
├─────────────────────────────────┤
│ parse_term("(2+3)*5", 0)       │
├─────────────────────────────────┤
│ parse_factor("(2+3)*5", 0)     │ ← Sees '(', recursive call
├─────────────────────────────────┤
│ parse_expression("(2+3)*5", 1) │ ← Inside parentheses
├─────────────────────────────────┤
│ parse_term("(2+3)*5", 1)       │
├─────────────────────────────────┤
│ parse_factor("(2+3)*5", 1)     │ ← Returns 2
└─────────────────────────────────┘
```

**LIFO Property**: The innermost `(2+3)` is evaluated first (last pushed, first popped), demonstrating the Last In, First Out principle.

**Stack Frame Size**: Each frame contains:
- Return address (4-8 bytes)
- Parser struct (12-16 bytes)
- Local variables (~16 bytes)
- **Total**: ~40 bytes per recursion level

**Maximum Depth**: Limited by available stack space (~1-2 MB on desktop, ~20 KB on ESP32), allowing ~1000-2000 levels of nested parentheses.

---

### 2.2 Circular Buffer / Ring Buffer (Queue variant)

**Location**: Calculation history storage (`calc_math.c:24-25`)

**Structure Definition**:
```c
typedef struct {
    char equation[128];
    char result[64];
} HistoryEntry;

static HistoryEntry history[MAX_HISTORY];  // Array of 10 entries
static int history_count = 0;              // Tracks current size
```

**Array Implementation**: Fixed-size array with manual wraparound logic

**Operations**:

**1. Enqueue (Add to history)** - `calc_math.c:114-127`
```
Algorithm: add_to_history(equation, result)
    IF history_count >= MAX_HISTORY THEN
        FOR i = 0 TO MAX_HISTORY-2 DO
            history[i] ← history[i+1]     // Shift left (O(n) operation)
        END FOR
        history_count ← MAX_HISTORY - 1
    END IF
    
    history[history_count].equation ← equation
    history[history_count].result ← result
    history_count ← history_count + 1
END
```

**Time Complexity**: O(n) where n = MAX_HISTORY (due to shifting)

**Space Complexity**: O(1) - fixed allocation

**FIFO Behavior**: Oldest entry is removed first when buffer is full (First In, First Out).

**Optimization Potential**: A true circular buffer using modulo arithmetic would achieve O(1) enqueue:
```c
rear = (rear + 1) % MAX_HISTORY;  // Constant time wraparound
```

---

### 2.3 Static Array (Linear List)

**Location**: Function storage for graphing (`graph.c:31-33`)

**Structure Definition**:
```c
typedef struct {
    char equation[64];
    int enabled;
} Function;

static Function functions[MAX_FUNCTIONS];  // Array of 4 elements
```

**Array Properties**:
- **Fixed size**: MAX_FUNCTIONS = 4
- **Homogeneous**: All elements are `Function` structs
- **Random access**: O(1) access time via index (functions[i])
- **Contiguous memory**: Elements stored sequentially in memory

**Memory Layout**:
```
Memory Address:     Content:
┌────────────┐
│ functions[0].equation[64]  │  68 bytes
│ functions[0].enabled       │   4 bytes
├────────────┤
│ functions[1].equation[64]  │  68 bytes
│ functions[1].enabled       │   4 bytes
├────────────┤
│ functions[2].equation[64]  │  68 bytes
│ functions[2].enabled       │   4 bytes
├────────────┤
│ functions[3].equation[64]  │  68 bytes
│ functions[3].enabled       │   4 bytes
└────────────┘
Total: 288 bytes (contiguous block)
```

**Sequential Access Pattern**: Used when rendering all enabled functions (`graph.c:165-170`):
```c
FOR i = 0 TO MAX_FUNCTIONS-1 DO
    IF functions[i].enabled AND functions[i].equation != "" THEN
        draw_function(i)
    END IF
END FOR
```

**Time Complexity**: O(n) for iteration, O(1) for access

---

### 2.4 Associative Array / Hash Table (Conceptual)

**Location**: Button keymap (`button_keymap.c:14-15`)

**Structure**:
```c
typedef struct {
    int row;              // Part of composite key
    int col;              // Part of composite key
    uint32_t lvgl_key;    // Value (mapped key code)
    char label[16];       // Display label
} button_mapping_t;

static button_mapping_t keymap[MAX_BUTTONS];  // 54 entries
```

**Hash Function**: Implicit mapping using (row, col) → index:
```
hash(row, col) = row * BUTTON_COLS + col
Example: hash(5, 3) = 5 * 6 + 3 = 33
```

**Lookup Operation** (`button_keymap.c:96-103`):
```
Algorithm: get_key(row, col)
    FOR i = 0 TO keymap_count-1 DO
        IF keymap[i].row = row AND keymap[i].col = col THEN
            RETURN keymap[i].lvgl_key
        END IF
    END FOR
    RETURN 0  // Not found
END
```

**Current Implementation**: Linear search - O(n)

**Optimized Alternative**: Direct addressing with hash function - O(1)
```c
// Could be optimized to:
uint32_t direct_keymap[BUTTON_ROWS][BUTTON_COLS];
return direct_keymap[row][col];  // O(1) lookup
```

---

### 2.5 Record Structure (Struct as Composite Data Type)

**Location**: SUVAT kinematics data (`mechanics.c:16`)

**Structure Definition**:
```c
typedef struct {
    double s;  // displacement (m)
    double u;  // initial velocity (m/s)
    double v;  // final velocity (m/s)
    double a;  // acceleration (m/s²)
    double t;  // time (s)
} SUVAT;
```

**Composite Type**: Aggregates 5 related floating-point values into a single logical unit.

**Encapsulation**: The struct groups related data (all kinematic variables) together, demonstrating the principle of **cohesion**.

**Accessor Functions** (Getter/Setter pattern):
```c
// Getter (mechanics.c:31-33)
double get_sv(SUVAT *sv, int i) {
    // Array-like access via pointer arithmetic
}

// Setter (mechanics.c:35-37)
void set_sv(SUVAT *sv, int i, double v) {
    // Encapsulated modification
}
```

**Benefits**:
- **Type safety**: Prevents mixing up s, u, v, a, t values
- **Modularity**: Can pass all 5 values with a single pointer
- **Maintainability**: Adding new properties requires changes in one place

---

## 3. Object-Oriented Design Principles (in C)

While C is a procedural language, this project demonstrates OOP principles through careful use of structs and function pointers.

### 3.1 Classes and Objects

**Class**: Defined via `typedef struct` with associated functions

**Object**: Instance of a struct, allocated either statically or on the stack

**Example 1: Parser "Class"** (`expr_eval.c:29-32`)

```c
// Class definition
typedef struct {
    const char *str;  // Instance variable (attribute)
    int pos;          // Instance variable (attribute)
} Parser;

// Constructor (implicit - stack allocation in eval_expression)
Parser p = {.str = expr, .pos = 0};

// Methods (functions that operate on Parser instances)
static void skip_whitespace(Parser *p);         // Instance method
static double parse_number(Parser *p);          // Instance method
static double parse_factor(Parser *p);          // Instance method
static double parse_term(Parser *p);            // Instance method
static double parse_expression(Parser *p);      // Instance method
```

**Encapsulation**: The Parser struct encapsulates parsing state (str, pos), and methods operate on this state via pointer passing (simulating `self` or `this`).

---

**Example 2: Context Pattern (Generic Object)** (`solver.c:150-155`)

```c
// Generic solver context "class"
typedef struct {
    lv_obj_t **fields;        // Array of input field pointers
    int field_count;          // Number of fields
    lv_obj_t *result_label;   // Result display widget
    void (*solve_fn)(lv_obj_t**, int, lv_obj_t*);  // Function pointer (method)
} SolverCtx;

// Polymorphism via function pointers
static SolverCtx linear_ctx = {
    .fields = linear_fields,
    .field_count = 2,
    .result_label = NULL,
    .solve_fn = solve_linear_fn  // Different implementation
};

static SolverCtx quad_ctx = {
    .fields = quad_fields,
    .field_count = 3,
    .result_label = NULL,
    .solve_fn = solve_quad_fn    // Different implementation
};
```

**Polymorphism**: The same `SolverCtx` "class" can have different `solve_fn` implementations, demonstrating **runtime polymorphism** via function pointers.

**Method Invocation**:
```c
// Calling the polymorphic solve method
ctx->solve_fn(ctx->fields, ctx->field_count, ctx->result_label);
```

This is equivalent to:
```python
# In Python (true OOP):
ctx.solve(ctx.fields, ctx.field_count, ctx.result_label)
```

---

### 3.2 Inheritance and Composition

**Composition Example**: `SubMenuCtx` ("Has-a" relationship) (`ui_submenu.c:7-16`)

```c
typedef struct {
    const SubMenuItem *items;     // HAS-A array of menu items
    int item_count;
    int selection;
    void (*on_menu)(void);        // HAS-A callback function
    lv_obj_t **labels;            // HAS-A array of UI labels
    lv_obj_t *key_recv;           // HAS-A key receiver widget
    lv_group_t *group;            // HAS-A LVGL group
    lv_color_t focus_bg;
} SubMenuCtx;
```

**Aggregation**: `SubMenuCtx` is composed of multiple sub-objects (items, labels, group), demonstrating the **composition** design pattern.

---

### 3.3 Abstraction and Interfaces

**Interface Definition**: `input_hal.h` defines an abstract interface:

```c
// Abstract interface (like a Java interface or C++ pure virtual class)
lv_indev_t *get_navigation_indev(void);
void lvgl_lock(void);
void lvgl_unlock(void);
```

**Concrete Implementations**:

**Implementation 1 (Desktop)**:
```c
lv_indev_t *get_navigation_indev(void) {
    return main_keyboard_indev;  // SDL keyboard
}

void lvgl_lock(void) { /* No-op */ }
void lvgl_unlock(void) { /* No-op */ }
```

**Implementation 2 (ESP32)**:
```c
lv_indev_t *get_navigation_indev(void) {
    return mcp23017_button_get_indev();  // I²C button matrix
}

void lvgl_lock(void) {
    lvgl_port_lock(0);  // FreeRTOS mutex
}

void lvgl_unlock(void) {
    lvgl_port_unlock();
}
```

**Benefit**: Application code calls the interface functions without knowing the underlying platform, achieving **platform independence** through abstraction.

---

## 4. Algorithm Analysis and Complexity

### 4.1 Recursive Descent Parser

**Algorithm**: Recursive descent with backtracking (`expr_eval.c:79-346`)

**Type**: Recursive, top-down parsing

**Grammar** (Backus-Naur Form):
```
<expression> ::= <term> (('+' | '-') <term>)*
<term>       ::= <factor> (('*' | '/' | '^' | '!') <factor> | <factor>)*
<factor>     ::= ['-' | '+'] (<number> | <variable> | <function> '(' <expression> ')' | '(' <expression> ')')
```

**Recursion Base Case**: Parsing a number or variable (no further recursion)

**Recursion Reduction**: Each level processes one operator precedence level, reducing the problem size

**Time Complexity Analysis**:
- **Best case**: O(n) - single pass through string of length n
- **Worst case**: O(n) - still single pass (no backtracking needed)
- **Average case**: O(n)

**Space Complexity**:
- **Call stack depth**: O(d) where d = maximum nesting depth
- **Worst case**: O(n) for expression like "((((n))))" with n nested parentheses

**Proof of Linearity**:
- Each character is examined exactly once (pos always increments forward)
- No backtracking occurs
- Therefore, total operations = O(n)

---

### 4.2 Newton-Raphson Iterative Method

**Algorithm**: Numerical root-finding via successive approximation (`solver.c:47-58`)

**Type**: Iterative (loop-based, not recursive)

**Pseudocode**:
```
Algorithm: newton_raphson(expr, x0, result, iterations)
    x ← x0
    h ← 1e-6
    
    FOR i = 0 TO MAX_ITERATIONS-1 DO
        fx ← evaluate_expression(expr, x)
        
        IF |fx| < TOLERANCE THEN
            result ← x
            iterations ← i + 1
            RETURN success
        END IF
        
        fpx ← numerical_derivative(expr, x, h)
        
        IF |fpx| < 1e-12 THEN
            RETURN failure  // Derivative too small
        END IF
        
        x ← x - fx / fpx  // Newton update formula
    END FOR
    
    result ← x
    iterations ← MAX_ITERATIONS
    RETURN failure
END
```

**Convergence Rate**: Quadratic convergence (errors square each iteration) when close to root

**Time Complexity**: O(k * m) where:
- k = number of iterations (typically < 10 for good initial guess)
- m = cost of evaluating expression (O(n) for expression length n)
- MAX_ITERATIONS = 100 (worst case upper bound)

**Space Complexity**: O(1) - constant extra space (only local variables)

**Termination**: Guaranteed to terminate after at most 100 iterations (no infinite loops)

---

### 4.3 SUVAT Iterative Equation Solver

**Algorithm**: Constraint satisfaction via iterative back-substitution (`mechanics.c:49-93`)

**Type**: Iterative, constraint propagation

**Pseudocode**:
```
Algorithm: solve_suvat(sv)
    max_iterations ← 10
    
    FOR iter = 0 TO max_iterations-1 DO
        prev_count ← count_known(sv)
        
        // Apply all applicable equations
        apply_equation_1(sv)  // v = u + at
        apply_equation_2(sv)  // s = ut + 0.5at²
        apply_equation_3(sv)  // v² = u² + 2as
        apply_equation_4(sv)  // s = (u+v)t/2
        
        curr_count ← count_known(sv)
        
        IF curr_count = 5 THEN
            RETURN 5  // Success: all values solved
        END IF
        
        IF curr_count = prev_count THEN
            BREAK  // No progress made, exit early
        END IF
    END FOR
    
    RETURN curr_count
END
```

**Termination Conditions**:
1. All 5 values solved (success)
2. No progress made in an iteration (failure/partial solution)
3. Maximum iterations reached (10)

**Time Complexity**: O(k * e) where:
- k = number of iterations (max 10, typically 1-2)
- e = number of equations checked per iteration (12)
- Total: O(120) = O(1) constant time

**Space Complexity**: O(1) - operates on fixed SUVAT struct

**Optimization**: Early exit when no progress made (line 88-89), demonstrating **efficiency**.

---

### 4.4 Linear Search

**Location**: Button keymap lookup (`button_keymap.c:96-103`)

**Algorithm**:
```
Algorithm: get_key(row, col)
    FOR i = 0 TO keymap_count-1 DO
        IF keymap[i].row = row AND keymap[i].col = col THEN
            RETURN keymap[i].lvgl_key  // Found
        END IF
    END FOR
    RETURN 0  // Not found
END
```

**Time Complexity**:
- **Best case**: O(1) - target in first position
- **Worst case**: O(n) - target in last position or not present
- **Average case**: O(n/2) = O(n)

**Space Complexity**: O(1) - no extra space needed

**Improvement Potential**: Could use binary search (O(log n)) if array were sorted, or direct addressing (O(1)) as discussed in section 2.4.

---

### 4.5 Binomial Coefficient Calculation

**Location**: `stats.c:79-85`

**Algorithm**: Iterative multiply-divide to avoid factorial overflow

**Pseudocode**:
```
Algorithm: binomial_coeff(n, k)
    IF k < 0 OR k > n THEN
        RETURN 0
    END IF
    
    IF k = 0 OR k = n THEN
        RETURN 1
    END IF
    
    result ← 1.0
    
    FOR i = 0 TO k-1 DO
        result ← result * (n - i)
        result ← result / (i + 1)
    END FOR
    
    RETURN result
END
```

**Mathematical Formula**: C(n,k) = n! / (k!(n-k)!)

**Optimization**: Avoids computing large factorials separately by interleaving multiplication and division, preventing integer overflow.

**Time Complexity**: O(k)

**Space Complexity**: O(1)

**Numerical Stability**: By dividing after each multiplication, intermediate values remain bounded, improving floating-point precision.

---

## 5. Processing Pipelines and Data Flow

### 5.1 Input Processing Pipeline (Event-Driven Architecture)

**Architecture Pattern**: Event-driven, message-passing

**Pipeline Stages**:

```
[Hardware Event]
      ↓
[Platform Driver] ← (SDL2 on desktop, MCP23017 I²C on ESP32)
      ↓
[Keymap Decoder] ← Translates physical position to logical key
      ↓
[Event Queue] ← LVGL internal event queue (FIFO)
      ↓
[LVGL Group Dispatcher] ← Routes to focused widget
      ↓
[Application Event Handler] ← App-specific key handling
      ↓
[State Update] ← Modify application state
      ↓
[UI Refresh] ← Redraw affected screen regions
```

**Queue Usage**: LVGL maintains an internal event queue (FIFO - First In, First Out) ensuring events are processed in the order they arrive.

**Asynchronous Processing**: Events are queued and processed in the main event loop, demonstrating **asynchronous I/O** handling.

---

### 5.2 Expression Evaluation Pipeline

**Pipeline Type**: Synchronous, sequential processing

**Stages**:

```
[String Input] "2+3*4"
      ↓
[Tokenization] ← Identify tokens: 2, +, 3, *, 4
      ↓
[Recursive Parsing] ← Build implicit parse tree
      ↓
[Operator Precedence] ← Apply PEMDAS rules
      ↓
[Function Evaluation] ← Compute sin, cos, etc.
      ↓
[Result] 14.0
```

**Parse Tree** (implicit, built via call stack):
```
      (+)
     /   \
    2    (*)
        /   \
       3     4
```

**Evaluation Order** (post-order traversal):
1. Evaluate 3
2. Evaluate 4
3. Multiply: 3 * 4 = 12
4. Evaluate 2
5. Add: 2 + 12 = 14

**Tree Traversal**: The recursive calls implicitly perform a **post-order traversal** of the expression tree.

---

### 5.3 Graph Rendering Pipeline

**Pipeline Type**: Parallel processing (multiple functions rendered simultaneously)

**Stages**:

```
[Function Definitions] "x^2", "sin(x)", "cos(x)", "x/2"
      ↓
[Enabled Filter] ← Skip disabled functions
      ↓
┌─────────┬─────────┬─────────┬─────────┐
│ Func 0  │ Func 1  │ Func 2  │ Func 3  │ ← Parallel evaluation
└─────────┴─────────┴─────────┴─────────┘
      ↓         ↓         ↓         ↓
[Sample x values] ← For each x in [x_min, x_max]
      ↓         ↓         ↓         ↓
[Evaluate y=f(x)] ← Call expression evaluator
      ↓         ↓         ↓         ↓
[Transform coords] ← Map (x,y) → (px, py)
      ↓         ↓         ↓         ↓
[Draw lines] ← Connect consecutive points
      ↓         ↓         ↓         ↓
      └─────────┴─────────┴─────────┘
                  ↓
          [Composite Frame Buffer]
                  ↓
          [Display to Screen]
```

**Sampling Rate**: One sample per horizontal pixel for smooth curves

**Discontinuity Detection**: Prevents drawing across asymptotes by checking vertical distance between consecutive points:
```
IF |y[i] - y[i-1]| > 2 * SCREEN_HEIGHT THEN
    skip_line_drawing()  // Likely a discontinuity
END IF
```

---

### 5.4 Statistics Calculation Pipeline

**Pipeline Type**: Sequential, single-threaded

**Stages**:

```
[Parameter Input] n, k, p (for binomial)
      ↓
[Validation] ← Check 0 ≤ k ≤ n, 0 ≤ p ≤ 1
      ↓
[Algorithm Selection] ← Route to correct distribution function
      ↓
┌───────────────┬──────────────┬──────────────┐
│ Binomial PMF  │ Normal CDF   │ Inverse Norm │
└───────────────┴──────────────┴──────────────┘
      ↓               ↓              ↓
[Compute Result]  [Erf approx]  [Erf inverse]
      ↓               ↓              ↓
      └───────────────┴──────────────┘
                  ↓
          [Format Result] ← 8 significant figures
                  ↓
          [Display to UI]
```

**Error Function Approximation**: Uses **Abramowitz & Stegun** polynomial approximation (1964), achieving accuracy of 1.5×10⁻⁷.

**Numerical Methods**: Demonstrates practical application of **approximation algorithms** in computational mathematics.

---

## 6. Memory Architecture and Management

### 6.1 Memory Allocation Strategies

**Static Allocation** (Compile-time allocation):

```c
// Allocated in .data or .bss segment (global/static variables)
static HistoryEntry history[MAX_HISTORY];          // calc_math.c:24
static Function functions[MAX_FUNCTIONS];          // graph.c:31
static button_mapping_t keymap[MAX_BUTTONS];       // button_keymap.c:14
static SUVAT current_suvat;                        // mechanics.c:29
```

**Characteristics**:
- **Lifetime**: Entire program execution
- **Location**: Global memory area (not stack, not heap)
- **Size**: Fixed at compile time
- **Access time**: O(1) via direct addressing
- **Thread safety**: Requires synchronization (mutex locks on ESP32)

---

**Automatic Allocation** (Stack allocation):

```c
// Allocated on function call stack
void math_app_start(void) {
    char buffer[128];           // Local array (stack)
    Parser p = {expr, 0};       // Local struct (stack)
    double result = eval(...);  // Local variable (stack)
}  // Automatically deallocated when function returns
```

**Characteristics**:
- **Lifetime**: Function scope
- **Location**: Call stack (grows/shrinks automatically)
- **Size**: Fixed at compile time, limited by stack size
- **Access time**: O(1) via stack pointer offset
- **Deallocation**: Automatic (LIFO stack unwinding)

---

**Dynamic Allocation** (Heap allocation):

```c
// LVGL internally uses heap allocation for UI objects
lv_obj_t *btn = lv_btn_create(parent);  // malloc() internally
lv_obj_t *label = lv_label_create(btn); // malloc() internally

// Manual deallocation when needed
lv_obj_del(btn);  // free() internally
```

**Characteristics**:
- **Lifetime**: Programmer-controlled
- **Location**: Heap (dynamic memory pool)
- **Size**: Runtime-determined
- **Access time**: O(1) via pointer
- **Deallocation**: Manual (requires explicit free/delete)

---

### 6.2 Memory Usage Analysis

**Desktop Platform**:
```
┌────────────────────────────────────┐
│ Stack (per thread):    ~1-2 MB     │ ← Local variables, call frames
├────────────────────────────────────┤
│ Heap (dynamic):        ~5-10 MB    │ ← LVGL objects, SDL buffers
├────────────────────────────────────┤
│ Global/Static:         ~50 KB      │ ← history, functions, keymap
├────────────────────────────────────┤
│ Code (.text):          ~500 KB     │ ← Compiled instructions
├────────────────────────────────────┤
│ Total RSS:             ~10-20 MB   │
└────────────────────────────────────┘
```

**ESP32 Platform**:
```
┌────────────────────────────────────┐
│ Stack (FreeRTOS):      ~20 KB      │ ← Main task stack
├────────────────────────────────────┤
│ Heap (SRAM):           ~250 KB     │ ← LVGL objects
├────────────────────────────────────┤
│ Global/Static:         ~50 KB      │ ← Same as desktop
├────────────────────────────────────┤
│ Frame Buffer (PSRAM):  ~150 KB     │ ← 320x240x2 bytes
├────────────────────────────────────┤
│ Code (Flash):          ~1.5 MB     │ ← Read-only instructions
├────────────────────────────────────┤
│ Total SRAM:            ~320 KB     │
└────────────────────────────────────┘
```

---

### 6.3 Canvas Buffer (Static Pre-allocation)

**Location**: `graph.c:61`

```c
LV_DRAW_BUF_DEFINE_STATIC(canvas_buf, CANVAS_W, CANVAS_H, LV_COLOR_FORMAT_NATIVE);
```

**Expanded Macro** (conceptual):
```c
static uint8_t canvas_buf_data[320 * 205 * 2];  // 131,200 bytes
static lv_draw_buf_t canvas_buf = {
    .data = canvas_buf_data,
    .width = 320,
    .height = 205,
    .color_format = RGB565
};
```

**Memory Layout**:
```
Address        Content
┌──────────────────────────────────────┐
│ Pixel (0,0):   [R5 G6 B5]  2 bytes  │
│ Pixel (1,0):   [R5 G6 B5]  2 bytes  │
│ Pixel (2,0):   [R5 G6 B5]  2 bytes  │
│ ...                                  │
│ Pixel (319,204): [R5 G6 B5] 2 bytes │
└──────────────────────────────────────┘
Total: 320 × 205 × 2 = 131,200 bytes
```

**Advantages of Static Allocation**:
- **No fragmentation**: Memory never freed/reallocated
- **Deterministic**: Size known at compile time
- **Fast access**: No pointer indirection
- **Cache-friendly**: Contiguous memory block

**Disadvantage**:
- **Inflexible**: Cannot resize at runtime

---

### 6.4 String Storage and Manipulation

**Fixed-size Strings** (Character Arrays):

```c
typedef struct {
    char equation[128];  // Fixed 128-byte buffer
    char result[64];     // Fixed 64-byte buffer
} HistoryEntry;
```

**Safety**: Prevents buffer overflow by using bounded functions:
```c
strncpy(history[i].equation, input, 127);  // Max 127 chars + null terminator
history[i].equation[127] = '\0';           // Ensure null termination
```

**String Termination**: All strings use **null-terminated** format (C string convention):
```
"Hello" in memory: ['H']['e']['l']['l']['o']['\0']
                    0    1    2    3    4    5
```

**Length Calculation**: O(n) via `strlen()` (iterates until '\0' found)

---

## 7. Computational Complexity Summary

### 7.1 Big O Notation Analysis

| Algorithm | Time Complexity | Space Complexity | Location |
|-----------|----------------|------------------|----------|
| **Expression Parsing** | O(n) | O(d) stack depth | expr_eval.c:327-346 |
| **Number Tokenization** | O(m) | O(1) | expr_eval.c:40-59 |
| **Function Evaluation** (sin, cos) | O(1) | O(1) | expr_eval.c:79-238 |
| **History Add** | O(h) | O(1) | calc_math.c:114-127 |
| **History Display** | O(h) | O(1) | calc_math.c:129-150 |
| **Button Lookup** | O(k) | O(1) | button_keymap.c:96-103 |
| **Graph Rendering** (per function) | O(w) | O(1) | graph.c:112-136 |
| **Newton-Raphson** | O(i·n) | O(1) | solver.c:47-58 |
| **SUVAT Solver** | O(1) | O(1) | mechanics.c:49-93 |
| **Binomial Coefficient** | O(k) | O(1) | stats.c:79-85 |

**Legend**:
- n = expression string length
- d = maximum nesting depth
- m = number length
- h = MAX_HISTORY (10)
- k = number of buttons (54) or binomial k parameter
- w = canvas width (320 pixels)
- i = Newton-Raphson iterations (≤100)

---

### 7.2 Optimization Opportunities

**1. Button Keymap Lookup**: O(n) → O(1)
```c
// Current: Linear search through 54 entries
// Optimized: Direct addressing
uint32_t keymap_2d[BUTTON_ROWS][BUTTON_COLS];
return keymap_2d[row][col];  // O(1) access
```

**2. History Buffer**: O(n) shift → O(1) circular buffer
```c
// Current: Shifts entire array when full
// Optimized: Circular buffer with modulo
int head = 0, tail = 0;
history[(tail++) % MAX_HISTORY] = new_entry;  // O(1) append
```

**3. String Copying**: Use `memcpy` instead of `strncpy` when length is known
```c
// Current: strncpy scans for null terminator
// Optimized: memcpy with known length
memcpy(dest, src, known_length);  // O(n) but faster constant factor
```

---

## 8. Software Engineering Principles Demonstrated

### 8.1 Modularity and Separation of Concerns

**Principle**: Each module has a single, well-defined responsibility.

**Examples**:
- `expr_eval.c` - Expression evaluation only
- `graph.c` - Graph rendering only
- `stats.c` - Statistical distributions only
- `input_hal.h` - Input abstraction only

**Benefits**:
- **Maintainability**: Changes to one module don't affect others
- **Testability**: Each module can be tested independently
- **Reusability**: Modules can be used in different contexts

---

### 8.2 DRY (Don't Repeat Yourself)

**Principle**: Avoid code duplication by abstracting common patterns.

**Example 1: Reusable Submenu Component** (`ui_submenu.c`)

Used by:
- Solver app (solver.c:127-146)
- Stats app (stats.c:238-249)
- Settings app (settings.c:348-355)

**Code Reduction**: Instead of 3 separate menu implementations (300+ lines each), one reusable component (144 lines) is used.

**Example 2: Design System** (`ui_common.h`)

Centralized:
- Color definitions (COL_ACCENT_MATH, COL_ACCENT_GRAPH, etc.)
- Layout constants (CONTENT_TOP, HINT_BAR_H, etc.)
- Helper functions (ui_setup_screen, ui_style_textarea, etc.)

**Consistency**: All apps use the same design tokens, ensuring visual uniformity.

---

### 8.3 KISS (Keep It Simple, Stupid)

**Principle**: Simple solutions are preferable to complex ones.

**Example**: Linear solver uses direct formula instead of matrix inversion:
```c
// Simple: x = -b/a
double x = -b / a;

// Complex (unnecessary): Use Gaussian elimination on 1×1 matrix
```

**Example**: Fixed-size arrays instead of dynamic data structures:
```c
static Function functions[MAX_FUNCTIONS];  // Simple, predictable
// vs.
Function **functions = malloc(...);        // Complex, error-prone
```

---

### 8.4 Defensive Programming

**Principle**: Anticipate and handle errors gracefully.

**Example 1: Division by Zero** (expr_eval.c:277-281)
```c
if (divisor != 0) {
    result /= divisor;
} else {
    return NAN;  // Safe failure mode
}
```

**Example 2: Null Pointer Check** (expr_eval.c:350-352)
```c
if (!expr || strlen(expr) == 0) {
    return NAN;  // Don't dereference null
}
```

**Example 3: Array Bounds** (mechanics.c:31-33)
```c
double get_sv(SUVAT *sv, int i) {
    switch(i) {
        case 0: return sv->s;
        // ... other cases
        default: return SUVAT_UNKNOWN;  // Out of range
    }
}
```

**Example 4: Buffer Overflow Prevention** (calc_math.c:114-127)
```c
strncpy(history[history_count].equation, equation, 127);
history[history_count].equation[127] = '\0';  // Explicit termination
```

---

### 8.5 Information Hiding (Encapsulation)

**Principle**: Hide implementation details, expose only necessary interfaces.

**Example**: Parser internals are private (static functions):
```c
// Public API (exported in header)
double eval_expression(const char *expr);
double eval_expression_x(const char *expr, double x_val);

// Private implementation (static, not exported)
static void skip_whitespace(Parser *p);
static double parse_number(Parser *p);
static double parse_factor(Parser *p);
static double parse_term(Parser *p);
static double parse_expression(Parser *p);
```

**Users can't access**: Internal parsing functions, Parser struct details

**Users can access**: High-level evaluation functions only

**Benefit**: Internal implementation can change without breaking code that uses the module.

---

## 9. Computational Thinking Skills Demonstrated

### 9.1 Decomposition

**Skill**: Breaking down complex problems into manageable sub-problems.

**Example**: Calculator system decomposed into:
1. Input handling (button matrix scanning)
2. Expression parsing (tokenization, parsing, evaluation)
3. Graphing (sampling, coordinate transformation, rendering)
4. Statistics (distribution calculation, numerical methods)
5. Equation solving (algebraic, iterative methods)

Each sub-problem is solved independently and integrated.

---

### 9.2 Pattern Recognition

**Skill**: Identifying recurring patterns and applying general solutions.

**Example 1**: All solver types (linear, quadratic, Newton-Raphson) follow the same pattern:
1. Read coefficients/inputs
2. Call solving function
3. Display result

This pattern is abstracted in `SolverCtx` (solver.c:150-155).

**Example 2**: All apps follow the same lifecycle pattern:
1. Entry point function `*_app_start()`
2. Clean screen
3. Create UI elements
4. Set up event handlers
5. Add to LVGL group

---

### 9.3 Abstraction

**Skill**: Removing unnecessary details to focus on essential concepts.

**Example**: Input handling abstracted via HAL:
- Apps don't know: SDL vs. I²C, Windows vs. ESP32, keyboard vs. buttons
- Apps only know: "Get input device", "Lock LVGL", "Unlock LVGL"

This abstraction allows the same application code to run on desktop and embedded platforms.

---

### 9.4 Algorithm Design

**Skill**: Creating step-by-step procedures to solve problems.

**Example**: SUVAT solver algorithm designed from scratch:
1. Identified: Given any 3 values, we can derive the other 2
2. Listed: All 4 fundamental SUVAT equations
3. Derived: Rearrangements of each equation
4. Designed: Iterative application strategy
5. Implemented: Early exit optimization

This demonstrates **algorithmic thinking** from problem to solution.

---

## 10. Conclusion

This scientific calculator system demonstrates extensive application of AQA Computer Science A-Level concepts:

**Data Structures**: Stack (LIFO), circular buffer (FIFO), arrays, records, associative arrays

**Algorithms**: Recursive descent parsing, Newton-Raphson iteration, linear search, numerical methods

**OOP Principles**: Classes/structs, encapsulation, polymorphism (via function pointers), abstraction

**Memory Management**: Static allocation, automatic allocation (stack), understanding heap vs. stack

**Complexity Analysis**: Big O notation, time/space tradeoffs, optimization opportunities

**Software Engineering**: Modularity, DRY, KISS, defensive programming, information hiding

**Computational Thinking**: Decomposition, pattern recognition, abstraction, algorithm design

The project successfully demonstrates how theoretical computer science concepts translate into practical, working software systems across multiple platforms.

---

**Total Lines of Code**: ~4,500 (excluding libraries)

**Languages**: C (99%), Python (build scripts)

**Platforms**: Windows, macOS, Linux (desktop), ESP32-S3 (embedded)

**Paradigm**: Procedural programming with OOP principles

**Architecture**: Layered, modular, event-driven
