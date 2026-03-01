SUBROUTINE eval_expression(expr)
    IF expr = NULL OR expr = "" THEN
        RETURN NaN
    ENDIF
    
    parser.str ← expr
    parser.pos ← 0
    
    result ← parse_expression(parser)
    RETURN result
ENDSUBROUTINE

SUBROUTINE parse_expression(parser)
    result ← parse_term(parser)
    
    WHILE current_char = '+' OR current_char = '-'
        operator ← current_char
        parser.pos ← parser.pos + 1
        
        IF operator = '+' THEN
            result ← result + parse_term(parser)
        ELSE IF operator = '-' THEN
            result ← result - parse_term(parser)
        ENDIF
    ENDWHILE
    
    RETURN result
ENDSUBROUTINE

SUBROUTINE parse_term(parser)
    result ← parse_factor(parser)
    
    IF current_char = '!' THEN
        parser.pos ← parser.pos + 1
        result ← factorial(result)
    ENDIF
    
    finished ← False
    WHILE finished = False
        operator ← current_char
        
        IF operator = '*' OR operator = '/' OR operator = '^' THEN
            parser.pos ← parser.pos + 1
            
            IF operator = '*' THEN
                result ← result * parse_factor(parser)
            
            ELSE IF operator = '/' THEN
                divisor ← parse_factor(parser)
                IF divisor ≠ 0 THEN
                    result ← result / divisor
                ELSE
                    RETURN NaN
                ENDIF
            
            ELSE IF operator = '^' THEN
                exponent ← parse_factor(parser)
                result ← result ^ exponent
            ENDIF
        
        ELSE IF current_char = '(' OR is_letter(current_char) = True THEN
            result ← result * parse_factor(parser)
            
            IF current_char = '!' THEN
                parser.pos ← parser.pos + 1
                result ← factorial(result)
            ENDIF
        
        ELSE
            finished ← True
        ENDIF
    ENDWHILE
    
    RETURN result
ENDSUBROUTINE

SUBROUTINE parse_factor(parser)
    
    IF current_char = '-' THEN
        parser.pos ← parser.pos + 1
        RETURN -parse_factor(parser)
    ENDIF
    
    IF current_char = '+' THEN
        parser.pos ← parser.pos + 1
        RETURN parse_factor(parser)
    ENDIF
    
    IF current_char = '(' THEN
        parser.pos ← parser.pos + 1
        result ← parse_expression(parser)
        
        IF current_char = ')' THEN
            parser.pos ← parser.pos + 1
        ENDIF
        
        RETURN result
    ENDIF
    
    IF SUBSTRING(parser.pos, parser.pos + 1, parser.str) = "pi" THEN
        parser.pos ← parser.pos + 2
        RETURN 3.14159265358979
    ENDIF
    
    IF current_char = 'e' THEN
        parser.pos ← parser.pos + 1
        RETURN 2.71828182845905
    ENDIF
    
    IF is_letter(current_char) = True THEN
        func_name ← ""
        WHILE is_letter(current_char) = True
            func_name ← func_name + current_char
            parser.pos ← parser.pos + 1
        ENDWHILE
        
        parser.pos ← parser.pos + 1
        argument ← parse_expression(parser)
        parser.pos ← parser.pos + 1
        
        IF func_name = "sin" THEN
            RETURN SIN(argument)
        ELSE IF func_name = "cos" THEN
            RETURN COS(argument)
        ELSE IF func_name = "tan" THEN
            RETURN TAN(argument)
        ELSE IF func_name = "sqrt" THEN
            RETURN SQRT(argument)
        ELSE IF func_name = "ln" THEN
            RETURN LN(argument)
        ELSE IF func_name = "log" THEN
            RETURN LOG(argument)
        ELSE IF func_name = "abs" THEN
            RETURN ABS(argument)
        ENDIF
    ENDIF
    
    IF is_variable(current_char) = True THEN
        var ← current_char
        parser.pos ← parser.pos + 1
        RETURN get_value(var)
    ENDIF
    
    IF is_digit(current_char) = True OR current_char = '.' THEN
        value ← 0
        decimal_place ← 0
        
        WHILE is_digit(current_char) = True
            value ← value * 10 + digit_value(current_char)
            parser.pos ← parser.pos + 1
        ENDWHILE
        
        IF current_char = '.' THEN
            parser.pos ← parser.pos + 1
            WHILE is_digit(current_char) = True
                decimal_place ← decimal_place + 1
                value ← value + digit_value(current_char) / (10 ^ decimal_place)
                parser.pos ← parser.pos + 1
            ENDWHILE
        ENDIF
        
        RETURN value
    ENDIF
    
    RETURN NaN
ENDSUBROUTINE

SUBROUTINE factorial(n)
    IF n < 0 THEN
        RETURN NaN
    ENDIF
    
    IF n = 0 OR n = 1 THEN
        RETURN 1
    ENDIF
    
    result ← 1
    FOR i ← 2 TO n
        result ← result * i
    ENDFOR
    
    RETURN result
ENDSUBROUTINE
