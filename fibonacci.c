#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
 * FibArgs: datos compartidos entre main y el hilo trabajador.
 * main asigna el arreglo y pasa el puntero al hilo.
 */
typedef struct {
    long long *arreglo; /* arreglo compartido donde se guarda la secuencia */
    int N;              /* cantidad de elementos a generar */
} FibArgs;

/*
 * hilo_fibonacci: genera los N primeros números de Fibonacci
 * y los almacena en arreglo[0..N-1].
 * Definición usada: F(0)=0, F(1)=1, F(n)=F(n-1)+F(n-2).
 */
void *hilo_fibonacci(void *arg) {
    FibArgs *datos = (FibArgs *) arg;
    long long *arr = datos->arreglo;
    int N = datos->N;

    if (N >= 1) arr[0] = 0;
    if (N >= 2) arr[1] = 1;

    for (int i = 2; i < N; i++) {
        arr[i] = arr[i-1] + arr[i-2];
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "N debe ser un entero positivo.\n");
        return 1;
    }

    /* main asigna la memoria dinámica del arreglo compartido */
    long long *arreglo = malloc(N * sizeof(long long));
    if (!arreglo) {
        fprintf(stderr, "Error al asignar memoria.\n");
        return 1;
    }

    FibArgs args = { arreglo, N };

    /* Crear el hilo trabajador y pasarle los argumentos */
    pthread_t hilo;
    pthread_create(&hilo, NULL, hilo_fibonacci, &args);

    /*
     * pthread_join bloquea main hasta que el hilo termine.
     * Garantiza que el arreglo esté completamente lleno
     * antes de que main lo imprima.
     */
    pthread_join(hilo, NULL);

    printf("Secuencia de Fibonacci (%d elementos):\n", N);
    for (int i = 0; i < N; i++) {
        printf("%lld ", arreglo[i]);
    }
    printf("\n");

    free(arreglo);
    return 0;
}