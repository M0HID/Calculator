#include "expr_eval.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Explicitly declare math functions to avoid implicit declaration errors
double sqrt(double x);
double sin(double x);
double cos(double x);
double tan(double x);
double pow(double x, double y);

// NAN definition if not available
#ifndef NAN
#define NAN (0.0/0.0)
#endif

// Current value of x for expression evaluation
static double current_x_value = 0.0;

typedef struct {
    const char *str;
    int pos;
} Parser;

static void skip_whitespace(Parser *p) {
    while (p->str[p->pos] == ' ' || p->str[p->pos] == '\t') {
        p->pos++;
    }
}

static double parse_number(Parser *p) {
    skip_whitespace(p);
    int start = p->pos;
    
    if (p->str[p->pos] == '-' || p->str[p->pos] == '+') {
        p->pos++;
    }
    
    while (isdigit((unsigned char)p->str[p->pos]) || p->str[p->pos] == '.') {
        p->pos++;
    }
    
    char num_str[64];
    int len = p->pos - start;
    if (len >= 64) len = 63;
    strncpy(num_str, &p->str[start], len);
    num_str[len] = '\0';
    
    return atof(num_str);
}

static double parse_expression(Parser *p);

static double parse_factor(Parser *p) {
    skip_whitespace(p);
    
    if (p->str[p->pos] == '(') {
        p->pos++;
        double result = parse_expression(p);
        skip_whitespace(p);
        if (p->str[p->pos] == ')') {
            p->pos++;
        }
        return result;
    }
    
    // Check for variable x
    if (p->str[p->pos] == 'x' || p->str[p->pos] == 'X') {
        p->pos++;
        return current_x_value;
    }
    
    // Check for functions
    if (strncmp(&p->str[p->pos], "sqrt", 4) == 0) {
        p->pos += 4;
        skip_whitespace(p);
        if (p->str[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            if (p->str[p->pos] == ')') p->pos++;
            return sqrt(arg);
        }
    }
    
    if (strncmp(&p->str[p->pos], "sin", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->str[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            if (p->str[p->pos] == ')') p->pos++;
            return sin(arg);
        }
    }
    
    if (strncmp(&p->str[p->pos], "cos", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->str[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            if (p->str[p->pos] == ')') p->pos++;
            return cos(arg);
        }
    }
    
    if (strncmp(&p->str[p->pos], "tan", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->str[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            if (p->str[p->pos] == ')') p->pos++;
            return tan(arg);
        }
    }
    
    return parse_number(p);
}

static double parse_term(Parser *p) {
    double result = parse_factor(p);
    
    while (1) {
        skip_whitespace(p);
        char op = p->str[p->pos];
        
        if (op == '*') {
            p->pos++;
            result *= parse_factor(p);
        } else if (op == '/') {
            p->pos++;
            double divisor = parse_factor(p);
            if (divisor != 0) {
                result /= divisor;
            } else {
                return NAN;
            }
        } else if (op == '^') {
            p->pos++;
            result = pow(result, parse_factor(p));
        } else {
            break;
        }
    }
    
    return result;
}

static double parse_expression(Parser *p) {
    double result = parse_term(p);
    
    while (1) {
        skip_whitespace(p);
        char op = p->str[p->pos];
        
        if (op == '+') {
            p->pos++;
            result += parse_term(p);
        } else if (op == '-') {
            p->pos++;
            result -= parse_term(p);
        } else {
            break;
        }
    }
    
    return result;
}

double eval_expression(const char *expr) {
    if (!expr || strlen(expr) == 0) {
        return NAN;
    }
    
    Parser p = { .str = expr, .pos = 0 };
    return parse_expression(&p);
}

double eval_expression_x(const char *expr, double x_val) {
    if (!expr || strlen(expr) == 0) {
        return NAN;
    }
    
    current_x_value = x_val;
    Parser p = { .str = expr, .pos = 0 };
    return parse_expression(&p);
}
