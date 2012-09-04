AC_INIT(mandelbrot, version-0.1)
AM_INIT_AUTOMAKE()
CC=mpicc.mpich2

echo "*****************************************************************"
echo "* This is the automake configuration for the MPI for Mandelbrot *"
echo "*****************************************************************"
echo "----------------------------------------"
echo "Testing for a MPI C compiler"
echo "----------------------------------------"
AC_PROG_CC

echo "----------------------------------------"
echo "Testing for Standard I/O headers"
echo "----------------------------------------"
AC_CHECK_HEADERS(stdio.h)
AM_PROG_CC_C_O

echo "----------------------------------------"
echo "Testing for MPI headers"
echo "----------------------------------------"
AC_CHECK_HEADERS(mpi.h)

echo "----------------------------------------"
echo "Testing for CHECK Unit testing framework"
echo "----------------------------------------"
AM_PATH_CHECK()
AC_CHECK_HEADERS(check.h)

AC_CONFIG_FILES([
 Makefile
 src/Makefile
 tests/Makefile
])

AC_OUTPUT
