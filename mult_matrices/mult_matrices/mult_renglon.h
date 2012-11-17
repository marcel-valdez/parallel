#ifndef __MULT_RENGLON_H
#define __MULT_RENGLON_H

#include "defines.h"
#include <stdio.h>
typedef struct {
    int tamano;
    float *Ai;
    float (*B)[SIZE];
    float *Ci;
} RenglonParams;

static DWORD WINAPI multiplica_renglon(LPVOID arg);
static DWORD WINAPI multiplica_vec_renglon(LPVOID arg);

void multiplica_renglones_helper(
    int size, 
    float matriz_a[SIZE][SIZE], 
    float matriz_b[SIZE][SIZE], 
    float matriz_c[SIZE][SIZE],
    BOOL vectorized);

RenglonParams th_renglon_arg[NUM_WORKER];

void multiplica_renglones(
    int size, 
    float matriz_a[SIZE][SIZE], 
    float matriz_b[SIZE][SIZE], 
    float matriz_c[SIZE][SIZE]) {
        multiplica_renglones_helper(size, matriz_a, matriz_b, matriz_c, FALSE);
}

void multiplica_renglones_vec(
    int size, 
    float matriz_a[SIZE][SIZE], 
    float matriz_b[SIZE][SIZE], 
    float matriz_c[SIZE][SIZE]) {
        multiplica_renglones_helper(size, matriz_a, matriz_b, matriz_c, TRUE);
}

// Multiplica 2 matrices particionando el trabajo por punto en la matriz resultante
// Espera que la matriz b esté transpuesta
void multiplica_renglones_helper(
    int size, 
    float matriz_a[SIZE][SIZE], 
    float matriz_b[SIZE][SIZE], 
    float matriz_c[SIZE][SIZE],
    BOOL vectorized) {
        int i = 0, j = 0, th_counter = 0, th_i = 0;	

        for(i=0; i < size; i++) {		
            th_renglon_arg[th_i].tamano = size;            
            th_renglon_arg[th_i].Ci = matriz_c[i];
            th_renglon_arg[th_i].Ai = matriz_a[i];
            th_renglon_arg[th_i].B = matriz_b;

            // Delegar cada punto a un procesador utilizando WorkPool pattern			
            // invoca hilo
            if(vectorized) {
                th_handles[th_i] = CreateThread(
                    NULL, 0, multiplica_vec_renglon, 
                    &th_renglon_arg[th_i], 0, NULL);			
            } else {
                th_handles[th_i] = CreateThread(
                    NULL, 0, multiplica_renglon, 
                    &th_renglon_arg[th_i], 0, NULL);			
            }

            // si todos los workers ya tienen trabajo
            if(th_counter >= NUM_WORKER - 1) {				
                // Espera a a que alguno termine.
                th_i = WaitForMultipleObjects(
                    NUM_WORKER, th_handles, 
                    FALSE, INFINITE);
            } else {				
                th_counter++;
                th_i = th_counter;
            }
        }

        WaitForMultipleObjects(NUM_WORKER, th_handles, TRUE, INFINITE);
}


// calcula la celda en la posición (ren, col) de la matriz resultante
// espera que la matriz sea cuadrada, y tamano representa la anchura y
// altura del cuadrado
static DWORD WINAPI multiplica_vec_renglon(LPVOID p_arg) {
    RenglonParams arg = *(RenglonParams*)p_arg;
    int j = 0, k = 0;
    float *Bj, *Ai = arg.Ai, *Ci = arg.Ci;
    float temp[SIZE + 4];
    temp[0] = temp[1] = temp[2] = temp[3] = 0;
    for(j=0; j < arg.tamano; j++) {     
        Bj = arg.B[j];
#pragma ivdep
        for(k = 0;k < SIZE; k+=4) {            
            temp[k+4] = temp[k] + Ai[k] * Bj[k];
            temp[k+5] = temp[k+1] + Ai[k+1] * Bj[k+1];
            temp[k+6] = temp[k+2] + Ai[k+2] * Bj[k+2];
            temp[k+7] = temp[k+3] + Ai[k+3] * Bj[k+3];
        }
        
        Ci[j] = temp[SIZE+0] + temp[SIZE+1] + temp[SIZE+2] + temp[SIZE+3];
    }

    return 0;
}

// calcula la celda en la posición (ren, col) de la matriz resultante
// espera que la matriz sea cuadrada, y tamano representa la anchura y
// altura del cuadrado
static DWORD WINAPI multiplica_renglon(LPVOID p_arg) {
    RenglonParams arg = *(RenglonParams*)p_arg;
    int j = 0, k = 0;
    float *Bj, *Ai = arg.Ai, *Ci = arg.Ci;
    for(j=0; j < arg.tamano; j++) {        
        Bj = arg.B[j];
        Ci[j] = 0;
        for(k = 0; k < arg.tamano; k++) {
            Ci[j] += Ai[k] * Bj[k];
        }
    }

    return 0;
}
#endif