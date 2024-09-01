#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

long double get_val(const char* expr, long double x);

// Helper function to parse numbers
long double parse_number(const char** expr) {
    long double result = 0.0;
    int divisor = 10;
    int is_fraction = 0;

    while (isdigit(**expr) || **expr == '.') {
        if (**expr == '.') {
            is_fraction = 1;
        }
        else {
            if (is_fraction) {
                result += (**expr - '0') / (long double)divisor;
                divisor *= 10;
            }
            else {
                result = result * 10 + (**expr - '0');
            }
        }
        (*expr)++;
    }
    return result;
}

// Helper function to handle parentheses and nested expressions
long double parse_factor(const char** expr, long double x) {
    long double result = 0.0;

    if (**expr == '(') {
        (*expr)++; // Skip '('
        result = get_val(*expr, x);
        (*expr)++; // Skip ')'
    }
    else if (**expr == 'x') {
        (*expr)++;
        result = x;
    }
    else if (isalpha(**expr)) {
        char func[4] = { 0 };
        int i = 0;

        while (isalpha(**expr) && i < 3) {
            func[i++] = *(*expr)++;
        }
        func[i] = '\0';

        if (**expr == '(') {
            (*expr)++;
        }

        long double argument = get_val(*expr, x);

        while (**expr != ')' && **expr != '\0') {
            (*expr)++;
        }
        (*expr)++;

        if (strcmp(func, "sin") == 0) result = sinl(argument);
        else if (strcmp(func, "cos") == 0) result = cosl(argument);
        else if (strcmp(func, "tan") == 0) result = tanl(argument);
        else if (strcmp(func, "exp") == 0) result = expl(argument);
        else if (strcmp(func, "log") == 0) result = logl(argument);
        else if (strcmp(func, "sqrt") == 0) result = sqrtl(argument);
        else {
            fprintf(stderr, "Unknown function: %s\n", func);
            exit(EXIT_FAILURE);
        }
    }
    else {
        result = parse_number(expr);
    }

    return result;
}

long double parse_term(const char** expr, long double x) {
    long double result = parse_factor(expr, x);

    while (**expr == '^') {
        (*expr)++;
        long double exponent = parse_factor(expr, x);
        result = powl(result, exponent);
    }

    return result;
}

long double parse_expression(const char** expr, long double x) {
    long double result = parse_term(expr, x);

    while (**expr == '*' || **expr == '/') {
        char op = *(*expr)++;
        long double right = parse_term(expr, x);

        if (op == '*') result *= right;
        else if (op == '/') result /= right;
    }

    return result;
}

long double get_val(const char* expr, long double x) {
    long double result = parse_expression(&expr, x);

    while (*expr != '\0') {
        char op = *expr++;

        if (op == '+' || op == '-') {
            long double right = parse_expression(&expr, x);

            if (op == '+') result += right;
            else if (op == '-') result -= right;
        }
    }

    return result;
}

long double integrate(const char* function, long double a, long double b, int accuracy) {
    long double dx = 1.0L / accuracy;
    long double* x_values = malloc((accuracy + 1) * sizeof(long double));
    long double integral = 0.0;
    if (x_values == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= accuracy; i++) {
        x_values[i] = a + i * dx * (b - a);
    }

    for (int i = 0; i < accuracy; i++) {
        integral += dx * get_val(function, x_values[i]);
    }

    free(x_values);
    return integral;
}

int main(int argc, char** argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <function> <a> <b> <accuracy>\n", argv[0]);
        fprintf(stderr, "Example: %s \"sin(x)/2\" 3 4 10000\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* function = argv[1];
    long double a = atof(argv[2]);
    long double b = atof(argv[3]);
    int accuracy = atoi(argv[4]);

    if (accuracy <= 0) {
        fprintf(stderr, "Accuracy must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    long double answer = integrate(function, a, b, accuracy);
    printf("Int(%s, %.2Lf, %.2Lf) with accuracy %d = %.15Lf\n", function, a, b, accuracy, answer);

    return 0;
}
