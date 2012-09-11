#ifndef MPI_LAYER_H
#define MPI_LAYER_H

#include <mpi.h>

// Initialization stuff
#ifndef INT_SIZE_SET
#define INT_SIZE_SET
/*
* El tamano del tipo int
*/
const int INT_SIZE = sizeof(int);
#endif

#ifndef ADDRESS_SET
#define ADDRESS_SET
/*
* Es el tipo utilizado para transversar direcciones de memoria
*/
typedef unsigned long int address;
#endif

/*
* Es el TAG default
*/
const int DEFAULT_TAG = 0xFFF;

/*
* Es el indice de proceso actual
*/
int my_proc_index = -1;

/*
* Es el numero de procesos en la sesion MPI
*/
int num_procs = -1;

/*
* Es el status del ultimo recv que llego
*/
MPI_Status *status;

int get_slave_from_status()
{
	if (status != NULL)
	{
		return (*status).MPI_SOURCE;
	}
	
	return -1;
}

/*
* Obtiene el proximo indice de esclavo, ignorando el indice del Maestro
*/
int next_slave(int slave_index)
{
	return ++slave_index >= num_procs ? 1 : slave_index;
}

/*
* Se encarga de recibir un valor del tipo int de parte del maestro con cualquier tag,
* el tag recibido se guarda en la direccion de memoria de tag_addr
*/
int mpi_receive_any_single_from_master(address value_addr, address tag_addr)
{
	int result = mpi_receive_single_from_master(value_addr, MPI_ANY_TAG);
	int* tag_ptr = tag_addr;
	*tag_ptr = (*status).MPI_TAG;
	
	return result;
}

/*
 * Se encarga de recibir un valor tipo int de parte del maestro, con el tag especificado
 */
int mpi_receive_single_from_master(address value_addr, int tag)
{
	return mpi_receive(value_addr, 1, 0, tag);
}

/* 
 * Se encarga de recibir de cualquier fuente valores int * data_size,
 * y los guarda en el data_addr, especificamente con el tag especificado
 */
int mpi_recv_from_any(address data_addr, int data_size, int tag)
{
	return mpi_receive(data_addr, data_size, MPI_ANY_SOURCE, tag);
}

/* 
 * Se encarga de recibir valores int * data_size, y los guarda en el data_addr
 * especificamente del proceso source con el DEFAULT_TAG
 */
int mpi_receive_default(address data_addr, int data_size, int source)
{
	return mpi_receive(data_addr, data_size, source, DEFAULT_TAG);
}

/* 
 * Se encarga de recibir valores int * data_size, y los guarda en el data_addr
 * especificamente del proceso source con el tag especificado
 */
int mpi_receive(address data_addr, int data_size, int source, int tag)
{
	return MPI_Recv(data_addr, data_size, MPI_INT, source, tag, MPI_COMM_WORLD, status);
}

/*
* Se encarga de enviar el buffer en data_addr con tamano data_size de valores int,
* con el tag especificado
*/
void mpi_send_master(address data_addr, int data_size, int tag)
{
	mpi_send(data_addr, data_size, 0, tag);
}

/*
* Se encarga de enviar el buffer en data_addr con el tamano data_size con el tag DEFAULT_TAG
*/
void mpi_send_default_master(address data_addr, int data_size)
{
	mpi_send_default(data_addr, data_size, 0);
}

/*
* Se encarga de enviar el buffer en data_addr con el tamano data_size de valores int al
* proceso con indice dest_process
*/
void mpi_send_default(address data_addr, int data_size, int dest_process)
{
	mpi_send(data_addr, data_size, dest_process, DEFAULT_TAG);
}

/*
* Se encarga de enviar el valor en value_addr al proceso con el indice dest_process y
* con el tag especificado
*/
void mpi_send_single(address value_addr, int dest_process, int tag)
{
	mpi_send(value_addr, 1, dest_process, tag);
}

/*
* Se encarga de enviar el buffer en data_addr con el tamano data_size de valores int al
* proceso con indice dest_process con el tag especificado
*/
void mpi_send(address data_addr, int data_size, int dest_process, int tag)
{
	MPI_Send(data_addr, data_size, MPI_INT, dest_process, tag, MPI_COMM_WORLD);
}

/*
* Se encarga de inicializar MPI
*/
void start(int arg_count, char * args[], int rows, int cols)
{
	/* Inicializar MP */
	printf("Initialize MPI\n");
	MPI_Init(&arg_count, &args);
        /* Get number of processes */
	#ifdef DEBUG
	printf("Get number of processes\n");
	#endif
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
        /* Get my process index */
	#ifdef DEBUG
	printf("Get my process index\n");
	#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &my_proc_index);       
	status = malloc(sizeof(MPI_Status));
}

/*
* Se encarga de finalizar MPI
*/
void end()
{
	printf("\bProc: %d. Finalizing MPI...\n", my_proc_index);
	MPI_Finalize();
	if (status != NULL)
	{
		#ifdef DEBUG
		printf("\nProc: %d. Freeing MPI_Status object.\n", my_proc_index);
		#endif
		free(status);
	}
}

#endif
