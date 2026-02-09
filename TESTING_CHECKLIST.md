# Calculator Testing Checklist

## Pre-Test Setup
- [ ] Build desktop application: `cd desktop && cmake --build build`
- [ ] Copy `button_keycodes.txt` to same directory as `main.exe` if needed
- [ ] Run: `./build/main.exe` (or `build\main.exe` on Windows)
- [ ] Verify two windows open: Main Display (320x240) and Keypad (320x270)

---

## 1. Button Label Display (CRITICAL - Was Main Bug)

### Expected Behavior
Button labels should display functional names, NOT SDL keycode names.

**BEFORE FIX (WRONG):**
- HOME, END, ESCAPE, TAB, DELETE showing as literal text

**AFTER FIX (CORRECT):**
- Row 0: `SHIFT`, `ALPHA`, `<`, `^`, `MENU`, `ON`
- Row 1: `VAR`, `CALC`, `v`, `>`, `CONST`, `EXP`
- Row 2: `FRAC`, `sqrt`, `x^2`, `^`, `log`, `ln`
- Row 3: `!`, `(`, `)`, `sin`, `cos`, `tan`
- Row 4: `A`, `B`, `C`, `D`, `E`, `F`
- Row 5: `x`, `7`, `8`, `9`, `DEL`, `AC`
- Row 6: `y`, `4`, `5`, `6`, `*`, `/`
- Row 7: `z`, `1`, `2`, `3`, `+`, `-`
- Row 8: `e`, `0`, `.`, `π` (or box), `Ans`, `=`

### Test Cases
- [ ] **Button labels** - All buttons show correct labels (not HOME/END/TAB/etc.)
- [ ] **Pi symbol** - Row 8, Col 3 shows either `π` or a white box (known issue)
- [ ] **ASCII operators** - `*`, `/`, `+`, `-` display correctly (not ×, ÷)
- [ ] **Lowercase variables** - `x`, `y`, `z` are lowercase (not X, Y, Z)
- [ ] **Function names** - `sqrt`, `sin`, `cos`, `tan`, `ln`, `log` are readable

---

## 2. Function Key Input

### Test Cases
- [ ] **sin button** - Pressing `s` key inserts `sin(` in input field
- [ ] **cos button** - Pressing `TAB` key (mapped to `C`) inserts `cos(`
- [ ] **tan button** - Pressing `t` key inserts `tan(`
- [ ] **sqrt button** - Pressing `r` key inserts `sqrt(`
- [ ] **ln button** - Pressing `l` key inserts `ln(`
- [ ] **log button** - Pressing `g` key inserts `log(`
- [ ] **abs button** - Pressing `a` key inserts `abs(`
- [ ] **floor button** - Pressing `f` key inserts `floor(`
- [ ] **ceil button** - Pressing `c` key inserts `ceil(`

### Expected Behavior
Each function key should insert the full function name with opening parenthesis, NOT just a single letter.

---

## 3. Key Mappings

### Special Keys
- [ ] **DELETE key** - Acts as AC (clear all), sends `LV_KEY_ESC`
- [ ] **ESCAPE key** - Types `e` constant (Euler's number), not clear
- [ ] **TAB key** - Types `cos(` function
- [ ] **HOME key** - Types `π` (pi value)
- [ ] **END key** - Types `Ans` (last answer)
- [ ] **RETURN/ENTER** - Calculates result

### Variable Keys
- [ ] **x key** - Types lowercase `x` (not uppercase `X`)
- [ ] **y key** - Types lowercase `y`
- [ ] **z key** - Types lowercase `z`

---

## 4. Basic Arithmetic

### Test Cases
| Expression | Expected Result | Pass/Fail |
|------------|-----------------|-----------|
| `2+3` | `5` | |
| `10-7` | `3` | |
| `4*5` | `20` | |
| `20/4` | `5` | |
| `2^8` | `256` | |
| `1/0` | `inf` or error | |
| `-5` | `-5` | |
| `+7` | `7` | |

---

## 5. Unary Operators (NEW FEATURE)

### Test Cases
| Expression | Expected Result | Notes |
|------------|-----------------|-------|
| `-5` | `-5` | Unary minus |
| `-(2+3)` | `-5` | Negate expression |
| `-5*2` | `-10` | Unary on factor |
| `+3` | `3` | Unary plus |
| `+(2+3)` | `5` | Unary plus on expr |

---

## 6. Factorial (NEW FEATURE)

### Test Cases
| Expression | Expected Result | Notes |
|------------|-----------------|-------|
| `5!` | `120` | Basic factorial |
| `0!` | `1` | Edge case |
| `1!` | `1` | Edge case |
| `3!` | `6` | Small number |
| `(2+3)!` | `120` | Factorial on expression |
| `(-5)!` | `nan` | Negative not allowed |
| `3.5!` | `nan` | Non-integer not allowed |
| `171!` | `inf` | Overflow protection |

---

## 7. Constants (NEW FEATURE)

### Test Cases
| Expression | Expected Result (Approx) | Notes |
|------------|--------------------------|-------|
| `pi` | `3.14159` | Pi constant |
| `e` | `2.71828` | Euler's number |
| `2*pi` | `6.28318` | Using pi |
| `pi/2` | `1.5708` | Division with pi |
| `e^1` | `2.71828` | e to power |
| `ln(e)` | `1` | Natural log of e |

---

## 8. Implicit Multiplication (NEW FEATURE)

### Test Cases
| Expression | Expected Result | Notes |
|------------|-----------------|-------|
| `2pi` | `6.28318` | Number * constant |
| `3x` (x=2) | `6` | Number * variable |
| `(2)(3)` | `6` | Parentheses |
| `2(3+1)` | `8` | Number * expression |
| `3(2x)` (x=1) | `6` | Nested implicit mult |
| `2sin(0)` | `0` | Number * function |
| `pi2` | `6.28318` | Constant * number |

### Notes
- Implicit multiplication should work WITHOUT needing to type `*`
- This is standard mathematical notation

---

## 9. Trigonometric Functions

### Test Cases
| Expression | Expected Result | Notes |
|------------|-----------------|-------|
| `sin(0)` | `0` | Basic sin |
| `sin(pi/2)` | `1` | Sin of 90° |
| `sin(pi)` | `0` | **CRITICAL** - Should be `0`, not `1.2e-16` |
| `cos(0)` | `1` | Basic cos |
| `cos(pi)` | `-1` | Cos of 180° |
| `tan(0)` | `0` | Basic tan |
| `sin(pi/6)` | `0.5` | Sin of 30° |

### Notes
- **sin(pi) MUST display as `0`** - This tests the rounding fix in math.c:53-59
- All trig functions expect radians, not degrees

---

## 10. Logarithmic Functions (NEW FEATURE)

### Test Cases
| Expression | Expected Result | Notes |
|------------|-----------------|-------|
| `ln(e)` | `1` | Natural log of e |
| `ln(1)` | `0` | ln(1) = 0 |
| `ln(2.718)` | `~1` | Approx ln(e) |
| `log(10)` | `1` | Base-10 log |
| `log(100)` | `2` | log10(100) |
| `log(1000)` | `3` | log10(1000) |
| `ln(0)` | `-inf` | Log of zero |
| `ln(-5)` | `nan` | Log of negative |

---

## 11. Root and Power Functions

### Test Cases
| Expression | Expected Result | Notes |
|------------|-----------------|-------|
| `sqrt(4)` | `2` | Basic square root |
| `sqrt(16)` | `4` | Square root |
| `sqrt(2)` | `1.41421` | Irrational result |
| `sqrt(-1)` | `nan` | Negative not allowed |
| `2^3` | `8` | Power function |
| `2^0` | `1` | Power zero |
| `2^-1` | `0.5` | Negative power |

---

## 12. Rounding Functions (NEW FEATURE)

### Test Cases
| Expression | Expected Result | Notes |
|------------|-----------------|-------|
| `abs(-5)` | `5` | Absolute value |
| `abs(5)` | `5` | Absolute positive |
| `abs(0)` | `0` | Absolute zero |
| `floor(3.7)` | `3` | Round down |
| `floor(3.2)` | `3` | Round down |
| `floor(-2.3)` | `-3` | Round down negative |
| `ceil(2.1)` | `3` | Round up |
| `ceil(2.9)` | `3` | Round up |
| `ceil(-2.9)` | `-2` | Round up negative |

---

## 13. Complex Expressions

### Test Cases
| Expression | Expected Result | Notes |
|------------|-----------------|-------|
| `2+3*4` | `14` | Order of operations |
| `(2+3)*4` | `20` | Parentheses override |
| `2^3^2` | `512` | Right associativity (2^9) |
| `sin(pi/2)+cos(0)` | `2` | Multiple functions |
| `2pi+e` | `9.00145` | Multiple constants |
| `3!+2!` | `8` | Multiple factorials |
| `-2^2` | `-4` | Unary with power |
| `2*3!` | `12` | Multiplication and factorial |

---

## 14. Backspace/Delete Behavior

### Test Cases
- [ ] **DEL on number** - Deletes single character (e.g., `12|3` → `12|`)
- [ ] **DEL on `sin(`** - Deletes entire function name (e.g., `sin(|` → `|`)
- [ ] **DEL on `cos(`** - Deletes entire function name
- [ ] **DEL on `sqrt(`** - Deletes entire function name
- [ ] **DEL on `floor(`** - Deletes entire function name (5 chars)
- [ ] **AC button (DELETE key)** - Clears entire input and result

### Expected Behavior
Function names should be deleted as a unit, not character by character.

---

## 15. Answer (Ans) Button

### Test Cases
- [ ] Calculate `2+3`, result is `5`
- [ ] Press `Ans`, should insert `5` into next calculation
- [ ] Calculate `Ans*2`, result should be `10`
- [ ] Verify `Ans` persists across multiple calculations

---

## 16. Error Handling

### Test Cases
| Expression | Expected Behavior | Notes |
|------------|-------------------|-------|
| `1/0` | Display `inf` or error | Division by zero |
| `sqrt(-1)` | Display `nan` | Invalid argument |
| `ln(-5)` | Display `nan` | Invalid argument |
| `5!!` | Should work or error gracefully | Double factorial |
| `sin(` | No crash, wait for closing paren | Incomplete expression |
| `2+` | No crash, wait for second operand | Incomplete expression |
| Empty input → `=` | No crash, blank or zero | Edge case |

---

## 17. Visual/UI Checks

### Display Window
- [ ] Result displays with appropriate precision (not excessive decimals)
- [ ] Very small numbers rounded to zero (< 1e-10)
- [ ] `inf` displays for infinity results
- [ ] `nan` displays for invalid results
- [ ] Expression history visible and scrollable
- [ ] Cursor visible in input field

### Keypad Window
- [ ] All 54 buttons visible (9 rows × 6 columns)
- [ ] Button labels readable (not too small)
- [ ] Buttons respond to mouse clicks
- [ ] Buttons respond to keyboard input
- [ ] No visual glitches or overlapping labels

---

## 18. Known Issues to Verify

### Issue #1: Pi Symbol Display
**Status:** Known issue - font doesn't include Greek letters

**Expected Behaviors:**
- Option A: Pi button shows white box (Unicode not in font)
- Option B: Pi button shows "π" UTF-8 text
- Option C: (If fixed) Pi button shows actual π glyph

**What to check:**
- [ ] Pi button is visible and clickable (even if showing as box)
- [ ] Pressing pi button types "pi" constant correctly
- [ ] Calculation with `pi` works (e.g., `2*pi` = `6.28318`)

### Issue #2: Case Sensitivity
**Fixed - Verify:**
- [ ] Lowercase letters (`x`, `y`, `z`) type as lowercase
- [ ] Function keys (like `s` for sin) still work despite case preservation
- [ ] No uppercase conversion breaking functionality

---

## Priority Test Summary

### CRITICAL (Must Pass)
1. Button labels display correctly (not SDL keycode names)
2. `sin(pi)` displays as `0` (not `1.2e-16`)
3. Function keys insert full function names (not single letters)
4. AC button (DELETE key) clears input

### HIGH PRIORITY (Should Pass)
5. All new functions work: `ln()`, `log()`, `abs()`, `floor()`, `ceil()`
6. Factorial works: `5!` = `120`
7. Implicit multiplication: `2pi` = `6.28318`
8. Constants work: `pi` and `e` evaluate correctly
9. Unary operators: `-5`, `+(2+3)` work

### MEDIUM PRIORITY (Nice to Have)
10. Pi symbol displays correctly (or shows acceptable placeholder)
11. Complex expressions with multiple features
12. Error handling for edge cases

---

## Regression Testing

### Previously Working Features (Don't Break)
- [ ] Basic arithmetic (+, -, *, /)
- [ ] Power operator (^)
- [ ] Parentheses grouping
- [ ] Original trig functions (sin, cos, tan)
- [ ] sqrt function
- [ ] Navigation (UP/DOWN arrows for history)
- [ ] Menu navigation (M key to return to main menu)

---

## Test Execution Notes

### How to Test
1. Open calculator application
2. Click Math mode (should be default)
3. Test each category sequentially
4. Mark Pass/Fail in checklist
5. Note any unexpected behavior

### What to Report
- **Pass:** Feature works as expected
- **Fail:** Feature doesn't work, note actual behavior
- **Partial:** Feature mostly works but has minor issues
- **Unknown:** Unable to test (e.g., can't compile)

### Common Issues
- If buttons show SDL names: Main fix didn't apply correctly
- If functions type single letters: Key handler not updated
- If pi shows box: Expected, font doesn't have Greek letters
- If sin(pi) shows 1.2e-16: Rounding fix not applied

---

## Expected Test Duration
- **Quick Test:** 10-15 minutes (critical tests only)
- **Full Test:** 30-45 minutes (all categories)
- **Thorough Test:** 1+ hour (including edge cases and regression)

---

## After Testing

### If All Tests Pass
Ready for next feature additions or deployment to ESP32.

### If Tests Fail
Note which specific tests failed and report:
1. Test name/category
2. Input expression
3. Expected result
4. Actual result
5. Any error messages

### Next Steps After Testing
1. Document any bugs found
2. Prioritize fixes (critical vs minor)
3. Consider pi symbol solution (custom font vs text label)
4. Plan next features (degrees/radians toggle, more functions, etc.)
