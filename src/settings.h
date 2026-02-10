#ifndef SETTINGS_H
#define SETTINGS_H

#include "lvgl.h"

/* ── Angle mode ── */
typedef enum { ANGLE_RAD = 0, ANGLE_DEG } AngleMode;

AngleMode settings_get_angle_mode(void);
void      settings_set_angle_mode(AngleMode m);

/* ── Decimal‑places (0 = auto/g format) ── */
int  settings_get_decimal_places(void);
void settings_set_decimal_places(int dp);

/* ── User variables (A–F) ── */
#define SETTINGS_VAR_COUNT 6
double settings_get_variable(int idx);          /* 0=A .. 5=F */
void   settings_set_variable(int idx, double v);
char   settings_get_variable_name(int idx);     /* 'A'..'F'   */

/* ── Entry point (called from main menu) ── */
void settings_app_start(void);

#endif
