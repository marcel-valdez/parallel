#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi_layer.h"
#include "traslate_unbalanced.h"

char const * translacion = "translacion.png";
char const * program_title = "\n*** TRANSLACION DE IMAGEN, EN PARALELO ***\n";
time_t seconds;

long int get_seconds_stamp()
{
    return time(NULL);
}

int main(int arg_count, char * args[])
{
    char* filename = args[arg_count - 2];
    int deltaX = atoi(args[arg_count - 1]);
    arg_count -= 2;
    
    start(arg_count, args);
    
    if (my_proc_index == 0)
    {
        printf(program_title);

        /* Obtener informacion de la imagen */
        int width;
        int height;
        int* pixel_data = read_file(&width, &height, filename);
    
        /* Iniciar temporizador */
        long int start_time = get_seconds_stamp();
    
        /* Ejecutar algoritmo */
        address new_image_addr = traslate_master(
                                                 pixel_data,
                                                 height,
                                                 width,
                                                 num_procs,
                                                 deltaX);
    
        /* Finalizar temporiador */
        long int end_time = get_seconds_stamp();
        long int elapsed_time = end_time - start_time;
        printf("\n**********************\n* TIEMPO DE EJECUCION: %ld s *\n**********************\n",
               elapsed_time);
    
        /* Imprimir resultados */
        printf("Escribiendo imagen: %s\n", translacion);
        write_to_file((char*)new_image_addr,
                      height,
                      width,
                      translacion);
    
        /* Liberar recursos */
        close_file(pixel_data);
        safe_free((void*)new_image_addr);
        printf("Programa terminado. Adios. :)\n");
    }
    else
    {
        traslate_slave(my_proc_index, num_procs);
    }
    
    end();
    
    
    return 0;
}
