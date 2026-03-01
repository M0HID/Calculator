SUBROUTINE add_to_history(equation, result)
    constant MAX_HISTORY ← 10
    
    IF history_count < MAX_HISTORY THEN
        history[history_count].equation ← equation
        history[history_count].result ← result
        history_count ← history_count + 1
    ELSE
        FOR i ← 0 TO MAX_HISTORY - 2
            history[i].equation ← history[i + 1].equation
            history[i].result ← history[i + 1].result
        ENDFOR
        
        history[MAX_HISTORY - 1].equation ← equation
        history[MAX_HISTORY - 1].result ← result
    ENDIF
ENDSUBROUTINE


SUBROUTINE get_history_entry(index)
    IF index < 0 OR index >= history_count THEN
        RETURN NULL
    ENDIF
    
    RETURN history[index]
ENDSUBROUTINE


SUBROUTINE clear_history()
    history_count ← 0
ENDSUBROUTINE
