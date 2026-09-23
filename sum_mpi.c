// sum_mpi.c  -  Exercise 2
// Parallelise adding up the numbers 1..10,000,000 across multiple MPI ranks.
//
// Each rank sums an (almost) equal contiguous block of the range, the
// partial sums are combined on rank 0 with MPI_Reduce, and rank 0 reports
// the total plus the wall-clock time taken.
//
// Compile:  mpic++ -o job2 sum_mpi.c        (cluster: mpiicpc -o job2 sum_mpi.c)
// Run:      mpirun -n <P> ./job2

#include <cstdio>
#include <mpi.h>

static const long long N = 10000000; // sum 1..10,000,000

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Barrier(MPI_COMM_WORLD); // line everyone up before timing starts
    double t0 = MPI_Wtime();

    // Block-decompose 1..N across ranks as evenly as possible.
    long long chunk = N / size;
    long long remainder = N % size;
    long long start = rank * chunk + (rank < remainder ? rank : remainder) + 1;
    long long count = chunk + (rank < remainder ? 1 : 0);
    long long end = start + count - 1; // inclusive

    long long local_sum = 0;
    for (long long i = start; i <= end; i++)
        local_sum += i;

    long long total_sum = 0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double t1 = MPI_Wtime();

    if (rank == 0) {
        printf("Sum 1..%lld = %lld (expected %lld)\n", N, total_sum, N * (N + 1) / 2);
        printf("Processors: %d  Elapsed time: %f seconds\n", size, t1 - t0);
    }

    MPI_Finalize();
    return 0;
}
