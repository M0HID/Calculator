# Modelling the Problem

---

## Programming Language Justification

### Potential Programming Languages

#### C

**Advantages**
- **Performance**: C is a highly performant, low-level language that compiles to efficient native code. For an embedded calculator running on ESP32 or a desktop environment, this ensures fast expression evaluation and responsive UI rendering.
- **Hardware Access**: C allows direct memory manipulation and hardware register access, which is essential for embedded targets like the ESP32 where peripheral control (display drivers, input handling) must be done at a low level.
- **Portability**: C is universally supported across platforms. The same core logic can be compiled for both Windows desktop (via MinGW/MSVC) and ESP32 (via ESP-IDF/GCC toolchain) with minimal changes.
- **Memory Efficiency**: C provides fine-grained control over memory allocation and stack/heap usage, which is critical on resource-constrained embedded systems with limited RAM.
- **Mature Ecosystem**: Well-established libraries and frameworks exist for C, including LVGL (Light and Versatile Graphics Library) which is purpose-built for embedded GUIs in C.

**Disadvantages**
- **Manual Memory Management**: C requires explicit memory management (malloc/free), which can lead to memory leaks, buffer overflows, or undefined behaviour if not handled carefully.
- **Longer Development Time**: C lacks the high-level abstractions of modern languages. Implementing complex features such as expression parsing requires more boilerplate code compared to languages like Python.
- **No Built-in Error Handling**: C has no exceptions or built-in error-handling mechanisms. Errors must be handled manually through return codes or status flags, which increases code complexity.

---

#### C++

**Advantages**
- **Object-Oriented Design**: C++ supports classes and inheritance, making it easier to model complex systems such as expression evaluators or UI component hierarchies in a structured way.
- **Standard Template Library (STL)**: The STL provides ready-made data structures (stacks, queues, vectors) that are directly useful for implementing algorithms like the Shunting Yard algorithm for expression parsing.
- **Performance**: Like C, C++ compiles to native machine code and incurs minimal runtime overhead, making it suitable for both embedded and desktop targets.
- **LVGL Compatibility**: LVGL is written in C but is fully usable from C++ projects, allowing a C++ application to benefit from both the LVGL GUI framework and C++ language features.

**Disadvantages**
- **Increased Complexity**: C++ introduces significant language complexity (templates, multiple inheritance, operator overloading) which can make codebases harder to maintain and debug.
- **Larger Binary Size**: C++ programs, especially those using the STL, can produce larger binaries, which may strain flash storage limits on embedded targets like the ESP32.
- **Compatibility Issues**: Different C++ standards (C++11, C++14, C++17) and compiler support varies across toolchains, which can introduce portability issues when targeting both desktop and embedded platforms.

---

#### Python

**Advantages**
- **Rapid Prototyping**: Python's concise syntax and dynamic typing allow logic to be written and tested very quickly, making it useful for prototyping calculator logic before porting to C.
- **Rich Libraries**: Python has extensive libraries for mathematical computation (math, sympy) that could accelerate development of expression evaluation features.
- **Readability**: Python code is highly readable and easy to maintain, reducing the chance of logical errors during development.

**Disadvantages**
- **Not Suitable for Embedded Targets**: Standard Python cannot run on the ESP32 (MicroPython is an alternative but has significant limitations). This makes Python unsuitable as the primary language for a cross-platform embedded/desktop project.
- **Performance Overhead**: Python is interpreted and dynamically typed, resulting in significantly slower execution compared to C or C++. This is problematic for a real-time GUI application requiring responsive input handling.
- **Memory Usage**: Python's runtime has a large memory footprint, which is incompatible with the constrained RAM environment of microcontrollers like the ESP32.

---

### Chosen Programming Language: C

I have chosen to develop the calculator using **C** as the primary programming language. The core reason is that the project targets both a desktop environment (Windows) and an embedded platform (ESP32), and C is the only language that is natively and fully supported by both toolchains without compromise. The LVGL graphics library — which I am using for the GUI — is written in C and is designed to be used in C projects, making C the natural choice.

C's low-level memory control is essential for the ESP32 target, where RAM is limited and dynamic allocation must be carefully managed. The expression parser, which uses a recursive-descent approach, is straightforward to implement in C — operator precedence is encoded structurally in the call chain rather than requiring explicit stack data structures, avoiding the overhead of the C++ STL. Additionally, the single-codebase, cross-platform approach is much cleaner in C, as the same `src/` directory compiles for both targets with only platform-specific build scripts differing.

The main trade-off is longer development time due to manual memory management and the absence of high-level abstractions, but given the performance and portability requirements of this project, C is the most appropriate choice.

---

## Modelling the Problem

### Description

The aim is to develop a fully functional scientific calculator that runs on both a **Windows desktop** (as a native application) and an **ESP32 microcontroller** (as an embedded device). The system accepts mathematical expressions through a graphical button interface, evaluates them correctly — respecting operator precedence, brackets, and mathematical functions — and displays the result in real time.

Below is a breakdown of the key components and how they work together:

---

### Expression Input and Display

- Responsible for capturing button presses and building a string representation of the mathematical expression
- The display shows the current expression as it is typed, then the result upon evaluation
- Handles edge cases such as consecutive operators, leading negatives, implicit multiplication (e.g. `2(3+4)`), and clearing/backspace operations

---

### Expression Parsing and Evaluation

- The core computational component of the calculator
- Uses a **hand-written recursive-descent parser** that evaluates expressions directly to a `double` result as it parses — no intermediate representation such as an AST or RPN token list is produced
- Operator precedence is enforced structurally by the depth of the function call chain across three mutually-recursive functions: `parse_expression()`, `parse_term()`, and `parse_factor()`
- The implicit C call stack acts as the precedence mechanism — there are no explicit stacks, queues, or token arrays

**The three-level grammar:**
| Level | Function | Operators handled | Precedence |
|-------|----------|-------------------|------------|
| Lowest | `parse_expression()` | `+`, `−` | Lowest |
| Middle | `parse_term()` | `×`, `÷`, `^`, `!`, implicit multiply | Medium |
| Highest | `parse_factor()` | numbers, variables, functions, `(…)`, unary `−`/`+` | Highest |

**Key formulas and mathematical functions supported:**
- Basic arithmetic: addition (`+`), subtraction (`−`), multiplication (`×`), division (`÷`)
- Exponentiation: `xʸ` using C's `pow(base, exponent)` from `math.h`
- Square root: `√x` using `sqrt(x)`
- Trigonometric functions: `sin(x)`, `cos(x)`, `tan(x)` — with degree/radian conversion depending on the current angle mode setting
- Logarithms: `log(x)` (base-10 via `log10()`), `ln(x)` (natural log via `log()`)
- Factorial: `n!` computed iteratively as `n! = n × (n−1) × … × 1`, with guards for non-integer and overflow inputs
- Constants: `π` (pi ≈ 3.14159265358979) and `e` (Euler's number ≈ 2.71828182845905), matched by name in the parser
- Floor, ceiling, and absolute value functions
- Implicit multiplication: expressions such as `2pi`, `3sin(x)`, and `2(x+1)` are handled without an explicit `×` operator
- Variable substitution: `x`, `y`, `z`, and `A`–`F` are looked up from a settings variable store at parse time, enabling graph plotting and the equation solver

---

### Data Structures

- **`Parser` struct**: A small struct holding a pointer to the expression string and an integer position index (`pos`). Stack-allocated once per evaluation call, it acts as a stateful cursor that advances through the input as characters are consumed. No heap allocation is used.
- **Implicit call stack**: The C runtime call stack itself encodes operator precedence — each nested call to `parse_expression()`, `parse_term()`, or `parse_factor()` represents a deeper precedence level. This replaces the need for an explicit operator stack.
- **String buffer**: The expression is stored as a character array (C string) that is built up incrementally as the user presses buttons, then passed directly to the parser.
- **History array**: A fixed-size circular array of 10 `HistoryEntry` structs, each storing a past expression string and its result, used to display calculation history.
- **Function array**: Up to 4 `Function` structs, each holding an equation string and an enabled flag, used by the graph plotter to plot multiple functions simultaneously.

---

### Graphical User Interface (GUI)

- Built using **LVGL (Light and Versatile Graphics Library)**, a free and open-source embedded graphics library written in C
- The GUI renders calculator buttons arranged in a grid layout, a display label for the expression and result, and a secondary label for mode indicators (e.g. DEG/RAD)
- On the **desktop**, LVGL runs on top of an SDL2 backend for window management and rendering
- On the **ESP32**, LVGL drives a connected TFT display directly via SPI, using the ESP-IDF display driver
- Button press events are handled through LVGL's event callback system, which calls the input handler to update the expression string

---

### Cross-Platform Architecture

- A shared `src/` directory contains all platform-independent logic: the expression parser, evaluator, and UI layout code
- Platform-specific code (display driver initialisation, input device setup) is isolated in `desktop/` and `esp32/` directories respectively
- Separate build scripts (`build-windows.bat` for desktop, `build-esp32.sh` for embedded) compile the appropriate combination of files for each target
- This architecture ensures that the core calculator logic is written and tested once and reused across both platforms

---

### Real-Time Considerations

- The calculator must respond to button presses with minimal latency, particularly on the ESP32 where CPU speed is lower than a desktop processor
- Expression evaluation is intentionally kept lightweight — the recursive-descent parser runs in O(n) time relative to the length of the expression, ensuring fast response even on constrained hardware
- The LVGL task handler (`lv_timer_handler`) is called in a main loop at a regular interval to ensure the GUI remains responsive and redraws efficiently
- Memory usage is kept minimal by using fixed-size stack arrays and avoiding heap allocation in the core evaluation logic

---

### Summary of Tools and Technologies

| Component | Tool / Technology |
|---|---|
| Primary language | C (C99) |
| GUI framework | LVGL v9 |
| Desktop rendering backend | SDL2 |
| Embedded target | ESP32 (ESP-IDF toolchain) |
| Desktop build system | MinGW / GCC (Windows) |
| Expression parsing algorithm | Recursive-descent parser |
| Mathematical functions | C standard `math.h` library |
| Version control | Git |
