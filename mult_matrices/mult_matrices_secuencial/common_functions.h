#ifndef __COMMON
#define __COMMON
#include <stdio.h>
#include <time.h>
#include "defines.h"

void print_matriz(int width, int height, long int *matriz) {
    int i, j;    
    printf("\n");
    for(i = 0; i < height; i++) {        
        for(j = 0; j < width; j++) {
            printf("(%d,%d):", i, j);
            printf("%d ", matriz[(i*width)+j]);
        }

        printf("\n");
    }

    printf("\n");
}

void transpose(long int source[HEIGHTB][WIDTHB], long int trans[WIDTHB][HEIGHTB]) {
    int i, j;    
    long int source_ij;    
    for(i = 0; i < HEIGHTB; i++) {
        for(j = 0; j < WIDTHB; j++) {            
            trans[j][HEIGHTB-1-i] = source[i][j];
        }       
    }

    PDEBUG("finished transposing\n\n");
}

void ejecuta_prueba(
    const char* granularidad,
    long int matriz_a[HEIGHTA][WIDTHA], 
    long int matriz_b[WIDTHB][HEIGHTB], 
    long int matriz_c[HEIGHTA][WIDTHB],
    void (*funcion_producto)(long int[HEIGHTA][WIDTHA], long int[WIDTHB][HEIGHTB], long int[HEIGHTA][WIDTHB])) {
    clock_t start, end;
    printf("Mutiplicando con granularidad: %s\n", granularidad);
    start = clock();
    funcion_producto(matriz_a, matriz_b, matriz_c);
    end = clock();
    
    printf("Tiempo de ejecucion: %ld miliseconds\n", end-start);
    if(HEIGHTA + WIDTHB < 32 ) {
        //printf("Resultado AxB: \n");
        //print_matriz(HEIGHTA, WIDTHB, &C[0][0]);
    }
}

void clear_matriz(long int matriz[HEIGHTA][WIDTHB]) {
    int i = 0, j = 0;
    for(i = 0; i < HEIGHTA; i++) {
        for(j = 0; j < WIDTHB; j++) {
            matriz[i][j] = 0;
        }
    }
}

#endif