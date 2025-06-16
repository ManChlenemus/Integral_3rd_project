#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

typedef double afunc(double);

// Объявления функций
double f1(double x);
double f2(double x);
double f3(double x);
double df1(double x);
double df2(double x);
double df3(double x);
double test1(double x);
double test2(double x);
double test3(double x);
double dtest1(double x);
double dtest2(double x);
double dtest3(double x);

static long long unsigned iterations;

// Комбинированный метод хорд и касательных
static double root(afunc *f, afunc *df, afunc *g, afunc *dg, 
                 double a, double b, double eps) {
    iterations = 0;
    double fa = f(a) - g(a);
    double fb = f(b) - g(b);
    
    if (fa * fb >= 0) {
        fprintf(stderr, "Root is not bracketed in [%g, %g] (f(a)=%g, f(b)=%g)\n", 
               a, b, fa, fb);
        return NAN;
    }
    
    // Определяем направление приближения
    int is_increasing = (fa < 0);
    double midpoint = (a + b) / 2;
    double fmid = f(midpoint) - g(midpoint);
    double chord_value = (fa + fb) / 2;
    int is_above_chord = (fmid > chord_value);
    int same_sign_derivatives = (is_increasing && !is_above_chord) || (!is_increasing && is_above_chord);
    
    double x0 = a, x1 = b;
    double y0 = fa, y1 = fb;
    
    while (fabs(x1 - x0) > eps) {
        iterations++;
        
        // Метод хорд
        double x_chord = (x0 * y1 - x1 * y0) / (y1 - y0);
        
        // Метод касательных
        double x_newton;
        if (same_sign_derivatives) {
            x_newton = x1 - y1 / (df(x1) - dg(x1));
        } else {
            x_newton = x0 - y0 / (df(x0) - dg(x0));
        }
        
        // Комбинированный подход
        if (same_sign_derivatives) {
            x0 = x_chord;
            x1 = x_newton;
        } else {
            x0 = x_newton;
            x1 = x_chord;
        }
        
        y0 = f(x0) - g(x0);
        y1 = f(x1) - g(x1);
    }
    
    return (x0 + x1) / 2;
}

// Формула Симпсона
static double integral(afunc *f, double a, double b, double eps) {
    int n = 10;
    double h = (b - a) / n;
    double sum_odd = 0.0, sum_even = 0.0;
    
    for (int i = 1; i < n; i += 2) {
        sum_odd += f(a + i * h);
    }
    
    for (int i = 2; i < n; i += 2) {
        sum_even += f(a + i * h);
    }
    
    double I = (f(a) + f(b) + 4 * sum_odd + 2 * sum_even) * h / 3;
    double I_prev;
    double error;
    
    do {
        I_prev = I;
        n *= 2;
        h = (b - a) / n;
        
        sum_even += sum_odd;
        sum_odd = 0.0;
        
        for (int i = 1; i < n; i += 2) {
            sum_odd += f(a + i * h);
        }
        
        I = (f(a) + f(b) + 4 * sum_odd + 2 * sum_even) * h / 3;
        error = fabs(I - I_prev) / 15;
    } while (error > eps);
    
    return I;
}

int main(int argc, char *argv[]) {
    afunc *f[6] = {f1, f2, f3, test1, test2, test3};
    afunc *df[6] = {df1, df2, df3, dtest1, dtest2, dtest3};
    
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"root", no_argument, 0, 'r'},
        {"iterations", no_argument, 0, 'i'},
        {"test-root", required_argument, 0, 'R'},
        {"test-integral", required_argument, 0, 'I'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "hriR:I:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                printf("Available options:\n");
                printf("  -h, --help           Show this help message\n");
                printf("  -r, --root           Print roots of intersections\n");
                printf("  -i, --iterations     Print number of iterations for root finding\n");
                printf("  -R, --test-root F1:F2:A:B:E:R   Test root function\n");
                printf("  -I, --test-integral F:A:B:E:R   Test integral function\n");
                return 0;
                
            case 'r':
                printf("Root f1 and f2: %.6f\n", root(f1, df1, f2, df2, 6, 7, 0.001));
                printf("Root f1 and f3: %.6f\n", root(f1, df1, f3, df3, 2.1, 3, 0.001));
                printf("Root f2 and f3: %.6f\n", root(f2, df2, f3, df3, 4, 5, 0.001));
                return 0;
                
            case 'i':
                iterations = 0;
                root(f1, df1, f2, df2, 6, 7, 0.001);
                printf("Iterations f1 and f2: %llu\n", iterations);
                
                iterations = 0;
                root(f1, df1, f3, df3, 2.1, 3, 0.001);
                printf("Iterations f1 and f3: %llu\n", iterations);
                
                iterations = 0;
                root(f2, df2, f3, df3, 4, 5, 0.001);
                printf("Iterations f2 and f3: %llu\n", iterations);
                return 0;
                
            case 'R': {
                int x, y;
                double a, b, eps, r;
                sscanf(optarg, "%d:%d:%lf:%lf:%lf:%lf", &x, &y, &a, &b, &eps, &r);
                x--; y--;
                double test_root = root(f[x], df[x], f[y], df[y], a, b, eps);
                printf("%.6f %.6f %.6f\n", test_root, fabs(test_root - r), fabs((test_root - r) / r));
                return 0;
            }
                
            case 'I': {
                int x;
                double a, b, eps, r;
                sscanf(optarg, "%d:%lf:%lf:%lf:%lf", &x, &a, &b, &eps, &r);
                x--;
                double test_integral = integral(f[x], a, b, eps);
                printf("%.6f %.6f %.6f\n", test_integral, fabs(test_integral - r), fabs((test_integral - r) / r));
                return 0;
            }
                
            default:
                fprintf(stderr, "Unknown option\n");
                return 1;
        }
    }
    
    // Основной расчет
    double r1 = root(f1, df1, f2, df2, 6, 7, 0.001);        // ln(x) = -2x + 14
    double r2 = root(f1, df1, f3, df3, 2.1, 3, 0.001);        // ln(x) = 1/(2-x) + 6
    double r3 = root(f2, df2, f3, df3, 4, 5, 0.001);        // -2x + 14 = 1/(2-x) + 6
        
    // Вычисление площади (правильный порядок!)
    double area1 = integral(f2, r3, r1, 0.001) - integral(f1, r3, r1, 0.001); // f2 сверху
    double area2 = integral(f3, r2, r3, 0.001) - integral(f1, r2, r3, 0.001); // f3 сверху
    double total_area = area1 + area2;
    
    printf("Total area: %.6f\n", total_area);
    
    return 0;
}
