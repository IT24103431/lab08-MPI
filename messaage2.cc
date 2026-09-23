// message2.cc  -  rank 0 sends three numbers to rank 1 using the same variable
// mpicxx message2.cc -o message2 && mpirun -np 2 ./message2
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int number;
    for (int i = 0; i < 3; i++) {
        if (rank == 0) {
            number = i * 10;
            MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            std::cout << "Process 0 sent " << number << "\n";
        } else if (rank == 1) {
            MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Process 1 received " << number << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}
