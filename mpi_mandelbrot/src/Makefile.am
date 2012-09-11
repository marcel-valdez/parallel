all: build build_balanced build_serial

build: build_unbalanced build_balanced build_serial

debug: debug_unbalanced debug_balanced debug_serial

build_unbalanced: compile clean

build_balanced: compile_balanced clean

build_serial: compile_serial clean

debug_balanced: compile_balanced_debug clean

debug_unbalanced: compile_debug clean

debug_serial: compile_serial_debug clean

compile_balanced:
	$(CC) mandelbrot.c -o mandelbrot_balanced -fno-stack-protector -D'BALANCED'; \
	cd ..;\
	mkdir -p bin; \
	mv src/mandelbrot_balanced bin; \
	cd src

compile: 
	$(CC) mandelbrot.c -o mandelbrot -fno-stack-protector; \
	cd ..;\
	mkdir -p bin; \
	mv src/mandelbrot bin; \
	cd src

compile_serial:
	gcc mandelbrot.c -o mandelbrot_serial -fno-stack-protector -D'SERIAL';\
	cd ..; \
	mkdir -p bin; \
	mv src/mandelbrot_serial bin; \
	cd src

clean:
	rm -f *.o

compile_debug:
	$(CC) mandelbrot.c -o mandelbrot -D'DEBUG' -fno-stack-protector; \
	cd ..;\
	mkdir -p bin; \
	mv src/mandelbrot bin; \
	cd src
	
compile_balanced_debug:
	$(CC) mandelbrot.c -o mandelbrot_balanced -fno-stack-protector -D'BALANCED' -D'DEBUG'; \
	cd ..;\
	mkdir -p bin; \
	mv src/mandelbrot_balanced bin; \
	cd src

compile_serial_debug:
	gcc mandelbrot.c -o mandelbrot_serial -fno-stack-protector -D'SERIAL' -D'DEBUG'; \
	cd ..; \
	mkdir -p bin; \
	mv src/mandelbrot_serial bin; \
	cd src
