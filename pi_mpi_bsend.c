// pi_mpi_bsend.c  -  Exercise 7
// Rewrite of Exercise 6 (pi_mpi_anysource.c): workers now send their local
// hit-count with Buffered Send (MPI_Bsend) instead of the blocking
// MPI_Send. Rank 0's receive loop is unchanged - still MPI_ANY_SOURCE.
//
// Each rank that calls MPI_Bsend needs its OWN attached buffer (buffering
// is per-process, not shared), so every worker attaches/detaches its own
// small buffer around its single send.
//
// Compile:  mpic++ -o job7 pi_mpi_bsend.c   (cluster: mpiicpc -o job7 pi_mpi_bsend.c)
// Run:      mpirun -n <P> ./job7

#include <cstdio>
#include <cstdlib>
#include <mpi.h>

static const long long N = 10000000; // total random points

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long chunk = N / size;
    long long remainder = N % size;
    long long niter = chunk + (rank < remainder ? 1 : 0);

    unsigned int seed = (unsigned int)(12345 + rank * 9973);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    long long local_count = 0;
    for (long long i = 0; i < niter; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        if (x * x + y * y <= 1.0)
            local_count++;
    }

    long long total_count = local_count;

    if (rank == 0) {
        // Unchanged from Exercise 6: still MPI_ANY_SOURCE, order not fixed.
        printf("Arrival order (source rank per message received): ");
        for (int i = 1; i < size; i++) {
            long long recv_count;
            MPI_Status status;
            MPI_Recv(&recv_count, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            total_count += recv_count;
            printf("%d ", status.MPI_SOURCE);
        }
        printf("\n");
    } else {
        // CHANGED from pi_mpi_anysource.c: buffered send, with its own
        // per-rank attached buffer.
        int buf_size = sizeof(long long) + MPI_BSEND_OVERHEAD;
        char *buffer = (char *)malloc(buf_size);
        MPI_Buffer_attach(buffer, buf_size);

        MPI_Bsend(&local_count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);

        MPI_Buffer_detach(&buffer, &buf_size); // blocks until the send completes
        free(buffer);
    }

    double t1 = MPI_Wtime();

    if (rank == 0) {
        double pi_estimate = 4.0 * (double)total_count / (double)N;
        printf("Pi estimate (N=%lld) = %.7f  (hits=%lld)\n", N, pi_estimate, total_count);
        printf("Processors: %d  Elapsed time: %f seconds\n", size, t1 - t0);
    }

    MPI_Finalize();
    return 0;
}
