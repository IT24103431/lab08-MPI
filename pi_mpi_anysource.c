// pi_mpi_anysource.c  -  Exercise 6
// Rewrite of Exercise 3 (pi_mpi.c): rank 0's receive loop now uses
// MPI_ANY_SOURCE instead of a fixed source order (src = 1, 2, 3, ...).
//
// pi_mpi.c always waited for rank 1 first, then rank 2, then rank 3, etc,
// even if e.g. rank 3 actually finished first - it would sit idle in that
// slot's MPI_Recv regardless. Here, rank 0 accepts whichever worker's
// message arrives first, in ARRIVAL order rather than RANK order, and
// prints the actual source (via MPI_Status) of each message received so
// the two orders can be compared directly (see README for a run-by-run
// comparison against pi_mpi.c).
//
// Compile:  mpic++ -o job6 pi_mpi_anysource.c   (cluster: mpiicpc -o job6 pi_mpi_anysource.c)
// Run:      mpirun -n <P> ./job6

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
        // CHANGED from pi_mpi.c: receive from MPI_ANY_SOURCE instead of a
        // fixed rank order. Still receives exactly (size - 1) messages -
        // one per worker - just not in a predetermined order.
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
        MPI_Send(&local_count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
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
