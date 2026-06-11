#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <linux/time.h>

/*
 * HiloArgs: estructura que encapsula todos los datos
 * que el hilo principal pasa a cada hilo trabajador.
 */
typedef struct {
    int inicio;       /* primer índice del sub-rango de este hilo */
    int fin;          /* último índice + 1 (exclusivo) */
    int n;            /* total de rectángulos (global) */
    double resultado; /* suma parcial — escrita por el hilo, leída por main */
} HiloArgs;

/* f(x) = 4 / (1 + x^2) */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

/* Retorna tiempo actual en segundos */
double GetTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/*
 * funcion_hilo: calcula la suma parcial del sub-rango [inicio, fin).
 * Cada hilo trabaja sobre su propia variable local para evitar
 * contención — sin mutex dentro del bucle.
 * Escribe el resultado en args->resultado antes de terminar.
 */
void *funcion_hilo(void *arg) {
    HiloArgs *args = (HiloArgs *) arg;
    const double fH = 1.0 / (double) args->n;
    double suma_local = 0.0;
    double fX;
    int i;

    for (i = args->inicio; i < args->fin; i++) {
        fX = fH * ((double)i + 0.5);
        suma_local += f(fX);
    }

    args->resultado = suma_local;
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <n> <T>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int T = atoi(argv[2]);

    pthread_t *hilos = malloc(T * sizeof(pthread_t));
    HiloArgs  *args  = malloc(T * sizeof(HiloArgs));

    int chunk = n / T;  /* iteraciones por hilo */

    double t_inicio = GetTime();

    /* Crear T hilos, cada uno con su sub-rango */
    for (int i = 0; i < T; i++) {
        args[i].inicio    = i * chunk;
        args[i].fin       = (i == T - 1) ? n : (i + 1) * chunk;
        args[i].n         = n;
        args[i].resultado = 0.0;
        pthread_create(&hilos[i], NULL, funcion_hilo, &args[i]);
    }

    /* Esperar a cada hilo y acumular su suma parcial */
    double suma_total = 0.0;
    for (int i = 0; i < T; i++) {
        pthread_join(hilos[i], NULL);
        suma_total += args[i].resultado;
    }

    double fH = 1.0 / (double) n;
    double pi  = fH * suma_total;

    double t_fin = GetTime();

    printf("pi = %.10f  (T=%d hilos)\n", pi, T);
    printf("Tiempo: %.6f segundos\n", t_fin - t_inicio);

    free(hilos);
    free(args);
    return 0;
}