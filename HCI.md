# Human Computer Interface

## 1. Virtual Keypad

The virtual keypad is displayed in a separate window below the main calculator display. It consists of a 9-row by 6-column grid of rounded rectangular buttons on a dark grey background, designed to simulate the physical button matrix that would be present on the hardware device. Each button is labelled with its function and styled with a slightly lighter grey fill and a thin border to give them a tactile, physical feel. The keypad is split into functional zones: the top two rows contain navigation and mode keys such as SHIFT, ALPHA, MENU, CONST, and the directional arrow keys arranged in a D-pad formation. Below that are the mathematical function keys including FRAC, √, x², ^, log, ln, sin, cos, and tan. The middle section provides variable keys A–F and the alphanumeric variables x, y, z, and e. The lower section is the numeric keypad with digits 0–9, the decimal point, π, and the standard arithmetic operators. The DEL and AC keys sit prominently in the top-right of the numeric area, and the equals key = is at the bottom-right. Having a separate keypad window means the user can interact with the calculator entirely using mouse clicks, making it easy to test and demonstrate the software on any desktop without needing a physical device.

---

## 2. Main Menu

The main menu screen of the calculator consists of 6 coloured tiles arranged in a 3x2 grid, one for each of the different applications available to the user. Each tile has a distinct accent colour — blue for Math, red for Graph, green for Stats, orange for Solver, purple for Mechanics, and teal for Numerical Methods — making it immediately clear which app is which without needing icons. The currently focused tile is highlighted with a white border and a darkened background to give clear visual feedback about which app the user is about to open. Navigation is done using the arrow keys on the keypad, and the application is launched by pressing the equals key. A hint bar at the bottom of the screen displays the available key bindings, which is helpful for a first-time user who may not know how to navigate the menu.

---

## 3. Math App

The math app consists of a text input box at the bottom of the screen where the user can type the expression they want to evaluate. Above the input box, the live result of the current expression is displayed in a highlighted colour and updates dynamically as the user types, so they do not have to press equals to see the answer. This makes it much faster to check expressions as they are being built up. Above the result is a scrollable history panel that logs all previously entered expressions alongside their results, displayed left-aligned with the result on the right. The user can scroll through this history using the up and down arrow keys. The hint bar at the bottom reminds the user of the key bindings for common operations such as entering the result, clearing the input, and returning to the menu.

---

## 4. Graph — Function List

The graph function list screen is the entry point to the graph app and consists of four rows, one for each of the four functions that can be plotted simultaneously. Each row displays a checkbox on the left to enable or disable the function, a coloured label showing which function it is (y1 in green, y2 in red, y3 in blue, y4 in orange), and the current equation for that function. The user can navigate between rows using the arrow keys, and pressing equals enters edit mode for the selected function, replacing the label with an inline text area that accepts keyboard input. This list format makes it easy to manage multiple functions at once without cluttering the screen. The CALC key switches directly to the graph view, so the user can move between editing and viewing without going back to a separate menu.

---

## 5. Graph — Graph View

The graph view screen renders the plotted functions on a full-screen canvas with a white background. The x and y axes are drawn in dark grey, and a light grey grid is drawn at regular intervals to help the user read off coordinates. Each of the four enabled functions is plotted in its own distinct colour corresponding to the function list — green, red, blue, and orange — so multiple curves can be distinguished at a glance. The user can zoom in and out using the + and − keys, and pan across the graph using the arrow keys. A trace mode can be activated using the VAR key, which places a crosshair cursor on the graph and displays the exact x and y coordinates of the cursor at the top of the screen, updating in real time as the cursor moves left and right. The range of the current viewport is shown at the top left so the user always knows what portion of the graph they are looking at.

---

## 6. Stats — Submenu

The statistics submenu presents the user with a scrollable vertical list of the seven available distribution calculations. Each item in the list is highlighted when selected, with the accent colour providing a visual indicator of focus. The user navigates the list using the up and down arrow keys and selects an item by pressing equals. This list format is used consistently across all submenus in the app, which makes the interface predictable and easy to learn. The hint bar at the bottom shows the key bindings available on this screen.

---

## 7. Stats — Binomial PMF (representative distribution screen)

Each distribution calculation screen follows the same layout. The title of the distribution is shown at the top in the accent colour. Below it are labelled input fields for each parameter, aligned to the right of the screen to give the labels room on the left. The user navigates between fields using the up and down arrow keys, and pressing equals triggers the calculation. The result is displayed below the input fields in a highlighted result colour. This screen shows the Binomial P(X=k) calculator with three input fields: n (number of trials), k (number of successes), and p (probability of success). The Binomial CDF, Normal PDF, Normal CDF, and Inverse Normal screens all follow the same three-field layout with different parameter labels appropriate to each distribution.

---

## 8. Stats — Poisson PMF (two-parameter variant)

The Poisson PMF screen follows the same layout as the distribution screens described above but with only two input fields: k (number of events) and λ (mean). This represents the two-parameter variant of the distribution input screen, compared to the three-parameter screens. The Poisson CDF screen follows the same two-field layout. Showing both variants in the HCI design makes it clear that the number of input fields adapts to the requirements of each distribution rather than always showing the same fixed number of fields.

---

## 9. Solver — Submenu

The solver submenu follows the same list format as the statistics submenu, presenting the user with two options: the linear equation solver and the quadratic equation solver. The accent colour for the solver app is orange, which is applied to the selected item highlight and the title text throughout the solver screens. The user selects a solver using the arrow keys and equals.

---

## 10. Solver — Linear

The linear solver screen displays the equation form ax + b = 0 as a title so the user immediately knows what format to enter their coefficients in. Below it are two input fields labelled a and b. The user fills in the coefficients and presses equals to solve. The result is displayed below the fields in the form x = value, or a message indicating there is no solution if a is zero. The fields can be navigated with the arrow keys and support all the standard mathematical function keys from the keypad.

---

## 11. Solver — Quadratic

The quadratic solver screen follows the same layout as the linear solver but with three input fields for the coefficients a, b, and c of the equation ax² + bx + c = 0, shown as the screen title. After solving, the result section can display up to two roots, shown as x1 and x2 on separate lines. If there is only one repeated root it is shown as a single value, and if there are no real solutions an appropriate message is displayed instead. This gives the user clear and unambiguous feedback regardless of the nature of the equation they have entered.

---

## 12. Mechanics — SUVAT Input

The mechanics app input screen presents five labelled input fields, one for each of the SUVAT variables: s (displacement in metres), u (initial velocity in m/s), v (final velocity in m/s), a (acceleration in m/s²), and t (time in seconds). The user fills in at least three of the five fields and leaves the unknowns blank, indicated by a question mark placeholder in each empty field. Pressing equals triggers the solver, which fills in all remaining unknowns. If fewer than three values are entered, an error message is shown below the fields reminding the user that at least three values are required. The field labels use superscript notation for units such as m/s² to make them easier to read.

---

## 13. Mechanics — SUVAT Results

After a successful calculation, the results screen replaces the input fields with a full readout of all five SUVAT values, showing the label and solved value for each on its own line. Values that were entered by the user and values that were calculated by the solver are displayed in the same format, so the user can see the complete picture of the motion problem at a glance. At the bottom of the screen, the four SUVAT equations are printed as a reference — v=u+at, s=ut+½at², v²=u²+2as, and s=(u+v)t/2 — which is useful for a student who wants to verify which equation was used. Pressing AC returns the user to the input screen with the previously entered values still filled in so they can modify them without starting from scratch.

---

## 14. Numerical Methods — Submenu

The numerical methods submenu follows the same list format as the other submenus and presents three options: Curve Fitting, Numerical Integration, and Newton-Raphson. The accent colour for this app is teal. The user selects a method using the arrow keys and equals.

---

## 15. Numerical Methods — Newton-Raphson

The Newton-Raphson screen has two input fields: one for the function f(x) and one for the initial guess x0. The function field accepts any valid expression in terms of x and supports all the mathematical function keys on the keypad such as sin, cos, sqrt, ln, and ^. After pressing equals, the solver iterates using the Newton-Raphson method and displays the root at the bottom of the screen along with the number of iterations it took to converge. If the solver does not converge within the maximum number of iterations, it displays a no convergence message alongside the last approximation. The Curve Fitting and Numerical Integration screens follow the same input-then-result layout, with Curve Fitting taking comma-separated x and y data lists and Numerical Integration taking a function expression, lower and upper bounds, and a step count.

---

## 16. Settings — Submenu

The settings submenu follows the same list format as all other submenus in the app and presents three options: Angle Unit, Decimal Places, and Variable Editor. The settings accent colour is a muted blue-grey, which distinguishes it visually from the six main apps. The consistent use of the submenu list component across Stats, Solver, Numerical Methods, and Settings means the user only needs to learn one navigation pattern for all menu-style screens in the application.

---

## 17. Settings — Angle Mode

The angle mode screen displays two options — Radians (RAD) and Degrees (DEG) — as a simple selection list. The currently active mode is highlighted with a shaded background. Below the list, the current active setting is shown as a text label so the user can see what is in effect before making a change. Pressing equals confirms the selection and returns to the settings submenu. The Decimal Places screen follows the same selection pattern with a longer list of options from 0 to 10 decimal places plus an Auto option, and the Variable Editor screen follows the same input-field pattern as the solver and mechanics screens, with one field for each of the nine user-defined variables A through F and x, y, z.
