#ifndef MANDELBROT_H
#define MANDELBROT_H

#ifndef bool
	#define bool char
	#define true 1
	#define false 0
#endif

#ifdef DEBUG
	#define DPRINT(MSG) (printf(MSG))
	#define DPRINT1(MSG, VAR) (printf(MSG, VAR))
	#define DPRINT2(MSG, VAR1, VAR2) (printf(MSG, VAR1, VAR2))
	#define DPRINT3(MSG, VAR1, VAR2, VAR3) (printf(MSG, VAR1, VAR2, VAR3))
#else
	#define DPRINT(X) /* X */
	#define DPRINT1(MSG, VAR) /* MSG, VAR */
	#define DPRINT2(MSG, VAR1, VAR2)
	#define DPRINT3(MSG, VAR1, VAR2, VAR3)
#endif

#include "mandelbrot_common.h"
#include <stdlib.h>
#include <stdio.h>

const int CALCULATE = 0x01;
const int DIE = 0x10;
const int RESULT = 0x100;

/*
* Obtiene la direccion de memoria del renglon de datos dentro del paquete de datos
*/
address get_row_addr(address data_addr)
{
	return data_addr + INT_SIZE;
}

/*
* Obtiene el indice del renglon en el paquete de datos
*/
int get_row_index(address data_addr)
{
	int* row_idx_ptr = data_addr;
	
	return *row_idx_ptr;
}

/*
* Establece el indice del renglon en los datos, y regresa el indice
* de la posicion 0 del renglon de datos.
*/
address set_row_index(address data_addr, int index)
{
	int* row_idx_ptr = data_addr;
	*row_idx_ptr = index;
	
	return data_addr + INT_SIZE;
}

void mandelbrot_master(address result, int height, int width, int proc_count)
{     
	int pending_rows = 0; /* contador de renglones pendientes */
	/* renglon que se envia */
	int row_idx = 0;
	/* indice del esclavo a quien enviar datos */
	int slave_idx = 0;

	printf("\nMaster process is preparing data.\n");
	/* Requiero que circule entre 1 - Max Slaves */
	for (slave_idx = 1; row_idx < proc_count - 1; slave_idx = next_slave(slave_idx)) 
	{    
		DPRINT2("Sending row: %d to slave %d\n", row_idx, slave_idx);
		/* supone que proc_count < disp_height */
		/* envia el comando de calcular el renglon al siguiente esclavo */
		mpi_send_single(&row_idx, slave_idx, CALCULATE);
		/* contar renglones enviados */
		pending_rows++;
		/* indice del siguiente renglon */
		row_idx++;               
	}
	
	printf("\nMaster process is mapping data.\n");
	address slave_data_addr = malloc(size_of_row(width) + INT_SIZE);
	do {
		/* Recibir resultados de cualquier esclavo */
		mpi_recv_from_any(slave_data_addr, size_of_row(width), RESULT);
		/* Reducir conteo de renglones pendientes conforme se reciben */
		pending_rows--; 
		/* Obtener el indice del esclavo que envio el renglon!		*/
		slave_idx = get_slave_from_status();
		DPRINT1("\nReceived data from slave %d\n", slave_idx);
		if (row_idx < height) {
			/* enviar siguiente comando de calculo de renglon */
			DPRINT2("\nSending row %d calculation to slave %d\n", row_idx, slave_idx);
			mpi_send_single(&row_idx, slave_idx, CALCULATE);
			/* incrementar el conteo de renglones pendientes */
			pending_rows++;
			/* incrementar el índice del renglon a calcular */
			row_idx++;
		} else  {
			/* matar al esclavo con indice slave_idx*/
			DPRINT1("\nKilling slave %d\n", slave_idx);
			mpi_send_single(&row_idx, slave_idx, DIE);
		}
		
		/* Obtener el indice del renglon que se recibio del esclavo */
		int new_row_idx = get_row_index(slave_data_addr);
		DPRINT2("\nGot row %d from slave %d\n", new_row_idx, slave_idx);
		/* Obtener la direccion de memoria del renglon recibido */
		address new_row_addr = get_row_addr(slave_data_addr);		
		/* Agregar el nuevo renglon a los datos */
		append_row(result, new_row_addr, new_row_idx, width); 
		
	} while (pending_rows > 0);
	
	DPRINT1("\nLast row_idx: %d\n", row_idx);

	if (slave_data_addr != NULL)
	{
		free(slave_data_addr);
		slave_data_addr = NULL;
	}
	
	printf("Master process finished.\n");
}

void mandelbrot_slave(
	address row_addr, 
	int total_rows, 
	int cols, 
	int my_proc_idx,
	int total_procs)/* Se usa como dummy para que sea igual que en el no balanceado */
{
	printf("\nSlave: %d starting\n", my_proc_idx);	
	double min_real = - 2.0;
	double max_real = 1.0;
	double min_imaginary = - 1.2;
	double max_imaginary = min_imaginary + (max_real - min_real) * total_rows / cols;
	double real_factor = (max_real - min_real) / (cols - 1);
	double imaginary_factor = (max_imaginary - min_imaginary) / (total_rows - 1);	
	
	/* Start waiting for rows */
	int row_index = 0;
	int command = CALCULATE;	
	mpi_receive_single_from_master(&row_index, CALCULATE);  
	DPRINT2("\nSlave %d received calculation for row: %d\n", my_proc_idx, row_index);
	#ifdef DEBUG
	char data_chars[cols];
	#endif
	while(command == CALCULATE)
	{		
		int y = row_index;
		double c_imaginary = max_imaginary - (y * imaginary_factor);
		int col = 0;
		/*DPRINT("cols: \n {");*/
		address data_row_addr = set_row_index(row_addr, y);
		for(col = 0; col < cols; col++)
		{
			/*DPRINT1("%d ", col)*/
			double c_real = min_real + col * real_factor;
			double z_real = c_real;
			double z_imaginary = c_imaginary;
			bool is_inside = true;
			int iterate = 0;
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
			int* pixel = get_cell(data_row_addr, col);
			
	/* If pixel is inside mandelbrot, then set it INNER_COLOR, colirify otherwise. */
			*pixel = is_inside ? INNER_COLOR : COLORIFY(iterate);
			#ifdef DEBUG
			data_chars[col] = sprintf(data_chars, "%d ", *pixel);
			#endif
		}
		DPRINT2("\nSlave %d calculated: %s\n", my_proc_idx, data_chars);
		/*DPRINT("}");*/
		printf("Slave %d is sending data back to the server\n", my_proc_idx);
		mpi_send_master(row_addr, cols, RESULT);
		mpi_receive_any_single_from_master(&row_index, &command);  
	}
	
	printf("Slave %d is dying now.\n", my_proc_idx);
}

#endif /* MANDELBROT_H */
