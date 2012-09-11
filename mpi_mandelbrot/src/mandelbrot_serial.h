#ifndef MANDELBROT_H
#define MANDELBROT_H

#ifndef bool
	#define bool char
	#define true 1
	#define false 0
#endif

#include "mandelbrot_common.h"

void mandelbrot_master(address my_rows, int total_rows, int cols, int num_procs)
{
	double min_real = - 2.0;
	double max_real = 1.0;
	double min_imaginary = - 1.2;
	double max_imaginary = min_imaginary + (max_real - min_real) * total_rows / cols;
	double real_factor = (max_real - min_real) / (cols - 1);
	double imaginary_factor = (max_imaginary - min_imaginary) / (total_rows - 1);
	
	/* If this process is the last to receive rows, then it expects all the remainder of rows */
	#ifdef DEBUG
	printf("My total_rows: %d\n", total_rows);
	#endif
	
	/* Calculate mandelbrot! */
	address rows_addr = my_rows;
	int row = 0;
	for(row = 0; row < total_rows; row++)
	{
		address row_addr = move_pointer(rows_addr, row * cols);
		int y = row;
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
}

#endif /* MANDELBROT_H */
