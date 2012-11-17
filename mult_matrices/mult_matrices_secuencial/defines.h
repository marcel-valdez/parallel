#ifndef __DEFINES
#define __DEFINES
#include <Windows.h>
#ifdef DEBUG
	#define PDEBUG(X) printf(X)
	#define PDEBUG1(X, X1) printf(X, X1)
	#define PDEBUG2(X, X1, X2) printf(X, X1, X2)
	#define PDEBUG3(X, X1, X2) printf(X, X1, X2, X3)
#else
	#define PDEBUG(X) 
	#define PDEBUG1(X, X1)
	#define PDEBUG2(X, X1, X2)
	#define PDEBUG3(X, X1, X2)
#endif

#ifndef MATRIX_SIZE
#define SIZE 56
#else
#define SIZE MATRIX_SIZE
#endif

#if SIZE >= 8
#define NUM_PROCS 8
#else
#define NUM_PROCS 5
#endif

#define NUM_WORKER 7//(NUM_PROCS-1)
HANDLE th_handles[NUM_WORKER];

#include "data.h"

float TRANS_B[SIZE][SIZE];

float C[SIZE][SIZE];
#endif