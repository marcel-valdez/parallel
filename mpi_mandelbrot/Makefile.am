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

build_balanced:
	cd src;\
	make compile_balanced;\
	make clean;\
	cd ..;

test: check

run: run_unbalanced run_balanced run_serial

run_custom: run_unbalanced_custom run_balanced_custom run_serial_custom

run_unbalanced:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np 5 $(PWD)/bin/mandelbrot 960 1280

run_balanced:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np 5 $(PWD)/bin/mandelbrot_balanced 960 1280
	
run_serial:
	cd bin;\
	chmod +x mandelbrot_serial;\
	./mandelbrot_serial 960 1280

run_unbalanced_custom:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np 5 $(PWD)/bin/mandelbrot $(height) $(width)

run_balanced_custom:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np 5 $(PWD)/bin/mandelbrot_balanced $(height) $(width)
	
run_serial_custom:
	cd bin;\
	chmod +x mandelbrot_serial;\
	./mandelbrot_serial $(height) $(width)
