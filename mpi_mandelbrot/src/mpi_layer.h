#ifndef MPI_LAYER_H
#define MPI_LAYER_H

#include <mpi.h>

// Initialization stuff
#ifndef INT_SIZE_SET
#define INT_SIZE_SET
const int INT_SIZE = sizeof(int);
#endif

#ifndef ADDRESS_SET
#define ADDRESS_SET
typedef unsigned long int address;
#endif

const int DEFAULT_TAG = 0xFFFF;
int my_proc_index = -1;
int num_procs = -1;

MPI_Status *status;

int mpi_receive(address data_addr, int data_size, int source)
{
	return MPI_Recv(data_addr, data_size, MPI_INT, source, DEFAULT_TAG, MPI_COMM_WORLD, &status);
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
	mpi_send(data_addr, data_size, dest_process, DEFAULT_TAG);
}

void mpi_send(address data_addr, int data_size, int dest_process, int tag)
{
	MPI_Send(data_addr, data_size, MPI_INT, dest_process, tag, MPI_COMM_WORLD);
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

#endif
