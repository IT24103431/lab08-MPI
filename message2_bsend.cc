// message2_bsend.cc  -  Exercise 5.2
// message2.cc rewritten to use Buffered Send (MPI_Bsend) instead of the
// blocking MPI_Send.
//
// MPI_Bsend copies the outgoing data into an MPI-managed buffer and
// returns immediately, so the sender does not need to wait for the
// matching receive. To make that independence visible - and per the
// exercise requirement to NOT overwrite the variable being sent - each of
// the three messages uses its own array slot instead of one shared
// "number" variable being reassigned each loop iteration.
//
// Compile: mpic++ -o message2_bsend message2_bsend.cc
// Run:     mpirun -n 2 ./message2_bsend

#include <mpi.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // One independent variable per message - never reused/overwritten.
    int numbers[3] = {0, 10, 20};

    if (rank == 0) {
        // A buffered send needs an attached buffer to copy into. Each
        // buffered int needs MPI_BSEND_OVERHEAD bytes of bookkeeping on
        // top of its payload.
        int buf_size = 3 * (sizeof(int) + MPI_BSEND_OVERHEAD);
        char *buffer = (char *)malloc(buf_size);
        MPI_Buffer_attach(buffer, buf_size);

        for (int i = 0; i < 3; i++) {
            MPI_Bsend(&numbers[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            std::cout << "Process 0 buffered-sent " << numbers[i] << "\n";
        }

        MPI_Buffer_detach(&buffer, &buf_size); // blocks until all buffered sends complete
        free(buffer);
    } else if (rank == 1) {
        for (int i = 0; i < 3; i++) {
            int received;
            MPI_Recv(&received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Process 1 received " << received << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}
