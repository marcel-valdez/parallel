#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>  
#include "data.h"

int calculate_slice_size(int my_proc_id, int total_procs, int total_data_size);
int calculate_sum(int* data, int start, int count);
void reduce_results(int* output, int* data, int start, int count);
void print_data(int* data, int start, int count);

void main(int num_args, char* args[]) {    
    int my_proc_id, proc_count, start, local_sum, total_sum;

    int data_slice_size;    
    int* output;	
    double time_start, time_end;
    	
    /* Initialize MPI */
    MPI_Init(&num_args, &args);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_proc_id);    
	if (my_proc_id == 0) {
		printf("Total processors: %d\n", proc_count);
	}

	if (my_proc_id == 0) {
		time_start = MPI_Wtime();
	}
	
    /* Prepare prefix sum calculation */
    data_slice_size = calculate_slice_size(my_proc_id, proc_count, DATA_SIZE);    
    output = (int *)malloc(sizeof(int) * DATA_SIZE);//data_slice_size);
      
    /* Calculate partial sum */
    start = ((DATA_SIZE / proc_count) * my_proc_id) + 1;
	local_sum = calculate_sum(data, start, data_slice_size);

	if(start == 1) { local_sum += data[0]; }
        
    /* Compute prefix sum */
    total_sum = 0;	
    MPI_Scan(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD); 	
	output[start] = data[start] + total_sum - local_sum;
    
	if (start == 1) { output[start] += data[0]; }
    
	reduce_results(output, data, start, data_slice_size);

	/*MPI_Barrier(MPI_COMM_WORLD);*/
	if (my_proc_id == 0) {
		time_end = MPI_Wtime();
	}

	/*print_data(data, start, data_slice_size);*/

	if (my_proc_id == 0)
	{
		printf("\nTiempo ejecucion: %f milliseconds\n", (time_end-time_start) * 1000);
	}
	
    /* Cleanup */
    free(output);
    MPI_Finalize();
    exit(0);
}

void print_data(int* data, int start, int count) {
	int i;
	for(i = start + 1; i < start + count; i++) {        
        printf("data[%d]: %d\n", i, data[i]);
    }
}

void reduce_results(int* output, int* data, int start, int count) {
    int i;

    if (start == 1) { 
        output[0] = data[0]; 
    }
    
    for(i = start + 1; i < start + count; i++) {
        output[i] = output[i - 1] + data[i];
    }
}

int calculate_sum(int* data, int start, int count) {
    int sum = 0;
    int i;		

    for (i = 0; i < count; i++) {
        sum += data[i + start];
    }
    
    return sum;
}

int calculate_slice_size(int my_proc_id, int total_procs, int total_data_size) {	
    int data_slice_size;
    /* Dado que se trata de prefix sum, se ignora el primer valor, y en base a 1 a N se determina el data slice */
    total_data_size -= 1;
    /* calculate size of data slice current process should use */	
    data_slice_size = total_data_size / total_procs;
    /* size of data slice of the last process  */
    if (my_proc_id == total_procs -1) {
        data_slice_size = total_data_size - (total_procs - 1) * data_slice_size;
    }
    
    printf("Proc: %d, slice size: %d\n", my_proc_id, data_slice_size);
    return data_slice_size;
}
