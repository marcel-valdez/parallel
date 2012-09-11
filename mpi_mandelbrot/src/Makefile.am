all: build build_balanced

build: compile clean

debug: debug_unbalanced debug_balanced 

debug_balanced: compile_balanced_debug clean

debug_unbalanced: compile_debug clean

build_balanced: compile_balanced clean

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
