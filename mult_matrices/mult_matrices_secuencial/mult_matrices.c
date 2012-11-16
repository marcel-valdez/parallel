#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "common_functions.h"

void secuencial(int tamano, float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE]);

int main(int argc, char* argv[]) {
    printf("Multiplicacion de matrices %dx%d\n", SIZE, SIZE);
    if(SIZE < 16) {
        printf("Matriz A:\n");
        print_matriz(SIZE, A);
        printf("Matriz B:\n");
        print_matriz(SIZE, B);
    }

    transpose(SIZE, B, TRANS_B);
    ejecuta_prueba("secuencial", A, TRANS_B, C, secuencial);
    return 0;
}

void secuencial(int tamano, float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE]) {
    int i, j, k;    
    for(i = 0; i < tamano; i++) {        
        for(j = 0; j < tamano; j++) {        
            for(k = 0; k < tamano; k++) {
                C[i][j] += A[i][k] * B[j][k];
            }
        }
    }
}