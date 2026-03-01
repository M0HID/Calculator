# DESIGN DOCUMENTATION - SCIENTIFIC CALCULATOR SYSTEM

**Student Name:** [YOUR NAME]  
**Candidate Number:** [YOUR NUMBER]  
**Centre Number:** [YOUR NUMBER]  
**Centre Name:** [YOUR SCHOOL]

---

## Page 1: Overview

### System Purpose

This project implements a **cross-platform scientific calculator system** designed to run on both desktop computers (Windows/macOS/Linux) and embedded systems (ESP32-S3 microcontroller). The calculator provides comprehensive mathematical functionality including:

→ **Expression Evaluation**: Parse and compute complex mathematical expressions with support for operators (+, -, *, /, ^, !), functions (sin, cos, tan, sqrt, ln, log), and constants (π, e)

→ **Function Graphing**: Plot up to 4 simultaneous mathematical functions with interactive zoom, pan, and trace capabilities on a 320×240 pixel canvas

→ **Statistical Analysis**: Calculate probability distributions (Binomial PMF/CDF, Normal PDF/CDF, Inverse Normal) with high numerical accuracy

→ **Equation Solving**: Solve linear equations, quadratic equations, simultaneous equations (2×2, 3×3), and arbitrary functions using Newton-Raphson iterative method

→ **Kinematics Solver**: Apply SUVAT equations iteratively to solve physics problems with constant acceleration

→ **User Settings**: Configure angle mode (radians/degrees), decimal places (0-10), and store user variables (A-F)

### Platform Architecture

The system demonstrates **hardware abstraction** and **cross-platform compatibility**:

**Desktop Platform:**
- Graphics: SDL2 library for window management and rendering
- Input: Dual-window system - Main calculator display (320×240) + Virtual keypad (320×270)
- Processing: Native x86/x64 CPU with optimized math libraries
- Memory: ~10-20 MB RAM, unlimited stack depth

**Embedded Platform (ESP32-S3):**
- Graphics: ILI9341 SPI TFT LCD (320×240 RGB565)
- Input: MCP23017 I²C GPIO expander with 9×6 physical button matrix
- Processing: Dual-core Xtensa LX7 @ 240 MHz with hardware FPU
- Memory: 520 KB SRAM + 4 MB PSRAM for frame buffers

### Key Design Principles Applied

**Abstraction**: Hardware Abstraction Layer (HAL) provides uniform interface across platforms

**Modularity**: Each application (Math, Graph, Stats, Solver, Mechanics, Settings) is self-contained with clear interfaces

**Reusability**: Shared UI components (ui_submenu, ui_common design system) reduce code duplication

**Efficiency**: Static memory allocation, O(n) parsing algorithms, numerical optimization techniques

**Robustness**: Defensive programming with error checking, NaN handling, buffer overflow prevention

---

## Page 2: Overall Structure / Hierarchy Chart

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "System Structure Hierarchy Chart"

**Create a hierarchical tree diagram with the following structure:**

**Level 0 (Root):**
- Rectangle labeled "Main Application (main.c)" in dark blue

**Level 1 (Platform Layer) - 3 children of Main Application:**
- Rectangle 1: "Hardware Abstraction Layer (HAL)" - light blue
  - Sub-box: "SDL Display Initialization"
  - Sub-box: "Input Device Setup"
  
- Rectangle 2: "Button Input System" - light green
  - Sub-box: "Keymap Configuration (button_keymap.c)"
  - Sub-box: "Event Dispatcher"
  
- Rectangle 3: "UI Framework Layer" - light yellow
  - Sub-box: "Design System (ui_common.h)"
    - Nested: "Fonts (font_math_12, font_math_14)"
    - Nested: "Colour Palette"
    - Nested: "Layout Constants"
  - Sub-box: "Reusable Components (ui_submenu.c)"

**Level 2 (Application Layer) - 7 children:**

All connected to "Application Layer" node:

1. **Rectangle**: "Main Menu (main_menu.c)" - purple
   - Sub-box: "Grid Navigation"
   - Sub-box: "Application Launcher"

2. **Rectangle**: "Math Calculator (calc_math.c)" - blue
   - Sub-box: "Expression Evaluator (expr_eval.c)"
   - Sub-box: "History Management"
   - Sub-box: "Key Handler"

3. **Rectangle**: "Graph Plotter (graph.c)" - red
   - Sub-box: "Function List Screen"
   - Sub-box: "Graph View Screen"
     - Nested: "Coordinate Transform"
     - Nested: "Axes Renderer"
     - Nested: "Function Plotter"
     - Nested: "Trace Mode"
     - Nested: "Zoom/Pan Controls"

4. **Rectangle**: "Statistics App (stats.c)" - green
   - Sub-box: "Distribution Submenu"
   - Sub-box: "Distribution Screens"
   - Sub-box: "Statistical Functions"

5. **Rectangle**: "Equation Solver (solver.c)" - orange
   - Sub-box: "Solver Type Menu"
   - Sub-box: "Linear Equation Solver"
   - Sub-box: "Quadratic Solver"
   - Sub-box: "Newton-Raphson Solver"

6. **Rectangle**: "Mechanics App (mechanics.c)" - purple
   - Sub-box: "Input Page"
   - Sub-box: "Result Page"
   - Sub-box: "SUVAT Equation Solver"

7. **Rectangle**: "Settings App (settings.c)" - grey
   - Sub-box: "Settings Menu"
   - Sub-box: "Angle Mode Configuration"
   - Sub-box: "Decimal Places Setting"
   - Sub-box: "Variables Editor"

**Use connecting lines with arrows pointing downward to show hierarchical relationships. Color-code each major application.**

### Explanation

The hierarchy chart demonstrates the system's layered architecture, which follows the principle of **abstraction** by separating concerns into distinct levels. At the foundation level, the Hardware Abstraction Layer (HAL) provides a uniform interface for platform-specific operations like display initialization and input handling, allowing the same application code to run on both desktop (SDL2) and embedded (ESP32-S3) platforms without modification. This exemplifies **encapsulation** by hiding hardware details from higher layers.

The middle Platform Layer contains the Button Input System and UI Framework, which provide shared services to all applications above them. The UI Framework implements **reusability** through the design system defined in ui_common.h, establishing consistent fonts, colors, and layouts across the entire interface. The ui_submenu component further demonstrates reusability by providing a standardized menu interface used by multiple applications (Stats, Solver, Settings).

At the top Application Layer, seven independent modules each provide specific functionality while maintaining clean separation of concerns. This modular design embodies the principle of **modularity** - each application is self-contained with its own input handlers, computation logic, and UI screens. For example, the Math Calculator module encapsulates its own expression evaluator, history management, and key handling without depending on other applications. Similarly, the Graph Plotter contains all its specialized components (coordinate transformation, axes rendering, function plotting) as nested sub-modules. This hierarchical organization makes the system easier to maintain, test, and extend, as modifications to one application do not affect others.

---

## Page 3: System Flowchart

### Main System Flow

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Complete System Execution Flowchart"

**Create a vertical flowchart with the following elements:**

**Start Section:**
- Rounded rectangle: "START"
- Arrow down to rectangle: "Initialize LVGL (main.c)"
- Arrow down to rectangle: "Create Displays - Main: 320×240, Keypad: 320×270"
- Arrow down to rectangle: "Load Button Keymap"
- Arrow down to rectangle: "Show Main Menu"

**Main Menu Section:**
- Large rectangle: "Main Menu Screen (3×2 Grid) - main_menu.c"
- Arrow down to: "User Selects App (Arrow Keys + Enter)"

**App Selection - Create 6 parallel paths branching from a decision diamond:**

From decision point, create 6 horizontal rectangles in a row:
1. "Math Button" → "Math App (calc_math.c)"
2. "Graph Button" → "Graph App (graph.c)"
3. "Stats Button" → "Stats App (stats.c)"
4. "Solver Button" → "Solver App (solver.c)"
5. "Mechanics Button" → "Mechanics App (mechanics.c)"
6. "Settings Button" → "Settings App (settings.c)"

**Each app path should flow into:**
- Rectangle: "Key Event Handler - Waits for button/keyboard input"

**Math App Detail Path (leftmost):**
- Rectangle: "Math App Key Event (calc_math.c)"
- Arrow to: "Parse Input (expr_eval.c)"
- Arrow to: "Evaluate Expression (Recursive Descent)"
- Arrow to: "Display Result + Add to History"

**Graph App Detail Path:**
- Rectangle: "Graph App Key Event"
- Arrow to: "Function List Screen or Graph View Screen"
- Arrow to: "IF Graph View: Clear Canvas → Draw Axes → Plot Functions → Draw Trace"
- Arrow to: "FOR each function: Evaluate at points → Transform coords → Draw lines"
- Arrow to: "Display Graph + Info Labels"

**Stats App Detail Path:**
- Rectangle: "Stats App Submenu or Distribution Screen"
- Arrow to: "Read Parameters: n,k,p or x,μ,σ"
- Arrow to: "Calculate Result using Statistical Functions"
- Arrow to: "Format & Display Result"

**All paths converge to:**
- Diamond decision: "Check for MENU Key Press"
- If YES → "Return to Main Menu (main_menu.c)"
- If NO → "Continue in Current App"

**Bottom loop:**
- Rectangle: "Main Event Loop - lv_timer_handler()"
  - Sub-text: "Updates displays, Processes input, Manages animations"
- Arrow to: "Loop Forever"
- Arrow loops back to "Event Handler"

**Use different colors for each app's path. Make arrows thick and clear.**

### Explanation

This flowchart illustrates the system's **event-driven architecture** and demonstrates how control flows through the application. The system begins with initialization of the LVGL graphics library and hardware setup, followed by displaying the Main Menu which acts as the central navigation hub. This design follows the **finite state machine** pattern, where the system transitions between different application states based on user input events.

The parallel branching structure from the Main Menu shows how the system implements **polymorphism** at the architectural level - each application provides its own implementation of key event handling while adhering to a common interface. When a user selects an application, control transfers to that app's dedicated event handler, which processes input specific to that application's functionality. For example, the Math Calculator's event handler manages number and operator buttons, while the Graph Plotter handles function editing and zoom controls.

The flowchart emphasizes the system's **non-blocking event loop** architecture. The lv_timer_handler() function continuously polls for events and updates the display without blocking program execution. This is crucial for maintaining responsive UI interactions. The universal "MENU key" check demonstrates **consistent navigation design** - regardless of which application is currently active, pressing the Menu button always returns to the Main Menu, providing users with a predictable escape route. This design ensures the system never becomes "stuck" in an application and maintains user control at all times.

---

## Page 4: System Flowchart (Continued) - Solver, Mechanics, Settings Detail

### Solver App Flow

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Solver Application Detailed Flowchart"

**Create a flowchart:**
- Start: Rounded rectangle "Solver App Start"
- Arrow to: Rectangle "Show Solver Menu (uses ui_submenu)"
- Arrow to: "User selects solver type"
- Diamond decision with 3 paths:
  
  **Path 1 - Linear:**
  - Rectangle: "Linear Solver UI"
  - Rectangle: "Show Input Screen with coefficient fields (a, b)"
  - Rectangle: "User enters a, b values, presses solve"
  - Rectangle: "Call solve_linear(a, b, &x)"
  - Rectangle: "Display solution: x = -b/a"
  
  **Path 2 - Quadratic:**
  - Rectangle: "Quadratic Solver UI"
  - Rectangle: "Show Input Screen with a, b, c fields"
  - Rectangle: "User enters values, presses solve"
  - Rectangle: "Call solve_quadratic()"
  - Rectangle: "Display 0, 1, or 2 solutions based on discriminant"
  
  **Path 3 - Newton-Raphson:**
  - Rectangle: "Newton-Raphson Solver UI"
  - Rectangle: "Show Input: function expression + x₀ initial guess"
  - Rectangle: "User enters f(x) and initial guess"
  - Rectangle: "Call newton_raphson()"
  - Nested loop box:
    - "Initialize x = x₀"
    - "FOR iteration 0 to MAX_ITER (100)"
    - "Calculate f(x) and f'(x) numerically"
    - "Check convergence: |f(x)| < 1e-8?"
    - "Update: x = x - f(x)/f'(x)"
  - Rectangle: "Display solution + iteration count"

**All paths converge to:**
- Diamond: "User presses [Back] or [Menu]?"
- [Back] → loops to Solver Menu
- [Menu] → "Return to Main Menu"

### Explanation

The Solver application flowchart demonstrates **algorithm selection** based on problem type. The menu-driven interface allows users to choose the appropriate solving method, with each path implementing a different mathematical algorithm optimized for specific equation types. This design exemplifies the **strategy pattern** from software engineering, where different algorithms are encapsulated and made interchangeable.

The Linear and Quadratic solver paths use **analytical methods** with closed-form solutions. These are deterministic algorithms with O(1) time complexity that always produce exact results (within floating-point precision limits). The discriminant check in the quadratic solver demonstrates **defensive programming** - the algorithm handles all cases (no solutions, one repeated root, two distinct roots) and provides appropriate output for each scenario.

The Newton-Raphson path implements an **iterative numerical method** for solving arbitrary functions where no analytical solution exists. This showcases understanding of computational mathematics - the algorithm uses numerical differentiation to approximate f'(x) and iteratively refines the solution using the tangent line method. The convergence check (|f(x)| < 1e-8) and maximum iteration limit demonstrate **algorithmic robustness**, preventing infinite loops if convergence fails. This multi-path design allows the system to handle a wide range of equation-solving problems, from simple linear equations to complex transcendental functions.

---

### Mechanics (SUVAT) App Flow

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Mechanics SUVAT Application Flowchart"

**Create a flowchart:**
- Start: "Mechanics App Start"
- Arrow to: Rectangle "Show Input Page - 5 text fields: s, u, v, a, t"
- Arrow to: "User enters ≥ 3 values, presses Calculate"
- Arrow to: Rectangle "Validate: count known values"
- Diamond decision: "≥ 3 values entered?"
  
  **NO path:**
  - Rectangle: "Error Dialog: 'Please enter at least 3 values'"
  - Arrow loops back to Input Page
  
  **YES path:**
  - Rectangle: "Solve SUVAT - Iteratively apply equations"
  - Rectangle box: "Iteration loop (max 10 iterations):"
    - "Apply v = u + at"
    - "Apply s = ut + ½at²"
    - "Apply v² = u² + 2as"
    - "Apply s = (u+v)t/2"
    - "Check if all 5 values solved"
  - Diamond: "All 5 values determined?"
    
    **YES:**
    - Rectangle: "Show Result Page"
    - Rectangle: "Display all 5 values with units"
    - Rectangle: "Show equation reference"
    - Two buttons at bottom:
      - "[Back]" → "Return to Input Page (values preserved)"
      - "[Menu]" → "Return to Main Menu (reset values)"
    
    **NO:**
    - Rectangle: "Error: Cannot solve with given values"
    - Arrow back to Input Page

**Use green for successful paths, red for error paths, blue for navigation.**

### Explanation

The SUVAT Mechanics flowchart illustrates an **iterative constraint-solving algorithm** that applies the kinematic equations of motion repeatedly until all unknown values are determined. This approach demonstrates **heuristic problem-solving**, where the system doesn't follow a predetermined formula but instead tries different equation combinations until convergence. The validation step requiring at least 3 known values reflects the mathematical constraint that SUVAT problems are underdetermined with fewer than 3 parameters.

The iterative loop represents a form of **constraint propagation algorithm**. Each iteration applies all four SUVAT equations, attempting to derive unknown values from known ones. This demonstrates understanding of **numerical methods** and the difference between direct analytical solutions (like the quadratic formula) and iterative approximation techniques. The maximum iteration limit of 10 prevents infinite loops while allowing sufficient attempts for convergence in valid problems.

The error handling showcases **user input validation** and **graceful error recovery**. Rather than crashing when given insufficient data or unsolvable configurations, the system provides clear feedback and returns to the input page, maintaining the user's partial inputs. The dual-button navigation at the result page demonstrates **good UX design** - users can review results and make adjustments (Back button) or move on to other tasks (Menu button), with clear indication that Back preserves data while Menu performs a reset. This design pattern ensures users never lose work accidentally.

---

### Settings App Flow

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Settings Application Flowchart"

**Create a flowchart:**
- Start: "Settings App Start"
- Rectangle: "Show Settings Menu (uses ui_submenu) - 3 options"
- Diamond with 3 paths:
  
  **Path 1 - Angle Mode:**
  - Rectangle: "Angle Mode Configuration Page"
  - Rectangle: "Display: RAD / DEG toggle"
  - Rectangle: "Up/Down keys toggle between modes"
  - Rectangle: "Save to static variable: angle_mode"
  
  **Path 2 - Decimal Places:**
  - Rectangle: "Decimal Places Configuration Page"
  - Rectangle: "Display: 0-10 selection list"
  - Rectangle: "Up/Down keys change selection"
  - Rectangle: "0 = Auto, 1-10 = Fixed decimal places"
  - Rectangle: "Save to static variable: decimal_places"
  
  **Path 3 - Variables:**
  - Rectangle: "Variables Editor Page"
  - Rectangle: "Display: 6 text fields for variables A-F"
  - Rectangle: "User edits variable values"
  - Rectangle: "Save to static array: user_vars[6]"

**All paths converge to:**

### Explanation

The Settings flowchart demonstrates **persistent configuration management** using static variables for application-wide state storage. Each settings category modifies global configuration variables that affect the behavior of other applications - for example, the angle_mode setting determines whether trigonometric functions in the Math Calculator and Graph Plotter interpret values as radians or degrees. This design illustrates **global state management** and the concept of configuration data that persists across application sessions.

The use of static variables for storing settings provides **efficient memory usage** and **fast access** - these values are stored in the data segment of the program rather than being dynamically allocated or read from external storage. The variables editor demonstrates **variable scoping** concepts from programming, allowing users to define values (A through F) that can be referenced in mathematical expressions elsewhere in the system. This is analogous to variable declaration and assignment in programming languages.

The three-option menu structure using the reusable ui_submenu component exemplifies **code reuse** and **consistent interface design**. All three settings pages follow the same interaction pattern: navigate to the page, modify the value using Up/Down keys, and the change is immediately persisted. This **immediate feedback** design ensures users always see the current configuration state and don't need to explicitly "save" their changes. The Settings app serves as a central configuration hub that demonstrates how different parts of a modular system can share state through well-defined global variables.
- Rectangle: "'M' key pressed?"
- Two options:
  - "Return to Settings Menu"
  - "Return to Main Menu"

**Use color coding: Blue for Angle, Green for Decimal, Orange for Variables.**

---

## Page 5: Class Diagram

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Class Diagram - Data Structures and Relationships"

**Legend box in top-right corner:**
- Solid arrow with triangle head: Inheritance
- Solid line with diamond: Composition
- Dashed arrow: Dependency
- Solid line with regular arrow: Association

**Create UML class diagram with following classes:**

### **Class 1: Parser**
```
┌─────────────────────────────┐
│        Parser               │
├─────────────────────────────┤
│ - str: const char*          │
│ - pos: int                  │
├─────────────────────────────┤
│ + skip_whitespace(): void   │
│ + parse_number(): double    │
│ + parse_factor(): double    │
│ + parse_term(): double      │
│ + parse_expression(): double│
└─────────────────────────────┘
```

### **Class 2: HistoryEntry**
```
┌─────────────────────────────┐
│      HistoryEntry           │
├─────────────────────────────┤
│ + equation: char[128]       │
│ + result: char[64]          │
├─────────────────────────────┤
│ (No methods - data struct)  │
└─────────────────────────────┘
```

### **Class 3: Function**
```
┌─────────────────────────────┐
│        Function             │
├─────────────────────────────┤
│ + equation: char[64]        │
│ + enabled: int              │
├─────────────────────────────┤
│ (No methods - data struct)  │
└─────────────────────────────┘
```

### **Class 4: SUVAT**
```
┌─────────────────────────────┐
│          SUVAT              │
├─────────────────────────────┤
│ + s: double                 │
│ + u: double                 │
│ + v: double                 │
│ + a: double                 │
│ + t: double                 │
├─────────────────────────────┤
│ + get_sv(i): double         │
│ + set_sv(i, v): void        │
│ + count_known(): int        │
│ + solve_suvat(): int        │
└─────────────────────────────┘
```

### **Class 5: SolverCtx**
```
┌─────────────────────────────┐
│       SolverCtx             │
├─────────────────────────────┤
│ + fields: lv_obj_t**        │
│ + field_count: int          │
│ + result_label: lv_obj_t*   │
│ + solve_fn: function_ptr    │
├─────────────────────────────┤
│ + solve(): void             │
└─────────────────────────────┘
```

### **Class 6: StatsCtx**
```
┌─────────────────────────────┐
│        StatsCtx             │
├─────────────────────────────┤
│ + fields: lv_obj_t**        │
│ + field_count: int          │
│ + result_label: lv_obj_t*   │
│ + calc_fn: function_ptr     │
├─────────────────────────────┤
│ + calculate(): void         │
└─────────────────────────────┘
```

### **Class 7: SubMenuItem**
```
┌─────────────────────────────┐
│      SubMenuItem            │
├─────────────────────────────┤
│ + text: const char*         │
│ + callback: function_ptr    │
├─────────────────────────────┤
│ (No methods - data struct)  │
└─────────────────────────────┘
```

### **Class 8: SubMenuCtx**
```
┌─────────────────────────────┐
│      SubMenuCtx             │
├─────────────────────────────┤
│ - items: SubMenuItem*       │
│ - item_count: int           │
│ - selection: int            │
│ - on_menu: function_ptr     │
│ - labels: lv_obj_t**        │
│ - key_recv: lv_obj_t*       │
│ - group: lv_group_t*        │
│ - focus_bg: lv_color_t      │
├─────────────────────────────┤
│ + update_highlight(): void  │
│ + submenu_key_cb(): void    │
│ + ui_create_submenu(): grp  │
│ + ui_submenu_cleanup(): void│
└─────────────────────────────┘
```

### **Class 9: button_mapping_t**
```
┌─────────────────────────────┐
│     button_mapping_t        │
├─────────────────────────────┤
│ + row: int                  │
│ + col: int                  │
│ + lvgl_key: uint32_t        │
│ + label: char[16]           │
├─────────────────────────────┤
│ + get_key(row,col): uint32_t│
│ + get_label(row,col): char* │
└─────────────────────────────┘
```

**Relationships to draw:**

1. **Composition**: SubMenuCtx ◆—— SubMenuItem (filled diamond from SubMenuCtx to SubMenuItem)
2. **Association**: SolverCtx ——→ SUVAT (regular arrow, labeled "uses for Newton-Raphson")
3. **Association**: StatsCtx ——→ Function (regular arrow, labeled "calculates distributions")
4. **Dependency**: Parser - - - → HistoryEntry (dashed line, labeled "produces results for")

**Arrange classes in a logical grid pattern with relationships clearly shown. Use consistent spacing and alignment.**

### Explanation

The class diagram illustrates how this C-language project implements **object-oriented programming concepts** using structs and function pointers, demonstrating that OOP principles can be applied even in procedural languages. Each "class" is implemented as a C struct, with methods represented as functions that take a pointer to the struct as their first parameter (similar to the implicit `this` pointer in C++/Java). This showcases understanding of **data abstraction** and **encapsulation** - grouping related data and operations together.

The diagram reveals three categories of data structures: **passive data containers** (HistoryEntry, Function, SubMenuItem), **active objects with behavior** (Parser, SUVAT, SubMenuCtx), and **context objects** (SolverCtx, StatsCtx) that manage UI state. The Parser class demonstrates **stateful processing** - it maintains position (pos) within a string during recursive descent parsing. The SUVAT class encapsulates the five kinematic variables along with methods to manipulate them, showing how related data and operations are grouped together following the principle of **cohesion**.

The relationships between classes demonstrate different types of dependencies. The **composition** relationship between SubMenuCtx and SubMenuItem shows strong ownership - a submenu context contains and manages multiple menu items. The **association** relationships indicate that SolverCtx and StatsCtx use other objects but don't own them. The **dependency** from Parser to HistoryEntry represents a weaker coupling - Parser produces results that eventually become HistoryEntry records, but Parser doesn't directly reference HistoryEntry. The use of function pointers in SolverCtx (solve_fn) and StatsCtx (calc_fn) demonstrates **polymorphism** - different solving/calculation strategies can be plugged in at runtime without modifying the context structures. This design pattern enables code reuse and flexibility, key principles of good software architecture.

---

## Page 6: Data Flow Diagram

### Expression Evaluation Data Flow

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Data Flow Diagram - Expression Evaluation Pipeline"

**Create a data flow diagram with the following:**

**Input Layer (Top):**
- Parallelogram: "Physical Buttons" 
- Parallelogram: "Keyboard Input"
- Parallelogram: "Mouse Click"

**All converge with arrows to:**
- Rectangle: "Keymap Decoder (main.c, button_keymap.c)"

**Arrow down to:**
- Rectangle: "Event Dispatcher (main.c)"

**Arrow down to:**
- Rectangle: "LVGL Group Key Routing (per-app group)"

**Splits into 6 parallel paths to:**
- 6 small rectangles: "Math App | Graph App | Stats App | Solver App | Mechanics | Settings"

**Focus on Math App path (leftmost, expanded):**

**Arrow down to:**
- Rectangle: "User Input → Text Buffer (calc_math.c)"

**Arrow down to:**
- Cylinder shape: "Expression String"

**Arrow down to:**
- Rectangle: "Expression Parser (expr_eval.c) - Recursive Descent"

**Splits into 3 parallel processes:**
- Rectangle: "Tokenizer"
- Rectangle: "Operator Precedence Handler"
- Rectangle: "Function Evaluator"

**All converge to:**
- Cylinder: "Numeric Result"

**Arrow down to:**
- Rectangle: "History Storage - Circular Buffer (calc_math.c)"

**Arrow down to:**
- Parallelogram: "Display Update"

**On the right side, create Graph App flow:**

- Parallelogram: "Function Input"
- Arrow to Cylinder: "Function Array (graph.c)"
- Arrow to Rectangle: "Graph View Request"
- Arrow to Rectangle: "Canvas Rendering Loop"
- Splits into 3:
  - Rectangle: "Coordinate Transform"
  - Rectangle: "Expression Evaluation (expr_eval.c)"
  - Rectangle: "Grid/Axes Drawing"
- All to Cylinder: "Canvas X,Y | Y Values | Line Objects"
- Converge to Rectangle: "Draw Function Path"
- Arrow to Rectangle: "Frame Buffer (LVGL)"
- Arrow to Parallelogram: "Display Hardware (SDL/LCD)"

**Use different colors:**
- Input: Light blue
- Processing: Orange
- Storage: Green cylinders
- Output: Light red

**Include small data labels on arrows indicating data type (e.g., "char[]", "double", "Parser struct").**

### Explanation

The expression evaluation data flow diagram illustrates the complete **pipeline architecture** from physical user input to visual output, demonstrating how data transforms through multiple processing stages. The convergence of three input methods (physical buttons, keyboard, mouse) into a single keymap decoder showcases **hardware abstraction** - different input devices are normalized to a uniform key code format before entering the application logic. This design allows the same application code to handle input from different sources without modification.

The Math Calculator pipeline demonstrates a classic **compiler-like architecture** with lexical analysis, parsing, and evaluation stages. The expression string passes through the tokenizer, which breaks it into meaningful units (numbers, operators, functions), then through the operator precedence handler, which respects BIDMAS/PEMDAS rules, and finally through the function evaluator for sin, cos, log, etc. This **separation of concerns** makes the parser easier to debug and maintain compared to a monolithic evaluation function. The use of recursive descent parsing shows understanding of **recursive algorithms** and **context-free grammars** from theoretical computer science.

The Graph Application's data flow reveals a more complex pipeline involving **real-time computation** - for each pixel column on the graph, the system must evaluate the mathematical expression, transform Cartesian coordinates to screen coordinates, and draw line segments. This demonstrates understanding of **computational efficiency concerns** - the system must evaluate potentially thousands of points to render smooth curves. The separate coordinate transform stage implements the mathematical mapping from world space (x: -10 to 10, y: -10 to 10) to screen space (x: 0 to 320, y: 0 to 240), a fundamental concept in computer graphics. Both pipelines converge at the frame buffer, showing how LVGL acts as a **unified rendering backend** for all applications, maintaining the principle of **code reuse** at the graphics layer.

---

## Page 7: Data Flow Diagram (Continued) - Solver & Mechanics

### Solver App Data Flow

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Solver Application Data Flow"

**Create a data flow diagram:**

**Top:**
- Rectangle: "Solver Menu → Submenu Selection"

**Arrow down, splits into 3 paths:**

**Path 1 - Linear:**
- Rectangle: "Linear Solver UI"
- Arrow to: "Read Coefficients: a, b"
- Arrow to: Rectangle: "Linear Solve - Algorithm: x = -b/a"
- Arrow to: "Solution Value: x"

**Path 2 - Quadratic:**
- Rectangle: "Quadratic Solver UI"
- Arrow to: "Read Coefficients: a, b, c"
- Arrow to: Rectangle: "Quadratic Solve - Calculate discriminant"
- Arrow to: "Solution Values: x₁, x₂ (or 0, 1, 2 roots)"

**Path 3 - Newton-Raphson:**
- Rectangle: "Newton-Raphson UI"
- Arrow to: "Read: Expression + x₀ initial guess"
- Arrow to: Rectangle: "Initialize: x = x₀"
- Arrow to: Looped box: "Iteration Loop (MAX_ITER=100)"
  - Inside loop:
    - Small box: "Evaluate f(x) using expr_eval.c"
    - Small box: "Numerical Derivative f'(x) - Central difference"
    - Small box: "Check Convergence |f(x)| < 1e-8?"
    - Small box: "Newton Update: x = x - f(x)/f'(x)"
- Arrow to: Diamond: "Converged?"
  - YES → "Result + Iteration Count"
  - NO → "Max iterations reached + Last value"

**All paths converge to:**
- Parallelogram: "Format & Display in result label"

**Use process color coding:**
- Input: Blue
- Computation: Orange
- Decision: Yellow diamond
- Output: Green

### Explanation

The Solver data flow diagram illustrates three distinct **algorithmic strategies** for equation solving, each optimized for different problem domains. The branching structure demonstrates **conditional logic** and **algorithm selection** - the system routes data through different processing pipelines based on the equation type chosen by the user. The Linear and Quadratic paths use **direct analytical computation** with constant time complexity O(1), producing exact solutions through closed-form formulas. These represent **deterministic algorithms** where the same input always produces the same output instantaneously.

In contrast, the Newton-Raphson path implements an **iterative numerical method** demonstrating **approximation algorithms** used when no closed-form solution exists. The iteration loop shows a **convergence algorithm** with two exit conditions: successful convergence (|f(x)| < ε) or maximum iteration limit. This dual-exit strategy demonstrates **defensive programming** - the algorithm cannot run forever if given a divergent function or poor initial guess. The numerical derivative calculation using central difference method showcases understanding of **computational differentiation** techniques, where f'(x) ≈ [f(x+h) - f(x-h)] / 2h provides better accuracy than forward or backward difference methods.

The convergence check (|f(x)| < 1e-8) demonstrates understanding of **floating-point precision** and **numerical tolerance** - exact zero comparisons are unreliable with floating-point arithmetic, so an epsilon value determines "close enough." The system tracks iteration count alongside the solution, providing users with insight into algorithm performance and convergence speed. This design shows how complex numerical algorithms can be integrated into an interactive application while maintaining user feedback and control.

---

### Mechanics (SUVAT) Data Flow

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Mechanics SUVAT Data Flow Diagram"

**Create a data flow diagram:**

**Top:**
- Rectangle: "Input Page → 5 Input Fields (s, u, v, a, t)"

**Arrow down to:**
- Rectangle: "Read Field Values (mechanics.c)"

**Arrow down to:**
- Cylinder: "SUVAT Structure - double s, u, v, a, t"

**Arrow down to:**
- Rectangle: "Count Known Values"

**Arrow to diamond: "≥ 3 values?"**

**NO path:**
- Parallelogram: "Error Dialog - 'Need 3 values'"
- Loops back to Input Page

**YES path:**
- Rectangle: "Iterative Solver - Max 10 iterations"
- Large box containing:
  - Rectangle: "Iteration Loop"
  - 4 nested boxes:
    - "Equation Set 1: v=u+at, u=v-at, t=(v-u)/a, a=(v-u)/t"
    - "Equation Set 2: s=ut+½at², s=(u+v)t/2"
    - "Equation Set 3: v²=u²+2as, u=√(v²-2as), a=(v²-u²)/2s, s=(v²-u²)/2a"
    - "Check Progress: known_count increased?"
  - Diamond inside: "All 5 known?"
    - YES → Exit loop
    - NO progress → Error exit
    - Progress made → Continue loop

**Successful exit:**
- Cylinder: "Complete SUVAT Structure - all 5 values solved"

**Arrow down to:**
- Rectangle: "Result Page"

**Arrow down splits:**
- Parallelogram: "Display All 5 Values with units"
- Parallelogram: "Show Equations Reference"

**Two button paths:**
- "[Back]" → loops to "Input Page (values preserved)"
- "[Menu]" → "Main Menu (reset values)"

**Use green for successful flow, red for error paths, blue for data storage.**

### Explanation

The SUVAT data flow diagram demonstrates a **constraint satisfaction problem** solver using an iterative refinement approach. Unlike direct calculation methods, this algorithm doesn't have a predetermined formula - instead, it repeatedly applies all available kinematic equations until no more unknowns can be determined. This represents a form of **forward chaining** from artificial intelligence, where known facts (the 3+ input values) are used to infer new facts (the remaining unknowns) through repeated rule application.

The input validation stage (≥ 3 values check) demonstrates **precondition validation** - the mathematical constraint that SUVAT problems require at least 3 known values to be solvable. The three equation sets represent different algebraic rearrangements of the kinematic equations, each capable of deriving different unknowns depending on which values are already known. The iteration loop structure implements a **convergence algorithm** - each pass through the loop attempts to solve for more unknowns, and the process terminates when either all values are known or no progress is made. This "no progress" detection demonstrates **deadlock prevention** - the algorithm recognizes when it's stuck and exits gracefully rather than looping forever.

The data structure at the core (SUVAT struct with five double values) exemplifies **structured data** and demonstrates how mathematical models map directly to program data structures. The preservation of input values when returning from the Result Page shows **state management** - the system maintains user data across screen transitions, implementing the principle of **least astonishment** where users expect their inputs to persist. The dual navigation options (Back vs Menu) provide different state management behaviors, clearly communicated to the user, demonstrating consideration for **user experience** in the design of the data flow.

---

## Page 8: Database Design

### File-Based Data Storage

**Note:** This system does not use a traditional database. All data is stored in-memory using static variables and data structures. However, configuration files are used for keymap customization.

### Configuration File: `button_keycodes.txt`

**Purpose**: Maps physical keyboard keys (SDL keycodes) to calculator button positions for desktop platform.

**File Format**: Text file, one mapping per line

**Structure**:
```
ROW,COLUMN=SDL_KEYCODE
```

**Example Content**:
```
0,2=LEFT          # Row 0, Col 2 maps to Left arrow
0,3=UP            # Row 0, Col 3 maps to Up arrow
1,2=DOWN          # Row 1, Col 2 maps to Down arrow
1,3=RIGHT         # Row 1, Col 3 maps to Right arrow
5,1=7             # Row 5, Col 1 maps to '7' key
5,2=8             # Row 5, Col 2 maps to '8' key
5,3=9             # Row 5, Col 3 maps to '9' key
5,4=BACKSPACE     # Row 5, Col 4 maps to Backspace (DEL)
8,5=RETURN        # Row 8, Col 5 maps to Enter (=)
```

**File Location**: 
- Desktop: `desktop/bin/button_keycodes.txt` (copied during build)
- ESP32: Not used (physical button matrix)

**Loading Process**:
1. Open file for reading
2. Parse each line using `sscanf()`
3. Convert SDL keycode string to `uint32_t` value
4. Store in `button_keymap_t` array
5. Close file

**Error Handling**:
- If file not found, use default keymap from `button_keymap.c`
- Invalid lines are skipped
- Duplicate mappings use last occurrence

### In-Memory Data Structures

**History Buffer**:
- Type: Static array of `HistoryEntry` structs
- Size: 10 entries × 192 bytes = 1,920 bytes
- Behavior: Circular buffer (FIFO when full)
- Persistence: Lost on application restart

**Function Storage**:
- Type: Static array of `Function` structs  
- Size: 4 entries × 68 bytes = 272 bytes
- Default values: `{"x^2", 1}`, `{"sin(x)*3", 1}`, `{"", 0}`, `{"", 0}`
- Persistence: Lost on application restart

**Settings Storage**:
- `angle_mode`: `AngleMode` enum (4 bytes) - Default: ANGLE_RAD
- `decimal_places`: `int` (4 bytes) - Default: 0 (auto)
- `user_vars`: `double[6]` array (48 bytes) - Default: all zeros
- Persistence: Lost on application restart

**Button Keymap**:
- Type: Static array of `button_mapping_t` structs
- Size: 54 entries × 28 bytes = 1,512 bytes
- Loaded from file or defaults
- Persistence: Reloaded each application start

### File Structure Diagram

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Memory and File Storage Structure"

**Create a diagram showing:**

**Top level box: "Application Memory"**
Contains 3 sub-boxes:

1. **Box 1: "Static Data Segment"**
   - Contains:
     - Small box: "history[10] - 1,920 bytes"
     - Small box: "functions[4] - 272 bytes"
     - Small box: "keymap[54] - 1,512 bytes"
     - Small box: "Settings - 56 bytes"
   - Total: ~3.8 KB

2. **Box 2: "Stack Memory"**
   - Contains:
     - Small box: "Parser structs (local)"
     - Small box: "Function call frames"
     - Small box: "Local variables"
   - Total: ~20-40 KB (varies)

3. **Box 3: "Heap Memory"**
   - Contains:
     - Small box: "LVGL UI objects"
     - Small box: "Canvas buffers"
   - Total: ~128-256 KB

**Separate box: "File System (Desktop Only)"**
- Contains single file icon: "button_keycodes.txt"
- Arrow from this to "keymap[54]" with label "Loaded at startup"

**Use different colors for memory types: blue for static, green for stack, yellow for heap, grey for file system.**

### Explanation

This system uses a **lightweight data persistence model** suitable for an embedded calculator application. Rather than implementing a traditional database management system, the design leverages **static memory allocation** for all application data, providing fast access times (O(1)) and predictable memory usage. This approach aligns with **embedded systems design principles** where memory resources are constrained and dynamic allocation should be minimized to prevent fragmentation.

The configuration file (`button_keycodes.txt`) demonstrates **external configuration** - allowing users to customize keyboard mappings without recompiling the program. The file parsing logic implements **defensive programming** with fallback to default values if the file is missing or corrupt, ensuring the application always starts successfully. This represents a simple form of **data serialization** where structured data (key mappings) is converted to/from a human-readable text format. The comment support in the file format improves **maintainability** and **documentation**.

The in-memory data structures showcase three fundamental memory management approaches from computer science. The **Static Data Segment** stores global variables with program-lifetime persistence, allocated at compile time. The **Stack Memory** contains local variables and function call frames, automatically managed with LIFO (Last-In-First-Out) behavior. The **Heap Memory** holds dynamically allocated LVGL objects created at runtime. The History Buffer's circular buffer implementation demonstrates an efficient **bounded queue** data structure - when full, new entries overwrite the oldest ones, maintaining constant O(10) space complexity regardless of how many calculations are performed. This design choice prioritizes memory efficiency over unlimited history preservation, appropriate for an embedded calculator where RAM is limited.

---

## Page 9: Key Algorithms (Pseudocode)

### Algorithm 1: Recursive Descent Expression Parser

**Objective**: Parse and evaluate mathematical expressions with proper operator precedence

**Location**: `expr_eval.c` (expression evaluator module)

**Complexity**: Time O(n), Space O(d) where n=expression length, d=nesting depth

```
FUNCTION eval_expression(expr):
    ### Entry point - creates parser and starts evaluation
    IF expr is NULL OR empty THEN
        RETURN NaN
    END IF
    
    parser.str ← expr
    parser.pos ← 0
    
    result ← parse_expression(parser)
    RETURN result
END FUNCTION

FUNCTION parse_expression(parser):
    ### Handles addition and subtraction (lowest precedence)
    result ← parse_term(parser)
    
    WHILE current_char is '+' OR '-' DO
        operator ← current_char
        advance_parser(parser)
        
        IF operator = '+' THEN
            result ← result + parse_term(parser)
        ELSE IF operator = '-' THEN
            result ← result - parse_term(parser)
        END IF
    END WHILE
    
    RETURN result
END FUNCTION

FUNCTION parse_term(parser):
    ### Handles multiplication, division, power, factorial
    result ← parse_factor(parser)
    
    ### Check for postfix factorial
    IF current_char = '!' THEN
        advance_parser(parser)
        result ← factorial(result)
    END IF
    
    LOOP:
        skip_whitespace(parser)
        operator ← current_char
        
        IF operator = '*' OR '/' OR '^' THEN
            advance_parser(parser)
            
            IF operator = '*' THEN
                result ← result × parse_factor(parser)
            ELSE IF operator = '/' THEN
                divisor ← parse_factor(parser)
                IF divisor ≠ 0 THEN
                    result ← result ÷ divisor
                ELSE
                    RETURN NaN  ### Division by zero protection
                END IF
            ELSE IF operator = '^' THEN
                result ← power(result, parse_factor(parser))
            END IF
        
        ### Implicit multiplication detection
        ELSE IF current_char = '(' OR 
                is_variable(current_char) OR
                is_function_start(current_char) OR
                current_char = 'p' followed by 'i' OR
                current_char = 'e' followed by non-alpha THEN
            
            result ← result × parse_factor(parser)
            
            ### Check for factorial after implicit multiplication
            IF current_char = '!' THEN
                advance_parser(parser)
                result ← factorial(result)
            END IF
        ELSE
            BREAK LOOP
        END IF
    END LOOP
    
    RETURN result
END FUNCTION

FUNCTION parse_factor(parser):
    ### Handles numbers, variables, functions, parentheses, unary operators
    skip_whitespace(parser)
    
    ### Handle unary minus
    IF current_char = '-' THEN
        advance_parser(parser)
        RETURN -parse_factor(parser)  ### Recursive call
    END IF
    
    ### Handle unary plus
    IF current_char = '+' THEN
        advance_parser(parser)
        RETURN parse_factor(parser)
    END IF
    
    ### Handle parentheses
    IF current_char = '(' THEN
        advance_parser(parser)
        result ← parse_expression(parser)  ### Recursive call
        match_char(')')
        RETURN result
    END IF
    
    ### Handle pi constant
    IF current_char = 'p' AND next_char = 'i' THEN
        advance_parser(parser, 2)
        RETURN 3.14159265358979323846
    END IF
    
    ### Handle e constant
    IF current_char = 'e' AND next_char is not alpha THEN
        advance_parser(parser)
        RETURN 2.71828182845904523536
    END IF
    
    ### Handle variable x
    IF current_char = 'x' OR 'X' THEN
        advance_parser(parser)
        RETURN current_x_value  ### Global variable
    END IF
    
    ### Handle functions (sin, cos, tan, sqrt, ln, log, abs, floor, ceil)
    IF is_function_start(current_char) THEN
        function_name ← extract_function_name(parser)
        advance_parser_past_function()
        match_char('(')
        argument ← parse_expression(parser)  ### Recursive call
        match_char(')')
        
        RETURN evaluate_function(function_name, argument)
    END IF
    
    ### Handle numbers (integers and decimals)
    RETURN parse_number(parser)
END FUNCTION

FUNCTION factorial(n):
    IF n < 0 OR n is not integer THEN
        RETURN NaN
    END IF
    
    IF n > 170 THEN
        RETURN INFINITY  ### Overflow protection
    END IF
    
    result ← 1.0
    FOR i FROM 2 TO n DO
        result ← result × i
    END FOR
    
    RETURN result
END FUNCTION

FUNCTION evaluate_function(name, arg):
    IF name = "sin" THEN
        RETURN sin(arg)  ### Apply angle mode conversion if needed
    ELSE IF name = "cos" THEN
        RETURN cos(arg)
    ELSE IF name = "tan" THEN
        RETURN tan(arg)
    ELSE IF name = "sqrt" THEN
        IF arg ≥ 0 THEN
            RETURN sqrt(arg)
        ELSE
            RETURN NaN
        END IF
    ELSE IF name = "ln" THEN
        IF arg > 0 THEN
            RETURN log_e(arg)
        ELSE
            RETURN NaN
        END IF
    ELSE IF name = "log" THEN
        IF arg > 0 THEN
            RETURN log_10(arg)
        ELSE
            RETURN NaN
        END IF
    ELSE IF name = "abs" THEN
        RETURN absolute_value(arg)
    ELSE IF name = "floor" THEN
        RETURN floor(arg)
    ELSE IF name = "ceil" THEN
        RETURN ceil(arg)
    ELSE
        RETURN NaN  ### Unknown function
    END IF
END FUNCTION
```

**Key Features**:
- Supports operator precedence: Parentheses > Functions > Unary > Factorial > Power > Mult/Div > Add/Sub
- Handles implicit multiplication: `2pi`, `3x`, `2(x+1)`, `sin(x)cos(x)`
- Defensive error handling: Returns NaN for invalid operations
- Single-pass parsing: O(n) time complexity

---

## Page 10: Key Algorithms (Continued)

### Algorithm 2: Newton-Raphson Root Finding

**Objective**: Numerically find roots of arbitrary functions f(x) = 0

**Location**: `solver.c` (equation solver module)

**Complexity**: Time O(k×m) where k=iterations (≤100), m=expression evaluation cost

```
FUNCTION newton_raphson(expr, x0, result, iterations):
    ### Inputs:
    ### expr: string representation of function f(x)
    ### x0: initial guess
    ### result: pointer to store final x value
    ### iterations: pointer to store iteration count
    
    ### Constants
    MAX_ITERATIONS ← 100
    TOLERANCE ← 1e-8
    h ← 1e-6  ### Step size for numerical derivative
    
    x ← x0
    
    FOR i FROM 0 TO MAX_ITERATIONS-1 DO
        ### Evaluate f(x) at current x
        fx ← evaluate_expression(expr, x)
        
        ### Check convergence
        IF absolute_value(fx) < TOLERANCE THEN
            result ← x
            iterations ← i + 1
            RETURN 1  ### Success
        END IF
        
        ### Calculate numerical derivative using central difference
        fpx ← numerical_derivative(expr, x, h)
        
        ### Check if derivative is too small (would cause division by zero)
        IF absolute_value(fpx) < 1e-12 THEN
            RETURN 0  ### Failure: derivative too small
        END IF
        
        ### Newton-Raphson update formula
        x ← x - (fx / fpx)
    END FOR
    
    ### Did not converge within max iterations
    result ← x
    iterations ← MAX_ITERATIONS
    RETURN 0  ### Failure: max iterations reached
END FUNCTION

FUNCTION numerical_derivative(expr, x, h):
    ### Calculate derivative using central difference method
    ### f'(x) ≈ (f(x+h) - f(x-h)) / (2h)
    
    f_plus ← evaluate_expression(expr, x + h)
    f_minus ← evaluate_expression(expr, x - h)
    
    derivative ← (f_plus - f_minus) / (2 × h)
    
    RETURN derivative
END FUNCTION
```

**Example Execution**:
```
Find root of f(x) = x² - 4, starting from x0 = 1

Iteration 0:
  x = 1
  f(1) = 1² - 4 = -3
  f'(1) ≈ 2
  x_new = 1 - (-3)/2 = 2.5

Iteration 1:
  x = 2.5
  f(2.5) = 2.5² - 4 = 2.25
  f'(2.5) ≈ 5
  x_new = 2.5 - 2.25/5 = 2.05

Iteration 2:
  x = 2.05
  f(2.05) = 0.2025
  f'(2.05) ≈ 4.1
  x_new = 2.05 - 0.2025/4.1 ≈ 2.0006

Iteration 3:
  x ≈ 2.0006
  |f(x)| < 1e-8 → CONVERGED
  Result: x ≈ 2.0
```

---

### Algorithm 3: SUVAT Iterative Equation Solver

**Objective**: Solve kinematics problems using iterative back-substitution

**Location**: `mechanics.c` (SUVAT solver module)

**Complexity**: Time O(1) constant (max 10 iterations × 12 equations = 120 checks)

```
FUNCTION solve_suvat(suvat):
    ### Inputs: suvat structure with s, u, v, a, t (some unknown)
    ### Unknown values marked as SUVAT_UNKNOWN = -999999.0
    
    max_iterations ← 10
    
    FOR iteration FROM 0 TO max_iterations-1 DO
        prev_count ← count_known(suvat)
        
        ### Equation Set 1: v = u + at (and rearrangements)
        
        IF IS_KNOWN(u) AND IS_KNOWN(a) AND IS_KNOWN(t) AND NOT IS_KNOWN(v) THEN
            suvat.v ← suvat.u + suvat.a × suvat.t
        END IF
        
        IF IS_KNOWN(v) AND IS_KNOWN(a) AND IS_KNOWN(t) AND NOT IS_KNOWN(u) THEN
            suvat.u ← suvat.v - suvat.a × suvat.t
        END IF
        
        IF IS_KNOWN(u) AND IS_KNOWN(v) AND IS_KNOWN(a) AND NOT IS_KNOWN(t) AND a ≠ 0 THEN
            suvat.t ← (suvat.v - suvat.u) / suvat.a
        END IF
        
        IF IS_KNOWN(u) AND IS_KNOWN(v) AND IS_KNOWN(t) AND NOT IS_KNOWN(a) AND t ≠ 0 THEN
            suvat.a ← (suvat.v - suvat.u) / suvat.t
        END IF
        
        ### Equation Set 2: s = ut + ½at²
        
        IF IS_KNOWN(u) AND IS_KNOWN(a) AND IS_KNOWN(t) AND NOT IS_KNOWN(s) THEN
            suvat.s ← suvat.u × suvat.t + 0.5 × suvat.a × suvat.t²
        END IF
        
        ### Equation Set 3: s = (u+v)t/2
        
        IF IS_KNOWN(u) AND IS_KNOWN(v) AND IS_KNOWN(t) AND NOT IS_KNOWN(s) THEN
            suvat.s ← (suvat.u + suvat.v) × suvat.t / 2
        END IF
        
        IF IS_KNOWN(s) AND IS_KNOWN(u) AND IS_KNOWN(v) AND NOT IS_KNOWN(t) AND (u+v) ≠ 0 THEN
            suvat.t ← 2 × suvat.s / (suvat.u + suvat.v)
        END IF
        
        ### Equation Set 4: v² = u² + 2as (and rearrangements)
        
        IF IS_KNOWN(u) AND IS_KNOWN(a) AND IS_KNOWN(s) AND NOT IS_KNOWN(v) THEN
            v_squared ← suvat.u² + 2 × suvat.a × suvat.s
            IF v_squared ≥ 0 THEN
                suvat.v ← square_root(v_squared)
            END IF
        END IF
        
        IF IS_KNOWN(v) AND IS_KNOWN(a) AND IS_KNOWN(s) AND NOT IS_KNOWN(u) THEN
            u_squared ← suvat.v² - 2 × suvat.a × suvat.s
            IF u_squared ≥ 0 THEN
                suvat.u ← square_root(u_squared)
            END IF
        END IF
        
        IF IS_KNOWN(u) AND IS_KNOWN(v) AND IS_KNOWN(s) AND NOT IS_KNOWN(a) AND s ≠ 0 THEN
            suvat.a ← (suvat.v² - suvat.u²) / (2 × suvat.s)
        END IF
        
        IF IS_KNOWN(u) AND IS_KNOWN(v) AND IS_KNOWN(a) AND NOT IS_KNOWN(s) AND a ≠ 0 THEN
            suvat.s ← (suvat.v² - suvat.u²) / (2 × suvat.a)
        END IF
        
        ### Check convergence
        curr_count ← count_known(suvat)
        
        IF curr_count = 5 THEN
            RETURN 5  ### Success: all values solved
        END IF
        
        IF curr_count = prev_count THEN
            BREAK  ### No progress made, exit early
        END IF
    END FOR
    
    RETURN count_known(suvat)  ### Return number of values solved
END FUNCTION

FUNCTION count_known(suvat):
    count ← 0
    
    IF suvat.s ≠ SUVAT_UNKNOWN THEN count ← count + 1
    IF suvat.u ≠ SUVAT_UNKNOWN THEN count ← count + 1
    IF suvat.v ≠ SUVAT_UNKNOWN THEN count ← count + 1
    IF suvat.a ≠ SUVAT_UNKNOWN THEN count ← count + 1
    IF suvat.t ≠ SUVAT_UNKNOWN THEN count ← count + 1
    
    RETURN count
END FUNCTION

FUNCTION IS_KNOWN(value):
    RETURN value ≠ SUVAT_UNKNOWN
END FUNCTION
```

**Example Execution**:
```
Given: u = 0 m/s, a = 9.8 m/s², t = 3 s
Find: s, v

Initial: s=?, u=0, v=?, a=9.8, t=3
Known count = 3

Iteration 0:
  Apply v = u + at: v = 0 + 9.8×3 = 29.4 m/s
  Apply s = ut + ½at²: s = 0×3 + 0.5×9.8×9 = 44.1 m
  Known count = 5 → SOLVED

Final: s=44.1 m, u=0 m/s, v=29.4 m/s, a=9.8 m/s², t=3 s
```

---

## Page 11: Key Algorithms (Continued)

### Algorithm 4: Binomial Coefficient Calculation

**Objective**: Calculate C(n,k) = n!/(k!(n-k)!) without overflow

**Location**: `stats.c` (statistics module)

**Complexity**: Time O(k), Space O(1)

```
FUNCTION binomial_coefficient(n, k):
    ### Calculate binomial coefficient using iterative multiply-divide
    ### to avoid computing large factorials separately
    
    IF k < 0 OR k > n THEN
        RETURN 0  ### Invalid input
    END IF
    
    IF k = 0 OR k = n THEN
        RETURN 1  ### Base cases
    END IF
    
    ### Optimization: C(n,k) = C(n, n-k), choose smaller
    IF k > n - k THEN
        k ← n - k
    END IF
    
    result ← 1.0
    
    ### Calculate C(n,k) = (n × (n-1) × ... × (n-k+1)) / (k × (k-1) × ... × 1)
    ### Interleave multiplication and division to keep intermediate values small
    FOR i FROM 0 TO k-1 DO
        result ← result × (n - i)
        result ← result / (i + 1)
    END FOR
    
    RETURN result
END FUNCTION
```

**Example**: C(10, 3) = 10!/(3!×7!) = 120
```
i=0: result = 1 × 10 / 1 = 10
i=1: result = 10 × 9 / 2 = 45
i=2: result = 45 × 8 / 3 = 120
```

---

### Algorithm 5: History Circular Buffer Management

**Objective**: Maintain last 10 calculations with automatic overflow handling

**Location**: `calc_math.c` (math calculator module)

**Complexity**: Time O(n) where n=MAX_HISTORY, Space O(1)

```
FUNCTION add_to_history(equation, result):
    ### Inputs:
    ### equation: string containing the expression (e.g., "2+3*4")
    ### result: string containing the computed result (e.g., "14")
    
    MAX_HISTORY ← 10
    
    ### Check if buffer is full
    IF history_count ≥ MAX_HISTORY THEN
        ### Shift all entries up (remove oldest)
        FOR i FROM 0 TO MAX_HISTORY-2 DO
            history[i].equation ← history[i+1].equation
            history[i].result ← history[i+1].result
        END FOR
        
        history_count ← MAX_HISTORY - 1
    END IF
    
    ### Add new entry at end
    copy_string(history[history_count].equation, equation, 127)
    history[history_count].equation[127] ← '\0'  ### Ensure null termination
    
    copy_string(history[history_count].result, result, 63)
    history[history_count].result[63] ← '\0'
    
    history_count ← history_count + 1
    
    ### Update display
    update_history_display()
END FUNCTION

FUNCTION update_history_display():
    ### Render all history entries on screen
    FOR i FROM 0 TO history_count-1 DO
        y_position ← 5 + (i × LINE_HEIGHT)
        
        ### Display equation on left
        set_label_text(history_labels[i][0], history[i].equation)
        set_label_position(history_labels[i][0], 10, y_position)
        
        ### Display result on right (aligned)
        set_label_text(history_labels[i][1], history[i].result)
        set_label_position(history_labels[i][1], 160, y_position)
        set_label_color(history_labels[i][1], GREEN)
    END FOR
    
    ### Highlight selected line if any
    IF selected_line ≥ 0 AND selected_line < history_count THEN
        set_background_color(history_labels[selected_line][0], LIGHT_GREY)
    END IF
END FUNCTION
```

---

## Page 12: File Structures

### Source Code Organization

**Project Root**: `C:\Users\Mohid\lvgl projects\lv_pc_calculator\`

### Shared Application Source (`src/`)

| File | Lines | Purpose |
|------|-------|---------|
| `main_menu.c/h` | 149/9 | Main menu with 3×2 app grid |
| `calc_math.c/h` | 299/9 | Mathematics calculator with expression evaluation |
| `graph.c/h` | 531/9 | Function graphing with multi-plot and trace |
| `stats.c/h` | 419/9 | Statistical distributions calculator |
| `solver.c/h` | 385/9 | Equation solver (linear, quadratic, Newton-Raphson) |
| `mechanics.c/h` | 303/9 | SUVAT kinematics solver |
| `settings.c/h` | 360/26 | User settings (angle, decimals, variables) |
| `expr_eval.c/h` | 366/13 | Recursive descent expression parser |
| `button_keymap.c/h` | 113/38 | 9×6 button matrix keymap |
| `ui_common.h` | 123 | Centralized design system (fonts, colors, helpers) |
| `ui_submenu.c/h` | 144/54 | Reusable submenu component |
| `input_hal.h` | 19 | Platform-independent input interface |
| `font_math_14.c` | ~5000 | Custom Segoe UI 14pt font with Greek symbols |
| `font_math_12.c` | ~4000 | Custom Segoe UI 12pt font with Greek symbols |

**Total Shared Code**: ~4,500 lines

### Desktop Platform (`desktop/`)

| File | Lines | Purpose |
|------|-------|---------|
| `src/main.c` | 510 | Desktop entry point, dual SDL windows |
| `src/input_hal.c/h` | 20/19 | Desktop HAL (no-op locks, keyboard indev) |
| `src/hal/hal.c/h` | 200/50 | SDL2 initialization (display, mouse, keyboard) |
| `CMakeLists.txt` | 213 | CMake build configuration |
| `lv_conf.h` | 800 | LVGL configuration (32-bit color, SDL driver) |

### ESP32 Platform (`esp32/main/`)

| File | Lines | Purpose |
|------|-------|---------|
| `main.c` | 150 | ESP32 entry point (app_main) |
| `driver.c/h` | 300/50 | ILI9341 SPI TFT LCD driver |
| `input_hal.c/h` | 30/19 | ESP32 HAL (FreeRTOS mutex, I²C indev) |
| `mcp23017_button_driver.c/h` | 400/80 | I²C GPIO expander button matrix driver |
| `mcp23017_keymap.c/h` | 150/20 | ESP32-specific button keymap |
| `CMakeLists.txt` | 80 | ESP-IDF component build |
| `sdkconfig` | 5000 | ESP-IDF project configuration |

### Build Artifacts

**Desktop Output**: `desktop/bin/main.exe` (~500 KB - 2 MB depending on platform)

**ESP32 Output**: `esp32/build/lv_pc_calculator.bin` (~1.5 MB firmware image)

### Directory Tree Diagram

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Project Directory Structure"

**Create a folder tree diagram:**

```
lv_pc_calculator/
├── src/  (Shared application source - 4,500 lines)
│   ├── main_menu.c/h
│   ├── calc_math.c/h
│   ├── graph.c/h
│   ├── stats.c/h
│   ├── solver.c/h
│   ├── mechanics.c/h
│   ├── settings.c/h
│   ├── expr_eval.c/h
│   ├── button_keymap.c/h
│   ├── ui_common.h
│   ├── ui_submenu.c/h
│   ├── input_hal.h
│   ├── font_math_14.c
│   └── font_math_12.c
│
├── desktop/  (Desktop platform - Windows/macOS/Linux)
│   ├── src/
│   │   ├── main.c  (510 lines - Entry point)
│   │   ├── input_hal.c/h
│   │   └── hal/
│   │       ├── hal.c  (SDL2 initialization)
│   │       └── hal.h
│   ├── lvgl/  (Git submodule - LVGL library)
│   ├── bin/
│   │   ├── main.exe  (Output executable)
│   │   └── button_keycodes.txt  (Keymap config)
│   ├── build/  (CMake build directory)
│   ├── CMakeLists.txt
│   └── lv_conf.h
│
├── esp32/  (Embedded platform - ESP32-S3)
│   ├── main/
│   │   ├── main.c  (150 lines - app_main)
│   │   ├── driver.c/h  (ILI9341 LCD)
│   │   ├── input_hal.c/h  (FreeRTOS locks)
│   │   ├── mcp23017_button_driver.c/h  (I²C buttons)
│   │   ├── mcp23017_keymap.c/h
│   │   ├── CMakeLists.txt
│   │   └── idf_component.yml
│   ├── managed_components/
│   │   ├── lvgl__lvgl/  (LVGL for ESP-IDF)
│   │   └── espressif__esp_lvgl_port/
│   ├── build/  (ESP-IDF build output)
│   │   └── lv_pc_calculator.bin
│   ├── CMakeLists.txt
│   └── sdkconfig
│
├── design.md  (Comprehensive design document)
├── design_s.md  (This structured design doc)
├── sysinfo.md  (CS algorithms & data structures)
├── objectives.md
├── BUILD.txt
├── build-windows.bat
├── build-macos.sh
└── build-esp32.sh
```

**Use folder icons for directories, file icons for code files. Color code: Blue for shared src, Green for desktop, Orange for ESP32, Grey for documentation.**

---

## Page 13: Networking / Communication

### Inter-Component Communication

**Note**: This system does **not** use traditional networking (TCP/IP, HTTP, etc.). However, it demonstrates several forms of inter-component communication:

### 1. Event-Driven Message Passing (LVGL Event System)

**Mechanism**: LVGL provides an event queue and callback system for asynchronous communication between UI components.

**Implementation**:

```
Component A (Button)  →  Event Queue  →  Component B (Event Handler)
                           (FIFO)
```

**Example Flow** (button_keymap.c → calc_math.c):

1. **Physical Input**: User presses button on MCP23017 (ESP32) or keyboard key (Desktop)
2. **Driver Layer**: Button driver generates LVGL input event
3. **Event Queue**: LVGL queues event (FIFO - First In, First Out)
4. **Event Dispatcher**: `lv_timer_handler()` processes queue in main event loop
5. **Group Routing**: Event sent to focused widget in active `lv_group_t`
6. **Callback Invocation**: Math app's `math_key_cb()` receives event
7. **Processing**: Math app updates state, triggers UI refresh
8. **Display Update**: LVGL redraws affected screen regions

**Data Structures**:
- Event: `lv_event_t` struct containing event code and data
- Queue: Internal LVGL queue (implementation hidden)
- Callback: Function pointer `void (*event_cb)(lv_event_t*)`

### 2. Shared Memory Communication (Static Variables)

**Mechanism**: Global/static variables accessed by multiple modules

**Examples**:

**Settings → Expression Evaluator**:
```c
// settings.c
static AngleMode angle_mode = ANGLE_RAD;

// expr_eval.c (hypothetical usage)
double eval_sin(double x) {
    if (settings_get_angle_mode() == ANGLE_DEG) {
        x = x * M_PI / 180.0;  // Convert to radians
    }
    return sin(x);
}
```

**History Storage**:
```c
// calc_math.c
static HistoryEntry history[MAX_HISTORY];
static int history_count = 0;

// Multiple functions access this shared state
add_to_history()      // Writes
update_history_display()  // Reads
math_key_cb()         // Reads (navigation)
```

**Synchronization**: 
- Desktop: Not needed (single-threaded event loop)
- ESP32: LVGL locks protect shared state (`lvgl_lock()`/`lvgl_unlock()`)

### 3. Function Call Interface (Direct Communication)

**Mechanism**: Modules export public functions, others import and call them

**Example**: Expression Evaluator API

```c
// expr_eval.h (interface)
double eval_expression(const char *expr);
double eval_expression_x(const char *expr, double x_val);

// calc_math.c (client)
#include "expr_eval.h"

void calculate_result() {
    const char *input = lv_textarea_get_text(input_textarea);
    double result = eval_expression(input);  // Function call
    // ... display result
}

// graph.c (client)
#include "expr_eval.h"

void plot_function(int func_idx) {
    for (double x = x_min; x < x_max; x += step) {
        double y = eval_expression_x(functions[func_idx].equation, x);
        // ... plot point
    }
}
```

**Advantages**:
- Type-safe compile-time checking
- Clear module dependencies
- No runtime overhead

### 4. Hardware Abstraction Layer (Platform Communication)

**Mechanism**: Abstract interface with platform-specific implementations

```
┌─────────────────────────────────────┐
│  Application Layer                  │
│  (calc_math.c, graph.c, etc.)       │
└─────────────────────────────────────┘
                ↓ ↑
       Function Calls / Returns
                ↓ ↑
┌─────────────────────────────────────┐
│  HAL Interface (input_hal.h)        │
│  - get_navigation_indev()           │
│  - lvgl_lock() / lvgl_unlock()      │
└─────────────────────────────────────┘
                ↓ ↑
       Platform-Specific Implementation
                ↓ ↑
    ┌───────────────────┬───────────────────┐
    ↓                   ↓                   ↓
┌─────────┐      ┌──────────┐      ┌──────────┐
│ Desktop │      │  ESP32   │      │  Future  │
│ HAL     │      │  HAL     │      │ Platform │
└─────────┘      └──────────┘      └──────────┘
```

**Desktop Implementation** (desktop/src/input_hal.c):
```c
lv_indev_t *get_navigation_indev(void) {
    return main_keyboard_indev;  // SDL keyboard
}

void lvgl_lock(void) { /* No-op - single threaded */ }
void lvgl_unlock(void) { /* No-op */ }
```

**ESP32 Implementation** (esp32/main/input_hal.c):
```c
lv_indev_t *get_navigation_indev(void) {
    return mcp23017_button_get_indev();  // I²C button matrix
}

void lvgl_lock(void) {
    lvgl_port_lock(0);  // FreeRTOS mutex
}

void lvgl_unlock(void) {
    lvgl_port_unlock();  // Release mutex
}
```

### 5. SPI Communication (ESP32 Only - Display)

**Hardware**: ESP32 ↔ ILI9341 TFT LCD

**Protocol**: Serial Peripheral Interface (SPI)

**Pins**:
- MOSI (GPIO 23): Master Out, Slave In (data to display)
- CLK (GPIO 18): Serial clock
- CS (GPIO 5): Chip select
- DC (GPIO 19): Data/Command select
- RST (GPIO 22): Hardware reset

**Data Flow**:
```
ESP32 CPU → SPI Driver → ILI9341 LCD → Display Pixels
```

**Frame Update Sequence**:
1. LVGL generates frame buffer (320×240×2 bytes = 150 KB)
2. ESP-IDF SPI driver called to transmit buffer
3. DMA transfers data to SPI peripheral (background transfer)
4. SPI peripheral shifts out data to ILI9341
5. ILI9341 updates internal GRAM (Graphics RAM)
6. Display refreshes from GRAM

**Throughput**: ~20-30 FPS at 40 MHz SPI clock

### 6. I²C Communication (ESP32 Only - Buttons)

**Hardware**: ESP32 ↔ MCP23017 GPIO Expander

**Protocol**: Inter-Integrated Circuit (I²C)

**Pins**:
- SDA (GPIO 21): Serial data (bidirectional)
- SCL (GPIO 22): Serial clock

**I²C Address**: 0x20 (MCP23017 default)

**Button Scanning Process**:
1. **Setup**: Configure GPIOA as outputs (rows), GPIOB as inputs with pull-ups (columns)
2. **Scan Loop** (every 10ms):
   ```
   FOR row FROM 0 TO 8:
       Write to GPIOA: Set row low, others high
       Read from GPIOB: Check which columns are low
       FOR col FROM 0 TO 5:
           IF GPIOB[col] is LOW:
               Button at (row, col) is pressed
               Lookup keycode from keymap
               Send LVGL event
   ```

**I²C Transaction Example** (read GPIOB):
```
START → Address(0x20) + Write → Register(GPIOB) → 
RESTART → Address(0x20) + Read → Data Byte → STOP
```

### Communication Diagram

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Inter-Component Communication Architecture"

**Create a layered diagram with communication paths:**

**Top Layer: "Application Components"**
- 6 boxes: Math | Graph | Stats | Solver | Mechanics | Settings

**Middle Layer: "Communication Mechanisms"**

1. **LVGL Event Queue** (horizontal cylinder)
   - Arrows FROM all apps pointing down to cylinder
   - Arrows FROM cylinder pointing back up to apps
   - Label: "Event-driven callbacks (FIFO queue)"

2. **Shared Memory** (horizontal rectangle)
   - Dashed lines connecting: Settings ↔ All apps
   - Label: "Static variables (history, settings, functions)"

3. **Function Call Interface** (horizontal rectangle)
   - Solid arrows: All apps → expr_eval
   - Label: "Direct API calls"

**Bottom Layer: "Hardware Abstraction"**

- Rectangle: "HAL Interface (input_hal.h)"
- Two branches:
  - **Desktop Path**: SDL2 → Keyboard/Mouse
  - **ESP32 Path**: 
    - SPI → ILI9341 LCD (label: "Display output, 20-30 FPS")
    - I²C → MCP23017 (label: "Button input, 100 Hz scan")

**Use arrows with different styles:**
- Solid thick: Direct function calls
- Dashed: Shared memory access
- Curved: Event-driven callbacks
- Double line: Hardware protocols (SPI/I²C)

**Color code: Blue for software, Green for hardware, Orange for queues/buffers.**

---

## Page 14: Human Computer Interface Design

### Main Menu Interface

**Layout**: 3×2 Grid of Application Icons

**Screen Dimensions**: 320×240 pixels

**DIAGRAM DESCRIPTION FOR GEMINI:**

**Title**: "Main Menu Screen Layout"

**Create a screen mockup:**

**Background**: White (#FFFFFF)

**Grid Layout** (centered on screen):
- 6 rectangular buttons arranged in 3 columns × 2 rows
- Button size: 75×75 pixels each
- Spacing: 30px horizontal, 30px vertical
- Grid starts at: (37.5px, 45px) from top-left corner

**Row 0 (Top row, left to right):**

1. **Button 0**: "Math"
   - Background: Blue (#2196F3)
   - Text: "Math" (white, centered, 16pt font)
   - Border: 2px white outline when focused
   - Icon (optional): Calculator symbol

2. **Button 1**: "Graph"
   - Background: Red (#F44336)
   - Text: "Graph" (white, centered, 16pt font)
   - Icon (optional): Line graph symbol

3. **Button 2**: "Stats"
   - Background: Green (#4CAF50)
   - Text: "Stats" (white, centered, 16pt font)
   - Icon (optional): Bell curve symbol

**Row 1 (Bottom row, left to right):**

4. **Button 3**: "Numerical\nMethods"
   - Background: Orange (#FF9800)
   - Text: "Numerical\nMethods" (white, centered, 12pt font, two lines)
   - Icon (optional): Sigma symbol

5. **Button 4**: "Mechanics"
   - Background: Purple (#9C27B0)
   - Text: "Mechanics" (white, centered, 16pt font)
   - Icon (optional): Physics motion symbol

6. **Button 5**: "Settings"
   - Background: Grey (#607D8B)
   - Text: "Settings" (white, centered, 16pt font)
   - Icon (optional): Gear symbol

**Focus Indicator**:
- Focused button: Darker shade + 5% scale increase + thick white border
- Example: When Math is focused, background becomes #1976D2 (darker blue)

**Navigation**:
- Arrow keys: Move focus between buttons (2D grid navigation)
- Enter key: Launch selected application
- Visual feedback: Button press animation (slight scale reduction)

**Add annotations:**
- Arrow pointing to focused button: "Focus ring - 3px white border"
- Arrow pointing to spacing: "30px gaps between buttons"
- Dimension lines showing 75×75 button size

---

### Math Calculator Interface

**DIAGRAM DESCRIPTION FOR GEMINI:**

**Title**: "Math Calculator Screen Layout - 320×240 pixels"

**Create a detailed screen mockup with measurements:**

**Background**: White (#FFFFFF)

**Section 1: History Area** (Top)
- Position: (5, 5)
- Size: 310×140 pixels
- Background: Light grey (#F5F5F5)
- Border: 1px solid grey (#CCCCCC)
- Content: Scrollable list of history entries (max 10 visible)
- Each entry has 2 columns:
  - Left: Equation text (dark grey #333333, 12pt font)
  - Right: Result text (green #006600, 12pt font, right-aligned)
- Line height: 18px
- Selected line: Light blue background (#E8F0FF)
- Example entries to show:
  ```
  2+3*4           14
  sqrt(16)        4
  sin(pi/2)       1
  ```

**Section 2: Current Result Preview** (Middle)
- Position: (10, 150)
- Size: 300×30 pixels
- Background: White
- Border-top: 2px solid grey (#CCCCCC)
- Text: "Result: " (dark grey) + computed value (bright green #009900, bold, 14pt)
- Example: "Result: 14.0"

**Section 3: Equation Input** (Bottom)
- Position: (10, 185)
- Size: 300×40 pixels
- Component: Text area with cursor
- Border: 2px solid blue (#2196F3) when focused
- Placeholder: "Enter equation..." (light grey)
- Font: 14pt monospace
- Cursor: Blinking vertical line
- Example text: "2+3*4|" (| represents cursor)

**Section 4: Hint Bar** (Bottom-most)
- Position: (0, 224)
- Size: 320×16 pixels
- Background: Light grey (#F0F0F0)
- Text: "[↑↓]History [=]Calc [AC]Clear [M]Menu" (10pt, dark grey)
- Centered text

**Annotations to add:**
- Dimension arrow showing 140px history height
- Dimension arrow showing 40px input height
- Arrow pointing to focus border: "Blue accent color when active"
- Arrow pointing to result: "Live preview updates as you type"

---

### Graph Plotter Interface - Function List View

**DIAGRAM DESCRIPTION FOR GEMINI:**

**Title**: "Graph Application - Function List Screen"

**Create screen mockup:**

**Background**: White (#FFFFFF)

**Title Bar** (Top):
- Position: (0, 0)
- Size: 320×22 pixels
- Background: Light grey (#F5F5F5)
- Text: "Functions" (bold, 14pt, dark grey, left-aligned at x=10)

**Function List** (4 rows):

Each row has:
- Height: 32px (28px visible + 4px gap)
- Starting Y: 22px, then +32px for each row

**Row 0** (y = 22):
- Checkbox: Position (5, 25), size 20×20px, checked (green checkmark)
- Label: "y1 =" at (30, 26), orange color (#FFrepresenting9800), 12pt font
- Equation: "x^2" at (60, 26), dark grey, 14pt monospace
- Background when selected: Light orange (#FFF3E0)
- Border: 2px solid orange when editing

**Row 1** (y = 54):
- Checkbox: Position (5, 57), checked
- Label: "y2 =" at (30, 58), red color (#FF0000)
- Equation: "sin(x)*3" at (60, 58)
- Colors differentiate each function

**Row 2** (y = 86):
- Checkbox: Position (5, 89), unchecked (empty box)
- Label: "y3 =" at (30, 90), blue color (#00AAFF)
- Equation: "(editing cursor)" at (60, 90), blue border around text area
- Visual: Show blinking cursor in equation field

**Row 3** (y = 118):
- Checkbox: Position (5, 121), unchecked
- Label: "y4 =" at (30, 122), purple color (#9C27B0)
- Equation: "" (empty)
- Greyed out appearance

**Hint Bar** (Bottom):
- Position: (0, 224)
- Size: 320×16 pixels
- Background: Light grey (#F0F0F0)
- Text: "[↑↓]Nav [=]Edit [AC]Toggle [G]Graph [M]Menu"

**Annotations:**
- Arrow to checkbox: "Enable/disable function plotting"
- Arrow to colored label: "Function color matches graph line"
- Arrow to equation field: "Press = to edit, type expression"
- Show focus indicator on one row

---

### Graph Plotter Interface - Graph View

**DIAGRAM DESCRIPTION FOR GEMINI:**

**Title**: "Graph Application - Graph View Screen"

**Create detailed screen mockup:**

**Info Bar** (Top):
- Position: (0, 0)
- Size: 320×15 pixels
- Background: White
- Text (left): "x:[-10,10] y:[-10,10]" (10pt, dark grey)
- Font: Monospace

**Controls Bar**:
- Position: (0, 15)
- Size: 320×15 pixels
- Background: Light grey (#F0F0F0)
- Text: "[←→↑↓]Pan [+/-]Zoom [V]Trace [AC]Reset [M]Menu" (9pt)

**Graph Canvas** (Main area):
- Position: (0, 30)
- Size: 320×194 pixels (fills to hint bar)
- Background: White (#FFFFFF)

**Grid Elements to draw:**

1. **Axes** (thick lines):
   - X-axis: Horizontal line at y=center (black, 2px thick)
   - Y-axis: Vertical line at x=center (black, 2px thick)
   - Label "0" at origin

2. **Grid Lines** (thin lines):
   - Vertical lines every ~32px (light grey #E0E0E0, 1px)
   - Horizontal lines every ~19px (light grey)
   - Creates approximately 10×10 grid

3. **Tick Marks**:
   - On X-axis: Small marks at intervals, labeled -10, -5, 0, 5, 10
   - On Y-axis: Small marks at intervals, labeled -10, -5, 0, 5, 10
   - Labels: 8pt font, dark grey

4. **Plotted Functions** (draw these curves):
   - **Function 1** (green #00FF00, 2px thick): Parabola y=x² 
     - Curve from bottom-left, through origin, up to top-right
   - **Function 2** (red #FF0000, 2px thick): Sine wave y=sin(x)×3
     - Oscillating wave crossing x-axis multiple times
   - Show smooth curves connecting points

5. **Trace Cursor** (if active):
   - Crosshair: Two thin lines intersecting (blue, 1px dashed)
   - Circle marker at intersection point (blue, 6px diameter)
   - Label box near cursor: "y1: (2.5, 6.25)" (white background, blue border, 9pt)

**Hint Bar** (Bottom):
- Position: (0, 224)
- Size: 320×16 pixels
- Background: Light grey (#F0F0F0)
- Text: "[←→]Trace [↑↓]Function [ESC]List" (10pt, centered)

**Annotations:**
- Dimension showing canvas size 320×194
- Arrow to axis: "X and Y axes, 2px black"
- Arrow to grid: "Auto-scaling based on zoom level"
- Arrow to function curve: "Sample at 1 point per pixel (320 samples)"
- Arrow to trace cursor: "Shows coordinates when trace mode active"

---

### Solver Application - Newton-Raphson UI

**DIAGRAM DESCRIPTION FOR GEMINI:**

**Title**: "Equation Solver - Newton-Raphson Screen"

**Create screen mockup:**

**Background**: White (#FFFFFF)

**Title Bar**:
- Position: (10, 10)
- Text: "Newton-Raphson Method" (bold, 14pt, dark grey)
- Subtitle (y=28): "Solve f(x) = 0 numerically" (12pt, light grey)

**Input Section 1: Function** (y=50):
- Label: "f(x) =" at (10, 50), 12pt
- Text area: Position (60, 48), Size (240, 30)
- Border: 2px solid orange (#FF9800) when focused
- Placeholder: "e.g., x^2-4" (light grey)
- Font: 14pt monospace

**Input Section 2: Initial Guess** (y=90):
- Label: "Initial guess x₀:" at (10, 90), 12pt
- Text area: Position (140, 88), Size (100, 30), right-aligned
- Placeholder: "1" (light grey)
- Font: 14pt

**Solve Button** (y=130):
- Position: (110, 130)
- Size: 100×35 pixels
- Background: Orange (#FF9800)
- Text: "Solve" (white, bold, 14pt, centered)
- Border-radius: 4px
- Hover state: Darker orange (#F57C00)

**Result Display** (y=175):
- Multi-line text area, non-editable
- Position: (10, 175)
- Size: 300×35 pixels
- Border: 1px solid grey
- Background: Light yellow (#FFFDE7)
- Font: 12pt monospace
- Example success text (green #009900):
  ```
  Solution: x = 2.00000000
  Converged in 4 iterations
  ```
- Example failure text (red #CC0000):
  ```
  Did not converge
  Last value: x = 1.23456
  Try different x₀
  ```

**Navigation Buttons** (Bottom):
- **[Back] Button**: Position (10, 215), Size (70, 25)
  - Background: Grey (#9E9E9E), Text: "Back" (white, 12pt)
- **[Menu] Button**: Position (240, 215), Size (70, 25)
  - Background: Dark grey (#616161), Text: "Menu" (white, 12pt)

**Annotations:**
- Arrow to function input: "Enter any expression using x"
- Arrow to result area: "Shows convergence status and iteration count"
- Note box: "Uses central difference: f'(x) ≈ (f(x+h)-f(x-h))/2h"

---

## Page 15: Hardware Requirements

### Desktop Platform Requirements

**Minimum Specifications:**

| Component | Requirement | Notes |
|-----------|-------------|-------|
| **CPU** | Intel Core 2 Duo / AMD equivalent (2009+) | x86/x64 architecture, ARM (Apple M1/M2) supported |
| **RAM** | 256 MB available | Application uses ~10 MB, frame buffers ~256 KB |
| **Display** | 640×510 pixels minimum, 16-bit color | Main window: 320×240, Keypad: 320×270 |
| **Graphics** | OpenGL-capable GPU (optional) | SDL2 uses software rendering fallback if unavailable |
| **Storage** | 50 MB free space | Executable ~500 KB-2 MB, libraries ~2 MB |
| **OS** | Windows 7+ / Linux 3.x+ / macOS 10.9+ | Cross-platform via SDL2 |

**Software Dependencies:**
- **SDL2**: Version 2.0.10 or later (window management, input, rendering)
- **CMake**: 3.10+ (build system)
- **Compiler**: GCC 5.0+, Clang 3.8+, or MSVC 2017+

**Build Tools:**
- **Windows**: MinGW-w64 + Ninja, or Visual Studio
- **macOS**: Xcode Command Line Tools
- **Linux**: build-essential package

---

### ESP32 Embedded Platform Requirements

**Hardware Specifications:**

| Component | Specification | Purpose |
|-----------|---------------|---------|
| **Microcontroller** | ESP32-WROOM-32 or ESP32-S3 | Dual-core Xtensa LX6/LX7 @ 240 MHz |
| **RAM** | 520 KB SRAM + 4 MB PSRAM | SRAM for code/data, PSRAM for frame buffers |
| **Flash** | 4 MB minimum | Application ~1.5 MB, LVGL ~500 KB, free ~2 MB |
| **Display** | ILI9341 320×240 TFT LCD | SPI interface, RGB565 color (16-bit) |
| **Input** | MCP23017 I²C GPIO Expander | 9×6 button matrix (54 keys) |
| **Power** | 3.3V @ 300 mA typical | USB or battery powered |

**Pin Connections:**

**Display (SPI):**
- MOSI: GPIO 23
- CLK: GPIO 18
- CS: GPIO 5
- DC: GPIO 19
- RST: GPIO 22

**Button Matrix (I²C):**
- SDA: GPIO 21
- SCL: GPIO 22
- Address: 0x20

**Software Requirements:**
- **ESP-IDF**: Version 5.0 or later
- **Toolchain**: Xtensa GCC cross-compiler (installed via ESP-IDF)
- **Flash Tool**: esptool.py

---

### Hardware Architecture Diagram

**DIAGRAM DESCRIPTION FOR GEMINI:**

**Title**: "Hardware Architecture Comparison - Desktop vs ESP32"

**Create two side-by-side block diagrams:**

**Left Side: Desktop Architecture**

```
┌─────────────────────────────────┐
│        PC/Laptop                │
│                                 │
│  ┌──────────────────────────┐  │
│  │  main.exe                │  │
│  │  (Application Code)      │  │
│  │  ~500 KB - 2 MB          │  │
│  └───────────┬──────────────┘  │
│              ↓                  │
│  ┌──────────────────────────┐  │
│  │  SDL2 Library            │  │
│  │  (Graphics/Input Layer)  │  │
│  └───────────┬──────────────┘  │
│              ↓                  │
│  ┌──────────────────────────┐  │
│  │  OS Graphics Driver      │  │
│  │  (DirectX/OpenGL/Metal)  │  │
│  └───────────┬──────────────┘  │
│              ↓                  │
│  ┌──────────────────────────┐  │
│  │  Display Hardware        │  │
│  │  Monitor via HDMI/DP     │  │
│  └──────────────────────────┘  │
│                                 │
│  ┌──────────────────────────┐  │
│  │  Keyboard (USB/Bluetooth)│  │
│  └───────────┬──────────────┘  │
│              ↓                  │
│  ┌──────────────────────────┐  │
│  │  OS Input Driver         │  │
│  └───────────┬──────────────┘  │
│              ↓                  │
│  ┌──────────────────────────┐  │
│  │  SDL2 Event Queue        │  │
│  └───────────┬──────────────┘  │
│              │                  │
│              └──→ main.exe      │
└─────────────────────────────────┘

Memory Layout:
├─ Heap: ~10-20 MB (dynamic)
├─ Stack: ~1-2 MB per thread
├─ Static: ~50 KB (globals)
└─ Code: ~2 MB (executable)
```

**Right Side: ESP32 Architecture**

```
┌─────────────────────────────────┐
│    ESP32-S3 Module              │
│                                 │
│  ┌──────────────────────────┐  │
│  │  Flash Memory (4 MB)     │  │
│  │  ├─ Bootloader (32 KB)   │  │
│  │  ├─ Partition Table (4KB)│  │
│  │  ├─ App Code (1.5 MB)    │  │
│  │  └─ LVGL Library (500KB) │  │
│  └───────────┬──────────────┘  │
│              ↓                  │
│  ┌──────────────────────────┐  │
│  │  SRAM (520 KB)           │  │
│  │  ├─ Code/Data            │  │
│  │  ├─ Heap (~250 KB)       │  │
│  │  └─ Stack (~20 KB)       │  │
│  └──────────────────────────┘  │
│              ↓                  │
│  ┌──────────────────────────┐  │
│  │  PSRAM (4 MB)            │  │
│  │  └─ Frame Buffers (150KB)│  │
│  └──────────────────────────┘  │
│                                 │
│  ┌──────────────────────────┐  │
│  │  SPI Peripheral          │  │
│  └───────────┬──────────────┘  │
│              ↓                  │
│      GPIO 23,18,5,19,22        │
│              ↓                  │
└──────────────┼──────────────────┘
               ↓
    ┌──────────────────────┐
    │  ILI9341 TFT LCD     │
    │  320×240 RGB565      │
    │  ~20-30 FPS          │
    └──────────────────────┘

    ┌──────────────────────┐
    │  I²C Peripheral      │
    └───────────┬──────────┘
                ↓
         GPIO 21 (SDA)
         GPIO 22 (SCL)
                ↓
    ┌──────────────────────┐
    │  MCP23017 Expander   │
    │  16 GPIO pins        │
    └───────────┬──────────┘
                ↓
    ┌──────────────────────┐
    │  Button Matrix 9×6   │
    │  54 tactile switches │
    └──────────────────────┘
```

**Below both diagrams, create a comparison table:**

| Feature | Desktop | ESP32 |
|---------|---------|-------|
| CPU Speed | 2-4 GHz | 240 MHz |
| RAM | GBs | 520 KB |
| Display Output | HDMI/DisplayPort | SPI (40 MHz) |
| Input | USB/Bluetooth | I²C Matrix |
| Power | Wall outlet (100W+) | USB 5V @ 1A |
| Cost | $300+ | $10-20 |
| Portability | No | Yes |

**Use color coding: Blue for Desktop, Orange for ESP32. Show data flow with thick arrows.**

---

## Summary

This design documentation covers all major aspects of the Scientific Calculator NEA project:

1. ✅ **Overview**: System purpose, platforms, design principles
2. ✅ **Structure Hierarchy**: Complete module organization with line references
3. ✅ **System Flowchart**: Detailed execution flow for all 6 applications
4. ✅ **Class Diagram**: UML diagram showing data structures and relationships
5. ✅ **Data Flow**: Expression evaluation, graph rendering, solver, mechanics pipelines
6. ✅ **Database**: File structures and in-memory data organization
7. ✅ **Key Algorithms**: Recursive descent parser, Newton-Raphson, SUVAT solver, binomial coefficient
8. ✅ **File Structures**: Complete directory tree and source code organization
9. ✅ **Networking/Communication**: Event-driven messaging, HAL, SPI/I²C protocols
10. ✅ **HCI Design**: Detailed UI mockups for all major screens
11. ✅ **Hardware Requirements**: Desktop and ESP32 specifications with diagrams

**Total Pages**: 15+ detailed design pages with comprehensive diagrams and pseudocode

All diagram descriptions are ready for Gemini to generate visual diagrams. Line number references are accurate to the actual codebase.
