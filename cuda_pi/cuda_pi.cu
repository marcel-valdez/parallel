#include "cuda_pi.h"

const double PI =  3.141592653589793;
double base = 0.0;

// Se utiliza un array donde se guardan los resultados parciales
// de cada streaming-thread
double partials[TH_COUNT];
double *gpu_partials;

__global__ void calc_pi_th(
    double *partial_results,
    long long int num_iteraciones,
    double base)
{    
    // indice global del thread (1D)
    int th_id = blockIdx.x * blockDim.x + threadIdx.x;
    // Se desea manejar cantidad enormes de intervalos
    long long int i = 0;
    // Valores para calculo de PI
    double acum = 0, x;
    // Se asume que la cantidad de intervalos es multiplo
    // de la cantidad de threads simultaneos.
    // Se inicia a calcular acorde al indice del hilo.
    x = base * (th_id) * num_iteraciones;    
    #pragma unroll
    for(i = 0; i < num_iteraciones; i++) {
        acum += 4/(1 + x * x);
        x += base;
    }

    partial_results[th_id] = acum;
}

int main(int argc, char const *argv[])
{    
    // cuda malloc
    cudaMalloc((void**)&gpu_partials, sizeof(double)*TH_COUNT);
    /* 245,760 */
    run_pi_calc(245760);
    /* 24,760,000 */
    run_pi_calc(24576000);
    /* 2,457,600,000 */
    run_pi_calc(2457600000);
    // cuda free
    cudaFree(gpu_partials);
    cudaDeviceReset();
}

void run_pi_calc(long long int intervalos)
{        
    double pi = 0.0;
    double elapsed;
    base = 1.0/intervalos;

    StartTimer();    
    printf("\nCalculando pi con %lld intervalos\n", intervalos);    

    map_pi_calc(intervalos, base);
    pi = reduce_pi_calc()*base;

    elapsed = GetTimer();
    printf("PI=%1.15lf\n", pi);
    printf("Error: %1.15lf\n", PI - pi);
    printf("Tiempo: %4.2lf ms\n", elapsed);
}

// maps the pi calculation to each device
void map_pi_calc(long long int intervalos, double base)
{
    long long int intervalo_count = intervalos / TH_COUNT;    
    // ejecutar calculo    
    calc_pi_th<<< GRID_SIZE, BLOCK_SIZE >>>(gpu_partials, intervalo_count, base);
    // memcpy gpu -> cpu
    cudaMemcpy(partials, gpu_partials, sizeof(double)*TH_COUNT, cudaMemcpyDeviceToHost);
}

// integrates the results of all the streaming threads
double reduce_pi_calc()
{
    double acum = 0;
    int i = 0;
    for(i = 0; i < TH_COUNT; i++) {
        acum += partials[i];
    }

    return acum;
}