#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ia32intrin.h>
#include "defines.h"
#include "common_functions.h"
#include "mult_producto.h"

clock_t start, end;

int main(int argc, char* argv[]) {
	printf("Matriz A:\n");
	print_matriz(SIZE, A);
	printf("Matriz B:\n");
	print_matriz(SIZE, B);
	transpose(SIZE, B, TRANS_B);	
	// transpose 8, B
#ifdef DEBUG
	PDEBUG("Transpuesta B:\n");
	print_matriz(SIZE, TRANS_B);
#endif
	// multiplica 8, A, B
	printf("Result AxB: \n");
	start = clock();
	multiplica_producto(SIZE, A, TRANS_B, C);
	end = clock();
	printf("Tiempo de ejecucion: %ld millis\n", end - start);
	// print C
	print_matriz(SIZE, C);
#ifdef DEBUG
	printf("Expected result:\n");
	print_matriz(SIZE, EXPECT_C);
#endif
	return 0;
}