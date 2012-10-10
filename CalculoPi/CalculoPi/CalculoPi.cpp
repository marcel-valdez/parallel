// CalculoPi.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <windows.h>
#include <strsafe.h>
#include "stdio.h"
#include "time.h"
#define CANTIDAD_INTERVALOS 10000000
//#define NUM_THREADS 2

void execute_in_parallel();
DWORD WINAPI partial_pi(LPVOID pArg);

clock_t start;
clock_t end;
int NUM_THREADS;

int _tmain(int argc, _TCHAR* argv[])
{	
    printf("Iniciando ejecucion en paralelo\n");
    execute_in_parallel();
    return(0);
}

CRITICAL_SECTION cs;
double result = 0.0;
SYSTEM_INFO sysinfo;

void execute_in_parallel()
{	    
    GetSystemInfo(&sysinfo);
    NUM_THREADS = sysinfo.dwNumberOfProcessors;
    HANDLE* hThread;
    hThread = (HANDLE*)malloc(NUM_THREADS * sizeof(HANDLE));
    int* parameters;
    parameters = (int*)malloc(NUM_THREADS * sizeof(int));

    int i;	
    printf("Parametros de ejecucion: cores disponibles= %d, intervalos= %d\n", NUM_THREADS, CANTIDAD_INTERVALOS);
    start = clock();
    InitializeCriticalSection(&cs);
    for(i = 0; i < NUM_THREADS; i++) {
        parameters[i] = i;
        hThread[i] = CreateThread(NULL, 0, partial_pi, &parameters[i], 0, NULL);
    }
    
    WaitForMultipleObjects(NUM_THREADS, hThread, TRUE, INFINITE);
    DeleteCriticalSection(&cs);

    end = clock();

    printf("PI=%15.12lf (%ld)\n", result, end - start);

    free(hThread);
    free(parameters);
}

double base = 1.0 / CANTIDAD_INTERVALOS;

DWORD WINAPI partial_pi(LPVOID pArg) 
{
    int i = 0;	
    double altura;
    double acum;
    int* index_ptr = (int*)pArg;
    int intervalos_por_thread = CANTIDAD_INTERVALOS / NUM_THREADS;
    double x = (*index_ptr) * base * intervalos_por_thread;
    int mis_intervalos = intervalos_por_thread + (*index_ptr == NUM_THREADS - 1 ? CANTIDAD_INTERVALOS % NUM_THREADS : 0);

    for(i = 0, acum = 0.0; i < mis_intervalos; i++, x+= base) {
        altura = 4.0/(1 + x*x);
        acum += base * altura;
        // Se puede usar una variable global con todos los números
        // results[*index_ptr] = acum;
        // Pero si se hace esto, entonces va a pedir acceso a todo el arreglo results
        // que está en L2, y ocupará cargarlo a L1		
    }

    // Se puede agregar todo a un resultado común
    EnterCriticalSection(&cs);
    result += acum;
    LeaveCriticalSection(&cs);

    return 0;
}

// 
// LPVOID p: (Long Pointer to Thread): 
// LPVOID Paramteers (Long Pointer to Void): Parámetros del thread
// DWORD CreationFlags (Double Word): ?
// LPDWORD ThreadID (Long Pointer to Word): Número o ID del thread