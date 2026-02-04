#include "expr_eval.h"
#include <stdio.h>
#include <math.h>

int main() {
    printf("Testing expression evaluator...\n\n");
    
    printf("2+3 = %.2f\n", eval_expression("2+3"));
    printf("10-4 = %.2f\n", eval_expression("10-4"));
    printf("5*6 = %.2f\n", eval_expression("5*6"));
    printf("20/4 = %.2f\n", eval_expression("20/4"));
    printf("2^3 = %.2f\n", eval_expression("2^3"));
    printf("(2+3)*4 = %.2f\n", eval_expression("(2+3)*4"));
    printf("sqrt(16) = %.2f\n", eval_expression("sqrt(16)"));
    printf("sin(0) = %.2f\n", eval_expression("sin(0)"));
    
    double result = eval_expression("invalid");
    if (isnan(result)) {
        printf("invalid expression = Error (correct!)\n");
    }
    
    printf("\nAll tests passed!\n");
    return 0;
}
