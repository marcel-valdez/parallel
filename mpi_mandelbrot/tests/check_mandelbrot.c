#include <stdlib.h>
#include <stdio.h>

#include "mpi_layer_stub.h"
#include "../src/mandelbrot_unbalanced.h"
#include <check.h>

START_TEST (test_initialize)
{
	printf("\n******************************\n");
	printf("*      Test for start        *\n");
	printf("******************************\n");
	
	printf("\nArrange... \n");
	int rows = 2;
	int cols = 4;
	int arg_count = 2;
	char* args[] = { "-np", "2"  };

	printf("\nAct... \n");
	start(arg_count, args, rows, cols);

	printf("\nAssert... \n");
	fail_unless(my_proc_index == test_rank, "Rank variable not used correctly");
	fail_unless(num_procs == test_size, "Size variable not used correctly");
	printf("\n********* PASS *********\n");
}
END_TEST

START_TEST (test_master)
{
	printf("\n******************************\n");
	printf("* Test for mandelbrot_master *\n");
	printf("******************************\n");
	
	printf("\nArrange... \n");
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
	
	printf("\nAct... \n");
	mandelbrot_master(result, rows, cols, proc_count);

	printf("\nAssert... \n");

	long unsigned int buf_addr = test_recv_buf;
	fail_unless(buf_addr == expected_addr,
			"Expected to receive %ul but got %ul", expected_addr, buf_addr);
	fail_unless(test_recv_count == rows_per_proc * cols, 
			"Expected %d size but found %d", rows_per_proc * cols, test_recv_count);
	
	printf("\n********* PASS *********\n");
}
END_TEST

START_TEST (test_slave)
{
	printf("\n******************************\n");
	printf("* Test for mandelbrot_slave *\n");
	printf("******************************\n");
	
	printf("\nArrange... \n");
	const int cols = 320;
	const int total_rows = 240;
	int data[total_rows][cols];
	zero_out(data, total_rows, cols);
	int total_procs = 5;
	int worker_procs = 4;
	respond_buf_addr = data;
	unsigned long int data_addr = data;
	
	printf("\nAct... \n");
	int i = 1;
	for (i = 1; i < total_procs; i++) {
		/* Se construye el mandelbrot por partes */
		int proc_idx = i;
		int rows_per_proc = total_rows / worker_procs;
		rows_per_proc += (i == worker_procs) ? (total_rows % worker_procs) : 0;
		unsigned long int current_data_addr = data_addr + ((i - 1) * cols * rows_per_proc * INT_SIZE);
		mandelbrot_slave(current_data_addr, total_rows, cols, proc_idx, total_procs);
		
	printf("\nAssert... \n");
		int expected_size = rows_per_proc * cols;
		fail_unless(test_count == expected_size,
		 		"Bad size received from slave. Expected %d but found %d",
				expected_size, test_count);
	}
	
	if (cols > 100 || total_rows > 100) {
		printf("Test data is too big to print, writing to file mandelbrot.png\n");
		write_to_file(&data, total_rows, cols, "mandelbrot.png");
	} else {
		print_mandelbrot(data, total_rows, cols);
	}
	printf("Is it a %d x %d mandelbrot?", total_rows, cols);
	
	/* Reset */ 	
	respond_buf_addr = -1;
	printf("\n********* PASS *********\n");
}
END_TEST

int main (void)
{
	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("@    Running Mandelbrot Unbalanced Tests   @\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	int number_failed;
	Suite *s = suite_create("mandelbrot");
	
	TCase *tc_test_initialize = tcase_create("Test initialization");
	tcase_add_test (tc_test_initialize, test_initialize);
	suite_add_tcase (s, tc_test_initialize);

	TCase *tc_test_master = tcase_create("Test master initialization");
	tcase_add_test (tc_test_master, test_master);
	suite_add_tcase(s, tc_test_master);
	
	TCase *tc_test_slave = tcase_create("Test slave calculation");
	tcase_add_test (tc_test_slave, test_slave);
	suite_add_tcase(s, tc_test_slave);

	SRunner *runner = srunner_create (s);
	srunner_run_all (runner, CK_NORMAL);
	number_failed = srunner_ntests_failed (runner);
	srunner_free (runner);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
