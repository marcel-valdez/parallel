#include <stdio.h>
#include <stdlib.h>
#include <ia32intrin.h>
#include "defines.h"
#include "common_functions.h"
#include "mult_dynamic_worker.h"

void secuencial(int tamano, long int A[HEIGHTA][WIDTHA], long int B[WIDTHB][HEIGHTB], long int C[HEIGHTA][WIDTHB]);
void secuencial_vec(int tamano, long int A[HEIGHTA][WIDTHA], long int B[WIDTHB][HEIGHTB], long int C[HEIGHTA][WIDTHB]);

int main(int argc, char* argv[]) {
    printf("Multiplicacion de matrices %dx%d X %dx%d\n", HEIGHTA, WIDTHA, HEIGHTB, WIDTHB);
    if(HEIGHTA + WIDTHA + WIDTHB + HEIGHTB <= 64) {
        printf("Matriz A:\n");
        print_matriz(WIDTHA, HEIGHTA, &A[0][0]);
        printf("Matriz B:\n");
        print_matriz(WIDTHB, HEIGHTB, &B[0][0]);
    }
        
    clear_matriz(C);    
    transpose(B, TRANS_B);        
#ifdef DEBUG
    PDEBUG("Transpuesta B:\n");
    print_matriz(SIZE, TRANS_B);
#endif
    //ejecuta_prueba("secuencial", A, TRANS_B, C, secuencial);
    //clear_matriz(C);
    //ejecuta_prueba("secuencial:vectorized", A, TRANS_B, C, secuencial_vec);
    //clear_matriz(C);

    ejecuta_prueba("celdas:dinamico", A, TRANS_B, C, multiplica_dyn_producto);
    clear_matriz(C);
    // ejecuta_prueba("celdas:dinamico:vectorized", A, TRANS_B, C, multiplica_dyn_producto_vec);    
    // clear_matriz(WIDTHA, HEIGHTB, (long int**)C);

    return 0;
}

void secuencial(long int A[HEIGHTA][WIDTHA], long int B[WIDTHB][HEIGHTB], long int C[HEIGHTA][WIDTHB]) {
    int i, j, k;    
#pragma novector
    for(i = 0; i < HEIGHTA; i++) {        
#pragma novector
        for(j = 0; j < WIDTHB; j++) {        
#pragma novector
            for(k = 0; k < WIDTHA; k++) {
#pragma novector
                C[i][j] += A[i][k] * B[j][k];
            }
        }
    }
}

void secuencial_vec(long int A[HEIGHTA][WIDTHA], long int B[WIDTHB][HEIGHTB], long int C[HEIGHTA][WIDTHB]) {
    int i, j, k;    
    long int temp[WIDTHA+4];    
    temp[0] = temp[1] = temp[2] = temp[3] = 0;    

    for(i = 0; i < HEIGHTA; i++)
    {
        for(j = 0; j < WIDTHB; j++) 
        {
            for(k = 0; k < WIDTHA; k+=4) 
            {
                temp[k+4] = temp[k]   + B[j][k] * A[i][k];
                temp[k+5] = temp[k+1] + B[j][k+1] * A[i][k+1];
                temp[k+6] = temp[k+2] + B[j][k+2] * A[i][k+2];
                temp[k+7] = temp[k+3] + B[j][k+3] * A[i][k+3];
            }

            C[i][j] = temp[WIDTHA] + temp[WIDTHA+1] + temp[WIDTHA+2] + temp[WIDTHA+3];
        }
    }
}