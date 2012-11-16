#include "defines.h"
#include <windows.h>

typedef struct {
    int tamano;
    int puntos;
    int renglon;
    int columna;
    float (*A)[SIZE];
    float (*B)[SIZE];
    float (*C)[SIZE];
} DynPuntoParams;

SYSTEM_INFO sysinfo;

static DWORD WINAPI multiplica_dyn_punto(LPVOID arg);

// Multiplica 2 matrices particionando el trabajo por punto en la matriz resultante
// Espera que la matriz b esté transpuesta
// Utiliza tantos threads como cores haya disponibles en el sistema.
void multiplica_dyn_producto(int size, float matriz_a[SIZE][SIZE], float matriz_b[SIZE][SIZE], float matriz_c[SIZE][SIZE]) {	
    int i,j;
    int th_counter = 0, th_i = 0, num_worker = 0, puntos_worker = 0;
    DynPuntoParams* th_dyn_params;

    GetSystemInfo(&sysinfo);
    num_worker = sysinfo.dwNumberOfProcessors - 1;
    if(num_worker > SIZE) {
        num_worker = SIZE;
    }

    if(SIZE%num_worker != 0) {
        num_worker++;
    }

    th_dyn_params = (DynPuntoParams*)malloc(sizeof(DynPuntoParams)*num_worker);
    // problema, talvez haya que hacer mas de un renglon por procesador
    puntos_worker = (size*size)/num_worker;

    for(i=0; i < size; i++) {
        for(j=0; j < size; j+= puntos_worker) {        
            // Se asume que j salta de 2 en 2, 4 en 4, 8 en 8
            // Se asume que se trata de un ambiente en que renglones%number_of_processors == 0

            // Delegar cada punto a un procesador utilizando WorkPool pattern
            th_dyn_params[th_i].tamano = size;			
            th_dyn_params[th_i].A = &matriz_a[i];
            th_dyn_params[th_i].B = &matriz_b[j];
            th_dyn_params[th_i].C = &matriz_c[i];
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
// espera que la matriz sea cuadrada, y tamano representa la anchura y
// altura del cuadrado
static DWORD WINAPI multiplica_dyn_punto(LPVOID p_arg) {
    DynPuntoParams arg = *(DynPuntoParams*)p_arg;
    int i, j, k, puntos = arg.puntos, renglon, inicio, fin, cuenta = 0;
    int renglones = (arg.puntos+arg.columna)/arg.tamano;
    float *Ci, *Ai, *Bj;
    for(i = 0; i < renglones;i++) {
        Ci = arg.C[i];
        Ai = arg.A[i];
        inicio = i == 0 ? arg.columna : 0;
        fin = arg.puntos - cuenta < arg.tamano ? arg.puntos - cuenta : arg.tamano;        
        for(j = inicio; j < fin; j++) {            
            Bj = arg.B[j];
            Ci[j] = 0;
            for(k = 0; k < arg.tamano; k++) {                
                Ci[j] += Ai[k] * Bj[k];
            }
            
            cuenta++;
        }
    }

    return 0;
}