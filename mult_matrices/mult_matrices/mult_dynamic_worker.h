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
    /// Tamano de un lado de la matriz cuadrada
    /// </summary>
    int tamano;
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
    float (*A)[SIZE];
    /// <summary>
    /// Apuntador a la matriz B en la posicion de la primer columna a sumar
    /// </summary>
    float (*B)[SIZE];
    /// <summary>
    /// Apuntador a la matriz resultante C en el renglon a sumar
    /// </summary>
    float (*C)[SIZE];
} DynPuntoParams;

/// <summary>
/// The sysinfo
/// </summary>
SYSTEM_INFO sysinfo;

static DWORD WINAPI multiplica_dyn_punto(LPVOID arg);
static DWORD WINAPI multiplica_dyn_punto_vec(LPVOID p_arg);
void multiplica_dyn_producto_helper(int size, float matriz_a[SIZE][SIZE], float matriz_b[SIZE][SIZE], float matriz_c[SIZE][SIZE], BOOL vectorized);

void multiplica_dyn_producto(int size, float matriz_a[SIZE][SIZE], float matriz_b[SIZE][SIZE], float matriz_c[SIZE][SIZE]) {	
    multiplica_dyn_producto_helper(size, matriz_a, matriz_b, matriz_c, FALSE);
}

void multiplica_dyn_producto_vec(int size, float matriz_a[SIZE][SIZE], float matriz_b[SIZE][SIZE], float matriz_c[SIZE][SIZE]) {	
    multiplica_dyn_producto_helper(size, matriz_a, matriz_b, matriz_c, TRUE);
}


/// <summary>
/// Multiplica 2 matrices particionando el trabajo por punto en la matriz
/// resultante. Espera que la matriz B esté transpuesta.
/// Utiliza tantos threads como cores haya disponibles en el sistema.
/// </summary>
/// <param name="size">The size.</param>
/// <param name="matriz_a">La matriz a sumar A.</param>
/// <param name="matriz_b">La matriz a sumar B.</param>
/// <param name="matriz_c">La matriz resultante C.</param>
void multiplica_dyn_producto_helper(int size, float matriz_a[SIZE][SIZE], float matriz_b[SIZE][SIZE], float matriz_c[SIZE][SIZE], BOOL vectorized) {	
    int i,j = 0;
    int th_counter = 0, th_i = 0, num_worker = 0, puntos_worker = 0, inicio = 0;
    DynPuntoParams* th_dyn_params;

    GetSystemInfo(&sysinfo);
    num_worker = sysinfo.dwNumberOfProcessors;
    if(num_worker > SIZE) {
        num_worker = SIZE;
    }

    th_dyn_params = (DynPuntoParams*)malloc(sizeof(DynPuntoParams)*num_worker);
    // problema, talvez haya que hacer mas de un renglon por procesador
    puntos_worker = (size*size)/num_worker;
    
    for(i=0; i < size; i++) {
        inicio = j > 0 ? j - size : 0;
        for(j=inicio; j < size; j += puntos_worker) {
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
            if(vectorized) {
                th_handles[th_i] = CreateThread(
                    NULL, 0, multiplica_dyn_punto_vec, 
                    &th_dyn_params[th_i], 0, NULL);
            } else {
                th_handles[th_i] = CreateThread(
                    NULL, 0, multiplica_dyn_punto, 
                    &th_dyn_params[th_i], 0, NULL);
            }

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
    int i, j, k, puntos = arg.puntos, renglon, inicio, fin, cuenta = 0;
    int renglones = (arg.puntos+arg.columna)/arg.tamano;
    float *Ci, *Ai, *Bj;
    float temp[SIZE+4];
    temp[0] = temp[1] = temp[2] = temp[3] = 0;

    for(i = 0; i < renglones;i++) {
        Ci = arg.C[i];
        Ai = arg.A[i];
        inicio = i == 0 ? arg.columna : 0;
        fin = arg.puntos - cuenta < arg.tamano ? arg.puntos - cuenta : arg.tamano;        
        for(j = inicio; j < fin; j++) {            
            Bj = arg.B[j];
#pragma ivdep
            for(k = 0; k < arg.tamano; k++) {
                temp[k+4] = temp[k] + Ai[k] * Bj[k];
            }
            
            Ci[j] = temp[SIZE] + temp[SIZE+1] + temp[SIZE+2] + temp[SIZE+3];
            cuenta++;
        }
    }

    return 0;
}
#endif