TESTS = check_mandelbrot_balanced check_mandelbrot check_mandelbrot_common
check_PROGRAMS = check_mandelbrot_balanced check_mandelbrot check_mandelbrot_common

check_mandelbrot_balanced_SOURCES = check_mandelbrot_balanced.c
check_mandelbrot_balanced_CFLAGS = -fno-stack-protector -D'DEBUG' @CHECK_CFLAGS@
check_mandelbrot_balanced_LDADD = @CHECK_LIBS@

check_mandelbrot_SOURCES = check_mandelbrot.c
check_mandelbrot_CFLAGS = -fno-stack-protector -D'DEBUG' @CHECK_CFLAGS@
check_mandelbrot_LDADD = @CHECK_LIBS@

check_mandelbrot_common_SOURCES = check_mandelbrot_common.c
check_mandelbrot_common_CFLAGS = -fno-stack-protector -D'DEBUG' @CHECK_CFLAGS@
check_mandelbrot_common_LDADD = @CHECK_LIBS@
