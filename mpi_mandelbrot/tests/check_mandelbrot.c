#include <stdlib.h>
#include <stdio.h>

#define MPI_LAYER_H

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
int my_proc_index = 1;
int num_procs = 1;

int MPI_COMM_WORLD = 1;
int MPI_INT = 4;
int** test_buf;
int test_count = -1, test_data_type = -1, test_source = -1, test_tag = -1, test_size = -1, test_rank = -1;

int** test_recv_buf;
int test_recv_count = -1, test_recv_source = -1, test_recv_tag = -1, test_dest = -1;

unsigned long int respond_buf_addr = -1;

int mpi_receive(address data_addr, int data_size, int source)
{
	test_recv_buf = data_addr;
	test_recv_count = data_size;
	test_recv_source = source;
	test_recv_tag = 1;
	
	if (respond_buf_addr != -1) {
		address recv_buf_addr = data_addr;
		int i = 0;
		for(i = 0; i < data_size; i++) {
			address respond_pixel_addr = respond_buf_addr + (i * sizeof(int));
			int* respond_pixel = respond_pixel_addr;
			
			address recv_pixel_addr = data_addr + (i * sizeof(int));
			int* recv_pixel = recv_pixel_addr;
			*recv_pixel = *respond_pixel;
		}
	}
}

void MPI_Send(address buf, int count, int dest, int tag)
{
	test_buf = buf;
	test_count = count;
	test_dest = dest;
	test_tag = tag;
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

void mpi_send(address data_addr, int data_size, int dest_process, int tag)
{
	test_count = data_size;
	void MPI_Send(data_addr, data_size, MPI_INT, dest_process, tag);
}

void start(int arg_count, char * args[], int rows, int cols)
{
	my_proc_index = test_rank = 1;
	num_procs = test_size = 1;
}

#include "../src/mandelbrot_common.h"

#ifdef BALANCED
	#include "../src/mandelbrot_balanced.h"
#else
	#include "../src/mandelbrot_unbalanced.h"
#endif

#include <check.h>

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

START_TEST (test_init_array)
{
	printf("***********************\n");
	printf("* Test for init_array *\n");
	printf("***********************\n");
	// Arrange
	int rows = 3;
	int cols = 4;
	int array[3][4] = { { 11, 12, 13, 14 }, 
	                    { 21, 22, 23, 24 },
        	            { 31, 33, 33, 34 } };
	// Act
	init_array(array, rows, cols);

	// Assert
	int i = 0, j = 0;
	for(i = 0; i < rows; i++)
	{	
		for(j = 0; j < cols; j++)
		{
			fail_unless(
				array[i][j] == 0,
				"@[%d, %d] expected: %d, found: %d",
				i, j, 0, array[i][j]);
		}
	}
	
	printf("\n********* END *********\n");
}
END_TEST

START_TEST (test_initialize)
{
	printf("******************************\n");
	printf("*      Test for start        *\n");
	printf("******************************\n");
	// Arrange
	int rows = 2;
	int cols = 4;
	int arg_count = 2;
	char* args[] = { "-np", "2"  };

	// Act
	start(arg_count, args, rows, cols);

	// Assert
	fail_unless(my_proc_index == test_rank, "Rank variable not used correctly");
	fail_unless(num_procs == test_size, "Size variable not used correctly");
	printf("\n********* END *********\n");
}
END_TEST

START_TEST (test_master)
{
	printf("******************************\n");
	printf("* Test for mandelbrot_master *\n");
	printf("******************************\n");
	// Arrange
	const int rows = 8;
	const int cols = 4;
	int arg_count = 0;
	int proc_count = 2;
	int rows_per_proc = rows / (proc_count - 1);
	char* args[] = { };
	start(arg_count, args, rows, cols);
	int result[rows][cols];
	long unsigned int result_addr = result;
	long unsigned int expected_addr = result_addr + (rows_per_proc * cols * INT_SIZE * (proc_count - 2));
	
	// Act
	mandelbrot_master(result, rows, cols, proc_count);

	// Assert

	long unsigned int buf_addr = test_recv_buf;
	fail_unless(buf_addr == expected_addr,
			"Expected to receive %ul but got %ul", expected_addr, buf_addr);
	fail_unless(test_recv_count == rows_per_proc * cols, 
			"Expected %d size but found %d", rows_per_proc * cols, test_recv_count);
	
	printf("\n********* END *********\n");
}
END_TEST

START_TEST (test_slave)
{
	printf("******************************\n");
	printf("* Test for mandelbrot_slave *\n");
	printf("******************************\n");
	
	/* Arrange */
	const int cols = 640;
	const int total_rows = 480;
	int data[total_rows][cols];
	zero_out(data, total_rows, cols);
	int total_procs = 5;
	int worker_procs = 4;
	respond_buf_addr = data;
	unsigned long int data_addr = data;
	
	/* Act */
	int i = 1;
	for (i = 1; i < total_procs; i++) {
		/* Se construye el mandelbrot por partes */
		int proc_idx = i;
		int rows_per_proc = total_rows / worker_procs;
		rows_per_proc += (i == worker_procs) ? (total_rows % worker_procs) : 0;
		unsigned long int current_data_addr = data_addr + ((i - 1) * cols * rows_per_proc * INT_SIZE);
		mandelbrot_slave(current_data_addr, total_rows, cols, proc_idx, total_procs);
		
	/* Assert */
		int expected_size = rows_per_proc * cols;
		fail_unless(test_count == expected_size,
		 		"Bad size received from slave. Expected %d but found %d",
				expected_size, test_count);
	}
	
	if (cols > 100 || total_rows > 100) {
		printf("Test data is too big, writing to file mandelbrot.bmp\n");
		process_mandelbrot(data_addr, total_rows, cols);
		write_to_file(&data, total_rows, cols, "mandelbrot.bmp");
	} else {
		print_mandelbrot(data, total_rows, cols);
	}
	printf("Is it a %d x %d mandelbrot?", total_rows, cols);
	
	/* Reset */ 	
	respond_buf_addr = -1;
	printf("\n********* END *********\n");
}
END_TEST

START_TEST (test_print)
{
	printf("******************************\n");
	printf("* Test for print_mandelbrot  *\n");
	printf("******************************\n");
	
	/* Arrange */
	int data[3][5] = { { 1, 1, 0, 1, 1 },
			   { 1, 0, 1, 0, 1 },
			   { 0, 1, 1, 1, 0 } };	
	
	/* Act */
	print_mandelbrot(data, 3, 5);
	
	/* Assert */
	printf("Is it a triangle?");
	/*char answer = 'n';
	scanf(&answer);
	fail_unless(answer == 'Y' || answer == 'y', "It did not print a triangle.");*/
	/* Reset */ 	
	
	printf("\n********* END *********\n");
}
END_TEST

int main (void)
{
	printf("*********************************\n");
	printf("*    Running Mandelbrot Tests   *\n");
	printf("*********************************\n");
	int number_failed;
	Suite *s = suite_create("mandelbrot");
	TCase *tc_init_array = tcase_create("Test helper functions");
	tcase_add_test (tc_init_array, test_init_array);
	suite_add_tcase (s, tc_init_array);

	
	TCase *tc_test_initialize = tcase_create("Test initialization");
	tcase_add_test (tc_test_initialize, test_initialize);
	suite_add_tcase (s, tc_test_initialize);

	TCase *tc_test_master = tcase_create("Test master initialization");
	tcase_add_test (tc_test_master, test_master);
	suite_add_tcase(s, tc_test_master);
	
	TCase *tc_test_slave = tcase_create("Test slave calculation");
	tcase_add_test (tc_test_slave, test_slave);
	suite_add_tcase(s, tc_test_slave);
	
	TCase *tc_test_print = tcase_create("Test mandelbrot print");
	tcase_add_test (tc_test_print, test_print);
	suite_add_tcase(s, tc_test_print);

	SRunner *runner = srunner_create (s);
	srunner_run_all (runner, CK_NORMAL);
	number_failed = srunner_ntests_failed (runner);
	srunner_free (runner);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


