#include <stdio.h>
#include <stdlib.h>
#include <ia32intrin.h>
#include "defines.h"
#include "common_functions.h"
#include "mult_producto.h"
#include "mult_renglon.h"
#include "mult_dynamic_worker.h"

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
    ejecuta_prueba("celda", A, TRANS_B, C, multiplica_producto);
    ejecuta_prueba("renglon", A, TRANS_B, C, multiplica_renglones);
    ejecuta_prueba("celdas:dinamico", A, TRANS_B, C, multiplica_dyn_producto);
#ifdef DEBUG
    printf("Expected result:\n");
    print_matriz(SIZE, EXPECT_C);
#endif
    return 0;
}