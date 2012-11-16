#include <stdio.h>
#include <stdlib.h>
#include <ia32intrin.h>
#include "defines.h"
#include "common_functions.h"
#include "mult_producto.h"
#include "mult_renglon.h"
#include "mult_dynamic_worker.h"
#include "mult_dynamic_vectorized.h"

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
    //print_matriz(SIZE, TRANS_B);
    // transpose 8, B
#ifdef DEBUG
    PDEBUG("Transpuesta B:\n");
    print_matriz(SIZE, TRANS_B);
#endif
   /* ejecuta_prueba("celda", A, TRANS_B, C, multiplica_producto);
    clear_matriz(SIZE, C);*/
    ejecuta_prueba("celdas:dinamico", A, TRANS_B, C, multiplica_dyn_producto);
    clear_matriz(SIZE, C);
    ejecuta_prueba("renglon", A, TRANS_B, C, multiplica_renglones);
    clear_matriz(SIZE, C);
    ejecuta_prueba("renglon:vectorized", A, TRANS_B, C, multiplica_renglones_vec);
    clear_matriz(SIZE, C);
    //ejecuta_prueba("celdas:dinamico:vectorized", A, TRANS_B, C, multiplica_vec_producto);
    //clear_matriz(SIZE, C);
    ejecuta_prueba("secuencial", A, TRANS_B, C, secuencial);
#ifdef DEBUG
    printf("Expected result:\n");
    print_matriz(SIZE, EXPECT_C);
#endif
    return 0;
}

void secuencial(int tamano, float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE]) {
    int i, j, k;    
#pragma novector
    for(i = 0; i < tamano; i++) {        
#pragma novector
        for(j = 0; j < tamano; j++) {        
#pragma novector
            for(k = 0; k < tamano; k++) {
                C[i][j] += A[i][k] * B[j][k];
            }
        }
    }
}