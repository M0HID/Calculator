SUBROUTINE detect_implicit_multiplication(parser)
    current ← current_char(parser)
    
    IF current = '(' THEN
        RETURN True
    ENDIF
    
    IF is_letter(current) = True THEN
        RETURN True
    ENDIF
    
    IF current = 'π' THEN
        RETURN True
    ENDIF
    
    next ← peek_next_char(parser)
    IF current = 'e' AND is_letter(next) = False THEN
        RETURN True
    ENDIF
    
    RETURN False
ENDSUBROUTINE
