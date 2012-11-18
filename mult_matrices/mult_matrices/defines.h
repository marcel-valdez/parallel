#ifndef __DEFINES_H
#define __DEFINES_H

#include <ia32intrin.h>
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

#define HEIGHTA 8//224//224//112//64//56*2*2//*2*2//8//16//56*2*2
#define WIDTHA 16

#define HEIGHTB WIDTHA
#define WIDTHB 8

#if WIDTHA >= 8
#define NUM_PROCS 8
#else
#define NUM_PROCS 5
#endif

#define NUM_WORKER (NUM_PROCS-1)
HANDLE th_handles[NUM_WORKER];

#include "data.h"

long int TRANS_B[WIDTHB][HEIGHTB];

long int C[HEIGHTA][WIDTHB];
#endif