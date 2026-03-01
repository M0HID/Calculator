1.	A solution must be created to provide students with an affordable alternative to current graphing calculators on the market, being able to support all the features needed in the A-level course.
1.1.	The system must support basic mathematical operations (arithmetic, trigonometric etc.)
1.2.	The system must be accurate and precise to at least 6 decimal places
1.3.	Variables and constants (such as x, y, π, e) should be able to be stored in memory
1.4.	The system must operate reliably, giving the correct answer to equations
1.5.	Special characters (π, etc.) should render properly on the display
1.6.	The system should support implicit multiplication (e.g., 2π, 3x)
1.7.	Very small numbers (< 1e-10) should be rounded to zero to avoid floating-point artifacts
2.	The system should be able to manage calculations, graphing and numerical methods efficiently
2.1.	Implement an expression parser capable of evaluating complex mathematical expressions
2.2.	Add a graphing application with the ability to plot at least one graph at a time, with the ability to zoom and pan around
2.3.	Create a visual aid to visualise different numerical methods, e.g. numerical integration, differentiation and solutions to equations
2.4.	Add statistical functions such as probability distributions
2.5.	Calculations must compute within at most 1 second for the majority of inputs
3.	A hardware platform should be used to create a portable standalone device
3.1.	Will run on a raspberry pi or similar microcontroller (ESP32)
3.2.	An LCD display should be used to display both numerical and graphical outputs clearly (320x240 resolution)
3.3.	A physical keypad will be used to control all functions and features of the calculator (9x6 button matrix)
3.4.	The system boots up in at most 2 seconds
3.5.	The system should have a battery life of at least 2 hours
4.	The calculator must provide a comprehensive expression evaluation system to handle complex mathematical calculations with support for multiple functions, operators, and constants.
4.1.	Implement a parser using the shunting yard algorithm that solves expressions according to the correct order of operations
4.2.	Support for unary operators (+ and -)
4.3.	Support for postfix factorial operator (!)
4.4.	Support for mathematical functions: sin, cos, tan, ln, log, sqrt, abs, floor, ceil
4.5.	Support for constants: π (pi) and e (Euler's number)
4.6.	Support for variables: x, y, z
4.7.	Expression history should store up to 10 previous calculations
4.8.	Answer recall (Ans button) should store the last calculated result
4.9.	Smart deletion should remove entire function names (e.g., "sin(") not character-by-character
4.10.	Make sure clear error messages are given for syntax errors or invalid expressions
4.11.	Handle division by zero gracefully (display inf)
4.12.	Handle invalid arguments gracefully (display nan for sqrt(-1), ln(-5), etc.)
5.	The system should provide statistical calculation capabilities to compute probability distributions required for A-level Mathematics.
5.1.	Compute and display probability distributions for binomial distribution
5.1.1.	P(X=k) - Probability mass function
5.1.2.	P(X≤k) - Cumulative distribution function
5.1.3.	Inputs: n (trials), k (successes), p (probability)
5.2.	Compute and display probability distributions for normal distribution
5.2.1.	PDF (Probability Density Function)
5.2.2.	CDF (Cumulative Distribution Function)
5.2.3.	Inverse Normal (find x given probability)
5.2.4.	Inputs: x (value), μ (mean), σ (standard deviation)
5.3.	Allow the user to select between different distribution types
5.4.	Allow the user to select between different function types (PDF/CDF/Inverse)
5.5.	Display results with appropriate precision
6.	The calculator must include a graphing application to visualise mathematical functions with interactive controls for analysis.
6.1.	The system should be able to plot single and multiple functions on the display with clearly labelled axes
6.2.	The system should support up to 4 functions simultaneously
6.3.	Each function should have a different color (Green, Red, Blue, Orange)
6.4.	The graph can be zoomed using + and - keys (5 zoom levels: 0.5x, 1x, 2x, 5x, 10x)
6.5.	The graph can be panned using arrow keys
6.6.	A two-screen interface should be provided: function list screen and graph view screen
6.7.	Functions can be edited, enabled, or disabled from the function list screen
6.8.	Trace mode should allow following curves and displaying exact coordinate values
6.9.	Adjustable viewport (x_min, x_max, y_min, y_max)
6.10.	Dynamic axes rendering with tick marks and grid lines
7.	The system must provide equation solving capabilities for linear, quadratic, and simultaneous equations to support algebraic problem solving.
7.1.	Solve single variable equations including linear polynomials (ax + b = 0)
7.2.	Solve single variable equations including quadratic polynomials (ax² + bx + c = 0)
7.2.1.	Use discriminant method
7.2.2.	Return 0, 1, or 2 real solutions as appropriate
7.3.	Solve simultaneous equations with 2 unknowns (2x2 systems)
7.3.1.	Use Cramer's rule with determinants
7.3.2.	Detect and report parallel/coincident lines
7.4.	Solve simultaneous equations with 3 unknowns (3x3 systems)
7.4.1.	Use Cramer's rule with 3×3 determinants
7.4.2.	Use compact grid UI layout for coefficient input
7.5.	Display all correct solutions clearly on the display
7.6.	Provide appropriate error messages on the display for invalid inputs
8.	The calculator should implement numerical methods to solve equations and perform calculations that cannot be solved algebraically.
8.1.	Be able to solve equations numerically using the Newton-Raphson method
8.1.1.	Use iterative method with convergence check
8.1.2.	Calculate numerical derivative using central difference
8.1.3.	Set max iterations to 100 and tolerance to 1e-8
8.1.4.	Report the number of iterations taken to converge
8.2.	Display in a visual way the processes of numerical differentiation and integration on the display
8.3.	Calculate curve fitting for linear and polynomial fits, being also able to plot this on a graph
8.4.	Allow the user to save and store functions for use throughout the interface
9.	The system must include a mechanics solver to assist with SUVAT kinematics problems commonly encountered in A-Level Physics.
9.1.	Support solving SUVAT kinematics problems for A-Level Physics
9.2.	Variables: s (displacement), u (initial velocity), v (final velocity), a (acceleration), t (time)
9.3.	Accept ANY 3 known values and calculate the remaining 2
9.4.	Use the five SUVAT equations:
9.4.1.	v = u + at
9.4.2.	s = ut + 0.5at²
9.4.3.	v² = u² + 2as
9.4.4.	s = (u+v)t/2
9.4.5.	s = vt - 0.5at²
9.5.	Provide a two-page interface: input page and result page
9.6.	Display all calculated values with appropriate units (m, m/s, m/s², s)
9.7.	Display which equations were used in the calculation
9.8.	Validate that at least 3 inputs are provided before calculating
10.	The calculator should provide an intuitive user interface with quality of life features to ensure efficient navigation and usability across all modes.
10.1.	Main menu for app selection (Math/Graph/Stats/Solver/Mechanics)
10.2.	Button labels should display functional names, not internal key codes
10.3.	Function keys should insert full function names with opening parenthesis (e.g., "sin(")
10.4.	Result formatting with appropriate precision (not excessive decimals)
10.5.	Consistent key mappings across all calculator modes
10.6.	Return to menu functionality (M key)
10.7.	Clear/AC button functionality (DELETE key)
10.8.	Case preservation for variables (x, y, z are lowercase)
10.9.	Expression cursor should be visible in input field
10.10.	Responsive button feedback for both mouse clicks and keyboard input
