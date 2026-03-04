#include "stats.h"
#include "ui_common.h"
#include "ui_submenu.h"
#include "input_hal.h"
#include "main_menu.h"
#include "lvgl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef NAN
#define NAN (0.0/0.0)
#endif

int isnan(double x);
int isinf(double x);
double exp(double x);
double log(double x);
double sqrt(double x);
double pow(double x, double y);
double fabs(double x);

/* ── Distribution types ── */
typedef enum {
    STATS_MENU,
    STATS_BINOMIAL,
    STATS_BINOMIAL_CDF,
    STATS_POISSON,
    STATS_POISSON_CDF,
    STATS_NORMAL,
    STATS_NORMAL_CDF,
    STATS_NORMAL_INV
} StatsMode;

typedef enum {
    DIST_BINOMIAL, DIST_BINOMIAL_CDF,
    DIST_POISSON, DIST_POISSON_CDF,
    DIST_NORMAL, DIST_NORMAL_CDF, DIST_NORMAL_INV
} DistType;

static const char *dist_names[] = {
    "Binomial P(X=k)",
    "Binomial CDF P(X<=k)",
    "Poisson P(X=k)",
    "Poisson CDF P(X<=k)",
    "Normal PDF",
    "Normal CDF",
    "Inverse Normal"
};

static StatsMode current_mode = STATS_MENU;

/* UI elements */
static lv_obj_t *dist_label = NULL;
static lv_obj_t *param_labels[3];
static lv_obj_t *param_textareas[3];
static lv_obj_t *result_label = NULL;
static lv_group_t *stats_group = NULL;
static lv_obj_t *hint_lbl = NULL;
static int focused_field = 0;  /* 0..2 = which textarea has focus */

/* Parameter names */
static const char *param_names[][3] = {
    {"n (trials):",      "k (successes):",   "p (prob):"},
    {"n (trials):",      "k (max):",         "p (prob):"},
    {"k (events):",      "\xCE\xBB (mean):", ""},
    {"k (max events):",  "\xCE\xBB (mean):", ""},
    {"x (value):",       "\xCE\xBC (mean):", "\xCF\x83 (std dev):"},
    {"x (value):",       "\xCE\xBC (mean):", "\xCF\x83 (std dev):"},
    {"p (prob):",        "\xCE\xBC (mean):", "\xCF\x83 (std dev):"}
};

/* ── Math functions ── */

static double factorial(int n) {
    if (n < 0) return NAN;
    if (n <= 1) return 1.0;
    double r = 1.0;
    for (int i = 2; i <= n; i++) r *= i;
    return r;
}

static double binomial_coeff(int n, int k) {
    if (k < 0 || k > n) return 0.0;
    if (k == 0 || k == n) return 1.0;
    double r = 1.0;
    for (int i = 0; i < k; i++) { r *= (n - i); r /= (i + 1); }
    return r;
}

static double binomial_pmf(int n, int k, double p) {
    if (p < 0.0 || p > 1.0) return NAN;
    if (n < 0 || k < 0 || k > n) return 0.0;
    return binomial_coeff(n, k) * pow(p, k) * pow(1 - p, n - k);
}

static double binomial_cdf(int n, int k, double p) {
    if (p < 0.0 || p > 1.0) return NAN;
    if (n < 0) return NAN;
    if (k < 0) return 0.0;
    if (k >= n) return 1.0;
    double sum = 0.0;
    for (int i = 0; i <= k; i++) sum += binomial_pmf(n, i, p);
    return sum;
}

static double poisson_pmf(int k, double lambda) {
    if (lambda < 0.0 || k < 0) return NAN;
    if (lambda == 0.0) return (k == 0) ? 1.0 : 0.0;
    /* P(X=k) = e^(-λ) * λ^k / k! — computed in log space to avoid overflow */
    double log_p = -lambda + k * log(lambda);
    for (int i = 2; i <= k; i++) log_p -= log((double)i);
    return exp(log_p);
}

static double poisson_cdf(int k, double lambda) {
    if (lambda < 0.0) return NAN;
    if (k < 0) return 0.0;
    double sum = 0.0;
    for (int i = 0; i <= k; i++) sum += poisson_pmf(i, lambda);
    if (sum > 1.0) sum = 1.0;
    return sum;
}

static double normal_pdf(double x, double mu, double sigma) {
    if (sigma <= 0) return NAN;
    double z = (x - mu) / sigma;
    return (1.0 / (sigma * sqrt(2.0 * M_PI))) * exp(-0.5 * z * z);
}

static double erf_approx(double x) {
    double a1=0.254829592, a2=-0.284496736, a3=1.421413741, a4=-1.453152027, a5=1.061405429, p=0.3275911;
    int sign = (x < 0) ? -1 : 1;
    x = fabs(x);
    double t = 1.0 / (1.0 + p * x);
    double y = 1.0 - (((((a5*t+a4)*t)+a3)*t+a2)*t+a1)*t*exp(-x*x);
    return sign * y;
}

static double normal_cdf(double x, double mu, double sigma) {
    if (sigma <= 0) return NAN;
    return 0.5 * (1.0 + erf_approx((x - mu) / (sigma * sqrt(2.0))));
}

static double erf_inv_approx(double x) {
    if (x <= -1.0 || x >= 1.0) return NAN;
    double a = 0.147;
    double b = 2.0 / (M_PI * a) + log(1 - x * x) / 2.0;
    double c = log(1 - x * x) / a;
    double sign = (x < 0) ? -1.0 : 1.0;
    return sign * sqrt(-b + sqrt(b * b - c));
}

static double normal_inv(double p, double mu, double sigma) {
    if (p <= 0.0 || p >= 1.0) return NAN;
    if (sigma <= 0) return NAN;
    return mu + sigma * sqrt(2.0) * erf_inv_approx(2.0 * p - 1.0);
}

/* Forward declarations */
static void show_stats_menu(void);
static void show_binomial_pmf(void);
static void show_binomial_cdf(void);
static void show_poisson_pmf(void);
static void show_poisson_cdf(void);
static void show_normal_pdf(void);
static void show_normal_cdf(void);
static void show_inverse_normal(void);

/* ── UI helpers ── */

static void cleanup_stats_ui(void) {
    /* Clean up group and UI */
    if (stats_group) {
        ui_submenu_cleanup(stats_group);
        stats_group = NULL;
    }
    dist_label = NULL;
    result_label = NULL;
    hint_lbl = NULL;
    for (int i = 0; i < 3; i++) {
        param_labels[i] = NULL;
        param_textareas[i] = NULL;
    }
}

/* Create a standard stats screen */
static lv_obj_t *setup_stats_screen(const char *hint_text) {
    cleanup_stats_ui();

    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    ui_setup_screen(scr);

    stats_group = lv_group_create();

    /* Hint bar */
    hint_lbl = ui_create_hint_bar(scr, hint_text);

    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, stats_group);

    return scr;
}

/* ── Context for active distribution ── */

typedef struct {
    lv_obj_t **fields;
    int field_count;
    lv_obj_t *result_label;
    void (*calc_fn)(lv_obj_t **fields, int count, lv_obj_t *result);
} StatsCtx;

static StatsCtx *active_stats_ctx = NULL;

/* Key handler on individual textareas: Up/Down navigate, Enter calculates */
static void stats_textarea_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);

    switch (key) {
    case LV_KEY_UP:
        if (stats_group) lv_group_focus_prev(stats_group);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_DOWN:
        if (stats_group) lv_group_focus_next(stats_group);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_ENTER:
        if (active_stats_ctx && active_stats_ctx->calc_fn)
            active_stats_ctx->calc_fn(active_stats_ctx->fields, active_stats_ctx->field_count, active_stats_ctx->result_label);
        lv_event_stop_bubbling(e);
        return;
    case LV_KEY_ESC:
    case LV_KEY_BACKSPACE:
        show_stats_menu();
        lv_event_stop_bubbling(e);
        return;
    case 'M':
        cleanup_stats_ui();
        main_menu_create();
        lv_event_stop_bubbling(e);
        return;
    default:
        break;
    }
}

/* ── Stats menu ── */

static const SubMenuItem stats_menu_items[] = {
    {"Binomial P(X=k)", show_binomial_pmf},
    {"Binomial CDF P(X<=k)", show_binomial_cdf},
    {"Poisson P(X=k)", show_poisson_pmf},
    {"Poisson CDF P(X<=k)", show_poisson_cdf},
    {"Normal PDF", show_normal_pdf},
    {"Normal CDF", show_normal_cdf},
    {"Inverse Normal", show_inverse_normal},
};

#define STATS_MENU_ITEMS 7

static void show_stats_menu(void) {
    current_mode = STATS_MENU;
    cleanup_stats_ui();

    SubMenuStyle style = {
        COL_ACCENT_STATS,
        COL_FOCUS_BG_STATS,
        "[=] Select  [M] Menu"
    };

    stats_group = ui_create_submenu(stats_menu_items, STATS_MENU_ITEMS, &style, main_menu_create);
}

/* ── Individual distribution screens ── */

/* Helper to create n-parameter distribution screen */
static void create_dist_screen_n(const char *title, const char **param_labels_text,
                                int nfields,
                                void (*calc_fn)(lv_obj_t**, int, lv_obj_t*),
                                lv_obj_t **fields_out, StatsCtx *ctx) {
    lv_obj_t *scr = setup_stats_screen("[=] Calc  [AC] Back  [M] Menu");

    /* Title */
    dist_label = ui_label(scr, title, CONTENT_SIDE, CONTENT_TOP);
    lv_obj_set_style_text_color(dist_label, COL_ACCENT_STATS, 0);

    /* Parameter rows */
    int y = CONTENT_TOP + 22;
    for (int i = 0; i < nfields; i++) {
        param_labels[i] = ui_label(scr, param_labels_text[i], CONTENT_SIDE, y + 4);
        lv_obj_set_style_text_font(param_labels[i], FONT_SECONDARY, 0);

        param_textareas[i] = lv_textarea_create(scr);
        lv_obj_set_size(param_textareas[i], 170, FIELD_H);
        lv_obj_set_pos(param_textareas[i], LCD_H_RES - CONTENT_SIDE - 170, y);
        lv_textarea_set_one_line(param_textareas[i], true);
        lv_textarea_set_placeholder_text(param_textareas[i], "0");
        ui_style_textarea(param_textareas[i], COL_ACCENT_STATS, COL_FOCUS_BG_STATS);
        lv_obj_add_event_cb(param_textareas[i], stats_textarea_key_cb, LV_EVENT_KEY, NULL);
        lv_group_add_obj(stats_group, param_textareas[i]);

        fields_out[i] = param_textareas[i];
        y += ROW_SPACING;
    }

    /* Result label */
    result_label = lv_label_create(scr);
    lv_obj_set_pos(result_label, CONTENT_SIDE, y + 6);
    lv_obj_set_size(result_label, LCD_H_RES - 2 * CONTENT_SIDE, 30);
    lv_obj_set_style_text_color(result_label, COL_RESULT, 0);
    lv_obj_set_style_text_font(result_label, FONT_PRIMARY, 0);
    lv_label_set_text(result_label, "");

    /* Set up context */
    ctx->fields = fields_out;
    ctx->field_count = nfields;
    ctx->result_label = result_label;
    ctx->calc_fn = calc_fn;
    active_stats_ctx = ctx;

    lv_group_focus_obj(param_textareas[0]);
}

/* Convenience wrapper for 3-parameter screens */
static void create_dist_screen(const char *title, const char **param_labels_text,
                              void (*calc_fn)(lv_obj_t**, int, lv_obj_t*),
                              lv_obj_t **fields_out, StatsCtx *ctx) {
    create_dist_screen_n(title, param_labels_text, 3, calc_fn, fields_out, ctx);
}

/* Binomial PMF calculation */
static lv_obj_t *binomial_pmf_fields[3];
static StatsCtx binomial_pmf_ctx;

static void calc_binomial_pmf(lv_obj_t **f, int n, lv_obj_t *res) {
    double n_val = atof(lv_textarea_get_text(f[0]));
    double k_val = atof(lv_textarea_get_text(f[1]));
    double p_val = atof(lv_textarea_get_text(f[2]));
    
    double result = binomial_pmf((int)n_val, (int)k_val, p_val);
    char buf[128];
    if (isnan(result))       snprintf(buf, sizeof(buf), "Error: Invalid input");
    else if (isinf(result))  snprintf(buf, sizeof(buf), "Infinity");
    else                     snprintf(buf, sizeof(buf), "= %.8g", result);
    lv_label_set_text(res, buf);
}

static void show_binomial_pmf(void) {
    current_mode = STATS_BINOMIAL;
    create_dist_screen("Binomial P(X=k)", param_names[DIST_BINOMIAL], 
                      calc_binomial_pmf, binomial_pmf_fields, &binomial_pmf_ctx);
}

/* Binomial CDF calculation */
static lv_obj_t *binomial_cdf_fields[3];
static StatsCtx binomial_cdf_ctx;

static void calc_binomial_cdf(lv_obj_t **f, int n, lv_obj_t *res) {
    double n_val = atof(lv_textarea_get_text(f[0]));
    double k_val = atof(lv_textarea_get_text(f[1]));
    double p_val = atof(lv_textarea_get_text(f[2]));
    
    double result = binomial_cdf((int)n_val, (int)k_val, p_val);
    char buf[128];
    if (isnan(result))       snprintf(buf, sizeof(buf), "Error: Invalid input");
    else if (isinf(result))  snprintf(buf, sizeof(buf), "Infinity");
    else                     snprintf(buf, sizeof(buf), "= %.8g", result);
    lv_label_set_text(res, buf);
}

static void show_binomial_cdf(void) {
    current_mode = STATS_BINOMIAL_CDF;
    create_dist_screen("Binomial CDF P(X<=k)", param_names[DIST_BINOMIAL_CDF], 
                      calc_binomial_cdf, binomial_cdf_fields, &binomial_cdf_ctx);
}

/* Poisson PMF calculation */
static lv_obj_t *poisson_pmf_fields[2];
static StatsCtx poisson_pmf_ctx;

static void calc_poisson_pmf(lv_obj_t **f, int n, lv_obj_t *res) {
    double k_val = atof(lv_textarea_get_text(f[0]));
    double lambda = atof(lv_textarea_get_text(f[1]));

    double result = poisson_pmf((int)k_val, lambda);
    char buf[128];
    if (isnan(result))       snprintf(buf, sizeof(buf), "Error: Invalid input");
    else if (isinf(result))  snprintf(buf, sizeof(buf), "Infinity");
    else                     snprintf(buf, sizeof(buf), "= %.8g", result);
    lv_label_set_text(res, buf);
}

static void show_poisson_pmf(void) {
    current_mode = STATS_POISSON;
    create_dist_screen_n("Poisson P(X=k)", param_names[DIST_POISSON], 2,
                        calc_poisson_pmf, poisson_pmf_fields, &poisson_pmf_ctx);
}

/* Poisson CDF calculation */
static lv_obj_t *poisson_cdf_fields[2];
static StatsCtx poisson_cdf_ctx;

static void calc_poisson_cdf(lv_obj_t **f, int n, lv_obj_t *res) {
    double k_val = atof(lv_textarea_get_text(f[0]));
    double lambda = atof(lv_textarea_get_text(f[1]));

    double result = poisson_cdf((int)k_val, lambda);
    char buf[128];
    if (isnan(result))       snprintf(buf, sizeof(buf), "Error: Invalid input");
    else if (isinf(result))  snprintf(buf, sizeof(buf), "Infinity");
    else                     snprintf(buf, sizeof(buf), "= %.8g", result);
    lv_label_set_text(res, buf);
}

static void show_poisson_cdf(void) {
    current_mode = STATS_POISSON_CDF;
    create_dist_screen_n("Poisson CDF P(X<=k)", param_names[DIST_POISSON_CDF], 2,
                        calc_poisson_cdf, poisson_cdf_fields, &poisson_cdf_ctx);
}

/* Normal PDF calculation */
static lv_obj_t *normal_pdf_fields[3];
static StatsCtx normal_pdf_ctx;

static void calc_normal_pdf(lv_obj_t **f, int n, lv_obj_t *res) {
    double x = atof(lv_textarea_get_text(f[0]));
    double mu = atof(lv_textarea_get_text(f[1]));
    double sigma = atof(lv_textarea_get_text(f[2]));
    
    double result = normal_pdf(x, mu, sigma);
    char buf[128];
    if (isnan(result))       snprintf(buf, sizeof(buf), "Error: Invalid input");
    else if (isinf(result))  snprintf(buf, sizeof(buf), "Infinity");
    else                     snprintf(buf, sizeof(buf), "= %.8g", result);
    lv_label_set_text(res, buf);
}

static void show_normal_pdf(void) {
    current_mode = STATS_NORMAL;
    create_dist_screen("Normal PDF", param_names[DIST_NORMAL], 
                      calc_normal_pdf, normal_pdf_fields, &normal_pdf_ctx);
}

/* Normal CDF calculation */
static lv_obj_t *normal_cdf_fields[3];
static StatsCtx normal_cdf_ctx;

static void calc_normal_cdf(lv_obj_t **f, int n, lv_obj_t *res) {
    double x = atof(lv_textarea_get_text(f[0]));
    double mu = atof(lv_textarea_get_text(f[1]));
    double sigma = atof(lv_textarea_get_text(f[2]));
    
    double result = normal_cdf(x, mu, sigma);
    char buf[128];
    if (isnan(result))       snprintf(buf, sizeof(buf), "Error: Invalid input");
    else if (isinf(result))  snprintf(buf, sizeof(buf), "Infinity");
    else                     snprintf(buf, sizeof(buf), "= %.8g", result);
    lv_label_set_text(res, buf);
}

static void show_normal_cdf(void) {
    current_mode = STATS_NORMAL_CDF;
    create_dist_screen("Normal CDF", param_names[DIST_NORMAL_CDF], 
                      calc_normal_cdf, normal_cdf_fields, &normal_cdf_ctx);
}

/* Inverse Normal calculation */
static lv_obj_t *inverse_normal_fields[3];
static StatsCtx inverse_normal_ctx;

static void calc_inverse_normal(lv_obj_t **f, int n, lv_obj_t *res) {
    double p = atof(lv_textarea_get_text(f[0]));
    double mu = atof(lv_textarea_get_text(f[1]));
    double sigma = atof(lv_textarea_get_text(f[2]));
    
    double result = normal_inv(p, mu, sigma);
    char buf[128];
    if (isnan(result))       snprintf(buf, sizeof(buf), "Error: Invalid input");
    else if (isinf(result))  snprintf(buf, sizeof(buf), "Infinity");
    else                     snprintf(buf, sizeof(buf), "= %.8g", result);
    lv_label_set_text(res, buf);
}

static void show_inverse_normal(void) {
    current_mode = STATS_NORMAL_INV;
    create_dist_screen("Inverse Normal", param_names[DIST_NORMAL_INV], 
                      calc_inverse_normal, inverse_normal_fields, &inverse_normal_ctx);
}

/* ── Public API ── */
void stats_app_start(void) {
    show_stats_menu();
}
