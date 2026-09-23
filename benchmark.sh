#!/usr/bin/env bash
# benchmark.sh  -  Exercise 4 (data collection)
#
# Builds Exercise 2 (sum_mpi.c) and Exercise 3 (pi_mpi.c) and times each one
# across a range of process counts (mean of 3 runs per count), writing:
#   results/sum_results.csv   (processors,time_seconds)
#   results/pi_results.csv    (processors,time_seconds)
#
# Run plot_graphs.py afterwards to turn these into the Exercise 4 graphs.
#
# Usage: ./benchmark.sh   (needs mpic++/mpirun on PATH; run from repo root)

set -e
cd "$(dirname "$0")"
mkdir -p results

mpic++ -o job2 sum_mpi.c
mpic++ -o job3 pi_mpi.c

PROC_COUNTS="1 2 4 8 16"
REPEATS=3

bench() {
    prog=$1
    outfile=$2
    echo "processors,time_seconds" > "$outfile"
    for p in $PROC_COUNTS; do
        total=0
        for r in $(seq 1 $REPEATS); do
            t=$(mpirun --oversubscribe -n "$p" "./$prog" | grep -o 'Elapsed time: [0-9.]*' | grep -o '[0-9.]*$')
            total=$(echo "$total $t" | awk '{printf "%.6f", $1+$2}')
        done
        avg=$(echo "$total $REPEATS" | awk '{printf "%.6f", $1/$2}')
        echo "$p,$avg" >> "$outfile"
        echo "$prog  P=$p  avg_time=${avg}s"
    done
}

bench job2 results/sum_results.csv
bench job3 results/pi_results.csv

rm -f job2 job3
echo "Done. Results in results/sum_results.csv and results/pi_results.csv"
