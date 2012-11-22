#include <stdio.h>
#include "data.h"
#include <time.h>
#include <sys/time.h>

void print_data(int* data, int count);

void main(int arg_count, char* args[])
{
	struct timeval start,end;	
	int runs = 10000;
	long sum = 0l;
	long bestTime = 999999999;
	int results[DATA_SIZE];
	int j;
	
	printf("Beginning time profiling for data size: %d\n", DATA_SIZE);
	
	for(j = 0; j < runs; j++) {
		gettimeofday(&start,0x0);
		results[0] = data[0];
		int i;
		for(i = 1; i < DATA_SIZE; i++) {
			results[i] = data[i] + results[i-1];
		}			

		
		gettimeofday(&end,0x0);	
		long time = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
		sum += time;
		if (bestTime > time) {
			bestTime = time;
		}
	}
			
	double average = (sum * 1.0d) / (runs * 1.0d);	
	printf("For %d executions\n", j);
	printf("Total execution time: %l\n", sum);
	printf("Average microseconds for prefix sum: %.1f\n", average);
	printf("Best time in microseconds for sequential prefix sum: %f\n", bestTime);
}