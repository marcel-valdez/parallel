#ifndef MANDELBROT_COMMON_H
#define MANDELBROT_COMMON_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define SQR(X) ((X) * (X))
#define MAX_ITERATIONS 8096

#ifndef COLORS
#define COLORS
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define INNER_COLOR BLACK
#define COLORIFY(X) ( WHITE * X / MAX_ITERATIONS )
#endif

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

void init_array(address array_address, int rows, int cols)
{
#ifdef DEBUG
	printf("Initializing array with weird values\n");
#endif
	int k = 0;
#ifdef DEBUG
	printf("Received array address: %lu\n", array_address);
#endif
	
	int i = 0, j = 0;
	for(i = 0; i < rows; i++)
	{
		address row_address = array_address + (i * cols * INT_SIZE);
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

void print_mandelbrot(address data_addr, int rows, int cols)
{
	printf("\n");
	int y = 0;
	for(y = 0; y < rows; y++) {
		address row_addr = data_addr + (y * cols * INT_SIZE);
		int x = 0;
		for(x = 0; x < cols; x++) {
			address pixel_addr = row_addr + (x * INT_SIZE);
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

void write_to_file(address data, int rows, int cols, char const* filename)
{
	stbi_write_png(filename, cols, rows, 3, data, cols * INT_SIZE);
}


int size_of_row(int width)
{
	return width * INT_SIZE;
}

address previous_row(address pixel_addr, int width)
{
	return pixel_addr - (width * INT_SIZE);
}

address previous_pixel(address pixel_addr)
{
	return pixel_addr - INT_SIZE;
}

address next_pixel(address pixel_addr)
{
	return pixel_addr + INT_SIZE;
}

address next_row(address pixel_addr, int width)
{
	return pixel_addr + (width * INT_SIZE);
}

address get_cell_addr(address data, int position)
{
	return data + (position * INT_SIZE);
}

int* get_cell(address data, int position)
{
	int* cell_ptr = data + (position * INT_SIZE);
	return cell_ptr;
}

void append_row(address all_data, address row_data, int row_index, int width)
{
	address append_start = all_data + (row_index * width * INT_SIZE);
	int i = 0;
	for(i = 0; i < width; i++)
	{
		int* dest_cell_ptr = get_cell(append_start, i);
		int* src_cell_ptr = get_cell(row_data, i);
		
		*dest_cell_ptr = *src_cell_ptr;
	}
}
#endif
