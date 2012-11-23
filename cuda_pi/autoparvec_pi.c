#include <stdio.h>
#include "timer.h"

double base = 0;
double four_base;
const double PI =  3.141592653589793;
double partials[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

__forceinline void calc_interval(int t_i, long long int th_interval, double base);
double calc_pi(long long int cantidad_intervalos, double base);
void run_pi_calc(long long int intervalos);

int main()
{
    /* 245,760 */
    run_pi_calc(245760);
    /* 245,760,000 */
    run_pi_calc(24576000);
    /* 2,457,600,000 */
    run_pi_calc(2457600000);
}

void run_pi_calc(long long int intervalos)
{
    double pi = 0;
    double elapsed;

    base = 1.0 / intervalos;
    StartTimer();
    printf("\nCalculando pi con %lld intervalos\n", intervalos);
    /* code */
    pi = calc_pi(intervalos, base);
    elapsed = GetTimer();

    printf("PI=%1.15lf\n", pi);
    printf("Error: %1.15lf\n", PI - pi);
    printf("Tiempo: %4.2lf ms\n", elapsed);
}


double calc_pi(long long int cantidad_intervalos, double base)
{
    int i = 0;
    long long int th_interval = cantidad_intervalos / 8;

    four_base = base + base + base + base;

    // Se indica al compilador que debe paralelizarlo en 8 hilos
    // e ignorar dependendias de memoria encontradas.
#pragma loop(hint_parallel(8))
#pragma loop(ivdep)
    for (i = 0; i < 8; i++)
    {
        calc_interval(i, th_interval, base);
    }

    // Se integran los resultados de los 8 hilos
    return partials[0] + partials[1] + partials[2] + partials[3]
         + partials[4] + partials[5] + partials[6] + partials[7];
}

__forceinline void calc_interval(int t_i, long long int th_interval, double base)
{
    long long int i, end = th_interval / 4;
    int j;
    double x = t_i * th_interval * base;    
    double vector[4];

    // Se calculan los valores correspondientes al intervalo de este hilo
    for (i = 0; i < end; i++)
    {
        // Se crea un for interno para que se vectorize de 4 en 4
        for (j = 0; j < 4; j++)
        {
            vector[j] =  4.0 / (1.0 + ((x + base * j) * (x + base * j)));
        }

        // Se realiza una suma de estos 4 valores.
        partials[t_i] += vector[0] + vector[1] + vector[2] + vector[3];
        x = x + four_base;
    }

    partials[t_i] = partials[t_i] * base;
}