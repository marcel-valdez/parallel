#include <stdlib.h>
#include <stdio.h>

#include "../src/mandelbrot_common.h"
#include <check.h>

START_TEST (test_init_array)
{
	printf("\n***********************\n");
	printf("* Test for init_array *\n");
	printf("***********************\n");
	printf("\nArrange... \n");	
	int rows = 3;
	int cols = 4;
	int array[3][4] = { { 11, 12, 13, 14 }, 
	                    { 21, 22, 23, 24 },
        	            { 31, 33, 33, 34 } };
			    
	printf("\nAct... \n");
	init_array(array, rows, cols);

	printf("\nAssert... \n");
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
	
	printf("\n********* PASS *********\n");
}
END_TEST

START_TEST (test_print)
{
	printf("\n******************************\n");
	printf("* Test for print_mandelbrot  *\n");
	printf("******************************\n");

	printf("\nArrange... \n");	
	int data[3][5] = { { 1, 1, 0, 1, 1 },
			   { 1, 0, 1, 0, 1 },
			   { 0, 1, 1, 1, 0 } };	
			   
	printf("\nAct... \n");
	print_mandelbrot(data, 3, 5);
	
	printf("\nAssert... \n");
	printf("Is it a triangle?");
	/*char answer = 'n';
	scanf(&answer);
	fail_unless(answer == 'Y' || answer == 'y', "It did not print a triangle.");*/
	/* Reset */ 	
	
	printf("\n********* PASS *********\n");
}
END_TEST

START_TEST (test_append)
{
	printf("\n************************\n");
	printf("* Test for append_row  *\n");
	printf("************************\n");
	
	printf("\nArrange... \n");	
	int all_data[4][4];
	address all_data_addr = all_data;
	int row_data[4] = { 10, 20, 30, 40 };
	address row_data_addr = row_data;
	int row_index = 2;
	int width = 4;
	
	printf("\nAct... \n");
	append_row(all_data_addr, row_data_addr, row_index, width);	
	
	printf("\nAssert... \n");
	int i = 0;
	for(i = 0; i < width; i++)
	{
		fail_unless(all_data[row_index][i] == row_data[i], 
			   "Expected all_data[%d][%d] to be %d @ row_data[%d]",
			   row_index, i, row_data[i], i);
	}
	
	/* Reset */
	
	printf("\n********* PASS *********\n");
}
END_TEST



int main (void)
{
	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("@    Running Mandelbrot Common HDR Tests   @\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	int number_failed;
	Suite *s = suite_create("mandelbrot_common");
	TCase *tc_init_array = tcase_create("Test helper functions");
	tcase_add_test (tc_init_array, test_init_array);
	suite_add_tcase (s, tc_init_array);	
	
	TCase *tc_test_print = tcase_create("Test mandelbrot print");
	tcase_add_test (tc_test_print, test_print);
	suite_add_tcase(s, tc_test_print);
	
	TCase *tc_test_append = tcase_create("Test append row");
	tcase_add_test (tc_test_append, test_append);
	suite_add_tcase(s, tc_test_append);
	
	SRunner *runner = srunner_create (s);
	srunner_run_all (runner, CK_NORMAL);
	number_failed = srunner_ntests_failed (runner);
	srunner_free (runner);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
