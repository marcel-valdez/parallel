// ***********************************************************************
// Assembly         : mult_matrices
// Author           : marcel
// Created          : 11-15-2012
//
// Last Modified By : marcel
// Last Modified On : 11-16-2012
// ***********************************************************************
// <copyright file="mult_dynamic_worker.h" company="">
//     Copyright (c) . All rights reserved.
// </copyright>
// <summary></summary>
// ***********************************************************************
#ifndef __MULT_DYNAMIC_WORKER_H
#define __MULT_DYNAMIC_WORKER
#include "defines.h"
#include <windows.h>

/// <summary>
/// Struct __unnamed_struct_0083_1
/// </summary>
typedef struct {
    /// <summary>
    /// Numero de puntos a calcular
    /// </summary>
    int puntos;
    /// <summary>
    /// Renglon en el que empieza a sumarse
    /// </summary>
    int renglon;
    /// <summary>
    /// Columna en la que empieza a sumarse
    /// </summary>
    int columna;
    /// <summary>
    /// Apuntador a la matriz A en la posicion del primer renglon a sumar
    /// </summary>
    long int (*A)[HEIGHTA];
    /// <summary>
    /// Apuntador a la matriz B en la posicion de la primer columna a sumar
    /// </summary>
    long int (*B)[HEIGHTB];
    /// <summary>
    /// Apuntador a la matriz resultante C en el renglon a sumar
    /// </summary>
    long int (*C)[WIDTHA];
} DynPuntoParams;

/// <summary>
/// The sysinfo
/// </summary>
SYSTEM_INFO sysinfo;

static DWORD WINAPI multiplica_dyn_punto_vec(LPVOID p_arg);

/// <summary>
/// Multiplica 2 matrices particionando el trabajo por punto en la matriz
/// resultante. Espera que la matriz B esté transpuesta.
/// Utiliza tantos threads como cores haya disponibles en el sistema.
/// </summary>
/// <param name="size">The size.</param>
/// <param name="matriz_a">La matriz a sumar A.</param>
/// <param name="matriz_b">La matriz a sumar B.</param>
/// <param name="matriz_c">La matriz resultante C.</param>
void multiplica_dyn_producto_vec(long int matriz_a[HEIGHTA][WIDTHA], long int matriz_b[WIDTHB][HEIGHTB], long int matriz_c[HEIGHTA][WIDTHB]) {	
    int i,j = 0;
    int th_counter = 0, th_i = 0, num_worker = 0, puntos_worker = 0, inicio = 0;
    DynPuntoParams* th_dyn_params;

    GetSystemInfo(&sysinfo);
    num_worker = sysinfo.dwNumberOfProcessors;
    if(num_worker > HEIGHTA*WIDTHB) {
        num_worker = HEIGHTA*WIDTHB;
    }

    th_dyn_params = (DynPuntoParams*)malloc(sizeof(DynPuntoParams)*num_worker);
    // problema, talvez haya que hacer mas de un renglon por procesador
    puntos_worker = (HEIGHTA*WIDTHB)/num_worker;

    for(i=0; i < HEIGHTA; i++) {
        inicio = j > 0 ? j - WIDTHB : 0;
        for(j=inicio; j < WIDTHB; j += puntos_worker) {
            // Se asume que j salta de 2 en 2, 4 en 4, 8 en 8
            // Se asume que se trata de un ambiente en que renglones%number_of_processors == 0            
            // Delegar cada punto a un procesador utilizando WorkPool pattern            
            th_dyn_params[th_i].A = &matriz_a[i];
            th_dyn_params[th_i].B = &matriz_b[j];
            th_dyn_params[th_i].C = &matriz_c[i];
            th_dyn_params[th_i].puntos = puntos_worker;
            th_dyn_params[th_i].renglon = i;
            th_dyn_params[th_i].columna = j;

            // invoca hilo            
            th_handles[th_i] = CreateThread(
                NULL, 0, multiplica_dyn_punto_vec, 
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


/// <summary>
/// Calcula DynPuntoParams.puntos celdas empezando en la posicion
/// DynPuntoParams.renglon, DynPuntoParams.columna de la matriz resultante C
/// espera que la matriz sea cuadrada, y tamano representa la anchura y altura
/// del cuadrado.
/// </summary>
/// <param name="p_arg">Espera que sea un apuntador a un tipo DynPuntoParams.</param>
/// <returns>Siempre regresa 0.</returns>
static DWORD WINAPI multiplica_dyn_punto_vec(LPVOID p_arg) {
    DynPuntoParams arg = *(DynPuntoParams*)p_arg;
    int i, j, k, inicio, fin, cuenta = 0;
    int renglones = (arg.puntos+arg.columna)/WIDTHB;
    long int temp[WIDTHA+4];
    temp[0] = temp[1] = temp[2] = temp[3] = 0;
    inicio = arg.columna;
    for(i = 0; i < renglones; i++) {
        fin = WIDTHB;
        if (WIDTHB > arg.puntos - cuenta) fin = arg.puntos - cuenta;

        for(j = inicio; j < fin; j++) {
#pragma ivdep
            for(k = 0; k < WIDTHA; k++) {
                temp[k+4] = temp[k] + arg.A[i][k] * arg.B[j][k];
            }

            arg.C[i][j] = temp[WIDTHA] + temp[WIDTHA+1] + temp[WIDTHA+2] + temp[WIDTHA+3];
            cuenta++;
        }

        inicio = i+1;
    }

    return 0;
}
#endif