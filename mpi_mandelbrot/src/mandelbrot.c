#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#ifdef BALANCED 
	#include "mandelbrot_balanced.h"
#else
	#include "mandelbrot_unbalanced.h"
#endif

#include "mandelbrot_common.h"

int main(int arg_count, char * args[])
{
	int rows = 480;
	int cols = 640;
	int data[rows][cols];
	arg_count = arg_count - 1;
	start(arg_count, args, rows, cols);
        if (my_proc_index == 0)
        {
                mandelbrot_master(data, rows, cols, num_procs);
        }
        else
        {
                mandelbrot_slave(data, rows, cols, my_proc_index, num_procs);
        }

	end();
	
	if (my_proc_index == 0)
	{
		if (rows > 100 || cols > 100) {
			printf("Writing image file: %s\n", args[arg_count]);
			write_to_file(data, rows, cols, args[arg_count]);
		} else {
			printf("Printing mandelbrot\n");
			print_mandelbrot(data, rows, cols);
		}
	}
	
	return 0;
}
