#ifndef MANDELBROT_H
#define MANDELBROT_H

#define SQR(X) ((X) * (X))
#define MAX_ITERATIONS 8096
#define WHITE 8096
#define BLACK 0x0
#define COLORIFY(X) ( WHITE * X / MAX_ITERATIONS )

#ifndef INT_SIZE_SET
#define INT_SIZE_SET
const int INT_SIZE = sizeof(int);
#endif

#ifndef bool
	#define bool char
	#define true 1
	#define false 0
#endif

address move_to_row(address row_addr, int row, int col_size)
{
	return move_pointer(row_addr, row * col_size);
}

void mandelbrot_master(int** result, int rows, int cols, int proc_count)
{
	int worker_count = proc_count - 1;
	int rows_slice = rows / worker_count;
	int remainder = rows % worker_count;
	
	printf("Master prepares data.\n");
	/* Inicializar datos */
	init_array(result, rows, cols);	

	/* Enviar renglones a cada proceso */
	address result_addr = result;
	
	int i;
	/* Wait for slaves to give data! */
	for(i = 1; i <= worker_count; i++) {
		MPI_Status* status;
		address rows_to_receive_addr = move_pointer(result_addr, rows_slice * cols *  (i - 1));
		int current_rows_slice = (i == worker_count) ? rows_slice + remainder : rows_slice;
		printf("Master is waiting on Slave %d\n", i);
		MPI_Recv(
			rows_to_receive_addr,  /* where to store rows */
			current_rows_slice * cols, /* amount of data to receive */
			MPI_INT, /* type of data to receive */
			i, /* Receive specifically from ith process  */
			1, /* dumb value */
			MPI_COMM_WORLD, /* WORLD! */
			&status); /* Won't actually be used */
	}	
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
	int rows_slice = rows_per_proc + (my_proc_idx == total_workers) ? total_rows % total_workers : 0;
	#ifdef DEBUG
	printf("My rows_slice: %d\n", rows_slice);
	#endif
	MPI_Status* status;
	
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
			*pixel = is_inside ? WHITE : COLORIFY(iterate);
		}
		#ifdef DEBUG_HIGH
		printf("}");
		#endif
	}
	
	printf("Slave %d is sending data back to the server\n", my_proc_idx);
	MPI_Send(
		&my_rows,
		rows_slice * cols,
		MPI_INT,
		0,
		1,
		MPI_COMM_WORLD);
	
	printf("Slave %d is dying now.\n", my_proc_idx);
}


address top_pixel(address pixel_addr, int col_size)
{
	return pixel_addr - (col_size * INT_SIZE);
}

address left_pixel(address pixel_addr)
{
	return pixel_addr - INT_SIZE;
}

address right_pixel(address pixel_addr)
{
	return pixel_addr + INT_SIZE;
}

address bot_pixel(address pixel_addr, int col_size)
{
	return pixel_addr + (col_size * INT_SIZE);
}

void process_mandelbrot(int** data, int rows, int cols)
{
	address data_addr = data;
	int pass;
	for(pass = 0; pass < 256; pass++)
	{
		int row;
		for(row = 1; row < rows - 2; row++)
		{
			address row_addr = move_to_row(data_addr, row, cols);
			int col;
			for(col = 1; col < col - 2; col++)
			{
				address pixel_addr = move_pointer(row_addr, col);
				int* pixel = pixel_addr;
				if(*pixel == COLORIFY(pass)) 
				{			
					int* top = top_pixel(pixel_addr, cols);
					int* left = left_pixel(pixel_addr);
					int* right = right_pixel(pixel_addr);
					int* bot = bot_pixel(pixel_addr, cols);
					*pixel = (*top + *left + *right + *bot) >> 2;								
				}
			}
		}
	}	
}

#endif /* MANDELBROT_H */
