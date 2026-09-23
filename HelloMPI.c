#include <cstdio>
#include <cstdlib>
#include <mpi.h>
int main(void)
{
    int rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char name[30];
    int len;
    MPI_Get_processor_name( name, &len );
    if (rank == 0)
      printf("Hello World! From rank %d machine %s\n", rank, name);
    else
      printf("Just a normal process From rank %d machine %s\n", rank, name);
    MPI_Finalize();
}
