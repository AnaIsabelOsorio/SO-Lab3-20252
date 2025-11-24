/*
 *  OpenMP lecture exercises
 *  Copyright (C) 2011 by Christian Terboven <terboven@rz.rwth-aachen.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

double CalcPi(int n, int T);
double f(double a);

// estructura para los argumentos de cada hilo
typedef struct {
    int start;
    int end;
    double h;
} thread_args_t;

double GetTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1e6;
}

// función que ejecuta cada hilo
void* thread_func(void* arg) {
    thread_args_t* args = (thread_args_t*) arg;
    int start = args->start;
    int end   = args->end;
    double h  = args->h;

    double local_sum = 0.0;
    double x;

    for (int i = start; i < end; i++) {
        x = h * ((double)i + 0.5);
        local_sum += f(x);
    }

    // devolvemos la suma parcial
    double* result = (double*) malloc(sizeof(double));
    *result = local_sum;

    pthread_exit((void*)result);
}

int main(int argc, char **argv)
{
    int n = 2000000000;
    int T;   // número de hilos
    const double fPi25DT = 3.141592653589793238462643;
    double fPi;
    double fTimeStart, fTimeEnd;
    
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <n> <T_hilos>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 2000000000 4\n", argv[0]);
        return 1;
    }

    n = atoi(argv[1]);
    T = atoi(argv[2]);

    if (n <= 0 || n > 2147483647 ) 
    {
        printf("\ngiven value has to be between 0 and 2147483647\n");
        return 1;
    }
    if (T <= 0) {
        printf("\nEl número de hilos T debe ser > 0\n");
        return 1;
    }
    
    fTimeStart = GetTime();

    // cálculo paralelo
    fPi = CalcPi(n, T);

    fTimeEnd = GetTime();
    
    printf("\npi is approximately = %.20f \nError               = %.20f\n",
           fPi, fabs(fPi - fPi25DT));
    
    printf("Tiempo (paralelo) T=%d = %.6f segundos\n", T, fTimeEnd - fTimeStart);

    return 0;
}

double f(double a)
{
    return (4.0 / (1.0 + a*a));
}

// versión paralela de CalcPi
double CalcPi(int n, int T)
{
    const double h = 1.0 / (double) n;

    pthread_t* threads = (pthread_t*) malloc(T * sizeof(pthread_t));
    thread_args_t* args = (thread_args_t*) malloc(T * sizeof(thread_args_t));

    int chunk = n / T;
    int remainder = n % T;

    int start = 0;

    // crear hilos
    for (int t = 0; t < T; t++) {
        int end = start + chunk;
        if (t == T - 1) {   // el último hilo se lleva el resto
            end += remainder;
        }

        args[t].start = start;
        args[t].end   = end;
        args[t].h     = h;

        pthread_create(&threads[t], NULL, thread_func, &args[t]);

        start = end;
    }

    // recoger resultados parciales
    double total_sum = 0.0;
    for (int t = 0; t < T; t++) {
        void* ret;
        pthread_join(threads[t], &ret);
        double* partial_sum = (double*) ret;
        total_sum += *partial_sum;
        free(partial_sum);
    }

    free(threads);
    free(args);

    return h * total_sum;
}
