#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ia32intrin.h>
#include "defines.h"
void transpose(int size, float* matriz, float* trans);
void multiplica(int size, float* matriz_a, float* matriz_b, float* matriz_c);
void print_matriz(int size, float* matriz);

int main(int argc, char* argv[]) {
	printf("Original B:\n");
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

void transpose(int size, float* source, float* trans) {	
	int i, j;
	float *trans_j, *src_i, tmp;
	// loop i from 1 to size
	for(i = 0; i < size; i++) {
		src_i = &source[i*size];
		// loop j from 1 to size
		for(j = 0; j < size; j++) {
			trans_j = &trans[j*size];
			printf("\tTranspose:\n");
			printf("\t\t(%d,%d) <-> (%d, %d)\n", i, j, j, size-1-i);
			// trans[i][j] = source[j][size-1-i]
			trans_j[size-1-i] = src_i[j];
		}
	}

	PDEBUG("finished transposing\n\n");
}

void multiplica(int size, float* matriz_a, float* matriz_b, float* matriz_c) {
	// loop i from 1 to size
	int i, j, k;
	float *a_row_i, *b_row_i, *c_row_i; 
	for(i=0; i < size; i++) {
		c_row_i = &matriz_c[i*size];
		// loop j from 1 to size
		for(j = 0; j < size; j++) {
			a_row_i = &matriz_a[j*size];
			b_row_i = &matriz_b[j*size];
			// vectorize this
			VECTORIZE_THIS
			// loop k from 1 to size by 4
			c_row_i[j] = 0;
			for(k=0; k < size; k+=INC) {
				// c[i][j] = a[j][k]*b[j][k]
				c_row_i[j] += a_row_i[k] * b_row_i[k];
			}
		}
	}	
}

void print_matriz(int size, float* matriz) {
	int i, j;
	float* row;
	printf("\n");
	for(i = 0; i < size; i++) {
		row = &matriz[i*size];		
		for(j = 0; j < size; j++) {
			printf("(%d,%d):%.0f ", i, j, row[j]);
		}
		printf("\n");
	}
	printf("\n");
}