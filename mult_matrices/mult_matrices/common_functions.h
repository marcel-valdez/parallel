#ifndef __COMMON
#define __COMMON
#include <stdio.h>
#include "defines.h"

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

void transpose(int size, float* source, float* trans) {	
	int i, j;
	float *trans_j, *src_i;
	for(i = 0; i < size; i++) {
		src_i = &source[i*size];
		for(j = 0; j < size; j++) {
			trans_j = &trans[j*size];
			trans_j[size-1-i] = src_i[j];
		}
	}

	PDEBUG("finished transposing\n\n");
}
#endif