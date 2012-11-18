#ifndef __MULT_PRODUCTO_H
#define __MULT_PRODUCTO_H
#include "defines.h"

typedef struct { 
    long int *Ai;
    long int *Bj;
    long int *Cij;
} PuntoArg;

static DWORD WINAPI multiplica_punto_vec(LPVOID arg);

PuntoArg *th_arg;
HANDLE *th_handles;
SYSTEM_INFO sysinfo;

// Multiplica 2 matrices particionando el trabajo por punto en la matriz resultante
// Espera que la matriz b esté transpuesta
void multiplica_producto_vec(long int A[HEIGHTA][WIDTHA], long int B[WIDTHB][HEIGHTB], long int C[HEIGHTA][WIDTHB]) {	
    int i, j, num_worker, th_counter = 0, th_i = 0;	
        
    GetSystemInfo(&sysinfo);
    num_worker = sysinfo.dwNumberOfProcessors - 1;
    if(num_worker > HEIGHTA*WIDTHB) {
        num_worker = HEIGHTA*WIDTHB;
    }

    th_arg = (PuntoArg*)malloc(sizeof(PuntoArg)*num_worker);
    th_handles = (HANDLE*)malloc(sizeof(HANDLE)*num_worker);

    for(i=0; i < HEIGHTA; i++) {				
        for(j=0; j < WIDTHB; j++) {
            // Delegar cada punto a un procesador utilizando WorkPool pattern            
            th_arg[th_i].Ai = A[i];			
            th_arg[th_i].Bj = B[j];			
            th_arg[th_i].Cij = &C[i][j];	

            // invoca hilo
            th_handles[th_i] = CreateThread(
                NULL, 0, multiplica_punto_vec, 
                &th_arg[th_i], 0, NULL);

            th_counter++;

            // si todos los workers ya tienen trabajo
            if(th_counter >= num_worker) {		
                // Espera a a que alguno termine.
                th_i = WaitForMultipleObjects(num_worker, th_handles, FALSE, INFINITE);
            } else {				
                th_i = th_counter;
            }
        }
    }

    // Asegurarse de esperar a todos los threads.
    WaitForMultipleObjects(num_worker, th_handles, TRUE, INFINITE);

    free(th_arg);
    free(th_handles);
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
    int k = 0;
    long int *Ai = arg.Ai, *Bj = arg.Bj, *Cij = arg.Cij;
    long int temp[WIDTHA + 4];
    temp[0] = temp[1] = temp[2] = temp[3];
#pragma ivdep
    for(k = 0; k < WIDTHA; k++) {
        temp[k+4] = temp[k] + Ai[k] * Bj[k];
    }

    *Cij = temp[WIDTHA] + temp[WIDTHA+1] + temp[WIDTHA+2] + temp[WIDTHA+3];

    return 0;
}
#endif