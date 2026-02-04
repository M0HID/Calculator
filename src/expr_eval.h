#ifndef EXPR_EVAL_H
#define EXPR_EVAL_H

// Simple expression evaluator
// Returns the result as a double, or NAN on error
double eval_expression(const char *expr);

// Evaluate expression with variable x substituted
// e.g., eval_expression_x("x^2 + 1", 3.0) returns 10.0
double eval_expression_x(const char *expr, double x_val);

#endif
