#ifndef MANDELBROT_COMMON_H
#define MANDELBROT_COMMON_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "mpi_layer.h"

#define SQR(X) ((X) * (X))
#define MAX_ITERATIONS 8096
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define INNER_COLOR BLACK
#define COLORIFY(X) ( WHITE * X / MAX_ITERATIONS )

// Initialization stuff
#ifndef INT_SIZE_SET
#define INT_SIZE_SET
const int INT_SIZE = sizeof(int);
#endif

#ifndef ADDRESS_SET
#define ADDRESS_SET
typedef unsigned long int address;
#endif

address move_pointer(address addr, int positions)
{
	address result = addr + (positions * INT_SIZE);
	
	return result;
}


address move_to_row(address row_addr, int row, int col_size)
{
	return move_pointer(row_addr, row * col_size);
}

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
	stbi_write_png(filename, cols, rows, 3, &data, cols * INT_SIZE/*rows * cols * INT_SIZE*/);
}


address previous_row(address pixel_addr, int col_size)
{
	return pixel_addr - (col_size * INT_SIZE);
}

address previous_pixel(address pixel_addr)
{
	return pixel_addr - INT_SIZE;
}

address next_pixel(address pixel_addr)
{
	return pixel_addr + INT_SIZE;
}

address next_row(address pixel_addr, int col_size)
{
	return pixel_addr + (col_size * INT_SIZE);
}

void process_mandelbrot(int** data, int rows, int cols)
{
	address data_addr = data;
	int pass;
	for(pass = 0; pass < MAX_ITERATIONS; pass++)
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
				//if(*pixel == COLORIFY(pass)) 
				{			
					int* top = previous_row(pixel_addr, cols);
					int* left = previous_pixel(pixel_addr);
					int* right = next_pixel(pixel_addr);
					int* bot = next_row(pixel_addr, cols);
					int new_pixel = (*top + *left + *right + *bot) >> 2;
					if (new_pixel > *pixel)
					{
						*pixel = new_pixel;
					}
				}
			}
		}
	}	
}

#endif
