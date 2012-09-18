#include <stdlib.h>
#include <stdio.h>

#include "mpi_layer_stub.h"
#define RECV_OPDATA_IMPL
#include "../src/traslate_unbalanced.h"
#include <check.h>
#define VALUE(X, Y) (*(X + (Y)))

void assert_array(int* data, int* expected_data, int height, int width);

OperationData* test_opData;
OperationData* receive_operation_data()
{
    return test_opData;
}

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
	printf("* Test for traslate_master *\n");
	printf("******************************\n");

	printf("\nArrange... \n");
	const int rows = 8;
	const int cols = 4;
	int arg_count = 0;
	int proc_count = 2;
	int rows_per_proc = rows / (proc_count - 1);
	int result[rows][cols];
	long unsigned int result_addr = result;
	long unsigned int expected_addr = result_addr + (rows_per_proc * cols * INT_SIZE * (proc_count - 2));

	printf("\nAct... \n");
	traslate_master(result, rows, cols, proc_count, 2);

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
	printf("* Test for traslate_slave *\n");
	printf("******************************\n");

	printf("\nArrange... \n");
	const int cols = 3;
	const int rows = 5;
	const int deltaX = 2;

	int data[5][3] =
	{
	    { 21, 22, 23 },
	    { 11, 12, 13 },
	    { 14, 15, 16 },
	    { 24, 25, 26 },
            { 27, 28, 29 }
	};

        mpi_send_copy = data;

        int expected_data[5][3] =
	{
	    { 22, 23, 21 },
	    { 12, 13, 11 },
	    { 15, 16, 14 },
	    { 25, 26, 24 },
            { 28, 29, 27 }
	};
	int total_procs = 3;
	int worker_procs = total_procs - 1;
	address data_addr = data;
	int worker_idx = 1;

	printf("\nAct... \n");
	for(worker_idx = 1; worker_idx < total_procs; worker_idx++) {
	    printf("worker_idx: %d, total_procs: %d\n", worker_idx, total_procs);
            /* Expected worker rows slice */
	    int row_slice = rows / worker_procs;

            /* Expected starting worker row index */
	    int row_idx = (worker_idx - 1) * row_slice;

            /* Expected remainder added for last worker */
	    row_slice += worker_idx == worker_procs ? rows % worker_procs : 0;

            /* Returned data address for the worker */
	    address worker_slice_addr = data_addr + (row_idx * cols * sizeof(int));
            printf("\nWorker slice memory @%lu", worker_slice_addr);

	    /* Set returned data for next worker */
	    /* We have to re-allocate the OperationData, because the slave
	     * frees that memory on each run.
	     */
            test_opData = (OperationData*)malloc(sizeof(OperationData));
	    (*test_opData).Width = cols;
	    (*test_opData).Height = rows;
	    (*test_opData).DeltaX = deltaX;
	    respond_buf_addr = worker_slice_addr;
            mpi_send_copy = &data[row_idx][0];
	    traslate_slave(worker_idx, total_procs);
	}

	printf("\nAssert... \n");
        print_array(data, cols, rows);
        assert_array(data, expected_data, cols, rows);

	/* Reset */
        test_opData = NULL;
	respond_buf_addr = -1;
        mpi_send_copy = -1;
	printf("\n********* PASS *********\n");
}
END_TEST

START_TEST (test_move_row)
{
	printf("\n********************************\n");
	printf("*      Test for move_row        *\n");
	printf("**********************************\n");

	printf("\nArrange... \n");
	int width = 3;
	int deltaX = 2;
	int data[] = { 11, 12, 13 };
	int expected_data[] = { 12, 13, 11 };
        address data_addr = data;

	printf("\nAct... \n");
        move_row(data_addr, width, deltaX);

	printf("\nAssert... \n");
        int i;
	for (i = 0; i < width; i++) {
	    fail_unless(data[i] == expected_data[i],
			"expected %d but found %d on index %d",
			expected_data[i], data[i], i);
	}

	printf("\n********* PASS *********\n");
}
END_TEST

START_TEST (test_move_section)
{
	printf("\n*****************************\n");
	printf("*      Test for move_section  *\n");
	printf("*******************************\n");

	printf("\nArrange... \n");
        const int height = 4;
	const int width = 3;
	int deltaX = 2;
	int data[4][3] =
	{
	    { 21, 22, 23 },
	    { 11, 12, 13 },
	    { 14, 15, 16 },
	    { 24, 25, 26 }
	};
	int expected_data[4][3] =
        {
	    { 21, 22, 23 },
	    { 12, 13, 11 },
	    { 15, 16, 14 },
	    { 24, 25, 26 }
	};

	address data_addr = data;
	int start = 1;
        int count = 2;

	printf("\nAct... \n");
        move_section(data_addr, start, count, width, deltaX);

	printf("\nAssert... \n");
        assert_array(data, expected_data, height, width);

	printf("\n********* PASS *********\n");
}
END_TEST

int main (void)
{
	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("@    Running traslate Unbalanced Tests   @\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	int number_failed;
	Suite *s = suite_create("traslate");

	TCase *tc_test_initialize = tcase_create("Test initialization");
	tcase_add_test (tc_test_initialize, test_initialize);
	suite_add_tcase (s, tc_test_initialize);

	TCase *tc_test_master = tcase_create("Test master initialization");
	tcase_add_test (tc_test_master, test_master);
	suite_add_tcase(s, tc_test_master);

	TCase *tc_test_slave = tcase_create("Test slave calculation");
	tcase_add_test (tc_test_slave, test_slave);
	suite_add_tcase(s, tc_test_slave);

        TCase *tc_test_move_row = tcase_create("Test for row data shift");
	tcase_add_test (tc_test_move_row, test_move_row);
	suite_add_tcase(s, tc_test_move_row);

        TCase *tc_test_move_section = tcase_create("Test for section data shift");
	tcase_add_test (tc_test_move_section, test_move_section);
	suite_add_tcase(s, tc_test_move_section);

	SRunner *runner = srunner_create (s);
	srunner_run_all (runner, CK_NORMAL);
	number_failed = srunner_ntests_failed (runner);
	srunner_free (runner);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

void print_array(int* data, int width, int height)
{
    int i;
    printf("\n{");
    for(i = 0; i < height; i++)
    {
	printf("\n\t{");
	int j;
	for(j = 0; j < width; j++)
	{
            printf(" %d ", data[(i * width) + j]);
	}
	printf("}\n");
    }
    printf("}\n");
}

void assert_array(int* data, int* expected_data, int height, int width)
{
    int i;
    for (i = 0; i < width * height; i++)
    {
	fail_unless(VALUE(data, i) == VALUE(expected_data, i),
		    "expected %d but found %d on index %d",
		    VALUE(expected_data, i), VALUE(data, i), i);
    }
}