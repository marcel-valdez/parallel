#ifndef MANDELBROT_H
#define MANDELBROT_H

#ifndef bool
	#define bool char
	#define true 1
	#define false 0
#endif

#include "mandelbrot_common.h"

void mandelbrot_master(address result_addr, int rows, int cols, int proc_count)
{
	int worker_count = proc_count - 1;
	int rows_slice = rows / worker_count;
	int remainder = rows % worker_count;
	
	printf("*************************\n* Master prepares data. *\n*************************\n");
	/* Inicializar datos */
	init_array(result_addr, rows, cols);	

	/* Enviar renglones a cada proceso */	
	int i;
	/* Wait for slaves to give data! */
	for(i = 1; i <= worker_count; i++) {
		/* MPI_Status* status; */
		address rows_to_receive_addr = move_pointer(result_addr, rows_slice * cols *  (i - 1));
		int current_rows_slice = (i == worker_count) ? rows_slice + remainder : rows_slice;
		printf("Master is waiting on Slave %d\n", i);
		mpi_receive_default(rows_to_receive_addr, current_rows_slice * cols, i);
	}
	
	printf("*******************\n* Master is done. *\n*******************\n");
}


void mandelbrot_slave(int** my_rows, int total_rows, int cols, int my_proc_idx, int total_procs)
{
	printf("Slave: %d starting\n", my_proc_idx);	
	double min_real = - 2.0;
	double max_real = 1.0;
	double min_imaginary = - 1.2;
	double max_imaginary = min_imaginary + (max_real - min_real) * total_rows / cols;
	double real_factor = (max_real - min_real) / (cols - 1);
	double imaginary_factor = (max_imaginary - min_imaginary) / (total_rows - 1);
	int total_workers = total_procs - 1;
	
	/* Calculate and send result back! */
	int rows_per_proc = total_rows / total_workers;
	
	/* If this process is the last to receive rows, then it expects all the remainder of rows */
	int rows_slice = rows_per_proc + ((my_proc_idx == total_workers) ? total_rows % total_workers : 0);
	#ifdef DEBUG
	printf("My rows_slice: %d\n", rows_slice);
	#endif
	
	/* Calculate mandelbrot! */
	address rows_addr = my_rows;
	int row = 0;
	for(row = 0; row < rows_slice; row++)
	{
		address row_addr = move_pointer(rows_addr, row * cols);
		
		/* 1 >= my_proc_idx < total_procs. No slave is process 0.  */
		int y = ((my_proc_idx - 1) * rows_per_proc) + row;
		#ifdef DEBUG_HIGH
		printf("\nrow: %d\n", y);
		#endif
		double c_imaginary = max_imaginary - (y * imaginary_factor);
		int col = 0;
		#ifdef DEBUG_HIGH
		printf("cols: \n {");
		#endif
		for(col = 0; col < cols; col++)
		{
			#ifdef DEBUG_HIGH
			printf("%d ", col);
			#endif
			double c_real = min_real + col * real_factor;
			bool is_inside = true;
			int iterate = 0;
			double z_real = c_real;
			double z_imaginary = c_imaginary;
			for(iterate = 0; iterate < MAX_ITERATIONS; iterate++)
			{
				double z_real2 = SQR(z_real);
				double z_imaginary2 = SQR(z_imaginary);
				if(z_real2 + z_imaginary2 > 4)
				{
					is_inside = false;
					break;
				}
				
				z_imaginary = 2 * z_real * z_imaginary + c_imaginary;
				z_real = z_real2 - z_imaginary2 + c_real;
			}
			
			/* Get the pixel to draw */
			address pixel_address = move_pointer(row_addr, col);
			int * pixel = pixel_address;
			
			/* If pixel is inside mandelbrot, then set it white, colirfy otherwise. */
			*pixel = is_inside ? INNER_COLOR : COLORIFY(iterate);
		}
		#ifdef DEBUG_HIGH
		printf("}");
		#endif
	}
	
	printf("Slave %d is sending data back to the server\n", my_proc_idx);
	mpi_send_default_master(rows_addr, rows_slice * cols);	
	printf("Slave %d is dying now.\n", my_proc_idx);
}

#endif /* MANDELBROT_H */
