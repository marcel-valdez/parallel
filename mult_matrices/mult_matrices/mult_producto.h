#include "defines.h"
#include <windows.h>

typedef struct {
   int lado;
   float* renglon_a;
   float* columna_b;
   float* celda_c;
} PuntoParams;


static DWORD WINAPI multiplica_punto(LPVOID arg);

PuntoParams th_params[NUM_WORKER];

// Multiplica 2 matrices particionando el trabajo por punto en la matriz resultante
// Espera que la matriz b esté transpuesta
void multiplica_producto(int size, float* matriz_a, float* matriz_b, float* matriz_c) {	
	int i, j;
	float *a_renglon_i, *b_renglon_i, *c_renglon_i; 
	int th_counter = 0, th_i = 0;	
	
	for(i=0; i < size; i++) {		
		c_renglon_i = &matriz_c[i*size];
		for(j=0; j < size; j++) {			
			a_renglon_i = &matriz_a[j*size];			
			b_renglon_i = &matriz_b[j*size];			
			c_renglon_i[j] = 0;
			// Delegar cada punto a un procesador utilizando WorkPool pattern
			th_params[th_i].lado = size;			
			th_params[th_i].renglon_a = a_renglon_i;			
			th_params[th_i].columna_b = b_renglon_i;			
			th_params[th_i].celda_c = &c_renglon_i[j];			
			
			// invoca hilo
			th_handles[th_i] = CreateThread(
									NULL, 0, multiplica_punto, 
									&th_params[th_i], 0, NULL);
			
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
}

// calcula la celda en la posición (ren, col) de la matriz resultante
// espera que la matriz sea cuadrada, y lado representa la anchura y
// altura del cuadrado
static DWORD WINAPI multiplica_punto(LPVOID arg) {
	PuntoParams* params = (PuntoParams*)arg;
	int lado = params->lado;
	float* renglon_a = params->renglon_a;
	float* columna_b = params->columna_b;
	float* celda_c = params->celda_c;
	int i;
	
	for(i = 0; i < lado; i+=INC) {
		(*celda_c) += renglon_a[i] * columna_b[i];
	}	

	return 0;
}