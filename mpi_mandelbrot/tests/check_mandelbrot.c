#include <check.h>
#ifdef BALANCED
	#include "../src/mandelbrot_balanced.h"
#else
	#include "../src/mandelbrot_unbalanced.h"
#endif

START_TEST (empty_test)
{
;	/* This test is empty */
}
END_TEST

int main (void)
{
	return 0;
}


