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
double log(double x);     // Natural logarithm (ln)
double log10(double x);   // Base-10 logarithm
double fabs(double x);    // Absolute value
double floor(double x);   // Round down
double ceil(double x);    // Round up

// NAN definition if not available
#ifndef NAN
#define NAN (0.0/0.0)
#endif

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif

// Math constants
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
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

// Calculate factorial
static double factorial(double n) {
    if (n < 0 || n != (long long)n) {
        return NAN; // Factorial only for non-negative integers
    }
    if (n > 170) {
        return INFINITY; // Overflow protection
    }
    
    double result = 1.0;
    for (long long i = 2; i <= (long long)n; i++) {
        result *= i;
    }
    return result;
}

static double parse_factor(Parser *p) {
    skip_whitespace(p);
    
    // Handle unary minus
    if (p->str[p->pos] == '-') {
        p->pos++;
        return -parse_factor(p);
    }
    
    // Handle unary plus
    if (p->str[p->pos] == '+') {
        p->pos++;
        return parse_factor(p);
    }
    
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
    
    // Check for pi constant
    if (strncmp(&p->str[p->pos], "pi", 2) == 0) {
        p->pos += 2;
        return M_PI;
    }
    
    // Check for e constant
    if (p->str[p->pos] == 'e' && !isalpha((unsigned char)p->str[p->pos + 1])) {
        p->pos++;
        return M_E;
    }
    
    // Check for functions - order matters! Check longer names first
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
    
    if (strncmp(&p->str[p->pos], "floor", 5) == 0) {
        p->pos += 5;
        skip_whitespace(p);
        if (p->str[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            if (p->str[p->pos] == ')') p->pos++;
            return floor(arg);
        }
    }
    
    if (strncmp(&p->str[p->pos], "ceil", 4) == 0) {
        p->pos += 4;
        skip_whitespace(p);
        if (p->str[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            if (p->str[p->pos] == ')') p->pos++;
            return ceil(arg);
        }
    }
    
    if (strncmp(&p->str[p->pos], "abs", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->str[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            if (p->str[p->pos] == ')') p->pos++;
            return fabs(arg);
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
    
    // ln = natural log
    if (strncmp(&p->str[p->pos], "ln", 2) == 0) {
        p->pos += 2;
        skip_whitespace(p);
        if (p->str[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            if (p->str[p->pos] == ')') p->pos++;
            return log(arg);
        }
    }
    
    // log = base-10 log
    if (strncmp(&p->str[p->pos], "log", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->str[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            if (p->str[p->pos] == ')') p->pos++;
            return log10(arg);
        }
    }
    
    // Catch-all: skip any unrecognized alpha characters to prevent infinite loops.
    // This handles stray letters like 'y', 'z', or incomplete function names.
    if (isalpha((unsigned char)p->str[p->pos])) {
        // Skip the entire unrecognized word
        while (isalpha((unsigned char)p->str[p->pos])) {
            p->pos++;
        }
        return NAN;
    }
    
    return parse_number(p);
}

static double parse_term(Parser *p) {
    double result = parse_factor(p);
    
    // Check for postfix factorial
    skip_whitespace(p);
    if (p->str[p->pos] == '!') {
        p->pos++;
        result = factorial(result);
    }
    
    while (1) {
        skip_whitespace(p);
        char op = p->str[p->pos];
        
        // Implicit multiplication: number/variable followed by ( or known token
        // e.g., "2pi", "3(x+1)", "2x", "(2)(3)", "2sin(x)"
        if (op == '(' || op == 'x' || op == 'X' || 
            (op == 'p' && p->str[p->pos + 1] == 'i') ||
            (op == 'e' && !isalpha((unsigned char)p->str[p->pos + 1])) ||
            (isalpha((unsigned char)op) && (
                strncmp(&p->str[p->pos], "sin", 3) == 0 ||
                strncmp(&p->str[p->pos], "cos", 3) == 0 ||
                strncmp(&p->str[p->pos], "tan", 3) == 0 ||
                strncmp(&p->str[p->pos], "sqrt", 4) == 0 ||
                strncmp(&p->str[p->pos], "ln", 2) == 0 ||
                strncmp(&p->str[p->pos], "log", 3) == 0 ||
                strncmp(&p->str[p->pos], "abs", 3) == 0 ||
                strncmp(&p->str[p->pos], "floor", 5) == 0 ||
                strncmp(&p->str[p->pos], "ceil", 4) == 0
            ))) {
            // Implicit multiplication detected
            result *= parse_factor(p);
            
            // Check for factorial after implicit multiplication
            skip_whitespace(p);
            if (p->str[p->pos] == '!') {
                p->pos++;
                result = factorial(result);
            }
        } else if (op == '*') {
            p->pos++;
            double factor = parse_factor(p);
            
            // Check for factorial after factor
            skip_whitespace(p);
            if (p->str[p->pos] == '!') {
                p->pos++;
                factor = factorial(factor);
            }
            
            result *= factor;
        } else if (op == '/') {
            p->pos++;
            double divisor = parse_factor(p);
            
            // Check for factorial after divisor
            skip_whitespace(p);
            if (p->str[p->pos] == '!') {
                p->pos++;
                divisor = factorial(divisor);
            }
            
            if (divisor != 0) {
                result /= divisor;
            } else {
                return NAN;
            }
        } else if (op == '^') {
            p->pos++;
            double exponent = parse_factor(p);
            
            // Check for factorial in exponent
            skip_whitespace(p);
            if (p->str[p->pos] == '!') {
                p->pos++;
                exponent = factorial(exponent);
            }
            
            result = pow(result, exponent);
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
