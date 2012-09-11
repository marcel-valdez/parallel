#include <stdlib.h>
#include <stdio.h>

#include "mpi_layer_stub.h"
#include "../src/mandelbrot_balanced.h"
#include <check.h>


START_TEST (test_set_row_index)
{
	printf("\n*******************************\n");
	printf("* Test for set_row_index *\n");
	printf("*******************************\n");
	
	printf("\nArrange... \n");
	int data[] = { 0, 20 };
	address data_addr = data;
	address expected_addr = data_addr + INT_SIZE;
	int expected_1 = 1;
	int expected_2 = data[1];
	
	printf("\nAct... \n");
	address actual_data_addr = set_row_index(data_addr, expected_1);
	
	printf("\nAssert... \n");
	fail_unless(expected_1 == data[0],
		"Expected to receive %d but got %d", expected_1, data[0]);		
	fail_unless(expected_2 == data[1],
		"Expected to receive %d but got %d", expected_2, data[1]);		
	fail_unless(expected_addr == actual_data_addr,
		"Expected to receive %lu but got %lu", expected_addr, actual_data_addr);		
	printf("\n********* PASS *********\n");	
}
END_TEST

START_TEST (test_get_row_index)
{
	printf("\n**************************\n");
	printf("* Test for get_row_index *\n");
	printf("**************************\n");
	
	printf("\nArrange... \n");
	int data[] = { 1, 20 };
	address data_addr = data;
	int expected = data[0];
	
	printf("\nAct... \n");
	int actual = get_row_index(data_addr);
	
	printf("\nAssert... \n");
	fail_unless(expected == actual,
		"Expected to receive %d but got %d", expected, actual);		
	printf("\n********* PASS *********\n");	
}
END_TEST

START_TEST (test_get_row_addr)
{
	printf("\n**************************\n");
	printf("* Test for get_row_addr *\n");
	printf("**************************\n");
	
	printf("\nArrange... \n");
	int data[] = { 1, 20 };
	address data_addr = data;
	address expected_addr = data_addr + INT_SIZE;
	
	printf("\nAct... \n");
	address actual_addr = get_row_addr(data_addr);
	
	printf("\nAssert... \n");
	fail_unless(expected_addr == actual_addr,
		"Expected to receive %lu but got %lu", expected_addr, actual_addr);		
	printf("\n********* PASS *********\n");	
}
END_TEST

START_TEST (test_balanced_master)
{
	printf("\n***************************************\n");
	printf("* Test for mandelbrot_balanced_master *\n");
	printf("***************************************\n");
	
	printf("\nArrange... \n");
	const int height = 8;
	const int width = 4;
	int arg_count = 0;
	int proc_count = num_procs = 4;
	int result[height][width];
	address result_addr = result;
	address expected_addr = result_addr + (width * INT_SIZE * (proc_count - 2));
	
	printf("\nAct... \n");
	mandelbrot_master(result_addr, height, width, proc_count);

	printf("\nAssert... \n");
	fail_unless(height == send_single_value, 
		"Expected to receive %d but got %d", height - 1, send_single_value);
	fail_unless(send_single_tag == DIE,
		"Expected to receive %d but got %d", DIE, test_tag);
			
	fail_unless(recv_from_any_data_size == (width * INT_SIZE),
		"Expected to receive %d but got %d", width * INT_SIZE, recv_from_any_data_size);
		
	fail_unless(recv_from_any_tag == RESULT,
		"Expected to receive %d but got %d", RESULT, recv_from_any_tag);		
	printf("\n********* PASS *********\n");
}
END_TEST

START_TEST (test_balanced_slave)
{
	printf("\n**************************************\n");
	printf("* Test for mandelbrot_balanced_slave *\n");
	printf("**************************************\n");
	
	printf("\nArrange... \n");
	/* Data initialization */
	const int rows = 240;
	const int cols = 320;
	int data[rows][cols];
	address data_addr = data;
	zero_out(data, rows, cols);
	int proc_idx = 1;
	single_from_master_tag = DIE;
	single_from_master_value = 0;
	
	/* Expectations */	
	int expected_size = cols;
	int expected_dest = 0;
	int expected_tag = RESULT;
	
	printf("\nAct... \n");
	printf("Se construye linea por linea el mandelbrot.\n");
	int row = 0;
	for(row = 0; 
	    row < rows;
   	    row++)
	{
		single_from_master_value = row;
		address row_addr = data_addr + (row * cols * INT_SIZE);
		/* Se construye una linea del mandelbrot */
		mandelbrot_slave(row_addr, rows, cols, -1, -1);		
		/*printf("Assert %d... ", single_from_master_value);*/
	
		fail_unless(test_count == expected_size,
	 		"Bad size received from slave. Expected %d but found %d",
			expected_size, test_count);		
		fail_unless(test_dest == expected_dest,
	 		"Bad destination from slave. Expected %d but found %d",
			expected_dest, test_dest);
		fail_unless(test_tag == expected_tag,
	 		"Bad tag from slave. Expected %d but found %d",
			expected_tag, test_tag);
		fail_unless(test_buf == row_addr,
			"Bad memory address from slave. Expected %lu but found %lu",
			row_addr, test_buf);
	}
	
	printf("\n");
	
	if (cols > 100 || rows > 100) {
		printf("Test data is too big, writing to file mandelbrot_balanced.png\n");
		write_to_file(&data, rows, cols, "mandelbrot_balanced.bmp");
	} else {
		print_mandelbrot(data, rows, cols);
	}
	printf("Is it a %d x %d mandelbrot?", rows, cols);
	
	/* Reset */ 	
	test_buf = -1;
	single_from_master_tag = -1;
	single_from_master_value = -1;
	printf("\n********* PASS *********\n");
}
END_TEST


int main (void)
{
	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("@    Running Mandelbrot Balanced Tests   @\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	int number_failed;
	Suite *s = suite_create("mandelbrot_balanced");
	
	TCase *tc_test_master = tcase_create("Test master mapping");
	tcase_add_test (tc_test_master, test_balanced_master);
	suite_add_tcase(s, tc_test_master);
	
	TCase *tc_test_slave = tcase_create("Test slave calculation");
	tcase_add_test (tc_test_slave, test_balanced_slave);
	suite_add_tcase(s, tc_test_slave);
						
	TCase *tc_test_set_row_index = tcase_create("Test set_row_index");
	tcase_add_test (tc_test_set_row_index, test_set_row_index);
	suite_add_tcase(s, tc_test_set_row_index);
	
	TCase *tc_test_get_row_index = tcase_create("Test get_row_index");
	tcase_add_test (tc_test_get_row_index, test_get_row_index);
	suite_add_tcase(s, tc_test_get_row_index);		
	
	TCase *tc_test_get_row_addr = tcase_create("Test test_get_row_addr");
	tcase_add_test (tc_test_get_row_addr, test_get_row_addr);
	suite_add_tcase(s, tc_test_get_row_addr);		
	
	SRunner *runner = srunner_create (s);
	srunner_run_all (runner, CK_NORMAL);
	number_failed = srunner_ntests_failed (runner);
	srunner_free (runner);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
