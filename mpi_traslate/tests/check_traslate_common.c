#include <stdlib.h>
#include <stdio.h>

#include "../src/traslate_common.h"
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
	printf("* Test for print_traslate  *\n");
	printf("******************************\n");

	printf("\nArrange... \n");	
	int data[3][5] = { { 1, 1, 0, 1, 1 },
			   { 1, 0, 1, 0, 1 },
			   { 0, 1, 1, 1, 0 } };	
			   
	printf("\nAct... \n");
	print_traslate(data, 3, 5);
	
	printf("\nAssert... \n");
	printf("Is it a triangle?");	
	
	printf("\n********* PASS *********\n");
}
END_TEST

START_TEST (test_read_file)
{
	printf("\n******************************\n");
	printf("* Test for read_image_file  *\n");
	printf("******************************\n");

	printf("\nArrange... \n");     
	char filename[] = "/mirror/mpi/mpi_traslate/bin/mandelbrot.png";
	int expected_width = 320;
	int expected_height = 240;
	int width;
	int height;
        int* pixel_ptr;
			   
	printf("\nAct... \n");
	pixel_ptr = read_file(&width, &height, filename);
	
	printf("\nAssert... \n");
	fail_unless(expected_width == width,
			"Expected %d but found %d", 
			expected_width,
			width);
	fail_unless(expected_height == height,
			"Expected %d but found %d", 
			expected_height,
		    height);

	int i;
	for(i = 0; i < width * height; i++) {
            // No debe dar seg_fault
	    int pixel = pixel_ptr[i];
	}

	printf("\nReset...\n");

        close_file(pixel_ptr);
	
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

START_TEST (test_safe_free)
{
	printf("\n*********************\n");
	printf("* Test for safe_free  *\n");
	printf("***********************\n");
	
	printf("\nArrange... \n");
	int* arr = (int*)malloc(2 * INT_SIZE);

	printf("\nAct... \n");
	safe_free(arr);

	printf("\nAssert... Automatically passes if no seg_faults. \n");

	/* Reset */
	
	printf("\n********* PASS *********\n");
}
END_TEST

START_TEST (test_get_slave_data_size)
{
	printf("\n******************************\n");
	printf("* Test for get_slave_data_size *\n");
	printf("********************************\n");
	
	printf("\nArrange... \n");
	int total_slaves = 3;
	int width = 1;
        int height = 4;
	int expected_size[] = { 0, 1, 1, 2 };
	int actual_size[4];

	printf("\nAct... \n");
        int index;
	for(index = 1; index <= total_slaves; index++)
	{
	    actual_size[index] = get_slave_data_size(index, total_slaves, width, height);

	    printf("\nAssert... \n");
	    fail_unless(actual_size[index] == expected_size[index],
			"Expected size %d but found %d, for index %d",
                        expected_size[index], actual_size[index], index);
	}

	/* Reset */
	
	printf("\n********* PASS *********\n");
}
END_TEST

int main (void)
{
	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("@    Running traslate Common HDR Tests   @\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	int number_failed;
	Suite *s = suite_create("traslate_common");
	TCase *tc_init_array = tcase_create("Test helper functions");
	tcase_add_test (tc_init_array, test_init_array);
	suite_add_tcase (s, tc_init_array);	
	
	TCase *tc_test_print = tcase_create("Test traslate print");
	tcase_add_test (tc_test_print, test_print);
	suite_add_tcase(s, tc_test_print);
	
	TCase *tc_test_append = tcase_create("Test append row");
	tcase_add_test (tc_test_append, test_append);
	suite_add_tcase(s, tc_test_append);

       	TCase *tc_test_read_file = tcase_create("Test read image file");
	tcase_add_test (tc_test_read_file, test_read_file);
	suite_add_tcase(s, tc_test_read_file);

        TCase *tc_test_safe_free = tcase_create("Test free memory safely");
	tcase_add_test (tc_test_safe_free, test_safe_free);
	suite_add_tcase(s, tc_test_safe_free);

        TCase *tc_test_get_slave_data_size = tcase_create("Test get slave data size");
	tcase_add_test (tc_test_get_slave_data_size, test_get_slave_data_size);
	suite_add_tcase(s, tc_test_get_slave_data_size);

	SRunner *runner = srunner_create (s);
	srunner_run_all (runner, CK_NORMAL);
	number_failed = srunner_ntests_failed (runner);
	srunner_free (runner);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
