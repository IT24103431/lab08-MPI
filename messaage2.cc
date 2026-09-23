#include <cstdio>
#include <cstdlib>
#include <mpi.h>
int main(void)
{
    int rank;
    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char name[30];
    int len;
    MPI_Get_processor_name( name, &len );
    int x[10], y[10];
    if (rank == 1) {
      for (int r =0;r <10; r++)
         x[r] = 10*r;

      printf("SEnding message to computer 3 from computer 1\n");
      MPI_Ssend(x, 10, MPI_INT, 3, 0, MPI_COMM_WORLD);
    }
    else if (rank == 3) {
      MPI_Recv(y, 10, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
      printf("in computer 3 the value of y is printed\n");
      for (int r=0;r<10;r++)
         printf(" %d ",y[r]);
    }
    else
      printf("Just a normal process From rank %d machine %s\n", rank, name);
    MPI_Finalize();
}
