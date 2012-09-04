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
	start(arg_count, args, rows, cols);
        if (my_proc_index == 0)
        {
                mandelbrot_master(data, rows, cols, num_procs);
        }
        else
        {
                mandelbrot_slave(data, rows, cols, my_proc, num_procs);
        }

	end();
	
	if (my_proc_index == 0)
	{
		if (rows > 100 || cols > 100) {
			write_to_file(data, rows, cols, "mandelbrot.png");
		} else {
			print_mandelbrot(data);
		}
	}
	
	return 0;
}
