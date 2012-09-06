SUBDIRS = src . tests

all: build test

test_unbalanced: test

run_unbalanced: run

build:
	cd src;\
	make compile;\
	make clean;\
	cd ..;

test: check

run:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np 5 $(PWD)/bin/mandelbrot 640 480
