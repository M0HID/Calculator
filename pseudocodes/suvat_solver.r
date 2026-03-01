SUBROUTINE solve_suvat(suvat_values)
    constant MAX_ITERATIONS ← 10
    constant EPSILON ← 0.0000000001
    SUVAT_UNKNOWN ← -999999.0
    
    s ← suvat_values.s
    u ← suvat_values.u
    v ← suvat_values.v
    a ← suvat_values.a
    t ← suvat_values.t
    
    FOR iteration ← 0 TO MAX_ITERATIONS
        known_before ← count_known_values(s, u, v, a, t)
        
        IF known_before = 5 THEN
            RETURN (s, u, v, a, t, True)
        ENDIF
        
        IF u ≠ SUVAT_UNKNOWN AND a ≠ SUVAT_UNKNOWN AND t ≠ SUVAT_UNKNOWN AND v = SUVAT_UNKNOWN THEN
            v ← u + a * t
        ENDIF
        
        IF v ≠ SUVAT_UNKNOWN AND a ≠ SUVAT_UNKNOWN AND t ≠ SUVAT_UNKNOWN AND u = SUVAT_UNKNOWN THEN
            u ← v - a * t
        ENDIF
        
        IF u ≠ SUVAT_UNKNOWN AND v ≠ SUVAT_UNKNOWN AND a ≠ SUVAT_UNKNOWN AND t = SUVAT_UNKNOWN AND ABS(a) > EPSILON THEN
            t ← (v - u) / a
        ENDIF
        
        IF u ≠ SUVAT_UNKNOWN AND v ≠ SUVAT_UNKNOWN AND t ≠ SUVAT_UNKNOWN AND a = SUVAT_UNKNOWN AND ABS(t) > EPSILON THEN
            a ← (v - u) / t
        ENDIF
        
        IF u ≠ SUVAT_UNKNOWN AND a ≠ SUVAT_UNKNOWN AND t ≠ SUVAT_UNKNOWN AND s = SUVAT_UNKNOWN THEN
            s ← u * t + 0.5 * a * t * t
        ENDIF
        
        IF u ≠ SUVAT_UNKNOWN AND v ≠ SUVAT_UNKNOWN AND t ≠ SUVAT_UNKNOWN AND s = SUVAT_UNKNOWN THEN
            s ← (u + v) * t / 2.0
        ENDIF
        
        IF u ≠ SUVAT_UNKNOWN AND a ≠ SUVAT_UNKNOWN AND s ≠ SUVAT_UNKNOWN AND v = SUVAT_UNKNOWN THEN
            v_squared ← u * u + 2.0 * a * s
            IF v_squared ≥ 0 THEN
                v ← SQRT(v_squared)
            ENDIF
        ENDIF
        
        IF v ≠ SUVAT_UNKNOWN AND a ≠ SUVAT_UNKNOWN AND s ≠ SUVAT_UNKNOWN AND u = SUVAT_UNKNOWN THEN
            u_squared ← v * v - 2.0 * a * s
            IF u_squared ≥ 0 THEN
                u ← SQRT(u_squared)
            ENDIF
        ENDIF
        
        IF u ≠ SUVAT_UNKNOWN AND v ≠ SUVAT_UNKNOWN AND s ≠ SUVAT_UNKNOWN AND a = SUVAT_UNKNOWN AND ABS(s) > EPSILON THEN
            a ← (v * v - u * u) / (2.0 * s)
        ENDIF
        
        IF u ≠ SUVAT_UNKNOWN AND v ≠ SUVAT_UNKNOWN AND a ≠ SUVAT_UNKNOWN AND s = SUVAT_UNKNOWN AND ABS(a) > EPSILON THEN
            s ← (v * v - u * u) / (2.0 * a)
        ENDIF
        
        known_after ← count_known_values(s, u, v, a, t)
        
        IF known_after = known_before THEN
            RETURN (s, u, v, a, t, False)
        ENDIF
    ENDFOR
    
    RETURN (s, u, v, a, t, False)
ENDSUBROUTINE


SUBROUTINE count_known_values(s, u, v, a, t)
    SUVAT_UNKNOWN ← -999999.0
    count ← 0
    
    IF s ≠ SUVAT_UNKNOWN THEN
        count ← count + 1
    ENDIF
    IF u ≠ SUVAT_UNKNOWN THEN
        count ← count + 1
    ENDIF
    IF v ≠ SUVAT_UNKNOWN THEN
        count ← count + 1
    ENDIF
    IF a ≠ SUVAT_UNKNOWN THEN
        count ← count + 1
    ENDIF
    IF t ≠ SUVAT_UNKNOWN THEN
        count ← count + 1
    ENDIF
    
    RETURN count
ENDSUBROUTINE
