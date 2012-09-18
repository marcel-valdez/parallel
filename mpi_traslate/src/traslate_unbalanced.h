#ifndef traslate_H
#define traslate_H

#ifndef bool
	#define bool char
	#define true 1
	#define false 0
#endif

#ifdef DEBUG
#define DPRINT(X) printf(X)
#define DPRINT1(X,X1) printf(X, X1)
#define DPRINT2(X,X1,X2) printf(X, X1,X2)
#define DPRINT3(X,X1,X2,X3) printf(X, X1,X2,X3)
#else
#define DPRINT(X)
#define DPRINT1(X,X1)
#define DPRINT2(X,X1,X2)
#define DPRINT3(X,X1,X2,X3)
#endif
#ifdef DEBUG_HIGH
	#define DHPRINT(X) printf(X)
	#define DHPRINT1(X,X1) printf(X, X1)
#else
	#define DHPRINT(X)
	#define DHPRINT1(X,X1)
#endif

#include "traslate_common.h"

void receive_operation_data(int* opData);

void move_image(address rows_addr, int start, int end, int deltaX);

void move_row(address row_addr, int width, int deltaX);

address traslate_master(address image_data, int height, int width, int proc_count, int deltaX)
{
    /* El proceso maestro no es un 'worker' */
    int worker_count = proc_count - 1;

    /* Se calcula el numero de renglones por 'worker' */
    int rows_slice = height / worker_count;

    /* Se calcular el remanente para el ultimo proceso */
    int remainder = height % worker_count;

    /* Se aloja memoria para la imagen 'trasladada' */
    address new_image_data_addr = malloc(size_of_data(width * height));
    DPRINT("*************************\n* Master prepares data. *\n*************************\n");
    /* Inicializar datos */
    int opData[3];
    opData[0] = width;
    opData[1] = height;
    opData[2] = deltaX;
    printf("Calculando traslate paralelamente sin optimizar mapeo....\n");


    /* Enviar renglones a cada proceso */
    int proc;
    for(proc = 1; proc <= worker_count; proc++) {

	/* Enviar primero los datos sobre los renglones a enviar*/
	mpi_send_default(opData, 3, proc);

	/* Obtener la direccion de memoria de los renglones que le corresponden al esclavo */
	address row_data = move_pointer(
					image_data,
					(proc - 1) * rows_slice * width);

	/* Obtener el tamano de los datos a enviar al esclavo */
	int slave_data_size = get_slave_data_size(proc, worker_count, width, height);

	/* Enviar los renglones tal cual.*/
	mpi_send_default(row_data, slave_data_size, proc);
    }

    int i;
    /* Wait for slaves to give data! */
    for(i = 1; i <= worker_count; i++) {
	address rows_to_receive_addr = move_pointer(image_data, rows_slice * width *  (i - 1));
	int current_rows_slice = (i == worker_count) ? rows_slice + remainder : rows_slice;
	DPRINT1("Master is waiting on Slave %d\n", i);
	mpi_receive_default(rows_to_receive_addr, current_rows_slice * width, i);
    }

    DPRINT("*******************\n* Master is done. *\n*******************\n");
}

/*
 * En este procedimiento se ejecuta el codigo esclago de la translacion de una imagen.
 */
void traslate_slave(int my_proc_idx, int total_procs)
{
    DPRINT1("\n\n*** Slave: %d starting ***\n", my_proc_idx);
    /* numero total de esclavos */
    int total_workers = total_procs - 1;

    /* direccion de memoria de los datos de los renglones */
    address rows_addr;

    /* obtener los datos que se recibiran del maestro: altura, anchura y delta X */
    int opData[3];
    receive_operation_data(opData);
    int width = opData[0];
    int height = opData[1];
    int deltaX = opData[2];

    /* cantidad de renglones que me toca calcular */
    int rows_slice = get_slave_rows_slice(
					  my_proc_idx,
					  total_workers,
					  height);

    /* alojar memoria, dinamicamente, para los renglones que se calcularan */
    int rows_data_size = size_of_data(rows_slice * width);
    rows_addr = malloc(rows_data_size);
    DPRINT3("\nAllocated %d x %d pixels (%d)", rows_slice, width, rows_data_size);
    DPRINT1(" @%lu\n", rows_addr);

    /* obtener renglones del maestro */
    mpi_recv_default_from_master(rows_addr, rows_slice * width);

    /* calcular nuevos pixeles */
    move_section(rows_addr, 0, rows_slice, width, deltaX);

    /* regresar pixeles al proceso maestro  */
    DPRINT1("Slave %d is sending data back to the server\n", my_proc_idx);
    mpi_send_default_master(rows_addr, rows_slice * width);

    /* liberar recursos */
    DPRINT("\n\ntraslate_slave: Freeing rows_addr resources");
    safe_free(rows_addr);
    DPRINT1("Slave %d is dying now.\n", my_proc_idx);
}

#ifndef RECV_OPDATA_IMPL
#define RECV_OPDATA_IMPL
void receive_operation_data(int* opData)
{
    DPRINT1("\nreceive_operation_data(opData: %lu)", opData);
    DPRINT("\nWaiting for operation data from master...");
    mpi_recv_default_from_master((address)opData, 3);
    DPRINT3("\nMaster gave data: %d, %d, %d", *opData, *(opData + 1), *(opData + 2));
}
#endif

void move_section(address rows_addr, int start, int count, int width, int deltaX)
{
    DPRINT3("\n*** move_section(rows_addr: %lu, start: %d, count: %d, ", rows_addr, start, count);
    DPRINT2("width: %d, deltaX: %d) ***\n", width, deltaX);
    int row;
    for(row = start; row < start + count; row++)
    {
      	address row_addr = move_pointer(rows_addr, row * width);
        move_row(row_addr, width, deltaX);
    }
}

void move_row(address row_addr, int width, int deltaX)
{
    DPRINT3("\nmove_row(row_addr: %lu, width: %d, deltaX: %d)\n", row_addr, width, deltaX);
    int x = 0;
    int x2 = 0;
    int* pending_pixel_ptr = get_cell(row_addr, 0);
    int i;
    for(i = 0; i < width; i++)
    {
	/* Calcular nuevo x2, y del pixel */
	x2 = x + deltaX;
	/* Si x2 > width, entonces x2 = x2 - width */
	if (x2 >= width) {
	    x2 -=  width;
	} else if (x2 < 0) {
            x2 += width;
	}

	/* Asignar pixel en nueva direccion */
	int* new_pixel_ptr = get_cell(row_addr, x2);
	int replaced_pixel = *new_pixel_ptr;
	*new_pixel_ptr = *pending_pixel_ptr;
	*pending_pixel_ptr = replaced_pixel;

        x = x2;
    }
}

#endif /* traslate_H */
