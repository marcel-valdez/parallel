#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ia32intrin.h>
#include "defines.h"
#include "common_functions.h"

void transpose(int size, float* matriz, float* trans);
void multiplica(int size, float* matriz_a, float* matriz_b, float* matriz_c);

int main(int argc, char* argv[]) {
	printf("Matrix A:\n");
	print_matriz(SIZE, A);
	printf("Matrix B:\n");
	print_matriz(SIZE, B);
	transpose(SIZE, B, TRANS_B);	
	// transpose 8, B
	printf("Tranposed B:\n");
	print_matriz(SIZE, TRANS_B);
	// multiplica 8, A, B
	printf("Result AxB: \n");
	multiplica(SIZE, A, TRANS_B, C);
	// print C
	print_matriz(SIZE, C);

	//printf("Expected result:\n");
	//print_matriz(SIZE, EXPECT_C);
	return 1;
}

void multiplica(int size, float* matriz_a, float* matriz_b, float* matriz_c) {
	int i, j, k;
	float *a_row_i, *b_row_i, *c_row_i; 
	for(i=0; i < size; i++) {
		c_row_i = &matriz_c[i*size];
		for(j = 0; j < size; j++) {
			a_row_i = &matriz_a[j*size];
			b_row_i = &matriz_b[j*size];
			c_row_i[j] = 0;
			for(k=0; k < size; k+=INC) {
				c_row_i[j] += a_row_i[k] * b_row_i[k];
			}
		}
	}	
}