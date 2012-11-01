#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#define PROC_COUNT 7

unsigned long cantidad_numeros = 21000000;
int num_per_proc;
clock_t start, end;
CRITICAL_SECTION cs;
// Variable compartida entre threads
unsigned long M = 0;

static DWORD WINAPI worker(LPVOID arg) {
	
	double x,y,r;
	int* mi_arg = (int*)arg;
	int th_index = *mi_arg;	
	unsigned long th_M = 0;
	
	int i = 0;	
	for(i=0;i<num_per_proc;i++) {
		x = ((double)rand()/RAND_MAX);
		y=((double)rand()/RAND_MAX);
		r = sqrt(x*x+y*y);
		if (r<=1.0) { th_M++; }
	}
	// Enter Critical Section
	EnterCriticalSection(&cs);
	M += th_M;
	LeaveCriticalSection(&cs);
	// Exit Critical Section

	return 0;
}

int main() {	
	int i;
	int th_indexes[] = {0, 1, 2, 3, 4};
	HANDLE th_handles[PROC_COUNT];
	num_per_proc = cantidad_numeros / PROC_COUNT;

	start = clock();
	InitializeCriticalSection(&cs);
	srand((unsigned)time(NULL));
	for(i=0;i<PROC_COUNT;i++) {		
		th_handles[i] = CreateThread(NULL, 0, worker, &th_indexes[i], 0,NULL);
	}

	WaitForMultipleObjects(PROC_COUNT, th_handles, TRUE, INFINITE);
	DeleteCriticalSection(&cs);
	end = clock();

	printf("PI=%f  TIME=%ld millis\n", (4.0*(double)M)/cantidad_numeros, end - start);	

	return 0;
}


void calculo_secuencial() {
	int i;
	double x,y,r,pi;
	srand((unsigned)time(NULL));
	for(i=0;i<cantidad_numeros;i++) {
		x = ((double)rand()/RAND_MAX);
		y=((double)rand()/RAND_MAX);
		r = sqrt(x*x+y*y);
		if (r<=1.0) { M++; }
	}
	
	printf("PI=%f\n", (4.0*(double)M)/cantidad_numeros);
}
