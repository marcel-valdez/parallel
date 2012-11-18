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

#define HEIGHTA 16//224//224//112//64//56*2*2//*2*2//8//16//56*2*2
#define WIDTHA 64

#define HEIGHTB WIDTHA
#define WIDTHB 8

#include "data.h"

long int TRANS_B[WIDTHB][HEIGHTB];

long int C[HEIGHTA][WIDTHB];
#endif