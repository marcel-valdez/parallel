SUBDIRS = src . tests

all: test run

test_unbalanced: test

run_unbalanced: run

test: check

run:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np 4 mandelbrot
