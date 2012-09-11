#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef SERIAL
#include "mpi_layer.h"
#endif
#ifdef SERIAL
	#include "mandelbrot_serial.h"
	char const * filename = "mandelbrot_serial.png";
	char const * program_title = "\n     MANDELBROT LINEAR CALCULATION\n";
#else
	#ifdef BALANCED 
		#include "mandelbrot_balanced.h"
		char const * filename = "mandelbrot_balanced.png";
		char const * program_title = "\n     MANDELBROT BALANCED PARALLEL CALCULATION\n";
	#else
		#include "mandelbrot_unbalanced.h"
		char const * filename = "mandelbrot.png";
		char const * program_title = "\n     MANDELBROT NAIVE PARALLEL CALCULATION\n";
	#endif
#endif


time_t seconds;

long int get_seconds_stamp()
{
	return time(NULL);
}

int main(int arg_count, char * args[])
{
	int rows = atoi(args[arg_count - 2]);
	int cols = atoi(args[arg_count - 1]);
	int** data = malloc(rows * cols * INT_SIZE);
	address data_addr = data;
	arg_count = arg_count - 2;
	#ifndef SERIAL
	start(arg_count, args, rows, cols);
	#else
	int my_proc_index = 0;
	int num_procs = 1;
	#endif
	if (my_proc_index == 0)
	{
		printf(program_title);
		long int start_time = get_seconds_stamp();
		mandelbrot_master(data_addr, rows, cols, num_procs);
		long int end_time = get_seconds_stamp();
		long int elapsed_time = end_time - start_time;
		printf("\n**********************\n* ELAPSED TIME: %ld s *\n**********************\n",
		       elapsed_time);
	}
	else
	{
		#ifndef SERIAL
		mandelbrot_slave(data_addr, rows, cols, my_proc_index, num_procs);
		#endif
	}
	#ifndef SERIAL
	end();
	#endif
	if (my_proc_index == 0)
	{
		if (rows > 100 || cols > 100) {
			printf("Writing image file: %s\n", filename);
			write_to_file(data_addr, rows, cols, filename);
		} else {
			printf("Printing mandelbrot\n");
			print_mandelbrot(data, rows, cols);
		}
		
		printf("Done creating mandelbrot. Good bye :)\n");
	}
	
	return 0;
}
