# SCREEN DIAGRAM PROMPTS - SCIENTIFIC CALCULATOR UI

Each section below is a standalone prompt you can give to Gemini to generate a screenshot mockup of that specific screen. The display is **320×240 pixels**, **black and white only** — no colour whatsoever. Use black, white, and shades of grey to differentiate elements. All screens have a thin **hint bar at the bottom** (~14px tall, light grey background) showing keyboard shortcuts in small dark grey text. Think Casio calculator aesthetic — clean, minimal, monochrome.

---

## SCREEN 1: Main Menu

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Main Menu Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White (#FFFFFF)

**Layout**: 3 columns × 2 rows grid of app launcher buttons, centered on screen. Each button is 80×65 pixels with 20px horizontal spacing and 18px vertical spacing. The entire grid is centered both horizontally and vertically (above the hint bar).

**Button grid (left to right, top to bottom):**

Row 1:
- Button 1: "Math" — dark grey fill (#333333), rounded corners (radius 6), white text centered, medium bold font
- Button 2: "Graph" — dark grey fill, rounded corners, white text centered
- Button 3: "Stats" — dark grey fill, rounded corners, white text centered

Row 2:
- Button 4: "Solver" — dark grey fill, rounded corners, white text centered
- Button 5: "Mech." — dark grey fill, rounded corners, white text centered
- Button 6: "Num.\nMethods" — dark grey fill, rounded corners, white text centered (two-line label, text-wrapped)

**The first button ("Math") should appear slightly highlighted/focused** with a white 3px border outline inside its dark fill, to show keyboard focus.

**Hint bar** at very bottom: thin horizontal strip (~14px tall), light grey background (#E0E0E0), small dark grey text:
`[Enter] Open  [Arrows] Navigate  [CONST] Settings`

**No title bar. No extra decorations. Black and white only. Clean minimalist embedded UI aesthetic.**

---

## SCREEN 2: Math Calculator

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Math Calculator Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White (#FFFFFF)

**Layout from top to bottom:**

1. **History area** (y=14 to y=154): A container with 1px black border, slight padding. Inside are stacked history rows, each 18px tall. Each row has two columns:
   - Left half: equation text in dark grey, small clean font (e.g. "sin(45)+2", "3^2+1", "sqrt(16)")
   - Right half: result text right-aligned in **bold dark grey** (e.g. "2.70711", "10", "4")
   - One selected row has a **light grey highlight background** (#DDDDDD)
   - Show 3–4 filled rows, rest empty

2. **Live result label** (y=148, height=22): Right-aligned **bold** black text, medium font. Shows current result e.g. "= 3.14159"

3. **Input textarea** (y=172, height=34): Full-width minus small margins. White background with 1px grey border and a **thick 2px black left border** (to indicate focus). Placeholder text "Enter expression..." in light grey. Shows a cursor. Current text example: "pi*2"

4. **Hint bar** at bottom: `[=] Enter  [AC] Clear  [M] Menu`

**Color palette**: White background, light grey borders, dark grey equation text, bold black result text, focused textarea has thick black left border.

---

## SCREEN 3: Graph App — Function List Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Graph App - Function List Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White (#FFFFFF)

**Layout**: A black-bordered container fills most of the screen. Inside are 4 function rows stacked vertically, each ~26px tall with 4px spacing.

**Each function row contains (left to right):**
- A small checkbox widget (checked or unchecked, ~16px, black outline)
- A bold label like "y1 =" in black
- The equation text in dark grey

**Function rows:**
- Row 1: checkbox ✓ (filled black tick) | **"y1 ="** bold | "x^2" — **this row has a light grey background (#DDDDDD) to show focus**, with a 2px black border
- Row 2: checkbox ✓ (filled black tick) | **"y2 ="** bold | "sin(x)*3"
- Row 3: checkbox ☐ (empty, black outline) | **"y3 ="** bold | "(empty)" in light grey
- Row 4: checkbox ☐ (empty, black outline) | **"y4 ="** bold | "(empty)" in light grey

**Hint bar** at bottom: `[=] Edit  [AC] Toggle  [CALC] Graph  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 4: Graph App — Graph View Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Graph App - Graph View (Canvas)"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Top info area** (y=0 to y=30, ~30px tall), white background:
- Line 1 (y=2): Small dark grey text: "x:[-10,10] y:[-10,10]"
- Line 2 (y=16): Small grey hint text: "[V] Trace  [+/-] Zoom  [=] Edit  [AC] Back  [M] Menu"

**Canvas area** (y=30 to ~y=210, fills remaining space above hint bar):
- White background
- **Grid lines**: very faint light grey (#DDDDDD), vertical and horizontal lines at every 2 units
- **Axes**: solid medium grey lines. X-axis horizontal at y=0, Y-axis vertical at x=0. Both cross through the centre
- **Function curves:**
  - Curve 1 (y1 = x²): smooth solid black parabola opening upward, 2px line
  - Curve 2 (y2 = sin(x)*3): smooth **dashed** dark grey sine wave, 2px dashed line
- **Trace cursor**: small black filled dot at a specific x position with thin grey crosshair lines

**Hint bar** at bottom: light grey background, small dark grey text.

**Black and white only. No colour.**

---

## SCREEN 5: Stats App — Distribution Menu (Submenu)

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Stats App - Distribution Selection Menu"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White (#FFFFFF)

**Layout**: A vertical list of menu items starting from near the top. Each item is a label row ~24px tall, full-width minus side margins, with a 1px black border, rounded corners (radius 3).

**Menu items (7 total):**
1. "Binomial P(X=k)" — **highlighted**: light grey fill (#DDDDDD), **2px black border**, bold dark grey text
2. "Binomial CDF P(X<=k)"
3. "Normal PDF"
4. "Normal CDF"
5. "Inverse Normal"
6. "Poisson P(X=k)"
7. "Poisson CDF P(X<=k)"

All non-highlighted items: white background, 1px grey border, dark grey text.

**Hint bar** at bottom: `[=] Select  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 6: Stats App — Binomial PMF Input Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Stats App - Binomial P(X=k) Calculation Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White (#FFFFFF)

**Layout from top to bottom:**

1. **Title label** (y=14): **"Binomial P(X=k)"** in bold black, medium font

2. **3 parameter rows** (starting y=36, each ~26px apart):
   - Row 1: small dark grey label "n (trials):" on left | text input field (170px wide, right-aligned), 1px grey border, shows "10"
   - Row 2: small dark grey label "k (successes):" | input field showing "3" — **focused**: 2px solid black border
   - Row 3: small dark grey label "p (prob):" | input field showing "0.5"

3. **Result label** (y~118): **Bold black** text "= 0.11718750", slightly larger font

**Hint bar** at bottom: `[=] Calc  [AC] Back  [M] Menu`

White background, light grey input borders, thick black border on focused field, bold black result text.

**Black and white only. No colour.**

---

## SCREEN 7: Stats App — Normal CDF Input Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Stats App - Normal CDF Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Normal CDF"** in bold black

2. **3 parameter rows:**
   - Row 1: "x (value):" | input field showing "1.96"
   - Row 2: "μ (mean):" | input field showing "0" — **focused** (2px solid black border)
   - Row 3: "σ (std dev):" | input field showing "1"

3. **Result label**: **Bold black** "= 0.97500210"

**Hint bar**: `[=] Calc  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 8: Stats App — Inverse Normal Input Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Stats App - Inverse Normal Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title**: **"Inverse Normal"** in bold black

2. **3 parameter rows:**
   - Row 1: "p (prob):" | input field showing "0.975"
   - Row 2: "μ (mean):" | input field showing "0" — **focused** (2px solid black border)
   - Row 3: "σ (std dev):" | input field showing "1"

3. **Result label**: **Bold black** "= 1.95996398"

**Hint bar**: `[=] Calc  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 9: Stats App — Poisson PMF Input Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Stats App - Poisson P(X=k) Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title**: **"Poisson P(X=k)"** in bold black

2. **2 parameter rows:**
   - Row 1: "λ (mean):" | input field showing "3"
   - Row 2: "k (value):" | input field showing "2" — **focused** (2px solid black border)

3. **Result label**: **Bold black** "= 0.22404181"

**Hint bar**: `[=] Calc  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 10: Solver App — Solver Type Menu (Submenu)

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Solver App - Solver Selection Menu"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White (#FFFFFF)

**Layout**: Vertical list of 2 menu items near the top, each ~24px tall, full-width minus margins, 1px black border, rounded corners.

**Menu items:**
1. "Linear (ax+b=0)" — **highlighted**: light grey fill (#DDDDDD), **2px black border**, bold dark grey text
2. "Quadratic (ax²+bx+c=0)" — white background, 1px grey border, dark grey text

**Hint bar**: `[=] Select  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 11: Solver App — Linear Solver Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Solver App - Linear Equation Solver"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Equation label** (y=14): "ax + b = 0" in dark grey, medium font

2. **2 input rows** (starting y=36):
   - Row 1: label "a:" | input field (200px wide, right side), **2px solid black border**, shows "3" — **focused**
   - Row 2: label "b:" | input field, 1px light grey border, shows "-6"

3. **Result label** (y~88): **Bold black** "x = 2", slightly larger font

**Hint bar**: `[=] Solve  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 12: Solver App — Quadratic Solver Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Solver App - Quadratic Equation Solver"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Equation label** (y=14): "ax² + bx + c = 0" in dark grey

2. **3 input rows:**
   - Row 1: label "a:" | input field showing "1" — **focused** (2px solid black border)
   - Row 2: label "b:" | input field showing "-5"
   - Row 3: label "c:" | input field showing "6"

3. **Result label** (y~114): **Bold black** text:
   ```
   x1 = 3
   x2 = 2
   ```

**Hint bar**: `[=] Solve  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 13: Numerical Methods App — Main Menu (Submenu)

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Numerical Methods App - Selection Menu"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout**: Vertical list of 3 menu items near the top, each ~24px tall, full-width minus margins, 1px black border, rounded corners.

**Menu items:**
1. "Curve Fitting" — **highlighted**: light grey fill (#DDDDDD), **2px black border**, bold dark grey text
2. "Numerical Integration" — white background, 1px grey border, dark grey text
3. "Newton-Raphson" — white background, 1px grey border, dark grey text

**Hint bar**: `[=] Select  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 14: Numerical Methods App — Curve Fitting Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Numerical Methods App - Linear Curve Fitting"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Curve Fitting  y = mx + c"** in bold black

2. **2 input rows:**
   - Row 1: label "x vals:" | wide input field (fills remaining right width), **2px solid black border**, shows "1 2 3 4" — **focused**
   - Row 2: label "y vals:" | wide input field, 1px light grey border, shows "2 4 5 4"

3. **Result area** (~2 lines): **Bold black** text:
   ```
   y = 0.9x + 0.5
   R² = 0.900000  (n=4)
   ```

**Hint bar**: `[=] Fit  [FRAC] Space  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 15: Numerical Methods App — Numerical Integration Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Numerical Methods App - Simpson's Rule Integration"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Numerical Integration (Simpson)"** in bold black

2. **Input rows:**
   - Row 1: label "f(x)=" | wide input field, shows "sin(x)" — **focused** (2px solid black border)
   - Row 2 (split into two halves side by side):
     - Left: label "a=" | short input field showing "0"
     - Right: label "b=" | short input field showing "3.14159"
   - Row 3: label "steps:" | short input field (80px wide) showing "100"

3. **Result label**: **Bold black** "= 2.00000000  (n=100 steps)"

**Hint bar**: `[=] Integrate  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 16: Numerical Methods App — Newton-Raphson Solver Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Numerical Methods App - Newton-Raphson Root Finder"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Newton-Raphson  f(x)=0"** in bold black

2. **Input rows:**
   - Row 1: label "f(x)=" | wide input field, shows "x^2-4" — **focused** (2px solid black border)
   - Row 2: label "x0:" | shorter input field (100px wide, right-aligned), shows "1"

3. **Result label**: **Bold black** "x = 2.00000000  (5 iters)"

**Hint bar**: `[=] Solve  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 17: Mechanics App — SUVAT Input Page

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Mechanics App - SUVAT Input Page"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White (#FFFFFF)

**Layout**: 5 input rows stacked vertically, each ~26px apart. Each row has:
- Left: small dark grey label with variable name and units
- Right: input field (180px wide), 1px grey border, placeholder "?" in light grey

**The 5 rows:**
- Row 1: label "s (m):" | field empty, placeholder "?" shown in light grey
- Row 2: label "u (m/s):" | field showing "10"
- Row 3: label "v (m/s):" | field empty — **focused** (2px solid black border)
- Row 4: label "a (m/s²):" | field showing "2"
- Row 5: label "t (s):" | field empty, placeholder "?"

Below the 5 rows: a small error label area (currently empty)

**Hint bar**: `[Up/Dn] Field  [=] Calculate  [AC] Clear  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 18: Mechanics App — SUVAT Result Page

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Mechanics App - SUVAT Results Page"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout**: 5 result label rows stacked vertically, each ~26px apart. Each row shows a solved SUVAT value in **bold black** text, medium font:

- "s (m) = 150"
- "u (m/s) = 10"
- "v (m/s) = 30"
- "a (m/s²) = 2"
- "t (s) = 10"

Below the 5 rows (~y=150): small secondary text in medium grey showing equations:
```
v=u+at  s=ut+0.5at²
v²=u²+2as  s=(u+v)t/2
```

**Hint bar**: `[AC] Back  [M] Menu`

All 5 result rows in bold black primary font. Equation reference in small secondary font, medium grey.

**Black and white only. No colour.**

---

## SCREEN 19: Settings App — Settings Menu (Submenu)

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Settings App - Settings Menu"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout**: Vertical list of 3 menu items near the top, each ~24px tall, full-width minus margins, 1px black border, rounded corners.

**Menu items:**
1. "Angle Unit (RAD/DEG)" — **highlighted**: light grey fill (#DDDDDD), **2px black border**, bold dark grey text
2. "Decimal Places" — white background, 1px grey border, dark grey text
3. "Variable Editor (A-F, x/y/z)" — white background, 1px grey border, dark grey text

**Hint bar**: `[Up/Dn] Select  [=] Open  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 20: Settings App — Angle Mode Page

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Settings App - Angle Unit Configuration"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Angle Unit"** in bold black

2. **2 option rows** (y=42 and y=70, each 24px tall, full-width minus margins, 1px black border, rounded corners):
   - Row 1: "Radians (RAD)" — **highlighted**: light grey fill (#DDDDDD), 2px black border, bold dark grey text
   - Row 2: "Degrees (DEG)" — white background, 1px grey border, dark grey text

3. **Current setting label** (y=92): small secondary font, medium grey, "Current: RAD"

**Hint bar**: `[Up/Dn] Select  [=] Confirm  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 21: Settings App — Decimal Places Page

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Settings App - Decimal Places Configuration"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Decimal Places"** in bold black

2. **Scrollable list of 11 options** (starting y=36), each 18px tall, 1px grey border, rounded corners:
   - "Auto (smart)" — **highlighted**: light grey fill (#DDDDDD), 2px black border, bold dark grey text
   - "1 decimal place"
   - "2 decimal places"
   - "3 decimal places"
   - "4 decimal places"
   - "5 decimal places"
   - "6 decimal places"
   - "7 decimal places"
   - "8 decimal places"
   - "9 decimal places"
   - "10 decimal places"

All non-highlighted items: white background, 1px grey border, dark grey text, small font.

**Hint bar**: `[Up/Dn] Select  [=] Confirm  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 22: Settings App — Variable Editor Page

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Settings App - Variable Editor"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Variables (A-F, x/y/z)"** in bold black

2. **9 variable input rows** (starting y=36, each ~22px apart):
   - Row 1: label "A:" | input field (200px wide), shows "3.14" — **focused** (2px solid black border)
   - Row 2: label "B:" | input field, shows "0"
   - Row 3: label "C:" | input field, shows "0"
   - Row 4: label "D:" | input field, shows "0"
   - Row 5: label "E:" | input field, shows "2.71"
   - Row 6: label "F:" | input field, shows "0"
   - Row 7: label "x:" | input field, shows "0"
   - Row 8: label "y:" | input field, shows "0"
   - Row 9: label "z:" | input field, shows "0"

**Hint bar**: `[Up/Dn] Field  [=] Save  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 23: Generic Submenu Component Pattern

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Reusable Submenu Component - Generic Example"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, showing the generic submenu component:**

**Background**: White

**Layout**: Vertical list of items starting near the top (y=14), each:
- Width: screen width minus 2× side margins (~296px)
- Height: 24px per item
- Padding: 4px top, 6px left
- Spacing: 26px between item tops
- Rounded corners: radius 3
- 1px black border on each item

**4 example items:**
1. "Option One" — **highlighted**: light grey fill (#DDDDDD), 2px solid black border, bold dark grey text
2. "Option Two" — white background, 1px grey border, dark grey text
3. "Option Three" — white background, 1px grey border, dark grey text
4. "Option Four" — white background, 1px grey border, dark grey text

The highlighted item uses grey fill and a thicker border to show focus — no colour.

**Hint bar**: `[=] Select  [M] Menu`

This component is reused by Stats, Solver, Settings, and Numerical Methods apps. Focus is indicated by grey fill + thick black border only.

**Black and white only. No colour.**

---

## SCREEN 24: Stats App — Binomial CDF Input Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Stats App - Binomial CDF P(X<=k) Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Binomial CDF P(X<=k)"** in bold black

2. **3 parameter rows:**
   - Row 1: "n (trials):" | input field showing "20"
   - Row 2: "k (max):" | input field showing "5" — **focused** (2px solid black border)
   - Row 3: "p (prob):" | input field showing "0.3"

3. **Result label**: **Bold black** "= 0.41637"

**Hint bar**: `[=] Calc  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 25: Stats App — Normal PDF Input Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Stats App - Normal PDF Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Normal PDF"** in bold black

2. **3 parameter rows:**
   - Row 1: "x (value):" | input field showing "0"
   - Row 2: "μ (mean):" | input field showing "0" — **focused** (2px solid black border)
   - Row 3: "σ (std dev):" | input field showing "1"

3. **Result label**: **Bold black** "= 0.39894228"

**Hint bar**: `[=] Calc  [AC] Back  [M] Menu`

**Black and white only. No colour.**

---

## SCREEN 26: Stats App — Poisson CDF Input Screen

### DIAGRAM DESCRIPTION FOR GEMINI:

**Title**: "Stats App - Poisson CDF P(X<=k) Screen"

**Create a pixel-accurate UI mockup of an embedded calculator screen (320×240 pixels), black and white only, with:**

**Background**: White

**Layout:**

1. **Title** (y=14): **"Poisson CDF P(X<=k)"** in bold black

2. **Parameter rows:**
   - Row 1: "λ (mean):" | input field showing "4"
   - Row 2: "k (max):" | input field showing "3" — **focused** (2px solid black border)

3. **Result label**: **Bold black** "= 0.43347"

**Hint bar**: `[=] Calc  [AC] Back  [M] Menu`

**Black and white only. No colour.**
