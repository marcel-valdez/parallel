#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Initialization stuff
#ifndef INT_SIZE_SET
#define INT_SIZE_SET
const int INT_SIZE = sizeof(int);
#endif
int my_proc_index = -1;
int num_procs = -1;

void init_array(int *array[], int rows, int cols)
{
	printf("Initializing array with weird values");
	int k = 0;
	long unsigned int array_address = array;
#ifdef DEBUG
	printf("Received array address: %lu\n", array_address);
#endif
	
	int i = 0, j = 0;
	for(i = 0; i < rows; i++)
	{
		long unsigned int row_address = array_address + (i * cols * INT_SIZE);
		for(j = 0; j < cols; j++)
		{
			int* cell_pointer = row_address + (j * INT_SIZE);
			*cell_pointer = 0;

			int new_value = *cell_pointer;
#ifdef DEBUG
			printf("{ [%d, %d]: %d }",
				 i, j, 
				 new_value);
#endif
		}
#ifdef DEBUG
		printf("\n");
#endif

	}
}

void start(int arg_count, char * args[], int rows, int cols)
{
	/* Inicializar MP */
	printf("Initialize MPI\n");
	MPI_Init(&arg_count, &args);
        /* Get number of processes */
	printf("Get number of processes\n");
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
        /* Get my process index */
	printf("Get my process index\n");
	MPI_Comm_rank(MPI_COMM_WORLD, &my_proc_index);       
}

void end()
{
	MPI_Finalize();
}

void print_mandelbrot(int** data, int rows, int cols)
{
	printf("\n");
	long unsigned int data_addr = data;
	int y = 0;
	for(y = 0; y < rows; y++) {
		long unsigned int row_addr = data_addr + (y * cols * INT_SIZE);
		int x = 0;
		for(x = 0; x < cols; x++) {
			long unsigned int pixel_addr = row_addr + (x * INT_SIZE);
			int * pixel = pixel_addr;
			if(*pixel == 0) {
				printf("x");
			} else {
				printf(" ");
			}
		}
		
		printf("\n");
	}
}

void write_to_file(int** data, int rows, int cols, char const* filename)
{
	stbi_write_bmp(filename, cols, rows, 1, &data);
}
