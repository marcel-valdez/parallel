#ifndef __CUDA_PI_H
#define __CUDA_PI_H
/// INCLUDES
#include <stdio.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include "timer.h"

// MACROS
// multiprocessor cores * numero de multiprocessors
#define GRID_SIZE (2*48)
// streaming thread count
#define BLOCK_SIZE (32*8)
// total thread count
#define TH_COUNT (GRID_SIZE*BLOCK_SIZE)

// FUNCTIONS
void run_pi_calc(long long int intervalos);
double reduce_pi_calc();
void map_pi_calc(long long int intervalos, double base);

#endif