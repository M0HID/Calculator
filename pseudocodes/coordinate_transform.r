SUBROUTINE world_to_screen_x(world_x, x_min, x_max, canvas_width)
    normalized ← (world_x - x_min) / (x_max - x_min)
    screen_x ← normalized * canvas_width
    RETURN screen_x
ENDSUBROUTINE


SUBROUTINE world_to_screen_y(world_y, y_min, y_max, canvas_height)
    normalized ← (world_y - y_min) / (y_max - y_min)
    screen_y ← canvas_height - (normalized * canvas_height)
    RETURN screen_y
ENDSUBROUTINE


SUBROUTINE screen_to_world_x(screen_x, x_min, x_max, canvas_width)
    normalized ← screen_x / canvas_width
    world_x ← x_min + normalized * (x_max - x_min)
    RETURN world_x
ENDSUBROUTINE


SUBROUTINE screen_to_world_y(screen_y, y_min, y_max, canvas_height)
    normalized ← (canvas_height - screen_y) / canvas_height
    world_y ← y_min + normalized * (y_max - y_min)
    RETURN world_y
ENDSUBROUTINE
