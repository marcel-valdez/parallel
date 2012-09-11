SUBDIRS = src . tests

all: build test

build:
	cd src;\
	make compile;\
	make clean;\
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

run:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np 5 $(PWD)/bin/mandelbrot 640 480

run_balanced:
	cd bin;\
	mpiexec.mpich2 -machinefile /mirror/machines -np 5 $(PWD)/bin/mandelbrot_balanced 640 480
