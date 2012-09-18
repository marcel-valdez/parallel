all: build

build: build_unbalanced

debug: debug_unbalanced

build_unbalanced: compile clean

debug_unbalanced: compile_debug clean

compile: 
	$(CC) traslate.c -Wall -o traslate -fno-stack-protector -lm; \
	cd ..;\
	mkdir -p bin; \
	mv src/traslate bin; \
	cd src

clean:
	rm -f *.o

compile_debug:
	$(CC) traslate.c -o traslate -D'DEBUG' -fno-stack-protector -lm; \
	cd ..;\
	mkdir -p bin; \
	mv src/traslate bin; \
	cd src
