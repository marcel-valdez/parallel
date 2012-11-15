#include "defines.h"
#include <windows.h>

typedef struct {
	int lado;
	int renglon;
	float* matriz_a;
	float* matriz_b;
	float* renglon_c;
} RenglonParams;


static DWORD WINAPI multiplica_renglon(LPVOID arg);

RenglonParams th_renglon_params[NUM_WORKER];

// Multiplica 2 matrices particionando el trabajo por punto en la matriz resultante
// Espera que la matriz b esté transpuesta
void multiplica_renglones(int size, float* matriz_a, float* matriz_b, float* matriz_c) {	
	int i, j;
	float *a_renglon_i, *b_renglon_i, *c_renglon_i; 
	int th_counter = 0, th_i = 0;	

	for(i=0; i < size; i++) {		
		c_renglon_i = &matriz_c[i*size];
		th_renglon_params[th_i].lado = size;
		th_renglon_params[th_i].renglon = i;
		th_renglon_params[th_i].matriz_a = matriz_a;
		th_renglon_params[th_i].matriz_b = matriz_b;
		th_renglon_params[th_i].renglon_c = c_renglon_i;

		// Delegar cada punto a un procesador utilizando WorkPool pattern			
		// invoca hilo
		th_handles[th_i] = CreateThread(
			NULL, 0, multiplica_renglon, 
			&th_renglon_params[th_i], 0, NULL);			
		th_counter++;

		// si todos los workers ya tienen trabajo
		if(th_counter >= NUM_WORKER) {				
			// Espera a a que alguno termine.
			th_i = WaitForMultipleObjects(
				NUM_WORKER, th_handles, 
				FALSE, INFINITE);
		} else {				
			th_i = th_counter;
		}
	}	
}

// calcula la celda en la posición (ren, col) de la matriz resultante
// espera que la matriz sea cuadrada, y lado representa la anchura y
// altura del cuadrado
static DWORD WINAPI multiplica_renglon(LPVOID arg) {
	RenglonParams* params = (RenglonParams*)arg;
	int lado = params->lado;
	float* matriz_a = params->matriz_a;
	float* a_renglon_i;
	float* b_columna_i;
	float* matriz_b = params->matriz_b;
	float* renglon_c = params->renglon_c;
	int i, j;
	for(j=0; j < lado; j++) {
		a_renglon_i = &matriz_a[j*lado];			
		b_columna_i = &matriz_b[j*lado];			
		renglon_c[j] = 0;

		for(i = 0; i < lado; i++) {
			renglon_c[j] += a_renglon_i[i] * b_columna_i[i];
		}	
	}

	return 0;
}