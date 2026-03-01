SUBROUTINE solve_quadratic(a, b, c)
    EPSILON ← 0.000000000001
    
    IF ABS(a) < EPSILON THEN
        RETURN solve_linear(b, c)
    ENDIF
    
    discriminant ← b * b - 4 * a * c
    
    IF discriminant < 0 THEN
        RETURN (0, NULL, NULL)
    ENDIF
    
    IF ABS(discriminant) < EPSILON THEN
        x1 ← -b / (2 * a)
        x2 ← x1
        RETURN (1, x1, x2)
    ENDIF
    
    sqrt_discriminant ← SQRT(discriminant)
    x1 ← (-b + sqrt_discriminant) / (2 * a)
    x2 ← (-b - sqrt_discriminant) / (2 * a)
    
    RETURN (2, x1, x2)
ENDSUBROUTINE


SUBROUTINE solve_linear(a, b)
    EPSILON ← 0.000000000001
    
    IF ABS(a) < EPSILON THEN
        RETURN (0, NULL)
    ENDIF
    
    x ← -b / a
    RETURN (1, x)
ENDSUBROUTINE


SUBROUTINE SQRT(value)
    IF value < 0 THEN
        RETURN NaN
    ENDIF
    RETURN square_root(value)
ENDSUBROUTINE
