all: compile clean

build: compile clean

compile: 
	$(CC) mandelbrot.c -o mandelbrot -fno-stack-protector; \
	cd ..;\
	mkdir -p bin; \
	mv src/mandelbrot bin; \
	cd src
clean:
	rm -f *.o


