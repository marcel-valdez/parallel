all: compile clean

build: compile clean

compile: 
	$(CC) mandelbrot.c -o mandelbrot; \
	cd ..; \
	mkdir bin; \
	mv src/mandelbrot bin
clean:
	rm -f *.o


