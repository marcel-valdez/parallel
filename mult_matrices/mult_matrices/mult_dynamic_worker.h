#include "defines.h"
#include <windows.h>

typedef struct {
	int lado;
	int puntos;
	int renglon;
	int columna;
	float* matriz_a;
	float* matriz_b;
	float* seccion_c;
} DynPuntoParams;

SYSTEM_INFO sysinfo;

static DWORD WINAPI multiplica_dyn_punto(LPVOID arg);

// Multiplica 2 matrices particionando el trabajo por punto en la matriz resultante
// Espera que la matriz b esté transpuesta
// Utiliza tantos threads como cores haya disponibles en el sistema.
void multiplica_dyn_producto(int size, float* matriz_a, float* matriz_b, float* matriz_c) {	
	int i, j;
	int th_counter = 0, th_i = 0, num_worker = 0, puntos_worker = 0;
	float *c_renglon_i; 	
	DynPuntoParams* th_dyn_params;

	GetSystemInfo(&sysinfo);
	num_worker = sysinfo.dwNumberOfProcessors;
	th_dyn_params = (DynPuntoParams*)malloc(sizeof(DynPuntoParams)*num_worker);
	puntos_worker = (size*size)/num_worker;

	for(i=0; i < size; i++) {		
		c_renglon_i = &matriz_c[i*size];
		// j salta de 2 en 2, 4 en 4, 8 en 8
		// Se asume que se trata de un ambiente en que renglones%number_of_processors == 0
		for(j=0; j < size; j+=puntos_worker) {						
			// Delegar cada punto a un procesador utilizando WorkPool pattern
			th_dyn_params[th_i].lado = size;			
			th_dyn_params[th_i].matriz_a = matriz_a;			
			th_dyn_params[th_i].matriz_b = matriz_b;			
			th_dyn_params[th_i].seccion_c = &c_renglon_i[j];
			th_dyn_params[th_i].puntos = puntos_worker;
			th_dyn_params[th_i].renglon = i;
			th_dyn_params[th_i].columna = j;

			// invoca hilo
			th_handles[th_i] = CreateThread(
				NULL, 0, multiplica_dyn_punto, 
				&th_dyn_params[th_i], 0, NULL);

			// si todos los workers ya tienen trabajo			
			if(th_counter >= num_worker - 1) {
				// Espera a a que alguno termine.
				th_i = WaitForMultipleObjects(num_worker, th_handles, FALSE, INFINITE);
			} else {
				th_counter++;
				th_i = th_counter;
			}
		}
	}

	WaitForMultipleObjects(num_worker, th_handles, TRUE, INFINITE);

	free(th_dyn_params);
}

// calcula la celda en la posición (ren, col) de la matriz resultante
// espera que la matriz sea cuadrada, y lado representa la anchura y
// altura del cuadrado
static DWORD WINAPI multiplica_dyn_punto(LPVOID arg) {
	DynPuntoParams* params = (DynPuntoParams*)arg;
	int i, j = params->renglon, puntos = params->puntos;	
	int lado = params->lado;
	float* matriz_a = params->matriz_a;
	float* matriz_b = params->matriz_b;
	float* seccion_c = params->seccion_c;
	float* celda_c, *renglon_a, *columna_b;	

	for(j = 0; j < params->puntos; j++) {
		celda_c = &seccion_c[j];
		renglon_a = &matriz_a[j*lado];
		columna_b = &matriz_b[j*lado];
		*celda_c = 0;
		for(i = params->columna; i < lado; i++) {
			(*celda_c) += renglon_a[i] * columna_b[i];
		}
	}

	return 0;
}