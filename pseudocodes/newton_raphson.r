SUBROUTINE newton_raphson(expression, initial_guess)
    # Solves f(x) = 0 using Newton-Raphson iterative method
    # Uses tangent line approximation: x_next = x - f(x)/f'(x)
    
    x ← initial_guess
    MAX_ITER ← 100
    TOLERANCE ← 0.00000001
    DERIVATIVE_STEP ← 0.000001
    
    FOR i ← 0 TO MAX_ITER
        # Evaluate function at current x
        f_x ← evaluate_expression(expression, x)
        
        # Check if we've converged (f(x) ≈ 0)
        IF ABS(f_x) < TOLERANCE THEN
            RETURN (x, i+1, True)
        ENDIF
        
        # Calculate derivative using central difference method
        f_prime_x ← numerical_derivative(expression, x, DERIVATIVE_STEP)
        
        # Check if derivative is too small (would cause division by zero)
        IF ABS(f_prime_x) < 0.000000000001 THEN
            RETURN (x, i, False)
        ENDIF
        
        # Newton-Raphson update step
        x ← x - (f_x / f_prime_x)
        
    ENDFOR
    
    # Max iterations reached without convergence
    RETURN (x, MAX_ITER, False)
ENDSUBROUTINE


SUBROUTINE numerical_derivative(expression, x, h)
    # Approximates f'(x) using central difference method
    # More accurate than forward or backward difference
    
    # Evaluate at x + h
    f_plus ← evaluate_expression(expression, x + h)
    
    # Evaluate at x - h
    f_minus ← evaluate_expression(expression, x - h)
    
    # Central difference formula: f'(x) ≈ [f(x+h) - f(x-h)] / 2h
    derivative ← (f_plus - f_minus) / (2 * h)
    
    RETURN derivative
ENDSUBROUTINE


SUBROUTINE evaluate_expression(expression, x_value)
    # Substitutes x with x_value and evaluates the expression
    # Uses the recursive descent parser from Algorithm 1
    
    # Replace variable 'x' with the numeric value
    substituted_expr ← replace_variable(expression, 'x', x_value)
    
    # Parse and evaluate using expression parser
    result ← eval_expression(substituted_expr)
    
    RETURN result
ENDSUBROUTINE


SUBROUTINE ABS(value)
    # Returns absolute value of a number
    IF value < 0 THEN
        RETURN -value
    ELSE
        RETURN value
    ENDIF
ENDSUBROUTINE
