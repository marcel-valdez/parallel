all: compile clean

build: compile clean

compile: 
	$(CC) mandelbrot.c -o mandelbrot -fno-stack-protector; \
	cd ..; \
	mkdir bin; \
	mv src/mandelbrot bin
clean:
	rm -f *.o


