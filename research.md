## Misc. Operations

Beyond basic arithmetic, the calculator will need to support a range of miscellaneous mathematical operations that are commonly found on scientific calculators. These include:

- **Trigonometric functions**: `sin`, `cos`, `tan` and their inverses (`asin`, `acos`, `atan`)
- **Logarithms**: Natural logarithm `ln(x)` and base-10 logarithm `log(x)`
- **Square root**: `sqrt(x)`
- **Absolute value**: `abs(x)`
- **Floor and ceiling**: `floor(x)` and `ceil(x)`
- **Factorial**: `n!` for non-negative integers
- **Constants**: `π` (pi) and `e` (Euler's number)
- **Powers and exponents**: `x^n` operator for arbitrary powers
- **Ans / Last Answer**: The ability to recall the result of the last calculation, commonly referred to as `Ans`, which is a standard feature on physical calculators

The calculator will also need to support angle modes — specifically degrees and radians — since trigonometric functions behave differently depending on which mode is active. A settings system will need to track the current angle mode and convert inputs accordingly before passing them to the underlying math functions. For example, when in degrees mode, `sin(90)` should return `1`. The conversion is straightforward: multiply the degree value by `π/180` before passing it to the C standard library `sin()` function, which always operates in radians.

Another consideration is **SUVAT mechanics**, which while technically a physics topic, fits naturally into the miscellaneous operations category of a calculator. The five SUVAT equations of motion relate displacement (s), initial velocity (u), final velocity (v), acceleration (a), and time (t):

```
v = u + at
s = ut + ½at²
v² = u² + 2as
s = ½(u + v)t
```

Given any three of the five variables, the remaining two can be solved. The solver can be implemented by iterating over all known equations and progressively filling in unknown values until all five are determined — essentially a constraint propagation approach.

---

## Graph Plotting

To draw a graph on a pixel display, mathematical coordinates need to be mapped to screen coordinates. The screen is defined in pixels with (0,0) at the top-left, whereas a mathematical graph has (0,0) at the centre with y increasing upward. A viewport is maintained using `x_min`, `x_max`, `y_min`, `y_max` values. Any mathematical point (gx, gy) is converted to a pixel (cx, cy) using a linear interpolation:

```
cx = (gx - x_min) / (x_max - x_min) * screen_width
cy = screen_height - (gy - y_min) / (y_max - y_min) * screen_height
```

The y term is subtracted from `screen_height` to flip the axis, since pixel rows increase downward on a display.

To render a curve, the program steps across every pixel column of the screen. For each column x-pixel, it works out the corresponding mathematical x value, evaluates f(x) using the expression parser, converts the result to a y-pixel, and draws a line segment from the previous point to the current one. This produces a connected curve rather than isolated dots. Where the function is undefined or the y value jumps excessively between two adjacent samples (e.g. at a vertical asymptote), the segment is skipped to prevent artefacts.

---

## Equation Solving

### Linear Equations (ax + b = 0)

The simplest form of equation solving is the linear case. Given coefficients a and b, the solution is direct:

```
x = -b / a
```

The edge case where a = 0 must be handled separately, as the equation has no unique solution.

### Quadratic Equations (ax² + bx + c = 0)

For quadratic equations, the standard quadratic formula is used:

```
x = (-b ± √(b² - 4ac)) / (2a)
```

The discriminant `D = b² - 4ac` determines the nature of the roots:

- D > 0: two distinct real roots
- D = 0: one repeated real root
- D < 0: no real roots

The case where a = 0 degenerates to a linear equation and can be handled by the linear solver.

### Newton-Raphson Method

For arbitrary functions where no closed-form solution exists, the **Newton-Raphson method** is used. This is an iterative numerical technique that starts from an initial guess x₀ and repeatedly refines it:

```
x_(n+1) = x_n - f(x_n) / f'(x_n)
```

Since the user supplies f(x) as a string expression, the derivative f'(x) cannot be computed symbolically. Instead it is approximated numerically using the **central difference formula**:

```
f'(x) ≈ (f(x + h) - f(x - h)) / (2h)
```

This is more accurate than a one-sided difference because the leading error term cancels, giving second-order accuracy O(h²). The iteration continues until `|f(x)|` is below a tolerance of 10⁻⁸, or a maximum iteration count is reached. The user provides the initial guess x₀ and the expression for f(x), and the solver reports the root found and the number of iterations taken.

---

## Numerical Methods

### Solutions of Equations

As described above, Newton-Raphson is the primary numerical technique used to find roots of arbitrary equations. It is well-suited here because the expression evaluator can compute f(x) for any x value, and the numerical derivative can be approximated without requiring the user to supply an analytic derivative. The method converges very quickly (quadratically) when the initial guess is close to the root, but can fail or converge to the wrong root if the starting point is poorly chosen or the derivative is near zero.

### Curve Fitting / Regression

Curve fitting is the process of finding a mathematical function that best fits a set of data points. The most common form is **linear regression**, which finds the best-fit straight line y = mx + c through a dataset using the **least squares method**:

```
m = (n·Σ(xi·yi) − Σxi·Σyi) / (n·Σ(xi²) − (Σxi)²)
c = (Σyi − m·Σxi) / n
```

where n is the number of data points. This is a direct algebraic computation with no iteration required. Curve fitting is a planned feature for the solver module, where the user would enter a set of data points and choose the regression type to apply.

### Numerical Integration

Numerical integration is used to compute definite integrals of functions that have no analytic antiderivative. The chosen method is **Simpson's Rule** (composite form), which approximates the integral by fitting parabolic segments over pairs of subintervals:

```
∫ from a to b of f(x) dx ≈ (h/3) * [f(x₀) + 4f(x₁) + 2f(x₂) + 4f(x₃) + ... + 2f(xₙ₋₂) + 4f(xₙ₋₁) + f(xₙ)]
```

where `h = (b − a) / n` and n must be an even number of subintervals. The coefficients follow the pattern 1, 4, 2, 4, 2, ..., 4, 1 — interior odd-indexed points are multiplied by 4 and interior even-indexed points by 2.

Simpson's Rule achieves fourth-order accuracy O(h⁴), making it significantly more efficient than the Trapezoid Rule O(h²) for smooth functions. For a given number of steps, a much smaller error is produced, meaning fewer subintervals are needed to reach the same precision. The user provides the expression f(x), the lower bound a, the upper bound b, and the number of steps n. The calculator enforces that n is even (incrementing by 1 if not) and caps it at 10,000 to prevent excessive computation on the microcontroller.

For the statistics module specifically, an approximation of the error function (`erf`) is used instead of runtime numerical integration, as this avoids the overhead of a general-purpose integrator on a resource-constrained device.
