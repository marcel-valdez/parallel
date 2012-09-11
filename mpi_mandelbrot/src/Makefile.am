all: build build_balanced

build: compile clean

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
