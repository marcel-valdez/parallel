SUBDIRS = src . tests

all: build test

build:
	cd src;\
	make build;\
	cd ..;

debug:
	cd src;\
	make debug;\
	cd ..;

test: check

run: run_unbalanced

run_custom: run_unbalanced_custom

run_unbalanced:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np 5 $(PWD)/bin/traslate mandelbrot.png 100

run_parallel_custom:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np $(np) $(PWD)/bin/traslate $(file) $(delta);\
	cd ..
