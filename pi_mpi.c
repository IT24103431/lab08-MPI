// pi_mpi.c  -  Exercise 3
// Parallelise the Monte Carlo estimation of Pi across multiple MPI ranks,
// using 10,000,000 random points in total.
//
// Method: throw darts at random (x, y) in [-1, 1] x [-1, 1]; the fraction
// landing inside the unit circle (x^2 + y^2 <= 1), times 4, estimates Pi.
// See: http://www.dartmouth.edu/~rc/classes/soft_dev/C_simple_ex.html
//
// Communication is deliberately explicit point-to-point (not MPI_Reduce):
// every worker rank MPI_Sends its local hit-count to rank 0, and rank 0
// MPI_Recvs from each worker IN RANK ORDER (source = 1, 2, 3, ...). This
// fixed receive order is what Exercise 6 changes to MPI_ANY_SOURCE.
//
// Compile:  mpic++ -o job3 pi_mpi.c        (cluster: mpiicpc -o job3 pi_mpi.c)
// Run:      mpirun -n <P> ./job3

#include <cstdio>
#include <cstdlib>
#include <mpi.h>

static const long long N = 10000000; // total random points

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Block-decompose the N points across ranks as evenly as possible.
    long long chunk = N / size;
    long long remainder = N % size;
    long long niter = chunk + (rank < remainder ? 1 : 0);

    // Give every rank its own random stream.
    unsigned int seed = (unsigned int)(12345 + rank * 9973);

    MPI_Barrier(MPI_COMM_WORLD); // line everyone up before timing starts
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
        // Fixed receive order: source 1, then 2, then 3, ...
        for (int src = 1; src < size; src++) {
            long long recv_count;
            MPI_Recv(&recv_count, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += recv_count;
        }
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
