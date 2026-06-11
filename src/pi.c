#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <linux/time.h>

/* f(x) = 4 / (1 + x^2) — integrando cuya integral en [0,1] es π */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

/* Retorna tiempo actual en segundos con precisión de nanosegundos */
double GetTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/*
 * CalcPi: aproxima π usando integración numérica (regla del punto medio).
 * Divide [0,1] en n rectángulos y suma sus áreas.
 * Parámetro n: número de rectángulos.
 * Retorna: aproximación de π.
 */
double CalcPi(int n) {
    const double fH = 1.0 / (double) n;
    double fSum = 0.0;
    double fX;
    int i;

    for (i = 0; i < n; i++) {
        fX = fH * ((double)i + 0.5);
        fSum += f(fX);
    }
    return fH * fSum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    double t_inicio = GetTime();
    double pi = CalcPi(n);
    double t_fin = GetTime();

    printf("pi = %.10f\n", pi);
    printf("Tiempo: %.6f segundos\n", t_fin - t_inicio);

    return 0;
}