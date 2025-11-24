#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estructura para pasar datos al hilo
typedef struct {
    long long* arr;
    int N;
} fibn_argumentos_t;


// funcion de hilo
void* fib_worker(void* arg) {
    fibn_argumentos_t* args = (fibn_argumentos_t*) arg;
    long long* arr = args->arr;
    int N = args->N;

    if (N <= 0) {
        pthread_exit(NULL);
    }

    if (N >= 1) arr[0] = 0;
    if (N >= 2) arr[1] = 1;

    for (int i = 2; i < N; i++) {
        arr[i] = arr[i - 1] + arr[i - 2];
    }

    pthread_exit(NULL);
}

// Función main
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "N debe ser > 0\n");
        return 1;
    }

    long long* fib = (long long*) malloc(N * sizeof(long long));
    if (fib == NULL) {
        perror("malloc");
        return 1;
    }

    pthread_t worker;
    fibn_argumentos_t args;
    args.arr = fib;
    args.N = N;

    // crear hilo
    pthread_create(&worker, NULL, fib_worker, &args);

    // esperar a que el hilo termine
    pthread_join(worker, NULL);

    // imprimir la secuencia
    for (int i = 0; i < N; i++) {
        printf("%lld ", fib[i]);
    }
    printf("\n");

    free(fib);
    return 0;
}
