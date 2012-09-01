/* C Example */
#include <mpi.h>
#include <stdio.h>

int main (int argc, char* argv[])
{
	int rank, size;

	MPI_Init (&argc, &argv); /* starts MPI */
	printf("Rank: %d Size: %d", rank, size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	printf("Hello world from process %d of %d\n", rank, size);
	MPI_Finalize();
	return 0;
}
