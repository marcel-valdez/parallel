#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "common_functions.h"

void openmp_calc(long int A[HEIGHTA][WIDTHA], long int B[WIDTHB][HEIGHTB], long int C[HEIGHTA][WIDTHB]);

int main() {
    printf("Multiplicacion de matrices %dx%d X %dx%d\n", HEIGHTA, WIDTHA, HEIGHTB, WIDTHB);        
    clear_matriz(C);    
    transpose(B, TRANS_B);
    ejecuta_prueba("calculo con openmp", A, TRANS_B, C, openmp_calc);

    return 0;
}

void openmp_calc(long int A[HEIGHTA][WIDTHA], long int B[WIDTHB][HEIGHTB], long int C[HEIGHTA][WIDTHB]) {
    int i, j, k;    
    for(i = 0; i < HEIGHTA; i++) {           
        for(j = 0; j < WIDTHB; j++) {
            #pragma omp parallel for
            for(k = 0; k < WIDTHA; k++) {
                C[i][j] += A[i][k] * B[j][k];                
            }            
        }
    }
}