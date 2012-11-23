#include <stdio.h>
#include "timer.h"

double base = 0;
const double PI =  3.141592653589793;

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

    base = 1.0/intervalos;
    StartTimer();
    printf("\nCalculando pi con %lld intervalos\n", intervalos);
    /* code */    
    pi = calc_pi(intervalos, base);
    elapsed = GetTimer();

    printf("PI=%1.15lf\n", pi);
    printf("Error: %1.15lf\n", PI - pi);
    printf("Tiempo: %4.2lf ms\n", elapsed);
}

/* version secuencial */
double calc_pi(long long int cantidad_intervalos, double base)
{
    long long int i = 0;
    double acum = 0;
    double x = base;
    for(i = 0; i < cantidad_intervalos; i++) {        
        acum += 4/(1 + (x * x));
        x += base;
    }
    
    return acum*base;
}