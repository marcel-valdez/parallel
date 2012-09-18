#ifndef MPI_LAYER_H
#define MPI_LAYER_H

#include "../src/traslate_common.h"

// Initialization stuff
#ifndef INT_SIZE_SET
#define INT_SIZE_SET
const int INT_SIZE = sizeof(int);
#endif

#ifndef ADDRESS_SET
#define ADDRESS_SET
typedef unsigned long int address;
#endif

int my_proc_index = 1;
int num_procs = 1;

int** test_buf;
int test_count = -1, test_data_type = -1, test_source = -1, test_tag = -1, test_size = -1, test_rank = -1;

int** test_recv_buf;
int test_recv_count = -1, test_recv_source = -1, test_dest = -1, test_recv_tag = -1;
address test_recv_tag_addr = -1;
address respond_buf_addr = -1;
address test_recv_value_addr = -1;

void array_copy(address source, address dest, int count);

int next_slave(int slave_index)
{
	return (++slave_index) >= num_procs ? 1 : slave_index;
}

int get_slave_from_status()
{	
	return 0;
}

int recv_from_any_data_size = -1;
int recv_from_any_tag = -1;
int mpi_recv_from_any(address data_addr, int data_size, int tag)
{
	test_recv_value_addr = data_addr;
	recv_from_any_data_size = data_size;
	recv_from_any_tag = tag;
	return 0;
}

int single_from_master_value = -1;
int mpi_receive_single_from_master(address value_addr, int tag)
{
	test_recv_value_addr = value_addr;
	test_recv_tag = tag;
	if (single_from_master_value != -1)
	{
		int* value_ptr = value_addr;
		*value_ptr = single_from_master_value;
	}
	
	return 0;
}

int single_from_master_tag = -1;
int mpi_receive_any_single_from_master(address value_addr, address tag_addr)
{
	test_recv_value_addr = value_addr;
	test_recv_tag_addr = tag_addr;
	
	if (single_from_master_tag != -1)
	{
		int* tag_ptr = tag_addr;
		*tag_ptr = single_from_master_tag;
	}
	
	mpi_receive_single_from_master(value_addr, tag_addr);
	
	return 0;
}

int recv_type_size = -1;
int mpi_recv_from_master_default_single_raw(address opData_addr, int type_size)
{
    test_recv_buf = opData_addr;
    recv_type_size = type_size;

    return 0;
}

int mpi_recv_default_from_master(address data_addr, int data_size)
{
    test_recv_buf = data_addr;
    test_recv_count = data_size;
    mpi_receive_default(data_addr, data_size, 0);

    return 0;
}

int mpi_receive_default(address data_addr, int data_size, int source)
{
    printf("\nmpi_receive_default(data_addr: %lu, data_size: %d, source: %d)\n",
           data_addr, data_size, source);
    test_recv_buf = data_addr;
    test_recv_count = data_size;
    test_recv_source = source;
    test_recv_tag = 1;
    if (respond_buf_addr != -1) {
        printf("\nmpi_receive_default: responding with memory @%lu\n", respond_buf_addr);
	int i = 0;
	for(i = 0; i < data_size; i++) {
		address respond_pixel_addr = respond_buf_addr + (i * sizeof(int));
		int* respond_pixel = respond_pixel_addr;
		
		address recv_pixel_addr = data_addr + (i * sizeof(int));
		int* recv_pixel = recv_pixel_addr;
		printf("\nSetting @%lu to @%lu, old: %d, new: %d",
		       recv_pixel,
		       respond_pixel,
		       *recv_pixel,
                       *respond_pixel);
		*recv_pixel = *respond_pixel;
	}
    }

    return 0;
}

address send_single_value_addr = -1;
int send_single_value = -1;
int send_single_dest_process = -1;
int send_single_tag = -1;

void mpi_send_single(address value_addr, int dest_process, int tag)
{
	int* value_ptr = value_addr;
	printf("mpi_send_single(%lu:%d, %d, %d)\n", value_addr, *value_ptr, dest_process, tag);
	send_single_value_addr = value_addr;
	send_single_value = *value_ptr;
	send_single_dest_process = dest_process;
	send_single_tag = tag;
}

int send_single_default_raw_data_size;
void mpi_send_single_default_raw(address value_addr, int data_size, int dest_process)
{
    printf("mpi_send_single_default_raw(value_addr: %lu, data_size: %d, dest_process: %d)",
	   value_addr, data_size, dest_process);
    send_single_value_addr = value_addr;
    send_single_dest_process = dest_process;
    send_single_default_raw_data_size = data_size;
}

void mpi_send_master(address data_addr, int data_size, int tag)
{
	mpi_send(data_addr, data_size, 0, tag);
}

void mpi_send_default_master(address data_addr, int data_size)
{
	mpi_send_default(data_addr, data_size, 0);
}

void mpi_send_default(address data_addr, int data_size, int dest_process)
{
	mpi_send(data_addr, data_size, dest_process);
}

address mpi_send_copy = -1;
void mpi_send(address data_addr, int data_size, int dest_process, int tag)
{
	test_buf = data_addr;
	test_count = data_size;
	test_dest = dest_process;
	test_tag = tag;

	if (mpi_send_copy != -1)
	{
            array_copy(data_addr, mpi_send_copy, data_size);
	}
}

void start(int arg_count, char * args[], int rows, int cols)
{
	my_proc_index = test_rank = 1;
	num_procs = test_size = 1;
}


void zero_out(int *array[], int rows, int cols)
{
	int k = 0;
	long unsigned int array_address = array;
	int i = 0, j = 0;
	
	for(i = 0; i < rows; i++)
	{
		long unsigned int row_address = array_address + (i * cols * INT_SIZE);
		for(j = 0; j < cols; j++)
		{
			int* cell_pointer = row_address + (j * INT_SIZE);
			*cell_pointer = 1;
		}
	}
}

void array_copy(address source, address dest, int count) {
    int i = 0;
    for(i = 0; i < count; i++) {
	address source_pixel_addr = source + (i * sizeof(int));
	int* source_pixel = source_pixel_addr;

	address dest_pixel_addr = dest + (i * sizeof(int));
	int* dest_pixel = dest_pixel_addr;

	printf("\nSetting @%lu to @%lu, old: %d, new: %d",
	       dest_pixel,
	       source_pixel,
	       *dest_pixel,
	       *source_pixel);

	*dest_pixel = *source_pixel;
    }
}
#endif
