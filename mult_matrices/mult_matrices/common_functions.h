#ifndef __COMMON
#define __COMMON
#include <stdio.h>
#include <time.h>
#include "defines.h"

void print_matriz(int size, float matriz[SIZE][SIZE]) {
    int i, j;    
    printf("\n");
    for(i = 0; i < size; i++) {        
        for(j = 0; j < size; j++) {
            printf("(%d,%d):%.0f ", i, j, matriz[i][j]);
        }

        printf("\n");
    }

    printf("\n");
}

void transpose(int size, float source[SIZE][SIZE], float trans[SIZE][SIZE]) {	    
    int i, j;    
    float source_ij;    
    for(i = 0; i < size; i++) {
        for(j = 0; j < size; j++) {            
            trans[j][size-1-i] = source[i][j];                        
        }        
    }

    PDEBUG("finished transposing\n\n");
}

void ejecuta_prueba(
    const char* granularidad,
    float matriz_a[SIZE][SIZE], 
    float matriz_b[SIZE][SIZE], 
    float matriz_c[SIZE][SIZE],
    void (*funcion_producto)(int, float[SIZE][SIZE], float[SIZE][SIZE], float[SIZE][SIZE])) {
    clock_t start, end;
    printf("Mutiplicando con granularidad: %s\n", granularidad);
    start = clock();
    funcion_producto(SIZE, matriz_a, matriz_b, matriz_c);
    end = clock();
    
    printf("Tiempo de ejecucion: %ld miliseconds\n", end-start);
    if(SIZE < 16) {
        printf("Resultado AxB: \n");
        print_matriz(SIZE, C);
    }
}

void clear_matriz(int tamano, float matriz[SIZE][SIZE]) {
    int i = 0, j = 0;
    for(i = 0; i < tamano; i++)  {
        for(j = 0; j < tamano; j++) {
            matriz[i][j] = 0;
        }
    }
}
#endif