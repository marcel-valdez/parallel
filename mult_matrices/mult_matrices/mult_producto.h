#ifndef __MULT_PRODUCTO_H
#define __MULT_PRODUCTO_H
#include "defines.h"
#include <windows.h>

typedef struct { 
    int tamano;
    float *Ai;
    float *Bj;
    float *Cij;
} PuntoArg;


static DWORD WINAPI multiplica_punto(LPVOID arg);

static DWORD WINAPI multiplica_punto_vec(LPVOID arg);

void multiplica_producto_helper(int size, float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE], BOOL vectorize);

PuntoArg th_arg[NUM_WORKER];

void multiplica_producto(int size, float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE]) {	
    multiplica_producto_helper(size, A, B, C, FALSE);
}

void multiplica_producto_vec(int size, float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE]) {	
    multiplica_producto_helper(size, A, B, C, TRUE);
}

// Multiplica 2 matrices particionando el trabajo por punto en la matriz resultante
// Espera que la matriz b esté transpuesta
void multiplica_producto_helper(int size, float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE], BOOL vectorize) {	
    int i, j;	
    int th_counter = 0, th_i = 0;	

    for(i=0; i < size; i++) {				
        for(j=0; j < size; j++) {			
            C[i][j] = 0;
            // Delegar cada punto a un procesador utilizando WorkPool pattern			
            th_arg[th_i].tamano = size;
            th_arg[th_i].Ai = A[i];			
            th_arg[th_i].Bj = B[j];			
            th_arg[th_i].Cij = &C[i][j];	

            // invoca hilo
            if(vectorize) {
                th_handles[th_i] = CreateThread(
                    NULL, 0, multiplica_punto_vec, 
                    &th_arg[th_i], 0, NULL);
            } else {
                th_handles[th_i] = CreateThread(
                    NULL, 0, multiplica_punto, 
                    &th_arg[th_i], 0, NULL);
            }

            th_counter++;						
            // si todos los workers ya tienen trabajo

            if(th_counter >= NUM_WORKER) {				
                // Espera a a que alguno termine.
                th_i = WaitForMultipleObjects(
                    NUM_WORKER, th_handles, 
                    FALSE, INFINITE);
            } else {				
                th_i = th_counter;
            }
        }
    }	
}


/// <summary>
/// Multiplica el punto PuntoArg.Cij de la matriz resultante
/// de la multiplicacion del renglon PuntoArg.Ai con el
/// renglon PuntoArg.Bj, para una matriz de tamano PutoArg.tamano.
/// </summary>
/// <param name="p_arg">The p_arg.</param>
/// <returns>DWORD.</returns>
static DWORD WINAPI multiplica_punto(LPVOID p_arg) {
    PuntoArg arg = *(PuntoArg*)p_arg;
    int lado = arg.tamano, k = 0;
    float *Ai = arg.Ai, *Bj = arg.Bj, *Cij = arg.Cij;

    for(k = 0; k < lado; k++) {
        (*Cij) += Ai[k] * Bj[k];
    }

    return 0;
}


/// <summary>
/// Utiliza vectorizacion para:
/// Multiplicar el punto PuntoArg.Cij de la matriz resultante
/// de la multiplicacion del renglon PuntoArg.Ai con el
/// renglon PuntoArg.Bj, para una matriz de tamano PutoArg.tamano.
/// </summary>
/// <param name="p_arg">The p_arg.</param>
/// <returns>DWORD.</returns>
static DWORD WINAPI multiplica_punto_vec(LPVOID p_arg) {
    PuntoArg arg = *(PuntoArg*)p_arg;
    int lado = arg.tamano, k = 0;
    float *Ai = arg.Ai, *Bj = arg.Bj, *Cij = arg.Cij;
    float temp[SIZE + 4];
    temp[0] = temp[1] = temp[2] = temp[3];
#pragma ivdep
    for(k = 0; k < lado; k++) {
        temp[k+4] = temp[k] + Ai[k] * Bj[k];
    }

    *Cij = temp[SIZE] + temp[SIZE+1] + temp[SIZE+2] + temp[SIZE+3];

    return 0;
}
#endif