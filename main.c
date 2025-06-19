#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

double root(
    double (*f)(double), double (*df)(double),
    double (*g)(double), double (*dg)(double),
    double a, double b,
    double eps1
) {
    iterations = 0;
    double ddf(double x) {
        double h = sqrt(eps1);
        return (df(x + h) - df(x - h)) / (2 * h);
    }

    double ddg(double x) {
        double h = sqrt(eps1);
        return (dg(x + h) - dg(x - h)) / (2 * h);
    }

    double fa = f(a) - g(a);
    double fb = f(b) - g(b);

    if (fa * fb >= 0) {
        printf("Error: No root in [%.2f, %.2f] (f(a) and f(b) have the same sign).\n", a, b);
        return NAN;
    }

    double x_chord, x_tangent;
    double x_prev = a;

    do {
        // Метод хорд
        x_chord = (a * fb - b * fa) / (fb - fa);

        // Метод Ньютона (касательных)
        double x0 = ((ddf(a) - ddg(a)) * (df(a) - dg(a))) > 0 ? b : a;
        x_tangent = x0 - (f(x0) - g(x0)) / (df(x0) - dg(x0));

        // Новое приближение — среднее между хордой и касательной
        x_prev = (x_chord + x_tangent) / 2;

        // Обновляем границы отрезка
        double fx = f(x_prev) - g(x_prev);
        if (fx * fa < 0) {
            // сохранение знаков
            b = x_prev;
            fb = fx;
        } else {
            a = x_prev;
            fa = fx;
        }
        iterations++;
    } while (fabs(b - a) > eps1);

    return (a + b) / 2;
}

// Функция для вычисления интеграла методом Симпсона с заданной точностью eps2
double integral(double (*f)(double), double a, double b, double eps2) {
    int n = 2; // Начальное число разбиений (должно быть чётным)
    double h = (b - a) / n;
    double prev_sum = 0.0; // Предыдущее значение интеграла
    double prev_sum2 = 0.0; // Предыдущая сумма четных
    double prev_sum3 = 0.0; // Предыдущая сумма нечетных
    double current_sum = 0.0; // Текущее значение интеграла
    double current_sum2 = 0.0; // Текущая сумма четных членов
    double current_sum3 = 0.0; // Текущая сумма нечетных элементов
    double error = eps2 + 1; // Погрешность (инициализируем значением больше eps2)

    // Первое приближение (n = 2)
    prev_sum2 = 0;
    prev_sum3 = 4 * f(a + h);
    prev_sum = (f(a) + prev_sum3 + f(b)) * h / 3;

    // Увеличиваем n в 2 раза на каждой итерации, пока не достигнем нужной точности
    while (error > eps2) {
        current_sum2 = prev_sum2 + prev_sum3 / 2.0; // Сохраняем прошлые вычисления

        n *= 2;
        h = (b - a) / n;

        current_sum3 = 0.0;
        for (int i = 1; i < n; i += 2) {
            current_sum3 += 4 * f(a + h * i);
        }
        current_sum = (f(a) + current_sum2 + current_sum3 + f(b)) * h / 3;

        // Оцениваем погрешность по правилу Рунге (p = 1/15 для Симпсона)
        error = fabs(current_sum - prev_sum) / 15;

        // Обновляем предыдущее значение для следующей итерации
        prev_sum = current_sum;
        prev_sum2 = current_sum2;
        prev_sum3 = current_sum3;
    }

    return current_sum;
}

int main(int argc, char *argv[]) {
    double (*f[6])(double) = {f1, f2, f3, test1, test2, test3};
    double (*df[6])(double) = {df1, df2, df3, dtest1, dtest2, dtest3};
    if (argc > 1) {
        if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
            printf("--help\n");
            printf("-h\n");
            printf("--root\n");
            printf("-r\n");
            printf("--iterations\n");
            printf("-i\n");
            printf("--test-root\n");
            printf("-R\n");
            printf("--test-integral\n");
            printf("-I\n");
        } else if ((strcmp(argv[1], "-r") == 0) || (strcmp(argv[1], "--root") == 0)) {
            printf("%lf\n", root(f1, df1, f2, df2, 6, 7, 0.001));
            printf("%lf\n", root(f1, df1, f3, df3, 2.001, 3, 0.001));
            printf("%lf\n", root(f3, df3, f2, df2, 4, 5, 0.001));
        } else if ((strcmp(argv[1], "-i") == 0) || (strcmp(argv[1], "--iterations") == 0)) {
            long long unsigned counter = 0;
            root(f1, df1, f2, df2, 6, 7, 0.001);
            counter += iterations;
            root(f1, df1, f3, df3, 2.001, 3, 0.001);
            counter += iterations;
            root(f2, df2, f3, df3, 4, 5, 0.001);
            counter += iterations;
            printf("%llu\n", counter);
        } else if ((strcmp(argv[1], "-R") == 0) || (strcmp(argv[1], "--test-root") == 0)) {
            int x, y;
            double a, b, eps, r;
            sscanf(argv[2], "%d:%d:%lf:%lf:%lf:%lf", &x, &y, &a, &b, &eps, &r);
            x--;
            y--;
            double test_root = root(f[x], df[x], f[y], df[y], a, b, eps);
            printf("%lf %lf %lf\n", test_root, fabs(test_root - r), fabs((test_root - r) / r));
        } else if ((strcmp(argv[1], "-I") == 0) || (strcmp(argv[1], "--test-integral") == 0)) {
            int x;
            double a, b, eps, r;
            sscanf(argv[2], "%d:%lf:%lf:%lf:%lf", &x, &a, &b, &eps, &r);
            x--;
            double test_integral = integral(f[x], a, b, eps);
            printf("%lf %lf %lf\n", test_integral, fabs(test_integral - r), fabs((test_integral - r) / r));
        }
    } else {
        double r12 = root(f1, df1, f2, df2, 6, 7, 0.001);
        double r13 = root(f1, df1, f3, df3, 2.001, 3, 0.001);
        double r23 = root(f2, df2, f3, df3, 4, 5, 0.001);
        double i1 = integral(f3, r13, r23, 0.001);
        double i2 = integral(f2, r23, r12, 0.001);
        double i3 = integral(f1, r13, r12, 0.001);
        printf("%lf\n", i1 + i2 - i3);
    }
}
