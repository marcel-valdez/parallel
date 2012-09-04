#ifndef MANDELBROT_H
#define MANDELBROT_H

#define BLACK 0x000000
#define WHITE 0xFFFFFF
#define SQR(X) ((X) * (X))
#define MAX_ITERATIONS 1000

#ifndef bool
	#define bool char
	#define true 1
	#define false 0
#endif

void mandelbrot_master(int **result, int rows, int cols, int proc_count)
{
	int rows_slice = rows / proc_count;
	int remainder = rows % proc_count;
	int resolution = rows * cols;
	int i = 0, j = 0, k = 0, row = 0;
	
	printf("Master prepares data.\n");
	// Inicializar datos
	init_array(result, rows, cols);	

	printf("Master is sending data.\n");
	// Enviar renglones a cada proceso
	long unsigned int result_addr = result;
	for(i = 0; i < proc_count; i++, row += rows_slice)
	{
		// send ( data, to_process)
		int current_rows_slice = i == proc_count - 1 ? rows_slice + remainder : rows_slice;		
		long unsigned int rows_to_send_addr = result_addr + (row * cols * INT_SIZE);
		
		// Variable para renglones a enviar
		int** rows_to_send = rows_to_send_addr;
		MPI_Send(rows_to_send, current_rows_slice * cols, MPI_INT, i, 1, MPI_COMM_WORLD);
	}
	
	printf("Master sent all data.\n");	
	/* Wait for slaves to give data! */
	for(i = 0; i < proc_count; i++) {
		MPI_Status* status;
		long unsigned int rows_to_receive_addr = result_addr + (rows_slice * cols * proc_count * INT_SIZE);
		int current_rows_slice = i == proc_count - 1 ? rows_slice + remainder : rows_slice;
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
	#ifdef DEBUG
	printf("Starting slave.\n");
	#endif
	long unsigned int rows_addr = my_rows;
	
	double min_real = - 2.0;
	double max_real = 1.0;
	double min_imaginary = - 1.2;
	double max_imaginary = min_imaginary + (max_real - min_real) * total_rows / cols;
	double real_factor = (max_real - min_real) / (cols - 1);
	double imaginary_factor = (max_imaginary - min_imaginary) / (total_rows - 1);
	
	/* Calculate and send result back! */
	int rows_per_proc = total_rows / total_procs;
	
	/* If this process is the last to receive rows, then it expects all the remainder of rows */
	int rows_slice = rows_per_proc + (my_proc_idx == (total_procs - 1) ? total_rows % total_procs : 0);
	#ifdef  DEBUG
	printf("My rows_slice: %d\n", rows_slice);
	#endif
	MPI_Status* status;
	
	/* Wait for rows from master */
	MPI_Recv(my_rows, rows_slice * cols, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
	
	/* Calculate mandelbrot! */
	int row = 0;
	for(row = 0; row < rows_slice; row++)
	{
		int y = row + (my_proc_idx * (total_rows / total_procs));
		#ifdef DEBUG
		printf("\nrow: %d\n", y);
		#endif
		double c_imaginary = max_imaginary - (y * imaginary_factor);
		long unsigned int row_addr = rows_addr + (y * cols * INT_SIZE);
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
			for(iterate = 0; iterate < MAX_ITERATIONS; ++iterate)
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
			long unsigned int pixel_address = row_addr + (col * INT_SIZE);
			int * pixel = pixel_address;
			
			/* If pixel is inside mandelbrot, then set it black, white otherwise. */
			*pixel = is_inside ? BLACK : WHITE;
		}
		#ifdef DEBUG_HIGH
		printf("}");
		#endif
	}
	
}

#endif /* MANDELBROT_H */