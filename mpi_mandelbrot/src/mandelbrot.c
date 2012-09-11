#include <stdio.h>
#include <stdlib.h>
#include "mpi_layer.h"
#ifdef BALANCED 
	#include "mandelbrot_balanced.h"
	char const * filename = "mandelbrot_balanced.png";
#else
	#include "mandelbrot_unbalanced.h"
	char const * filename = "mandelbrot.png";
#endif

int main(int arg_count, char * args[])
{
	int rows = atoi(args[arg_count - 2]);
	int cols = atoi(args[arg_count - 1]);
	int** data = malloc(rows * cols * INT_SIZE);
	address data_addr = data;
	arg_count = arg_count - 2;
	start(arg_count, args, rows, cols);
	
	if (my_proc_index == 0)
	{
		mandelbrot_master(data_addr, rows, cols, num_procs);
	}
	else
	{
		mandelbrot_slave(data_addr, rows, cols, my_proc_index, num_procs);
	}

	end();
	
	if (my_proc_index == 0)
	{
		if (rows > 100 || cols > 100) {
			printf("Writing image file: %s\n", filename);
			write_to_file(data_addr, rows, cols, filename);
		} else {
			printf("Printing mandelbrot\n");
			print_mandelbrot(data, rows, cols);
		}
		
		printf("Done creating mandelbrot. Good bye :)");
	}
	
	return 0;
}
