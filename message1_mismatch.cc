// message1_mismatch.cc  -  Exercise 5.1
// Based on message1.cc, but with a deliberate source/destination mismatch:
// rank 0 sends its number to rank 2, while rank 1's receive still asks for
// a message "from rank 0" that will never arrive.
//
// Compile: mpic++ -o message1_mismatch message1_mismatch.cc
//
// Run with 3 processes to see the DEADLOCK (see README for what happens
// and why):
//   mpirun -n 3 ./message1_mismatch      # hangs forever
//
// Run with only 2 processes instead and you get a DIFFERENT failure: rank
// 0's destination (rank 2) doesn't exist in a 2-process job, so MPI aborts
// immediately with an "invalid rank" error rather than hanging:
//   mpirun -n 2 ./message1_mismatch      # aborts immediately

#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int number;
    if (rank == 0) {
        number = 42;
        // MISMATCH: sent to rank 2, not rank 1.
        MPI_Send(&number, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        std::cout << "Process 0 sent " << number << " to rank 2\n";
    } else if (rank == 1) {
        // Rank 1 still waits for a message "from rank 0" - but rank 0 never
        // sent anything to rank 1, so this call blocks forever.
        std::cout << "Process 1 waiting to receive from rank 0...\n";
        MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process 1 received " << number << "\n"; // never reached
    } else if (rank == 2) {
        // Rank 2 happens to receive rank 0's message correctly - it was
        // never "meant" to, it just matches by accident.
        MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process 2 (unexpectedly) received " << number << "\n";
    }

    MPI_Finalize();
    return 0;
}
