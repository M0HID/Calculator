# DOCUMENTED DESIGN

## Overview

This project implements a graphical scientific calculator system based on the LVGL (Light and Versatile Graphics Library) framework. The calculator provides six main applications accessible through a grid menu interface: a Mathematics calculator with expression evaluation and history, a Function Graphing application with multi-function plotting and trace mode, a Statistics calculator for probability distributions, a Numerical Methods solver (planned), a Mechanics calculator (planned), and a Matrices calculator (planned). The system is designed to run on multiple platforms including desktop (Windows/Linux) using SDL2, and embedded systems (ESP32) with physical button input.

The architecture follows a modular design pattern where each application is self-contained and can be launched from a main menu interface. Currently, three core applications are fully implemented: Math, Graph, and Stats. The expression evaluator forms the computational core, implementing a recursive descent parser to evaluate mathematical expressions with support for standard operators, functions, and variables. Input handling is abstracted through a Hardware Abstraction Layer (HAL) allowing the same codebase to support both physical buttons and keyboard input across different platforms.

The system demonstrates key computer science concepts including parsing algorithms, event-driven programming, real-time graphics rendering, numerical computation, and cross-platform hardware abstraction.

## Structure Hierarchy Chart

```
Main Application (main.c:394-469)
├── Hardware Abstraction Layer (hal.c)
│   ├── SDL Display Initialization (main.c:403-423)
│   └── Input Device Setup (main.c:406-427)
│
├── Button Input System (main.c:264-355)
│   ├── Keymap Configuration (button_keymap.c:18-92)
│   └── Event Dispatcher (main.c:206-221, main.c:230-262)
│
└── Application Layer
    ├── Main Menu (main_menu.c:155-239)
    │   ├── Grid Navigation (main_menu.c:48-90)
    │   └── App Launcher (main_menu.c:92-108)
    │
    ├── Math Calculator (math.c:286-374)
    │   ├── Expression Evaluator (expr_eval.c:193-210)
    │   ├── History Management (math.c:153-172)
    │   └── Special Key Handler (math.c:110-151)
    │
    ├── Graph Plotter (graph.c:630-642)
    │   ├── Function List Screen (graph.c:488-587)
    │   │   ├── Function Editor (graph.c:318-337)
    │   │   └── List Navigation (graph.c:339-382)
    │   │
    │   └── Graph View Screen (graph.c:589-626)
    │       ├── Coordinate Transform (graph.c:98-104)
    │       ├── Axes Renderer (graph.c:108-165)
    │       ├── Function Plotter (graph.c:167-202)
    │       ├── Trace Mode (graph.c:204-243)
    │       └── Zoom/Pan Controls (graph.c:384-484)
    │
    ├── Statistics App (stats.c:292-366)
    │   ├── Distribution Calculator (stats.c:174-234)
    │   └── Math Functions
    │       ├── Binomial Distribution (stats.c:77-113)
    │       ├── Normal Distribution (stats.c:115-149)
    │       └── Inverse Normal (stats.c:163-170)
    │
    ├── Solver/Numerical Methods App (solver.c:1012-1014)
    │   ├── Solver Menu (solver.c:949-1009)
    │   ├── Linear Solver (ax+b=0) (solver.c:293-372)
    │   ├── Quadratic Solver (ax²+bx+c=0) (solver.c:404-492)
    │   ├── 2×2 Simultaneous Equations (solver.c:520-596)
    │   ├── 3×3 Simultaneous Equations (solver.c:628-723)
    │   ├── Newton-Raphson Solver (solver.c:750-835)
    │   ├── Numerical Methods (Placeholder) (solver.c:838-859)
    │   ├── Curve Fitting (Placeholder) (solver.c:861-882)
    │   ├── Function Storage (Placeholder) (solver.c:884-905)
    │   └── Math Functions
    │       ├── Linear Equation Solver (solver.c:108-114)
    │       ├── Quadratic Equation Solver (solver.c:117-141)
    │       ├── Cramer's Rule (2×2) (solver.c:146-156)
    │       ├── Cramer's Rule (3×3) (solver.c:159-186)
    │       ├── Newton-Raphson Method (solver.c:73-105)
    │       ├── Numerical Derivative (solver.c:57-70)
    │       ├── Linear Regression (solver.c:189-202)
    │       └── Quadratic Regression (solver.c:205-242)
    │
    ├── Mechanics App (mechanics.c:381-401)
    │   ├── Input Page (mechanics.c:231-314)
    │   ├── Result Page (mechanics.c:316-379)
    │   └── SUVAT Solver
    │       ├── Equation System (mechanics.c:9-14)
    │       ├── Value Management (mechanics.c:49-69)
    │       ├── Known Value Counter (mechanics.c:72-80)
    │       └── Iterative Equation Solver (mechanics.c:83-167)
    │
    └── Matrices App (Planned - main_menu.c:106)
        ├── Matrix Operations (Add, Multiply, Transpose)
        ├── Determinant Calculator
        └── Matrix Inversion
```

## System Flowchart

```
[START] → [Initialize LVGL] (main.c:399)
              ↓
    [Create Displays] (main.c:403-419)
    (Main: 320×240, Keypad: 320×270)
              ↓
    [Load Button Keymap] (main.c:430-433)
              ↓
    [Show Main Menu] (main.c:452)
              ↓
    ┌─────────────────────────────────────┐
    │   Main Menu Screen (3×2 Grid)       │
    │   (main_menu.c:155)                 │
    └─────────────────────────────────────┘
              ↓
    User Selects App (Arrow Keys + Enter)
              ↓
    ┌─────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
    │         │          │          │          │          │          │
  [Math]   [Graph]   [Stats]  [Numerical] [Mechanics] [Matrices]
  Button0   Button1   Button2   Button3      Button4     Button5
    │         │          │          │          │          │
    ↓         ↓          ↓          ↓          ↓          ↓
┌────────┐┌────────┐┌────────┐┌──────────┐┌──────────┐┌──────────┐
│ Math   ││ Graph  ││ Stats  ││ Solver/  ││Mechanics ││ Matrices │
│  App   ││  App   ││  App   ││Numerical ││   App    ││   App    │
│(math.c)││(graph.c││(stats.c││(solver.c)││(mechanics││(Planned) │
│:286-374││:630-642││:292-366││:1012-1014││.c:381-401││          │
└────────┘└────────┘└────────┘└──────────┘└──────────┘└──────────┘
    │         │          │          │          │
    ↓         ↓          ↓          ↓          ↓
    │         │          │          │          │
    │         │          │  [Key Event Handler]
    │         │          │  (Waits for button/keyboard input)
    │         │          │
    ↓         ↓          ↓
┌────────────────────────────────────────┐
│        Key Event Routing               │
│  - Number/Operator Keys                │
│  - Function Keys (sin, cos, sqrt, etc) │
│  - Navigation Keys (↑↓←→)              │
│  - Action Keys (=, AC, DEL, MENU)      │
└────────────────────────────────────────┘
    │         │          │
    ↓         ↓          ↓
    │         │          │
┌───────────┐ │          │
│ Math App  │ │          │
│ Key Event │ │          │
│(math.c:   │ │          │
│ 207-284)  │ │          │
└───────────┘ │          │
    ↓         │          │
┌───────────┐ │          │
│Parse Input│ │          │
│(expr_eval.│ │          │
│c:193-210) │ │          │
└───────────┘ │          │
    ↓         │          │
┌───────────┐ │          │
│Evaluate   │ │          │
│Expression │ │          │
│(Recursive │ │          │
│ Descent)  │ │          │
└───────────┘ │          │
    ↓         │          │
┌───────────┐ │          │
│Display    │ │          │
│Result +   │ │          │
│Add to     │ │          │
│History    │ │          │
│(math.c:   │ │          │
│ 153-172)  │ │          │
└───────────┘ │          │
    │         │          │
    │    ┌────────────┐  │
    │    │ Graph App  │  │
    │    │ Key Event  │  │
    │    │(graph.c:   │  │
    │    │ 339-382,   │  │
    │    │  384-484)  │  │
    │    └────────────┘  │
    │         ↓          │
    │    ┌────────────┐  │
    │    │Function    │  │
    │    │List Screen │  │
    │    │or          │  │
    │    │Graph View  │  │
    │    │Screen      │  │
    │    └────────────┘  │
    │         ↓          │
    │    ┌────────────┐  │
    │    │IF Graph    │  │
    │    │View:       │  │
    │    │- Clear     │  │
    │    │  Canvas    │  │
    │    │- Draw Axes │  │
    │    │- Plot All  │  │
    │    │  Functions │  │
    │    │- Draw Trace│  │
    │    │(graph.c:   │  │
    │    │ 245-252)   │  │
    │    └────────────┘  │
    │         ↓          │
    │    ┌────────────┐  │
    │    │FOR each    │  │
    │    │function:   │  │
    │    │- Evaluate  │  │
    │    │  at points │  │
    │    │- Transform │  │
    │    │  coords    │  │
    │    │- Draw lines│  │
    │    │(graph.c:   │  │
    │    │ 167-202)   │  │
    │    └────────────┘  │
    │         ↓          │
    │    ┌────────────┐  │
    │    │Display     │  │
    │    │Graph +     │  │
    │    │Info Labels │  │
    │    └────────────┘  │
    │         │          │
    │         │     ┌────────────┐
    │         │     │ Stats App  │
    │         │     │ Key Event  │
    │         │     │(stats.c:   │
    │         │     │  244-290)  │
    │         │     └────────────┘
    │         │          ↓
    │         │     ┌────────────┐
    │         │     │Read Params:│
    │         │     │- n, k, p   │
    │         │     │  or        │
    │         │     │- x, μ, σ   │
    │         │     │(stats.c:   │
    │         │     │  185-193)  │
    │         │     └────────────┘
    │         │          ↓
    │         │     ┌────────────┐
    │         │     │Select Dist:│
    │         │     │- Binomial  │
    │         │     │- Normal    │
    │         │     │- Inverse   │
    │         │     │(stats.c:   │
    │         │     │  197-220)  │
    │         │     └────────────┘
    │         │          ↓
    │         │     ┌────────────┐
    │         │     │Calculate   │
    │         │     │Result using│
    │         │     │Math Funcs  │
    │         │     │(stats.c:   │
    │         │     │  67-170)   │
    │         │     └────────────┘
    │         │          ↓
    │         │     ┌────────────┐
    │         │     │Format &    │
    │         │     │Display     │
    │         │     │Result      │
    │         │     │(stats.c:   │
    │         │     │  222-233)  │
    │         │     └────────────┘
    │         │          │
    └─────────┴──────────┘
              ↓
    ┌─────────────────┐
    │ Check for MENU  │
    │ Key Press       │
    │ (Key code 'M')  │
    └─────────────────┘
              ↓
         [Is MENU?]
              │
       Yes ←──┴──→ No
        │           │
        ↓           ↓
[Return to      [Continue in
 Main Menu]      Current App]
 (main_menu.         │
  c:155)             │
        │            │
        └────────────┘
              ↓
    ┌─────────────────────────┐
    │   Main Event Loop       │
    │   lv_timer_handler()    │
    │   (main.c:454-466)      │
    │   - Updates displays    │
    │   - Processes input     │
    │   - Manages animations  │
    └─────────────────────────┘
              ↓
         [Loop Forever]
              │
              └──────→ Back to Event Handler
```

## Data Flow Diagram

```
┌──────────────────────────────────────────────────────────────────┐
│                     INPUT LAYER                                  │
└──────────────────────────────────────────────────────────────────┘
    ↓                                    ↓
[Physical       [Keyboard               [Mouse
 Buttons]        Input]                  Click]
    │              │                       │
    └──────────────┴───────────────────────┘
                   ↓
         ┌──────────────────┐
         │  Keymap Decoder  │ (main.c:157-203, button_keymap.c:96-112)
         └──────────────────┘
                   ↓
         ┌──────────────────┐
         │ Event Dispatcher │ (main.c:206-221)
         └──────────────────┘
                   ↓
         ┌──────────────────┐
         │  LVGL Group      │ (main_menu.c:182-188)
         │  Key Routing     │
         └──────────────────┘
                   ↓
    ┌──────────────┼──────────────┐
    ↓              ↓              ↓
┌────────┐   ┌──────────┐   ┌────────┐
│  Math  │   │  Graph   │   │ Stats  │
│  App   │   │   App    │   │  App   │
└────────┘   └──────────┘   └────────┘
    │              │              │
    ↓              ↓              ↓

─────────── MATH APP DATA FLOW ────────────

User Input → [Text Buffer] (math.c:47,69-72)
                   ↓
          [Expression String]
                   ↓
         ┌──────────────────┐
         │ Expression Parser│ (expr_eval.c:35-191)
         │ (Recursive       │
         │  Descent)        │
         └──────────────────┘
                   ↓
    ┌──────────────┼──────────────┐
    ↓              ↓              ↓
[Tokenizer]  [Operator    [Function
(expr_eval.  Precedence]   Evaluator]
c:35-60)     (expr_eval.   (expr_eval.
             c:143-191)    c:64-141)
                   ↓
          [Numeric Result]
                   ↓
         ┌──────────────────┐
         │ History Storage  │ (math.c:16-19, 153-172)
         │ (Circular Buffer)│
         └──────────────────┘
                   ↓
         [Display Update] (math.c:46-67)

─────────── GRAPH APP DATA FLOW ───────────

Function Input → [Function Array] (graph.c:40-50)
                       ↓
              [Graph View Request]
                       ↓
         ┌──────────────────────────┐
         │  Canvas Rendering Loop   │ (graph.c:245-252)
         └──────────────────────────┘
                       ↓
    ┌──────────────────┼──────────────────┐
    ↓                  ↓                  ↓
[Coordinate      [Expression         [Grid/Axes
Transform]       Evaluation]         Drawing]
(graph.c:        (expr_eval.c:       (graph.c:
98-104)          202-210)            108-165)
    ↓                  ↓                  ↓
[Canvas X,Y]     [Y Values]         [Line Objects]
    │                  │                  │
    └──────────────────┴──────────────────┘
                       ↓
              [Draw Function Path] (graph.c:167-202)
                       ↓
         ┌──────────────────────────┐
         │   Frame Buffer (LVGL)    │
         └──────────────────────────┘
                       ↓
         [Display Hardware (SDL/LCD)]

──────────── STATS APP DATA FLOW ──────────

Parameter Input → [Parameter Array] (stats.c:49-50, 185-193)
                       ↓
              [Calculate Request]
                       ↓
         ┌──────────────────────────┐
         │ Distribution Type Router │ (stats.c:197-220)
         └──────────────────────────┘
                       ↓
    ┌──────────────────┼──────────────────┐
    ↓                  ↓                  ↓
[Binomial PMF]   [Normal PDF/CDF]   [Inverse Normal]
(stats.c:        (stats.c:          (stats.c:
93-99)           116-149)           164-170)
    │                  │                  │
    └──────────────────┴──────────────────┘
                       ↓
         [Statistical Result Value]
                       ↓
         [Format & Display] (stats.c:222-233)

──────────── SOLVER APP DATA FLOW ──────────

Solver Menu → [Mode Selection] (solver.c:949-1009)
                      ↓
    ┌─────────────────┼──────────────────┬──────────────┐
    ↓                 ↓                  ↓              ↓
[Linear]        [Quadratic]        [2×2 Simul]   [3×3 Simul]
solver.c:       solver.c:          solver.c:      solver.c:
293-372         404-492            520-596        628-723
    │                 │                  │              │
    ↓                 ↓                  ↓              ↓
[Read Coeffs]   [Read Coeffs]    [Read Coeffs]  [Read Coeffs]
a, b            a, b, c          a1,b1,c1       a[3][3]
                                 a2,b2,c2       b[3]
    │                 │                  │              │
    ↓                 ↓                  ↓              ↓
[Linear Solve]  [Quadratic       [Cramer's 2×2] [Cramer's 3×3]
solver.c:       Formula]         solver.c:      solver.c:
108-114         solver.c:        146-156        159-186
x = -b/a        117-141          det, det_x,    4 determinants
                discriminant     det_y
    │                 │                  │              │
    └─────────────────┴──────────────────┴──────────────┘
                       ↓
              [Solution Values]
              x, (y), (z)
                       ↓
              [Format & Display]


Newton-Raphson Path:
User Input → [Function Expression + x0] (solver.c:750-835)
                       ↓
            [Initialize: x = x0]
                       ↓
          ┌────────────────────────┐
          │  Iteration Loop        │ (solver.c:77-99)
          │  (MAX_ITER = 100)      │
          └────────────────────────┘
                       ↓
     ┌─────────────────┼─────────────────┐
     ↓                 ↓                 ↓
[Evaluate f(x)]  [Numerical      [Check Convergence]
expr_eval.c:     Derivative]     |f(x)| < 1e-8?
202-210          solver.c:57-70
                 f'(x) ≈ (f(x+h)-f(x-h))/2h
     │                 │                 │
     └─────────────────┴─────────────────┘
                       ↓
              [Newton Update]
              x_new = x - f(x)/f'(x)
                       ↓
         ┌─────────────┴─────────────┐
         │                           │
    Converged?                  Not converged?
         │                           │
         ↓                           ↓
    [Result +              [Max iterations]
     Iterations]           [Last value]
         │                           │
         └───────────────┬───────────┘
                         ↓
                [Display Result]

──────────── MECHANICS APP DATA FLOW ──────────

Input Page → [5 Input Fields] (mechanics.c:231-314)
  s, u, v, a, t                ↓
                    [Read Field Values]
                    mechanics.c:177-185
                               ↓
                    [Count Known Values]
                    mechanics.c:72-80
                               ↓
                     ┌─────────┴─────────┐
                     │                   │
                 < 3 values?         ≥ 3 values?
                     │                   │
                     ↓                   ↓
            [Error Dialog]      [SUVAT Structure]
            "Need 3 values"     mechanics.c:20-26
                                        ↓
                          ┌─────────────────────────┐
                          │ Iterative Solver        │
                          │ (Max 10 iterations)     │
                          │ mechanics.c:83-167      │
                          └─────────────────────────┘
                                        ↓
              ┌─────────────────────────┼─────────────────────────┐
              ↓                         ↓                         ↓
      [Equation Set 1]          [Equation Set 2]          [Equation Set 3]
      v = u + at               s = ut + ½at²            v² = u² + 2as
      u = v - at               s = (u+v)t/2             u = √(v²-2as)
      t = (v-u)/a                                       a = (v²-u²)/2s
      a = (v-u)/t                                       s = (v²-u²)/2a
      mechanics.c:             mechanics.c:             mechanics.c:
      91-112                   114-124                  126-154
              │                         │                         │
              └─────────────────────────┴─────────────────────────┘
                                        ↓
                          [Check Progress]
                          known_count increased?
                                        ↓
                          ┌─────────────┴─────────────┐
                          │                           │
                     All 5 known?              No progress?
                          │                           │
                          ↓                           ↓
                 [Success: 5 values]        [Error: Cannot solve]
                          │
                          ↓
                 [Result Page] (mechanics.c:316-379)
                          ↓
              ┌───────────┴───────────┐
              ↓                       ↓
      [Display All 5 Values]   [Show Equations]
      s, u, v, a, t            v=u+at, s=ut+½at²
      mechanics.c:342-352      v²=u²+2as, s=(u+v)t/2
                               mechanics.c:355-358
              │
              ↓
    [Back Button] → Return to Input Page (values preserved)
    [Menu Button] → Return to Main Menu (reset values)
```

## Key Algorithms

### 1. Recursive Descent Expression Parser

**Location:** src/expr_eval.c:65-286

**Purpose:** Parse and evaluate mathematical expressions with proper operator precedence

**Algorithm Description:**
The parser implements a recursive descent algorithm that respects mathematical operator precedence (PEMDAS). It uses three mutually recursive functions representing different precedence levels:

```
parse_expression() → handles addition/subtraction (lowest precedence)
    ↓
parse_term() → handles multiplication/division/power/factorial/implicit multiplication
    ↓
parse_factor() → handles unary operators, numbers, variables, functions, parentheses (highest precedence)
```

**Enhanced Features:**
1. **Unary Operators:** Supports unary minus and plus (e.g., `-5`, `+3`)
2. **Implicit Multiplication:** Automatically handles `2pi`, `3x`, `(2)(3)` without explicit `*`
3. **Postfix Factorial:** Supports `5!` notation
4. **Extended Functions:** `sin()`, `cos()`, `tan()`, `sqrt()`, `ln()`, `log()`, `abs()`, `floor()`, `ceil()`
5. **Constants:** `pi` (3.14159...), `e` (2.71828...)
6. **Variables:** `x` (for graphing)

**Pseudocode:**
```
FUNCTION parse_expression(parser):
    result = parse_term(parser)
    
    WHILE current_token is '+' or '-':
        operator = current_token
        advance_parser()
        
        IF operator is '+':
            result = result + parse_term(parser)
        ELSE IF operator is '-':
            result = result - parse_term(parser)
    
    RETURN result

FUNCTION parse_term(parser):
    result = parse_factor(parser)
    
    // Check for postfix factorial
    IF current_token is '!':
        advance_parser()
        result = factorial(result)
    
    LOOP:
        skip_whitespace(parser)
        operator = current_token
        
        IF operator is '*' or '/' or '^':
            advance_parser()
            
            IF operator is '*':
                result = result * parse_factor(parser)
            ELSE IF operator is '/':
                divisor = parse_factor(parser)
                IF divisor ≠ 0:
                    result = result / divisor
                ELSE:
                    RETURN NaN
            ELSE IF operator is '^':
                result = power(result, parse_factor(parser))
        
        // Implicit multiplication detection
        ELSE IF operator is '(' OR 
                operator is 'x' OR 'X' OR
                operator starts with function name OR
                operator is 'p' followed by 'i' (pi) OR
                operator is 'e' followed by non-alpha (e constant):
            result = result * parse_factor(parser)
            
            // Check for factorial after implicit multiplication
            IF current_token is '!':
                advance_parser()
                result = factorial(result)
        
        ELSE:
            BREAK LOOP
    
    RETURN result

FUNCTION parse_factor(parser):
    skip_whitespace(parser)
    
    // Handle unary minus
    IF current_token is '-':
        advance_parser()
        RETURN -parse_factor(parser)
    
    // Handle unary plus
    IF current_token is '+':
        advance_parser()
        RETURN parse_factor(parser)
    
    // Handle parentheses
    IF current_token is '(':
        advance_parser()
        result = parse_expression(parser)  // Recursive call
        match_token(')')
        RETURN result
    
    // Handle pi constant
    IF current_token is "pi":
        advance_parser_by(2)
        RETURN 3.14159265358979323846
    
    // Handle e constant (followed by non-alpha)
    IF current_token is 'e' AND next_token is not alpha:
        advance_parser()
        RETURN 2.71828182845904523536
    
    // Handle variable x
    IF current_token is 'x' or 'X':
        advance_parser()
        RETURN current_x_value
    
    // Handle functions (check longer names first to avoid prefix conflicts)
    IF current_token matches function name:
        function_name = extract_function_name()
        advance_parser_past_function_name()
        match_token('(')
        argument = parse_expression(parser)
        match_token(')')
        RETURN evaluate_function(function_name, argument)
    
    // Handle numbers
    RETURN parse_number(parser)

FUNCTION factorial(n):
    IF n < 0 OR n is not integer:
        RETURN NaN
    
    IF n > 170:
        RETURN INFINITY  // Overflow protection
    
    result = 1.0
    FOR i FROM 2 TO n:
        result = result * i
    
    RETURN result
```

**Implementation References:**
- Factorial function: expr_eval.c:65-78
- Unary operators & factor parsing: expr_eval.c:80-204
- Multiplication/division/power/implicit mult: expr_eval.c:211-286
- Addition/subtraction level: expr_eval.c:288-306
- Expression parsing entry point: expr_eval.c:309-323
- Number tokenization: expr_eval.c:47-63

**Function Support:**
- Trigonometric: `sin()`, `cos()`, `tan()` (expr_eval.c:159-188)
- Logarithmic: `ln()` (natural log), `log()` (base-10) (expr_eval.c:191-204)
- Roots: `sqrt()` (expr_eval.c:144-149)
- Rounding: `floor()`, `ceil()`, `abs()` (expr_eval.c:128-143)
- Factorial: `n!` postfix operator (expr_eval.c:65-78, 228-231, 278-281)

**Constants:**
- π (pi): 3.14159265358979323846 (expr_eval.c:113-117, M_PI defined at line 26)
- e (Euler's number): 2.71828182845904523536 (expr_eval.c:119-123, M_E defined at line 29)

**Example Evaluations:**
```
Expression          → Parsing Steps → Result
--------------------|----------------|----------
"5!"                → 5, factorial   → 120
"-5"                → unary minus    → -5
"2pi"               → 2*3.14159...   → 6.28318...
"sin(pi)"           → sin(3.14159)   → 0 (rounded from 1.2e-16)
"3(2+1)"            → 3*(2+1)        → 9
"ln(e)"             → ln(2.71828)    → 1
"log(100)"          → log10(100)     → 2
"abs(-5)"           → fabs(-5)       → 5
"floor(3.7)"        → floor(3.7)     → 3
"ceil(2.1)"         → ceil(2.1)      → 3
"2^8"               → pow(2, 8)      → 256
"(2+3)!"            → 5, factorial   → 120
"2x" (x=3)          → 2*3            → 6
```

**Operator Precedence (highest to lowest):**
1. Parentheses `( )`
2. Functions (sin, cos, sqrt, etc.)
3. Unary operators (`-`, `+`)
4. Factorial (`!`)
5. Exponentiation (`^`, right associative)
6. Implicit multiplication (detected before explicit)
7. Multiplication/Division (`*`, `/`, left associative)
8. Addition/Subtraction (`+`, `-`, left associative)

**Time Complexity:** O(n) where n is the length of the expression string
**Space Complexity:** O(d) where d is the maximum nesting depth of parentheses/functions

---

### 2. Binomial Coefficient Calculation

**Location:** src/stats.c:78-90

**Purpose:** Efficiently calculate combinations C(n,k) = n!/(k!(n-k)!) without integer overflow

**Algorithm Description:**
Instead of calculating three large factorials separately, the algorithm computes the binomial coefficient using a product-division approach that maintains smaller intermediate values to prevent overflow.

**Mathematical Formula:**
```
C(n,k) = n!/(k!(n-k)!) = (n × (n-1) × ... × (n-k+1)) / (k × (k-1) × ... × 1)
```

**Pseudocode:**
```
FUNCTION binomial_coefficient(n, k):
    IF k < 0 OR k > n:
        RETURN 0
    
    IF k = 0 OR k = n:
        RETURN 1
    
    result = 1.0
    
    FOR i FROM 0 TO k-1:
        result = result × (n - i)
        result = result / (i + 1)
    
    RETURN result
```

**Implementation Reference:** stats.c:78-90

**Example Execution:** C(5,2)
```
i=0: result = 1 × 5 / 1 = 5
i=1: result = 5 × 4 / 2 = 10
Return: 10
```

**Time Complexity:** O(k)
**Space Complexity:** O(1)

---

### 3. Error Function Approximation (Abramowitz-Stegun)

**Location:** src/stats.c:124-141

**Purpose:** Approximate the error function erf(x) required for normal distribution CDF calculations

**Algorithm Description:**
Implements the Abramowitz and Stegun formula (1964), which provides an approximation accurate to within 1.5×10⁻⁷ for all x. The error function is defined as:

```
erf(x) = (2/√π) ∫₀ˣ e^(-t²) dt
```

**Formula:**
```
erf(x) ≈ 1 - (a₁t + a₂t² + a₃t³ + a₄t⁴ + a₅t⁵)e^(-x²)
where t = 1/(1 + px), p = 0.3275911
```

**Pseudocode:**
```
FUNCTION erf_approximation(x):
    // Constants from Abramowitz & Stegun
    a1 =  0.254829592
    a2 = -0.284496736
    a3 =  1.421413741
    a4 = -1.453152027
    a5 =  1.061405429
    p  =  0.3275911
    
    sign = 1
    IF x < 0:
        sign = -1
        x = absolute_value(x)
    
    t = 1.0 / (1.0 + p × x)
    y = 1.0 - (((((a5×t + a4)×t + a3)×t + a2)×t + a1)×t × exp(-x²))
    
    RETURN sign × y
```

**Implementation Reference:** stats.c:124-141

**Usage:** Used in normal distribution CDF calculation at stats.c:144-149

**Accuracy:** Maximum error < 1.5×10⁻⁷

---

### 4. Coordinate Transformation for Graphing

**Location:** src/graph.c:98-104

**Purpose:** Transform mathematical coordinates (x,y) to screen pixel coordinates for rendering

**Algorithm Description:**
Linear transformation that maps a mathematical viewing window [x_min, x_max] × [y_min, y_max] to canvas pixel space [0, CANVAS_W] × [0, CANVAS_H]. Note that Y-axis is inverted because screen coordinates have origin at top-left.

**Mathematical Formulas:**
```
screen_x = (graph_x - x_min) / (x_max - x_min) × CANVAS_W
screen_y = CANVAS_H - (graph_y - y_min) / (y_max - y_min) × CANVAS_H
```

**Pseudocode:**
```
FUNCTION graph_to_canvas_x(graph_x):
    range_x = x_max - x_min
    normalized = (graph_x - x_min) / range_x  // 0 to 1
    canvas_x = normalized × CANVAS_W
    RETURN integer(canvas_x)

FUNCTION graph_to_canvas_y(graph_y):
    range_y = y_max - y_min
    normalized = (graph_y - y_min) / range_y  // 0 to 1
    canvas_y = CANVAS_H - normalized × CANVAS_H  // Invert Y
    RETURN integer(canvas_y)
```

**Implementation References:**
- X transformation: graph.c:98-100
- Y transformation: graph.c:102-104
- Usage in axis drawing: graph.c:119-132
- Usage in function plotting: graph.c:183-186

**Example:**
```
Given: x_min=-10, x_max=10, CANVAS_W=320
For graph_x=0: canvas_x = (0-(-10))/(10-(-10)) × 320 = 160
For graph_x=5: canvas_x = (5-(-10))/(10-(-10)) × 320 = 240
```

---

### 5. Function Plotting with Adaptive Sampling

**Location:** src/graph.c:167-202

**Purpose:** Render mathematical functions as continuous curves on screen while handling discontinuities

**Algorithm Description:**
The algorithm samples the function at regular intervals across the x-axis and draws line segments between consecutive valid points. It includes discontinuity detection to avoid drawing lines across jumps (e.g., tan(x) at π/2).

**Pseudocode:**
```
FUNCTION draw_function(function_index):
    IF function is disabled OR equation is empty:
        RETURN
    
    step = (x_max - x_min) / CANVAS_W  // One sample per pixel
    previous_point = NULL
    previous_valid = FALSE
    
    FOR graph_x FROM x_min TO x_max STEP step:
        graph_y = evaluate_expression(equation, graph_x)
        canvas_x = transform_x(graph_x)
        canvas_y = transform_y(graph_y)
        
        // Check if point is valid (not NaN, not infinite, on screen)
        current_valid = (NOT is_nan(graph_y)) AND 
                       (NOT is_infinite(graph_y)) AND
                       (canvas_y within reasonable bounds)
        
        IF current_valid AND previous_valid:
            vertical_distance = abs(canvas_y - previous_canvas_y)
            
            // Avoid drawing across discontinuities
            IF vertical_distance < CANVAS_H × 2:
                draw_line(previous_canvas_x, previous_canvas_y,
                         canvas_x, canvas_y,
                         function_color)
        
        previous_canvas_x = canvas_x
        previous_canvas_y = canvas_y
        previous_valid = current_valid
```

**Implementation References:**
- Main plotting loop: graph.c:180-199
- Discontinuity detection: graph.c:192-196
- Valid point checking: graph.c:187
- Function evaluation: graph.c:184 (calls expr_eval.c:202-210)

**Discontinuity Handling:** 
The line at graph.c:192-194 prevents drawing vertical lines across jumps by checking if the vertical distance exceeds 2× screen height, which indicates a discontinuity rather than a steep slope.

**Time Complexity:** O(w) where w is canvas width in pixels
**Space Complexity:** O(1)

---

### 6. History Management (Circular Buffer)

**Location:** src/math.c:153-172

**Purpose:** Store calculation history with automatic overflow handling in a fixed-size buffer

**Data Structure:**
```c
typedef struct {
    char equation[128];
    char result[64];
} HistoryEntry;

static HistoryEntry history[MAX_HISTORY];  // MAX_HISTORY = 10
static int history_count = 0;
```

**Algorithm Description:**
Implements a circular buffer that shifts older entries when capacity is reached, maintaining the most recent calculations.

**Pseudocode:**
```
FUNCTION add_to_history(equation, result):
    IF history_count >= MAX_HISTORY:
        // Shift all entries up (remove oldest)
        FOR i FROM 0 TO MAX_HISTORY-2:
            history[i] = history[i+1]
        
        history_count = MAX_HISTORY - 1
    
    // Add new entry at end
    history[history_count].equation = equation
    history[history_count].result = result
    history_count = history_count + 1
    
    update_display()
```

**Implementation References:**
- History structure definition: math.c:16-19
- Add to history function: math.c:153-172
- History display update: math.c:174-197
- Usage when pressing '=' key: math.c:214-217

**Time Complexity:** O(n) for insertion where n is MAX_HISTORY (due to shifting)
**Space Complexity:** O(n) where n is MAX_HISTORY

---

### 7. Cramer's Rule for 2×2 Linear Systems

**Location:** src/solver.c:146-156

**Purpose:** Solve systems of two linear equations with two unknowns using Cramer's Rule

**Algorithm Description:**
Given two equations:
```
a1·x + b1·y = c1
a2·x + b2·y = c2
```

Cramer's Rule uses determinants to find the solution. The determinant of the coefficient matrix is:
```
det = a1·b2 - a2·b1
```

If det ≠ 0, the system has a unique solution:
```
x = (c1·b2 - c2·b1) / det
y = (a1·c2 - a2·c1) / det
```

**Mathematical Background:**
Cramer's Rule states that for a system Ax = b, if det(A) ≠ 0, then:
- x_i = det(A_i) / det(A)

where A_i is the matrix formed by replacing the i-th column of A with vector b.

**Pseudocode:**
```
FUNCTION solve_2x2(a1, b1, c1, a2, b2, c2, *x, *y):
    // Calculate determinant of coefficient matrix
    det = a1 × b2 - a2 × b1
    
    IF absolute_value(det) < 1e-12:
        RETURN 0  // No unique solution (parallel or coincident lines)
    
    // Apply Cramer's Rule
    // x = det([c1 b1; c2 b2]) / det
    x = (c1 × b2 - c2 × b1) / det
    
    // y = det([a1 c1; a2 c2]) / det
    y = (a1 × c2 - a2 × c1) / det
    
    RETURN 1  // Success
```

**Implementation Reference:** solver.c:146-156

**Example Calculation:**
```
Equations:  2x + 3y = 8
           -x + 4y = 3

det = 2×4 - (-1)×3 = 8 + 3 = 11

x = (8×4 - 3×3) / 11 = (32 - 9) / 11 = 23/11 ≈ 2.091
y = (2×3 - (-1)×8) / 11 = (6 + 8) / 11 = 14/11 ≈ 1.273
```

**Edge Cases:**
- det = 0: Equations are parallel (no solution) or coincident (infinite solutions)
- det ≈ 0 (< 1e-12): Treated as singular to avoid numerical instability

**Time Complexity:** O(1)
**Space Complexity:** O(1)

---

### 8. Cramer's Rule for 3×3 Linear Systems

**Location:** src/solver.c:159-186

**Purpose:** Solve systems of three linear equations with three unknowns using Cramer's Rule

**Algorithm Description:**
Given three equations:
```
a[0][0]·x + a[0][1]·y + a[0][2]·z = b[0]
a[1][0]·x + a[1][1]·y + a[1][2]·z = b[1]
a[2][0]·x + a[2][1]·y + a[2][2]·z = b[2]
```

The determinant of a 3×3 matrix using cofactor expansion along the first row:
```
det(A) = a[0][0]·(a[1][1]·a[2][2] - a[1][2]·a[2][1])
       - a[0][1]·(a[1][0]·a[2][2] - a[1][2]·a[2][0])
       + a[0][2]·(a[1][0]·a[2][1] - a[1][1]·a[2][0])
```

Cramer's Rule for 3×3 system:
```
x = det(A_x) / det(A)    where A_x replaces column 0 with b
y = det(A_y) / det(A)    where A_y replaces column 1 with b
z = det(A_z) / det(A)    where A_z replaces column 2 with b
```

**Pseudocode:**
```
FUNCTION solve_3x3(a[3][3], b[3], *x, *y, *z):
    // Calculate determinant of coefficient matrix
    det = a[0][0] × (a[1][1] × a[2][2] - a[1][2] × a[2][1])
        - a[0][1] × (a[1][0] × a[2][2] - a[1][2] × a[2][0])
        + a[0][2] × (a[1][0] × a[2][1] - a[1][1] × a[2][0])
    
    IF absolute_value(det) < 1e-12:
        RETURN 0  // No unique solution
    
    // Calculate determinant for x (replace column 0 with b)
    det_x = b[0] × (a[1][1] × a[2][2] - a[1][2] × a[2][1])
          - a[0][1] × (b[1] × a[2][2] - a[1][2] × b[2])
          + a[0][2] × (b[1] × a[2][1] - a[1][1] × b[2])
    
    // Calculate determinant for y (replace column 1 with b)
    det_y = a[0][0] × (b[1] × a[2][2] - a[1][2] × b[2])
          - b[0] × (a[1][0] × a[2][2] - a[1][2] × a[2][0])
          + a[0][2] × (a[1][0] × b[2] - b[1] × a[2][0])
    
    // Calculate determinant for z (replace column 2 with b)
    det_z = a[0][0] × (a[1][1] × b[2] - b[1] × a[2][1])
          - a[0][1] × (a[1][0] × b[2] - b[1] × a[2][0])
          + b[0] × (a[1][0] × a[2][1] - a[1][1] × a[2][0])
    
    // Apply Cramer's Rule
    x = det_x / det
    y = det_y / det
    z = det_z / det
    
    RETURN 1  // Success
```

**Implementation Reference:** solver.c:159-186

**Example Calculation:**
```
Equations:  2x + y - z = 8
           -3x - y + 2z = -11
           -2x + y + 2z = -3

det(A) = 2×(-1×2 - 2×1) - 1×(-3×2 - 2×(-2)) + (-1)×(-3×1 - (-1)×(-2))
       = 2×(-4) - 1×(-2) + (-1)×(-5)
       = -8 + 2 + 5 = -1

det_x = 8×(-4) - 1×(-11×2 - 2×(-3)) + (-1)×(-11×1 - (-1)×(-3))
      = -32 - 1×(-16) + (-1)×(-14)
      = -32 + 16 + 14 = -2

x = -2 / -1 = 2
```

**Time Complexity:** O(1) - fixed number of operations
**Space Complexity:** O(1)

---

### 9. Newton-Raphson Root Finding Method

**Location:** src/solver.c:73-105

**Purpose:** Numerically find roots (zeros) of a function f(x) = 0 using iterative refinement

**Algorithm Description:**
The Newton-Raphson method is an iterative numerical technique for finding roots. Starting from an initial guess x₀, it uses the derivative to refine the estimate:

```
x_{n+1} = x_n - f(x_n) / f'(x_n)
```

Geometrically, this finds where the tangent line at (x_n, f(x_n)) intersects the x-axis.

**Convergence:**
- Quadratic convergence near the root (errors square each iteration)
- Requires good initial guess x₀
- May diverge if f'(x) ≈ 0 or initial guess is poor

**Pseudocode:**
```
FUNCTION newton_raphson(expr, x0, *result, *iterations):
    x = x0
    h = 1e-6  // Step size for numerical derivative
    
    FOR i FROM 0 TO MAX_ITERATIONS-1:
        fx = evaluate_expression(expr, x)
        
        // Check if we've found the root
        IF absolute_value(fx) < TOLERANCE:
            result = x
            iterations = i + 1
            RETURN 1  // Success
        
        // Calculate numerical derivative using central difference
        fpx = numerical_derivative(expr, x, h)
        
        // Check if derivative is too small
        IF absolute_value(fpx) < 1e-12:
            RETURN 0  // Cannot continue (derivative near zero)
        
        // Newton-Raphson update step
        x = x - fx / fpx
    
    // Did not converge within max iterations
    result = x
    iterations = MAX_ITERATIONS
    RETURN 0  // Failure
```

**Numerical Derivative:**
Uses central difference approximation (solver.c:57-70):
```
f'(x) ≈ (f(x+h) - f(x-h)) / (2h)
```

This is more accurate than forward/backward difference.

**Implementation References:**
- Newton-Raphson solver: solver.c:73-105
- Numerical derivative: solver.c:57-70
- Constants: solver.c:17-18 (MAX_ITERATIONS=100, TOLERANCE=1e-8)

**Example Execution:**
Find root of f(x) = x² - 4 starting from x₀ = 1:

```
Iteration 0: x = 1, f(1) = -3
             f'(1) ≈ 2
             x_new = 1 - (-3)/2 = 2.5

Iteration 1: x = 2.5, f(2.5) = 2.25
             f'(2.5) ≈ 5
             x_new = 2.5 - 2.25/5 = 2.05

Iteration 2: x = 2.05, f(2.05) = 0.2025
             f'(2.05) ≈ 4.1
             x_new = 2.05 - 0.2025/4.1 ≈ 2.0006

Iteration 3: x ≈ 2.0006, |f(x)| < 1e-8 → CONVERGED
Result: x ≈ 2.0
```

**Time Complexity:** O(n·k) where n is number of iterations, k is cost of evaluating f(x)
**Space Complexity:** O(1)

---

### 10. SUVAT Iterative Equation Solver

**Location:** src/mechanics.c:83-167

**Purpose:** Solve kinematics problems by iteratively applying SUVAT equations until all five variables are determined

**Algorithm Description:**
SUVAT equations describe motion with constant acceleration:
- **s** = displacement (m)
- **u** = initial velocity (m/s)
- **v** = final velocity (m/s)
- **a** = acceleration (m/s²)
- **t** = time (s)

The five fundamental equations are (mechanics.c:9-14):
1. v = u + at
2. s = ut + ½at²
3. v² = u² + 2as
4. s = (u+v)t/2
5. s = vt - ½at² (derived from equations 1 and 2)

Given any 3 known values, the system iteratively applies equations to solve for the remaining 2 unknowns.

**Pseudocode:**
```
FUNCTION solve_suvat(suvat):
    max_iterations = 10
    
    FOR iter FROM 0 TO max_iterations-1:
        previous_known_count = count_known(suvat)
        
        // Equation 1: v = u + at
        IF is_known(u) AND is_known(a) AND is_known(t) AND NOT is_known(v):
            v = u + a × t
        
        // Equation 1 rearranged: u = v - at
        IF is_known(v) AND is_known(a) AND is_known(t) AND NOT is_known(u):
            u = v - a × t
        
        // Equation 1 rearranged: t = (v-u)/a
        IF is_known(u) AND is_known(v) AND is_known(a) AND NOT is_known(t) AND a ≠ 0:
            t = (v - u) / a
        
        // Equation 1 rearranged: a = (v-u)/t
        IF is_known(u) AND is_known(v) AND is_known(t) AND NOT is_known(a) AND t ≠ 0:
            a = (v - u) / t
        
        // Equation 2: s = ut + 0.5at²
        IF is_known(u) AND is_known(a) AND is_known(t) AND NOT is_known(s):
            s = u × t + 0.5 × a × t²
        
        // Equation 4: s = (u+v)t/2
        IF is_known(u) AND is_known(v) AND is_known(t) AND NOT is_known(s):
            s = (u + v) × t / 2
        
        // Equation 3: v² = u² + 2as → v = sqrt(u² + 2as)
        IF is_known(u) AND is_known(a) AND is_known(s) AND NOT is_known(v):
            v_squared = u² + 2 × a × s
            IF v_squared ≥ 0:
                v = square_root(v_squared)
        
        // Equation 3 rearranged: u = sqrt(v² - 2as)
        IF is_known(v) AND is_known(a) AND is_known(s) AND NOT is_known(u):
            u_squared = v² - 2 × a × s
            IF u_squared ≥ 0:
                u = square_root(u_squared)
        
        // Equation 3 rearranged: a = (v² - u²)/(2s)
        IF is_known(u) AND is_known(v) AND is_known(s) AND NOT is_known(a) AND s ≠ 0:
            a = (v² - u²) / (2 × s)
        
        // Equation 3 rearranged: s = (v² - u²)/(2a)
        IF is_known(u) AND is_known(v) AND is_known(a) AND NOT is_known(s) AND a ≠ 0:
            s = (v² - u²) / (2 × a)
        
        // Equation 4 rearranged: t = 2s/(u+v)
        IF is_known(s) AND is_known(u) AND is_known(v) AND NOT is_known(t) AND (u+v) ≠ 0:
            t = 2 × s / (u + v)
        
        // Check convergence
        current_known_count = count_known(suvat)
        IF current_known_count = 5:
            RETURN 5  // All values solved
        
        IF current_known_count = previous_known_count:
            BREAK  // No progress made
    
    RETURN count_known(suvat)
```

**Implementation References:**
- SUVAT structure: mechanics.c:20-26
- Value getters/setters: mechanics.c:49-69
- Known counter: mechanics.c:72-80
- Iterative solver: mechanics.c:83-167
- Equation list: mechanics.c:9-14

**Example Execution:**
Given: u = 0 m/s, a = 9.8 m/s², t = 3 s

```
Initial: s = ?, u = 0, v = ?, a = 9.8, t = 3
Known count = 3

Iteration 0:
  Apply v = u + at: v = 0 + 9.8×3 = 29.4 m/s
  Apply s = ut + ½at²: s = 0×3 + 0.5×9.8×3² = 44.1 m
  Known count = 5 → SOLVED

Final: s = 44.1 m, u = 0 m/s, v = 29.4 m/s, a = 9.8 m/s², t = 3 s
```

**Advantages:**
- Handles any 3 known values (not limited to specific combinations)
- Automatically selects applicable equations
- Validates division by zero and negative square roots

**Time Complexity:** O(k·n) where k is max iterations (10), n is number of equation checks per iteration (12)
**Space Complexity:** O(1)

---

## Data Structures

### 1. Parser Structure (Expression Evaluator)

**Location:** src/expr_eval.c:30-33

**Definition:**
```c
typedef struct {
    const char *str;  // Pointer to expression string
    int pos;          // Current position in string
} Parser;
```

**Purpose:** Maintains state during recursive descent parsing, tracking position in input string

**Usage:**
- Initialized: expr_eval.c:198
- Position advanced: expr_eval.c:37, 46, 49, etc.
- String read: expr_eval.c:36, 45, 49, etc.

**Lifetime:** Stack-allocated per evaluation, destroyed when parse completes

---

### 2. History Entry Structure

**Location:** src/math.c:16-19

**Definition:**
```c
typedef struct {
    char equation[128];  // User input expression
    char result[64];     // Calculated result
} HistoryEntry;

static HistoryEntry history[MAX_HISTORY];  // Array of 10 entries
static int history_count = 0;
```

**Purpose:** Store calculation history in math calculator app

**Memory Layout:**
```
history[0]:  equation[128]  result[64]
history[1]:  equation[128]  result[64]
...
history[9]:  equation[128]  result[64]
             ─────────────────────────
Total:       ~1900 bytes
```

**Operations:**
- Add entry: math.c:153-172
- Display: math.c:174-197
- Navigate: math.c:224-236

---

### 3. Function Data Structure (Graphing)

**Location:** src/graph.c:40-50

**Definition:**
```c
typedef struct {
    char equation[64];  // Function expression (e.g., "x^2")
    int enabled;        // Boolean: 1 = plot this function, 0 = hide
} Function;

static Function functions[MAX_FUNCTIONS];  // MAX_FUNCTIONS = 4
```

**Initial Values:**
```c
functions[0] = { "x^2", 1 }         // Enabled parabola
functions[1] = { "sin(x)*3", 1 }    // Enabled sine wave
functions[2] = { "", 0 }            // Empty slot
functions[3] = { "", 0 }            // Empty slot
```

**Purpose:** Store up to 4 mathematical functions for simultaneous plotting

**Associated Colors:** Defined at graph.c:26-31
```c
func_colors[0] = 0x00FF00  // Green
func_colors[1] = 0xFF0000  // Red
func_colors[2] = 0x00AAFF  // Blue
func_colors[3] = 0xFFAA00  // Orange
```

**Operations:**
- Edit equation: graph.c:324-327
- Toggle enable: graph.c:370-372
- Plot function: graph.c:167-202

---

### 4. Button Keymap Structure

**Location:** src/button_keymap.c:7-12

**Definition:**
```c
typedef struct {
    int row;              // Button matrix row (0-8)
    int col;              // Button matrix column (0-5)
    uint32_t lvgl_key;    // LVGL key code to send
    char label[16];       // Display label on button
} button_mapping_t;

static button_mapping_t keymap[MAX_BUTTONS];  // MAX_BUTTONS = 54
```

**Purpose:** Map physical button positions to logical key codes

**Example Entries:**
```
Row 0, Col 2: UP arrow
  → row=0, col=2, lvgl_key=LV_KEY_UP, label="^"
  
Row 5, Col 1: Number 7
  → row=5, col=1, lvgl_key='7', label="7"
  
Row 8, Col 5: Equals
  → row=8, col=5, lvgl_key=LV_KEY_ENTER, label="="
```

**Layout (9 rows × 6 columns):**
```
Row 0: SHIFT  ALPHA  <(left)  ^(up)    MENU   ON
Row 1: VAR    CALC   v(down)  >(right) CONST  EXP
Row 2: FRAC   sqrt   x^2      ^        log    ln
Row 3: !      (      )        sin      cos    tan
Row 4: A      B      C        D        E      F
Row 5: x      7      8        9        DEL    AC
Row 6: y      4      5        6        *      /
Row 7: z      1      2        3        +      -
Row 8: e      0      .        pi       Ans    =
```

**Implementation References:**
- Structure definition: button_keymap.c:7-12
- Initialization: button_keymap.c:18-92
- Lookup function: button_keymap.c:96-103
- Label retrieval: button_keymap.c:105-112

---

### 5. Canvas Buffer (Graph Rendering)

**Location:** src/graph.c:84

**Definition:**
```c
LV_DRAW_BUF_DEFINE_STATIC(canvas_buf, CANVAS_W, CANVAS_H, LV_COLOR_FORMAT_NATIVE);
```

**Dimensions:**
- Width: 320 pixels (CANVAS_W = SCREEN_W)
- Height: 205 pixels (CANVAS_H = SCREEN_H - 35)

**Memory Size:** 
- At 16-bit color (RGB565): 320 × 205 × 2 = 131,200 bytes ≈ 128 KB
- At 32-bit color (RGB888): 320 × 205 × 4 = 262,400 bytes ≈ 256 KB

**Purpose:** Frame buffer for graph rendering, allowing direct pixel manipulation

**Usage Flow:**
```
1. Clear buffer: graph.c:246 (lv_canvas_fill_bg)
2. Draw axes: graph.c:108-165
3. Draw functions: graph.c:167-202
4. Draw trace cursor: graph.c:204-243
5. Display to screen: Automatic via LVGL
```

**Implementation References:**
- Buffer definition: graph.c:84
- Buffer initialization: graph.c:607-608
- Drawing operations: graph.c:245-252

---

### 6. LVGL Group Structure (Input Handling)

**Type:** `lv_group_t*` (LVGL framework type)

**Purpose:** Manage keyboard focus and input routing between UI widgets

**Instances:**
1. **menu_group** (main_menu.c:16)
   - Controls: 6 menu buttons
   - Focus: Grid navigation
   - Reference: main_menu.c:182

2. **calc_group** (math.c:36)
   - Controls: Key receiver object
   - Focus: Expression input
   - Reference: math.c:361

3. **graph_group** (graph.c:87)
   - Controls: Function textareas, key receiver
   - Focus: Dynamic (list editing or graph viewing)
   - References: graph.c:567, 612

4. **stats_group** (stats.c:52)
   - Controls: 3 parameter textareas, key receiver
   - Focus: Parameter input
   - Reference: stats.c:350

**Key Operations:**
```c
// Create group
lv_group_t *group = lv_group_create();

// Add widget to group
lv_group_add_obj(group, widget);

// Set group for input device
lv_indev_set_group(input_device, group);

// Change focus
lv_group_focus_obj(widget);

// Send key to focused widget
lv_group_send_data(group, key_code);
```

---

## User Interface Design

### 1. Main Menu Screen

**Layout:** 3×2 grid of app launcher buttons (6 total applications)

**Visual Design:**
```
┌──────────────────────────────────┐ 320×240 pixels
│                                  │
│      ┌────┐  ┌────┐  ┌────┐    │
│      │Math│  │Grph│  │Stat│    │ Row 0: Implemented Apps
│      └────┘  └────┘  └────┘    │ 75×75 buttons
│                                  │ 30px spacing
│      ┌────┐  ┌────┐  ┌────┐    │
│      │Numer│ │Mech│  │Matr│    │ Row 1: 2 Implemented,
│      │Mthds│ │ancs│  │ices│    │        1 Planned
│      └────┘  └────┘  └────┘    │
│                                  │
└──────────────────────────────────┘
```

**Application Grid Map:**
```
Position    Index    Name               Status        Color          File
(0,0)       0        Math               Implemented   Blue (#2196F3) math.c
(0,1)       1        Graph              Implemented   Red (#FF0000)  graph.c
(0,2)       2        Stats              Implemented   Green (#00FF00) stats.c
(1,0)       3        Numerical Methods  Implemented   Orange (#FF9800) solver.c
(1,1)       4        Mechanics          Implemented   Purple (#9C27B0) mechanics.c
(1,2)       5        Matrices           Planned       Red (#FF0000)  N/A
```

**Color Scheme:**
- Button 0 (Math): Blue (#2196F3) - main_menu.c:175
- Button 1 (Graph): Red (#FF0000) - main_menu.c:175
- Button 2 (Stats): Green (#00FF00) - main_menu.c:175
- Button 3 (Numerical Methods): Orange (#FF9800) - main_menu.c:176
- Button 4 (Mechanics): Purple (#9C27B0) - main_menu.c:176
- Button 5 (Matrices): Red (#FF0000) - main_menu.c:176

**Button Labels:** (main_menu.c:171-172)
```c
const char *names[6] = {
    "Math",                // Single line
    "Graph",               // Single line
    "Stats",               // Single line
    "Numerical\n Methods", // Two lines
    "Mechanics",           // Single line
    "Matrices"             // Single line
};
```

**Implementation:** main_menu.c:155-239

**App Launcher Logic:** (main_menu.c:92-108)
```c
static void openApp(int row, int col) {
    int index = row * MENU_COLS + col;
    switch (index) {
        case 0: math_app_start(); break;       // Math calculator
        case 1: graph_app_start(); break;      // Graphing app
        case 2: stats_app_start(); break;      // Statistics app
        case 3: solver_app_start(); break;     // Numerical Methods/Solver
        case 4: mechanics_app_start(); break;  // Mechanics (SUVAT)
        case 5: // TODO: Matrices
        default: break;
    }
}
```

**Navigation:**
- Arrow keys: Move focus between buttons in 2D grid (main_menu.c:48-90)
  - UP: Decrement row if row > 0
  - DOWN: Increment row if row < 1
  - LEFT: Decrement column if col > 0
  - RIGHT: Increment column if col < 2
- Enter: Launch selected app (main_menu.c:127-132)
- Mouse click: Direct launch (main_menu.c:141-153)

**Button States:**
- Default: Colored background (main_menu.c:204)
- Focused: Darker color + 5% scale increase (main_menu.c:207-216)
  ```c
  lv_obj_set_style_bg_color(btn, lv_color_darken(colors[i], 40),
                            LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_transform_zoom(btn, 256 * 105 / 100,
                                  LV_PART_MAIN | LV_STATE_FOCUSED);
  ```

**Screen Calculation:**
```c
// Grid: 3 columns × 2 rows
icon_w = 75, icon_h = 75
spacing_x = 30, spacing_y = 30

// Centered on screen
start_x = (320 - (3×75 + 2×30)) / 2 = 37.5 px
start_y = (240 - (2×75 + 1×30)) / 2 = 45 px

// Button position formula:
x = start_x + col × (icon_w + spacing_x)
y = start_y + row × (icon_h + spacing_y)
```

**Future Expansion:**
The current 3×2 grid can accommodate 6 applications. Currently, 5 applications are fully implemented (Math, Graph, Stats, Solver, Mechanics), with one remaining slot for the Matrices application. The system is designed to be easily expandable by:
1. Adding new app modules (e.g., `matrices.c`)
2. Implementing corresponding `*_app_start()` functions
3. Adding case statements in `openApp()` function (main_menu.c:106)

---

### 2. Math Calculator Screen

**Layout:** Vertical stack with history, preview, and input

**Visual Design:**
```
┌──────────────────────────────────┐ 320×240 pixels
│ ┌────────────────────────────┐  │ History Area
│ │ 2+2           4            │  │ (140px height)
│ │ sqrt(16)      4            │  │ 10 entries max
│ │ sin(pi/2)     1            │  │
│ │                            │  │
│ └────────────────────────────┘  │
│                                  │
│              Result: 10          │ Current Preview
│ ──────────────────────────────── │ (30px height)
│ Enter equation...                │ Input Line
│ [cursor]                         │ (40px height)
└──────────────────────────────────┘
```

**Color Scheme:**
- Background: White (#FFFFFF)
- Equations: Dark gray (#333333)
- Results: Green (#006600)
- Preview: Bright green (#009900)
- Selected history: Light gray (#E8E8F0)

**Implementation:** math.c:286-374

**Key Features:**
1. **Real-time Preview** (math.c:46-67)
   - Calculates result as user types
   - Shows below history area
   - Updates on every keystroke

2. **History Scrolling** (math.c:224-236)
   - UP arrow: Navigate up through history
   - DOWN arrow: Navigate down
   - Highlight selected line

3. **Smart Backspace** (math.c:74-108)
   - Delete functions as units (e.g., "sin(" deleted together)
   - Check function names: math.c:75-93

**Text Layout:**
```
Position          Widget           Size
(5, 5)           History box       310×140
(10, 150)        Preview label     300×30
(10, 185)        Input textarea    300×40
```

**Function Key Mapping:**
- 's' → "sin("
- 'C' → "cos("
- 't' → "tan("
- 'r' → "sqrt("
- 'P' → "pi"
- 'E' → "e"
- 'N' → last answer

---

### 3. Graph Plotting Screen

**Two Views:**

#### A. Function List View

**Visual Design:**
```
┌──────────────────────────────────┐
│ Functions                        │ Title (22px)
│ ┌────────────────────────────┐  │
│ │☑ y1 = x^2                  │  │ Row 0 (selected)
│ │                            │  │ 28px height
│ │☑ y2 = sin(x)*3             │  │ Row 1
│ │                            │  │
│ │☐ y3 = (editing...)         │  │ Row 2
│ │                            │  │
│ │☐ y4 =                      │  │ Row 3
│ └────────────────────────────┘  │
│                                  │
│ [↑↓]Nav [=]Edit [AC]Toggle     │ Hints (22px)
└──────────────────────────────────┘
```

**Implementation:** graph.c:488-587

**Function Colors:**
- y1: Green (#00FF00)
- y2: Red (#FF0000)
- y3: Blue (#00AAFF)
- y4: Orange (#FFAA00)

**Edit Mode:**
- Enter key: Enable textarea editing
- Typing: Modify equation
- Enter again: Save changes
- AC: Cancel editing

**Row Layout:**
```c
y_position = 22 + (row_index × 32)  // 32 = 28px height + 4px spacing
checkbox_x = 0
prefix_x = 30    // "y1 ="
equation_x = 75  // Equation text
```

#### B. Graph View

**Visual Design:**
```
┌──────────────────────────────────┐
│ x:[-10,10] y:[-10,10]           │ Info (15px)
│ [VAR]Trace [+/-]Zoom...         │ Controls (15px)
├──────────────────────────────────┤
│          │                       │ Canvas (210px)
│     10 ──┼────                   │
│          │                       │
│  ────────┼────────── 0           │ Grid with axes
│          │                       │
│    -10 ──┼────                   │
│          │                       │
└──────────────────────────────────┘
    -10    0    10
```

**Implementation:** graph.c:589-626

**Grid Rendering:** graph.c:134-162
- Auto-scaling tick marks
- Dynamic spacing based on zoom level
- Tick calculation:
  ```
  If range > 100: tick = 20
  If range > 50:  tick = 10
  If range > 20:  tick = 5
  If range > 10:  tick = 2
  If range < 2:   tick = 0.25
  Else:           tick = 1
  ```

**Trace Mode:**
- Crosshair at current x position
- Coordinates displayed: "y1: (x, y)"
- Color matches function
- Implementation: graph.c:204-243

**Controls:**
- Arrow keys: Pan view (or move trace cursor)
- '+'/'-': Zoom in/out (graph.c:438-448)
- 'V': Toggle trace mode (graph.c:450-463)
- 'AC': Reset view to [-10,10] (graph.c:465-468)

---

### 4. Statistics Screen

**Visual Design:**
```
┌──────────────────────────────────┐
│ Statistics                       │ Title (20px)
│                                  │
│ Binomial P(X=k)        [↑↓]     │ Distribution (20px)
│                                  │
│ n (trials):      [10        ]   │ Parameter 1
│                                  │
│ k (successes):   [3         ]   │ Parameter 2
│                                  │
│ p (probability): [0.5       ]   │ Parameter 3
│                                  │
│ Result: 0.117188                │ Result (30px)
│                                  │
│                                  │
│ [↑↓]Dist [=]Calc [AC]Clear      │ Hints (22px)
└──────────────────────────────────┘
```

**Implementation:** stats.c:292-366

**Distribution Types:**
1. Binomial P(X=k) - Point probability
2. Binomial CDF P(X≤k) - Cumulative probability
3. Normal PDF - Probability density
4. Normal CDF - Cumulative probability
5. Inverse Normal - Find x given probability

**Parameter Labels:** stats.c:56-62
- Dynamically change based on distribution type
- Example: Binomial uses (n, k, p)
- Example: Normal uses (x, μ, σ)

**Navigation:**
- UP/DOWN: Cycle through distributions (stats.c:248-270)
- Number keys: Enter parameters
- '=' or ENTER: Calculate result
- AC: Clear all fields

**Layout Positions:**
```c
title_y = 4
dist_label_y = 25
param1_y = 50
param2_y = 85    // param1_y + 35
param3_y = 120   // param2_y + 35
result_y = 165   // param3_y + 45
hint_y = LCD_V_RES - 22
```

**Result Formatting:** stats.c:222-233
- Integers: "%.0f" (no decimal)
- Small decimals: "%.8g" (8 significant figures)
- Errors: "Error: Invalid input"
- Infinity: "Infinity"

---

### 5. Solver/Numerical Methods Application Screens

**Implementation:** solver.c:1012-1014 (entry point), solver.c:949-1009 (menu)

#### A. Solver Menu

**Visual Design:**
```
┌──────────────────────────────────┐
│ Equation Solver                  │ Title (18px)
│                                  │
│ ┌──────────────────────────────┐│
│ │ Linear (ax+b=0)              ││ Button 0 (22px height)
│ └──────────────────────────────┘│
│ ┌──────────────────────────────┐│
│ │ Quadratic (ax^2+bx+c=0)      ││ Button 1
│ └──────────────────────────────┘│
│ ┌──────────────────────────────┐│
│ │ 2x2 Simultaneous             ││ Button 2
│ └──────────────────────────────┘│
│ ┌──────────────────────────────┐│
│ │ 3x3 Simultaneous             ││ Button 3
│ └──────────────────────────────┘│
│ ┌──────────────────────────────┐│
│ │ Newton-Raphson               ││ Button 4
│ └──────────────────────────────┘│
│ ┌──────────────────────────────┐│
│ │ Numerical Methods            ││ Button 5 (Placeholder)
│ └──────────────────────────────┘│
│ ┌──────────────────────────────┐│
│ │ Curve Fitting                ││ Button 6 (Placeholder)
│ └──────────────────────────────┘│
│ ┌──────────────────────────────┐│
│ │ Function Storage             ││ Button 7 (Placeholder)
│ └──────────────────────────────┘│
│                                  │
│         [Main Menu]              │ Button (Bottom, 28px)
└──────────────────────────────────┘
```

**Implementation:** solver.c:949-1009

**Menu Items:**
```c
MenuItem menu_items[] = {
    {"Linear (ax+b=0)", linear_btn_cb},           // solver.c:974
    {"Quadratic (ax^2+bx+c=0)", quadratic_btn_cb},// solver.c:975
    {"2x2 Simultaneous", sim_2x2_btn_cb},         // solver.c:976
    {"3x3 Simultaneous", sim_3x3_btn_cb},         // solver.c:977
    {"Newton-Raphson", newton_btn_cb},            // solver.c:978
    {"Numerical Methods", numerical_btn_cb},      // solver.c:979
    {"Curve Fitting", curve_fit_btn_cb},          // solver.c:980
    {"Function Storage", func_storage_btn_cb},    // solver.c:981
};
```

**Layout:**
- Button width: 280px
- Button height: 22px
- Spacing: 26px vertical (4px gap)
- Position: Top center at y = 30 + (index × 26)

**Navigation:**
- Up/Down arrow keys: Navigate through menu options
- Enter: Select solver type
- 'M' key: Return to main menu

---

#### B. Linear Equation Solver

**Visual Design:**
```
┌──────────────────────────────────┐
│ Linear Equation: ax + b = 0      │ Title
│                                  │
│ a: [________________]            │ Input field (200×30px)
│                                  │
│ b: [________________]            │ Input field
│                                  │
│                                  │
│       [Solve]                    │ Button (100×35px)
│                                  │
│ Enter coefficients and           │ Result label
│ press Solve                      │
│                                  │
│                                  │
│ [Back]                  [Menu]   │ Bottom buttons (80×30px)
└──────────────────────────────────┘
```

**Implementation:** solver.c:293-372

**Input Fields:**
- a coefficient: Top left (50, 35), solver.c:318-322
- b coefficient: Top left (50, 75), solver.c:329-333

**Solve Button:** solver.c:336-344
- Calls: solve_linear(a, b, &x) at solver.c:284
- Result format: "Solution: x = %.6g" or "No solution (a cannot be 0)"

---

#### C. Quadratic Equation Solver

**Visual Design:**
```
┌──────────────────────────────────┐
│ Quadratic: ax^2 + bx + c = 0     │ Title
│                                  │
│ a: [________________]            │
│                                  │
│ b: [________________]            │
│                                  │
│ c: [________________]            │
│                                  │
│       [Solve]                    │
│                                  │
│ Enter coefficients               │ Result:
│                                  │ - "No real solutions"
│                                  │ - "One solution: x = ..."
│                                  │ - "Two solutions:
│                                  │    x1 = ..., x2 = ..."
│ [Back]                  [Menu]   │
└──────────────────────────────────┘
```

**Implementation:** solver.c:404-492

**Input Fields:** 3 coefficients (a, b, c)
- Position: Top left (50, 30/65/100)
- Size: 200×30px
- solver.c:428-454

**Solve Logic:** solver.c:375-402
- Handles discriminant calculation
- Returns 0 (no real solutions), 1 (repeated root), or 2 (distinct roots)

---

#### D. 2×2 Simultaneous Equations Solver

**Visual Design:**
```
┌──────────────────────────────────┐
│ 2x2 Simultaneous Equations       │ Title
│ a1*x + b1*y = c1                 │ Subtitle (12pt font)
│ a2*x + b2*y = c2                 │
│                                  │
│ a1: [_______]                    │ 6 input fields
│ b1: [_______]                    │ 120×25px each
│ c1: [_______]                    │ Right-aligned
│ a2: [_______]                    │
│ b2: [_______]                    │
│ c2: [_______]                    │
│                                  │
│         [Solve]                  │ Button (80×30px)
│                                  │
│ Enter coefficients               │ Result label
│                                  │
│ [Back]              [Menu]       │ Bottom buttons (70×25px)
└──────────────────────────────────┘
```

**Implementation:** solver.c:520-596

**Input Layout:** solver.c:546-558
- 6 input fields arranged vertically
- Labels: "a1:", "b1:", "c1:", "a2:", "b2:", "c2:"
- Spacing: 25px vertical intervals
- Starting y position: 55px

**Solve Logic:** solver.c:495-518
- Uses Cramer's Rule (solver.c:146-156)
- Calculates determinant: det = a1·b2 - a2·b1
- Returns "No unique solution" if det ≈ 0

---

#### E. 3×3 Simultaneous Equations Solver

**Visual Design:**
```
┌──────────────────────────────────┐
│ 3x3 Simultaneous Equations       │ Title (12pt font)
│                                  │
│ [__]x + [__]y + [__]z = [__]     │ Row 0: 4 inputs (50×25px)
│                                  │ Coefficient grid layout
│ [__]x + [__]y + [__]z = [__]     │ Row 1
│                                  │
│ [__]x + [__]y + [__]z = [__]     │ Row 2
│                                  │
│         [Solve]                  │ Button (70×28px)
│                                  │
│ Enter coefficients               │ Result label (12pt font)
│                                  │
│ [Back]              [Menu]       │
└──────────────────────────────────┘
```

**Implementation:** solver.c:628-723

**Matrix Input Grid:** solver.c:651-684
- 12 input fields (3 rows × 4 columns)
- Each row: 3 coefficients + 1 result value
- Field size: 50×25px
- Horizontal spacing: 65px
- Vertical spacing: 28px
- Variable labels (x, y, z) shown after coefficients

**Scrollable Container:** solver.c:636
- Allows scrolling if content exceeds screen height
- Accommodates all inputs + solve button + results

**Solve Logic:** solver.c:599-626
- Uses 3×3 Cramer's Rule (solver.c:159-186)
- Computes 4 determinants (main + 3 for x, y, z)
- Result format: "Solution:\nx = %.6g\ny = %.6g\nz = %.6g"

---

#### F. Newton-Raphson Solver

**Visual Design:**
```
┌──────────────────────────────────┐
│ Newton-Raphson Method            │ Title
│ Solve f(x) = 0 numerically       │ Subtitle (12pt font)
│                                  │
│ f(x) =                           │
│ [_________________________]      │ Function input (240×30px)
│ e.g., x^2-4                      │ Placeholder text
│                                  │
│ Initial guess x0:                │
│                   [_________]    │ Input (100×30px, right-aligned)
│                   1              │ Placeholder
│                                  │
│       [Solve]                    │ Button (100×35px)
│                                  │
│ Enter equation and               │ Result label (12pt font):
│ initial guess                    │ - "Solution: x = %.8g
│                                  │    Converged in N iterations"
│                                  │ - "Did not converge
│                                  │    Last value: x = ...
│ [Back]              [Menu]       │    Try different x0"
└──────────────────────────────────┘
```

**Implementation:** solver.c:750-835

**Function Input:** solver.c:779-784
- Expression textarea (supports math expressions)
- Example: "x^2-4", "sin(x)-x/2", "ln(x)+x-5"
- Uses expression evaluator (expr_eval.c)

**Initial Guess:** solver.c:791-796
- Default placeholder: "1"
- User provides starting point for iteration

**Solve Button:** solver.c:799-807
- Callback: solve_newton_raphson_btn_cb (solver.c:726-748)
- Calls: newton_raphson(expr, x0, &result, &iterations) at solver.c:739
- Max iterations: 100 (solver.c:17)
- Tolerance: 1e-8 (solver.c:18)

**Result Display:** solver.c:810-813
- Success: Shows solution + iteration count
- Failure: Shows last value + suggestion to try different x0
- Uses numerical derivative with h=1e-6 (solver.c:75)

---

#### G. Placeholder Screens

Three placeholder screens are implemented but show "Coming Soon" messages:

1. **Numerical Methods** (solver.c:838-859)
   - Planned features: Differentiation, Integration, Visualization

2. **Curve Fitting** (solver.c:861-882)
   - Planned features: Linear Regression, Quadratic Regression, Plot Results
   - Functions already implemented: linear_regression (solver.c:189-202), quadratic_regression (solver.c:205-242)

3. **Function Storage** (solver.c:884-905)
   - Planned features: Save functions, Load functions, Manage storage
   - Data structure defined: StoredFunction (solver.c:34-38)

---

### 6. Mechanics (SUVAT) Application Screens

**Implementation:** mechanics.c:381-401 (entry point)

The Mechanics application has two screens that alternate: Input Page and Result Page.

#### A. Input Page

**Visual Design:**
```
┌──────────────────────────────────┐
│ SUVAT Mechanics                  │ Title (20px from top)
│ Enter any 3 values:              │ Subtitle
│                                  │
│ s (m):          [___________]    │ Input field (150×30px)
│                 ?                │ Placeholder
│                                  │
│ u (m/s):        [___________]    │
│                                  │
│ v (m/s):        [___________]    │
│                                  │
│ a (m/s^2):      [___________]    │
│                                  │
│ t (s):          [___________]    │
│                                  │
│                                  │
│       [Calculate]                │ Button (120×35px, centered)
│                                  │
└──────────────────────────────────┘
```

**Implementation:** mechanics.c:231-314

**Input Fields:** mechanics.c:264-292
- 5 fields for s, u, v, a, t
- Labels: suvat_names[] array at mechanics.c:37
  ```c
  {"s (m)", "u (m/s)", "v (m/s)", "a (m/s^2)", "t (s)"}
  ```
- Vertical spacing: 35px
- Starting y position: 50px
- Max length: 15 characters
- Placeholder: "?"

**Validation:** mechanics.c:172-196
- Checks that at least 3 fields are filled
- Shows error dialog if < 3 values provided
- Error message: "Please enter at least 3 values"

**Calculate Button:** mechanics.c:295-303
- Calls: solve_suvat(&current_suvat) at mechanics.c:199
- On success: Switches to Result Page
- On failure: Shows error "Cannot solve with given values"

**SUVAT Variables:**
- **s**: Displacement (meters)
- **u**: Initial velocity (m/s)
- **v**: Final velocity (m/s)
- **a**: Acceleration (m/s²)
- **t**: Time (seconds)

---

#### B. Result Page

**Visual Design:**
```
┌──────────────────────────────────┐
│ SUVAT Results                    │ Title
│                                  │
│ s (m) = 44.1                     │ Result labels (35px spacing)
│                                  │
│ u (m/s) = 0                      │
│                                  │
│ v (m/s) = 29.4                   │
│                                  │
│ a (m/s^2) = 9.8                  │
│                                  │
│ t (s) = 3                        │
│                                  │
│                                  │
│ Equations:                       │ Reference (10pt font)
│ v=u+at  s=ut+0.5at^2             │
│ v^2=u^2+2as  s=(u+v)t/2          │
│                                  │
│ [Back]                  [Menu]   │ Buttons (100×30px)
└──────────────────────────────────┘
```

**Implementation:** mechanics.c:316-379

**Result Display:** mechanics.c:342-352
- All 5 SUVAT values shown with labels
- Format: "%.6g" (6 significant figures)
- Example: "s (m) = 44.1"
- Vertical spacing: 35px
- Starting y: 30px

**Equations Reference:** mechanics.c:355-358
- Shows the 4 primary equations used
- Font: lv_font_montserrat_10 (small)
- Position: Bottom area (y=210px)
- Helps user understand calculation method

**Back Button:** mechanics.c:361-368
- Returns to Input Page with values preserved
- Allows user to modify inputs and recalculate

**Menu Button:** mechanics.c:371-378
- Returns to main menu
- Resets SUVAT values (mechanics.c:382-387)

**SUVAT Structure:** mechanics.c:20-26
```c
typedef struct {
    double s;  // displacement
    double u;  // initial velocity
    double v;  // final velocity
    double a;  // acceleration
    double t;  // time
} SUVAT;
```

**Unknown Value Marker:** mechanics.c:17
```c
#define SUVAT_UNKNOWN -999999.0
```

---

#### C. Navigation Flow

```
[Input Page]
     ↓
  User enters 3+ values
     ↓
  Press [Calculate]
     ↓
  Validation (≥3 values?)
     ↓
  Solve iteratively
     ↓
[Result Page]
     ↓
  Press [Back] → Return to Input Page (values preserved)
  Press [Menu] → Return to Main Menu (values reset)
```

**Implementation References:**
- Page switcher: mechanics.c:217 (back to input), mechanics.c:211 (show results)
- Input validation: mechanics.c:188-196
- Solver: mechanics.c:83-167 (iterative SUVAT equation application)
- Value tracking: mechanics.c:46 (current_suvat static variable)

---

### 7. Virtual Keypad (Desktop Only)

**Layout:** 9 rows × 6 columns = 54 buttons

**Visual Design:**
```
┌─────────────────────────────────┐ 320×270 pixels
│ [SHIFT][ALPHA][  <][  ^][MENU] │ Row 0 (Navigation)
│ [ VAR ][ CALC][  v][  >][CONST]│ Row 1
│ [FRAC ][sqrt ][x^2 ][ ^ ][ log]│ Row 2 (Functions)
│ [  !  ][  (  ][  ) ][ sin][ cos]│ Row 3
│ [  A  ][  B  ][  C ][  D ][  E ]│ Row 4 (Alpha)
│ [  x  ][  7  ][  8 ][  9 ][ DEL]│ Row 5 (Numbers)
│ [  y  ][  4  ][  5 ][  6 ][  * ]│ Row 6
│ [  z  ][  1  ][  2 ][  3 ][  + ]│ Row 7
│ [  e  ][  0  ][  . ][ pi ][ Ans]│ Row 8
└─────────────────────────────────┘
```

**Implementation:** desktop/src/main.c:264-355

**Button Dimensions:**
```c
button_width = (320 - 10) / 6 - 2 = 49px
button_height = 28px
margin = 2px
```

**Arrow Key Diamond Layout:**
The UP, DOWN, LEFT, RIGHT keys are arranged in a diamond pattern:
```
      [^]         Row 0, Col 3
[<]       [>]     Row 1, Col 2-3
      [v]         Row 1, Col 2
```
Implementation: main.c:298-321
- Arrow buttons are 2/3 width of normal buttons
- Position offsets create diamond arrangement

**Color Scheme:**
- Button default: Dark gray (#404040)
- Button border: Medium gray (#606060)
- Button pressed: White flash (#FFFFFF) for 100ms
- Background: Very dark gray (#202020)

**Physical Keyboard Mapping:**
The desktop version maps PC keyboard keys to calculator buttons via a configuration file (button_keycodes.txt):
```
0,2=LEFT      # Row 0, Col 2 maps to arrow left key
0,3=UP
1,2=DOWN
1,3=RIGHT
5,4=BACKSPACE # Row 5, Col 4 maps to DEL button
8,5=RETURN    # Row 8, Col 5 maps to = button
```

**Event Flow:**
```
PC Keyboard Press
      ↓
SDL Key Event (main.c:358)
      ↓
Find Button Mapping (main.c:147-154)
      ↓
Flash Button Visual (main.c:380-384)
      ↓
Convert to LVGL Key (main.c:157-203)
      ↓
Send to Main App (main.c:206-221)
```

---

## Hardware Requirements

### Desktop Platform (Windows/Linux/macOS)

**Minimum Requirements:**
- **CPU:** Any modern x86/x64 processor (2009+)
  - Intel Core 2 Duo or equivalent
  - ARM processors (Apple M1/M2) supported
- **RAM:** 256 MB available
  - Application uses ~5-10 MB
  - Frame buffers: ~128-256 KB
- **Display:** 640×510 pixels minimum
  - Main window: 320×240
  - Keypad window: 320×270
  - 16-bit color depth (RGB565) minimum
- **Graphics:** OpenGL-capable GPU (optional)
  - SDL2 uses software rendering if unavailable
- **Storage:** 50 MB free space
  - Executable: ~500 KB - 2 MB
  - LVGL library: ~200 KB
  - SDL2 library: ~1 MB

**Software Dependencies:**
- **SDL2** version 2.0.10 or later
  - Cross-platform window management
  - Keyboard/mouse input handling
  - Reference: desktop/src/hal/hal.c
- **C Standard Library** (libc/msvcrt)
  - Math functions: sin, cos, tan, sqrt, pow, exp, log
  - Memory management: malloc, free
- **Operating System:**
  - Windows 7 or later
  - Linux kernel 3.x or later
  - macOS 10.9 or later

**Build System:**
- CMake 3.10 or later
- C11-compatible compiler:
  - GCC 5.0+
  - Clang 3.8+
  - MSVC 2017+

**Hardware Configuration:**
```
┌─────────────────────┐
│   PC/Laptop         │
│  ┌──────────────┐   │
│  │ Application  │   │
│  │  (main.exe)  │   │
│  └──────┬───────┘   │
│         │           │
│  ┌──────▼───────┐   │
│  │   SDL2 DLL   │   │
│  └──────┬───────┘   │
│         │           │
│  ┌──────▼───────┐   │
│  │ OS Graphics  │   │
│  │   Driver     │   │
│  └──────┬───────┘   │
│         │           │
│  ┌──────▼───────┐   │
│  │  Display HW  │   │
│  └──────────────┘   │
└─────────────────────┘
```

---

### ESP32 Embedded Platform

**Hardware Specifications:**
- **Microcontroller:** ESP32-WROOM-32 or compatible
  - CPU: Dual-core Xtensa LX6 @ 240 MHz
  - RAM: 520 KB SRAM
  - Flash: 4 MB minimum (application + LVGL)
  - WiFi/Bluetooth: Not used in this application
  
**Display Module:**
- **Type:** ILI9341 TFT LCD or compatible
- **Resolution:** 320×240 pixels (QVGA)
- **Interface:** SPI (Serial Peripheral Interface)
  - MOSI: GPIO 23
  - CLK: GPIO 18
  - CS: GPIO 5
  - DC: GPIO 19
  - RST: GPIO 22
- **Color Depth:** 16-bit RGB565
- **Frame Buffer:** External PSRAM recommended (4 MB)

**Button Matrix Input:**
- **Type:** MCP23017 I²C GPIO Expander
- **Configuration:** 9 rows × 6 columns = 54 buttons
- **Interface:** I²C
  - SDA: GPIO 21
  - SCL: GPIO 22
- **Scan Method:** Row-column matrix scanning
  - Rows: Output pins (driven low sequentially)
  - Columns: Input pins (pulled high, read for active low)
- **Debounce:** Hardware capacitors + software filtering
- **Implementation:** esp32/main/mcp23017_button_driver.c

**Power Requirements:**
- **Voltage:** 3.3V regulated (from USB or battery)
- **Current:**
  - ESP32: 80-160 mA active, <10 µA deep sleep
  - Display: 100-150 mA (backlight on)
  - Total: ~300 mA typical

**Memory Usage:**
```
ESP32 Memory Map:
┌─────────────────────────────────┐
│ Flash (4 MB)                    │
│ ├─ Bootloader (32 KB)           │
│ ├─ Partition Table (4 KB)       │
│ ├─ Application Code (1.5 MB)    │
│ ├─ LVGL Library (500 KB)        │
│ └─ Free Space (2 MB)            │
├─────────────────────────────────┤
│ SRAM (520 KB)                   │
│ ├─ Code/Data (.text/.data)      │
│ ├─ Heap (malloc) (~250 KB free) │
│ └─ Stack (~20 KB)               │
├─────────────────────────────────┤
│ PSRAM (4 MB) - Optional         │
│ └─ Frame Buffers (~150 KB)      │
│    └─ Free (~3.8 MB)            │
└─────────────────────────────────┘
```

**Pin Connections:**
```
ESP32-WROOM-32 Module
┌────────────────────┐
│                    │
│  GPIO 23 ─────────── MOSI (Display)
│  GPIO 18 ─────────── CLK  (Display)
│  GPIO 5  ─────────── CS   (Display)
│  GPIO 19 ─────────── DC   (Display)
│  GPIO 22 ─────────── RST  (Display + I²C SCL)
│  GPIO 21 ─────────── I²C SDA (Button Matrix)
│                    │
│  3V3 ────────────── Power Out
│  GND ────────────── Ground
│  EN  ────────────── Reset Button
│  USB ────────────── Programming/Power
│                    │
└────────────────────┘
```

**Build Environment:**
- **ESP-IDF:** Version 5.0 or later
- **CMake:** Build system
- **Toolchain:** Xtensa GCC cross-compiler
- **Flash Tool:** esptool.py

**Implementation References:**
- Main entry point: esp32/main/main.c
- Display driver: esp32/main/driver.c
- Button driver: esp32/main/button_driver.c
- MCP23017 driver: esp32/main/mcp23017_button_driver.c

---

### Comparison Table

| Feature | Desktop | ESP32 |
|---------|---------|-------|
| Display | SDL Window (640×510) | ILI9341 LCD (320×240) |
| Input | PC Keyboard + Mouse | Physical Button Matrix |
| RAM | 256 MB system | 520 KB + 4 MB PSRAM |
| Storage | 50 MB HDD/SSD | 4 MB Flash |
| Power | Wall outlet (120V/240V) | 3.3V @ 300mA (USB/Battery) |
| CPU | x86/x64 multi-GHz | Dual-core 240 MHz |
| Platform | Windows/Linux/macOS | FreeRTOS embedded OS |
| Cost | $300+ (PC) | $10-20 (ESP32 module) |

---

## System Security and Data Integrity

This calculator system is designed as a standalone computational tool without network connectivity or data persistence requirements. However, several design considerations ensure robust operation and prevent common software vulnerabilities:

### Input Validation and Safety

**Expression Parser Safety (expr_eval.c:193-210)**

The expression evaluator implements multiple layers of protection against malicious or malformed input:

1. **Null Pointer Checks:** Every public function validates input pointers
   ```c
   if (!expr || strlen(expr) == 0) {
       return NAN;  // Safe failure mode
   }
   ```
   Reference: expr_eval.c:194-196

2. **Bounded String Operations:** All string copies use safe functions with size limits
   ```c
   strncpy(num_str, &p->str[start], len);
   num_str[len] = '\0';  // Explicit null termination
   ```
   Reference: expr_eval.c:56-57

3. **Division by Zero Protection:** Explicit check before division operations
   ```c
   if (divisor != 0) {
       result /= divisor;
   } else {
       return NAN;  // Return error indicator
   }
   ```
   Reference: expr_eval.c:156-160

4. **Buffer Overflow Prevention:** Fixed-size buffers with bounds checking
   - Expression string: 128 bytes max (math.c:17)
   - Number parsing: 64 bytes max (expr_eval.c:53)
   - No dynamic allocation in parser

5. **Infinite Loop Prevention:** Parser position always advances or terminates
   - Forward progress guaranteed in tokenization
   - Malformed expressions return NaN rather than hanging

**Statistical Function Safety (stats.c:67-170)**

1. **Parameter Range Validation:**
   ```c
   if (p < 0.0 || p > 1.0) return NAN;  // Probability must be [0,1]
   if (k < 0 || k > n) return 0.0;       // k cannot exceed n trials
   ```
   Reference: stats.c:94-95, stats.c:103-106

2. **Overflow Prevention in Factorial Calculation:**
   - Uses iterative multiplication/division approach
   - Avoids computing large factorials directly
   - Reference: stats.c:78-90

3. **NaN/Infinity Propagation:**
   - All math functions check for invalid results
   - Display layer handles special values (stats.c:223-231)

### Memory Safety

**Stack Usage Control**

1. **Recursion Depth Limits:**
   - Expression parser has implicit depth limit based on nesting
   - Typical stack usage: ~50 bytes per recursion level
   - Maximum practical depth: ~100 levels (nested parentheses)
   - Reference: expr_eval.c:64-141 (parse_factor recursion)

2. **No Dynamic Allocation in Core Logic:**
   - Expression evaluator uses stack-only Parser structure
   - No malloc/free in calculation paths
   - Eliminates memory leak vulnerabilities

**Static Buffer Management**

1. **History Buffer (math.c:16-19):**
   ```c
   static HistoryEntry history[MAX_HISTORY];  // Fixed size: 10 entries
   ```
   - Circular buffer with predictable memory usage
   - Total size: ~1.9 KB
   - No dynamic growth

2. **Function Storage (graph.c:45-50):**
   ```c
   static Function functions[MAX_FUNCTIONS];  // Fixed size: 4 functions
   ```
   - Total size: ~280 bytes
   - No allocation/deallocation

3. **Canvas Buffer (graph.c:84):**
   - Statically allocated at compile time
   - Size: 128-256 KB depending on color depth
   - Cannot be overflowed by drawing operations

### Integer Overflow Protection

**Coordinate Calculations (graph.c:98-104)**

Integer overflow in coordinate transformation could cause buffer overruns. Protection mechanisms:

1. **Range Validation:**
   ```c
   int valid = !isnan(gy) && !isinf(gy) && 
               cy >= -CANVAS_H && cy <= 2*CANVAS_H;
   ```
   Reference: graph.c:187

2. **Discontinuity Detection:**
   - Prevents drawing lines across extreme value jumps
   - Reference: graph.c:192-196

**Statistical Calculations**

1. **Binomial Coefficient:**
   - Uses double-precision floating point
   - Avoids integer overflow in large factorials
   - Reference: stats.c:78-90

### Data Integrity

**History Consistency (math.c:153-172)**

The history buffer maintains consistency through:

1. **Synchronized Updates:**
   ```c
   strncpy(history[history_count].equation, equation, ...);
   strncpy(history[history_count].result, result, ...);
   history_count++;
   update_history_display();
   ```
   - Atomic update of entry
   - Display update after modification

2. **Bounds Checking:**
   ```c
   if (history_count >= MAX_HISTORY) {
       // Shift entries before adding
   }
   ```
   - Never exceeds buffer capacity

**Function Data Integrity (graph.c:318-337)**

Function editing maintains data consistency:

1. **Edit Mode Flag:**
   ```c
   static int editing_function = -1;  // -1 = not editing
   ```
   - Prevents simultaneous edits
   - Reference: graph.c:61

2. **Cancel vs. Save:**
   - ESC key cancels changes (restores original)
   - ENTER key commits changes
   - Reference: graph.c:323-336

### Error Handling Strategy

**Graceful Degradation**

The system follows a fail-safe philosophy:

1. **NaN Propagation:**
   - Invalid calculations return NaN
   - NaN is detected and displayed as "Error"
   - System continues running
   - References: math.c:56-58, stats.c:223

2. **Display Protection:**
   - Invalid graph points are skipped, not drawn
   - Reference: graph.c:187

3. **No Critical Failures:**
   - No operation can crash the system
   - Invalid input produces safe error indication

**User Feedback**

1. **Immediate Visual Feedback:**
   - Real-time result preview in math app (math.c:46-67)
   - Live graph updates during pan/zoom
   - Button press visual confirmation (main.c:241-244)

2. **Error Messages:**
   - "Error" for invalid math (math.c:57)
   - "Error: Invalid input" for stats (stats.c:224)
   - "Infinity" for overflow (math.c:59)

### Platform-Specific Security Considerations

**Desktop Platform**

1. **File System Access:**
   - Only reads button_keycodes.txt configuration
   - No write operations to disk
   - Reference: main.c:82-145

2. **No Network Access:**
   - SDL2 networking disabled
   - No external communication

**ESP32 Platform**

1. **Memory Protection:**
   - Flash write protection enabled
   - Application partition read-only at runtime

2. **Hardware Watchdog:**
   - Resets system if main loop hangs
   - Configured in ESP-IDF project

3. **No Debug Interfaces in Production:**
   - JTAG disabled in production builds
   - Serial output optional

### Conclusion

While this is a standalone calculator without traditional security threats (no network, no persistent storage, no multi-user access), the design incorporates defensive programming practices that ensure:

- **Reliability:** No crashes from invalid input
- **Predictability:** Fixed memory usage, no leaks
- **Safety:** All operations are bounds-checked
- **Maintainability:** Clear error handling paths

These practices are essential for embedded systems where robustness is paramount, and they demonstrate good software engineering principles applicable to larger systems.

