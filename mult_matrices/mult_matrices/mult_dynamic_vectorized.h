#include "defines.h"
#include <windows.h>

typedef struct {
    int tamano;
    int puntos;
    int renglon;
    int columna;
    float* Ai;
    float* Bj;
    float* C;
} VecPuntoParams;

SYSTEM_INFO sysinfo;

static DWORD WINAPI multiplica_vec_punto(LPVOID arg);

// Multiplica 2 matrices particionando el trabajo por punto en la matriz resultante
// Espera que la matriz b esté transpuesta
// Utiliza tantos threads como cores haya disponibles en el sistema.
void multiplica_vec_producto(int size, float* matriz_a, float* matriz_b, float* matriz_c) {	
    int i, j;
    int th_counter = 0, th_i = 0, num_worker = 0, puntos_worker = 0;
    float *Ci; 	
    VecPuntoParams* th_vec_params;

    GetSystemInfo(&sysinfo);
    num_worker = sysinfo.dwNumberOfProcessors - 1;
    if(num_worker > SIZE) {
        num_worker = SIZE;
    }

    th_vec_params = (VecPuntoParams*)malloc(sizeof(VecPuntoParams)*num_worker);
    puntos_worker = (size*size)/num_worker;

    for(i=0; i < size; i++) {	
        Ci = &matriz_c[i*size];
        // Se asume que j salta de 2 en 2, 4 en 4, 8 en 8
        // Se asume que se trata de un ambiente en que renglones%number_of_processors == 0
        for(j=0; j < size; j+=puntos_worker) {						
            // Delegar cada punto a un procesador utilizando WorkPool pattern
            th_vec_params[th_i].tamano = size;			
            th_vec_params[th_i].Ai = &matriz_a[j*size];
            th_vec_params[th_i].Bj = &matriz_b[j*size];
            th_vec_params[th_i].C = &Ci[j];
            th_vec_params[th_i].puntos = puntos_worker;
            th_vec_params[th_i].renglon = i;
            th_vec_params[th_i].columna = j;

            // invoca hilo
            th_handles[th_i] = CreateThread(
                NULL, 0, multiplica_vec_punto, 
                &th_vec_params[th_i], 0, NULL);

            // si todos los workers ya tienen trabajo			
            if(th_counter >= num_worker - 1) {
                // Espera a a que alguno termine.
                th_i = WaitForMultipleObjects(num_worker, th_handles, FALSE, INFINITE);
            } else {
                th_counter++;
                th_i = th_counter;
            }
        }
    }

    WaitForMultipleObjects(num_worker, th_handles, TRUE, INFINITE);

    free(th_vec_params);
}

// calcula la celda en la posición (ren, col) de la matriz resultante
// espera que la matriz sea cuadrada, y tamano representa la anchura y
// altura del cuadrado
static DWORD WINAPI multiplica_vec_punto(LPVOID p_arg) {
    VecPuntoParams arg = *(VecPuntoParams*)p_arg;
    int inicio = 0, fin = 0, i = 0, j = 0, k = 0;
    int cuenta  = 0;
    int renglones = (arg.puntos+arg.columna)/arg.tamano;
    float *Ai, *Bj, *Ci;
    float temp[SIZE + 4];

    temp[0] = temp[1] = temp[2] = temp[3] = 0;
    for(i = 0; i < renglones; i++) {
        Ci = &arg.C[i*arg.tamano];
        inicio = i == 0 ? arg.columna : 0;
        fin = arg.puntos - cuenta < arg.tamano ? arg.puntos - cuenta : arg.tamano;

        for(j = inicio; j < fin; j++) {
            Ai = &arg.Ai[j*arg.tamano];
            Bj = &arg.Bj[j*arg.tamano];			
            #pragma ivdep
            for(k = 0; k < arg.tamano;k++) {
                temp[k+4] = temp[k] + Ai[k] * Bj[k];                
            }

            Ci[j] = temp[SIZE + 0] + temp[SIZE + 1] + temp[SIZE + 2] + temp[SIZE + 3];

            cuenta++;
        }
    }

    return 0;
}