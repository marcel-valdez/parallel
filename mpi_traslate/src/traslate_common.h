#ifndef traslate_COMMON_H
#define traslate_COMMON_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE
#include "stb_image.h"

#define SQR(X) ((X) * (X))

// Initialization stuff
#ifndef INT_SIZE_SET
#define INT_SIZE_SET
const int INT_SIZE = sizeof(int);
#endif

#ifndef ADDRESS_SET
#define ADDRESS_SET
typedef unsigned long int address;
#endif

#ifndef DPRINT
	#ifdef DEBUG
		#define DPRINT(X) printf(X)
		#define DPRINT1(X, X1) printf(X, X1)
		#define DPRINT2(X, X1, X2) printf(X, X1, X2)
		#define DPRINT3(X, X1, X2, X3) printf(X, X1, X2, X3)
	#else
		#define DPRINT(X)
		#define DPRINT1(X, X1)
		#define DPRINT2(X, X1, X2)
		#define DPRINT3(X, X1, X2, X3)
	#endif
#endif


void safe_free(void* ptr);

unsigned char* read_file(int* width_ptr, int* height_ptr, char* filename);

void close_file(int* pixel_data);

int size_of_data(int size);

int get_slave_data_size(int slave_index, int total_slaves, int width, int height);

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
	DPRINT("Initializing array with 0 values\n");
	DPRINT1("Received array address: %lu\n", array_address);
	int i = 0, j = 0;
	for(i = 0; i < rows; i++)
	{
		address row_address = array_address + (i * cols * INT_SIZE);
		for(j = 0; j < cols; j++)
		{
			int* cell_pointer = row_address + (j * INT_SIZE);
			*cell_pointer = 0;
		}
	}
}

void print_traslate(address data_addr, int rows, int cols)
{
	printf("\n");
	int y = 0;
	for(y = 0; y < rows; y++) {
		address row_addr = data_addr + (y * cols * INT_SIZE);
		int x = 0;
		for(x = 0; x < cols; x++) {
			address pixel_addr = row_addr + (x * INT_SIZE);
			int* pixel = pixel_addr;
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

unsigned char* read_file(int* width_ptr, int* height_ptr, char* filename)
{
    DPRINT("\nInit read_file variables.\n");
    int comp = 0;
    /* Se especifican 4 componentes de 8 bits por pixel, para tener
     * pixeles que quepan exactamente en un numero int */
    int req_comp = 4;
    DPRINT("\nLoading image data.\n");
    unsigned char* pixel_data = stbi_load(filename, width_ptr, height_ptr, &comp, req_comp);
    DPRINT3("stbi_load results= *width_ptr: %d,*height_ptr: %d, comp: %d\n",
            *width_ptr, *height_ptr, comp);

    DPRINT("\nReturning control to main program.\n");
    return pixel_data;
}

void close_file(int* pixel_data)
{
    stbi_image_free((char*)pixel_data);
}


int size_of_data(int size)
{
    return size * INT_SIZE;
}

int size_of_row(int width)
{
	return width * INT_SIZE;
}

int get_slave_rows_slice(int slave_index, int total_slaves, int height)
{
    int slice = (height / total_slaves);
    slice += slave_index == total_slaves ? (height % total_slaves) : 0;

    return slice;
}

int get_slave_data_size(int slave_index, int total_slaves, int width, int height)
{
    return get_slave_rows_slice(slave_index, total_slaves, height) * width;
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

void safe_free(void* ptr)
{
    DPRINT1("\nsafe_free: Preparing pointer %lu.", ptr);
    if (ptr != NULL)
    {
	DPRINT("\nsafe_free: freeing pointer.");
	free(ptr);
	DPRINT("\nsafe_free: making pointer NULL.\n");
	ptr = NULL;
    }
}
#endif
