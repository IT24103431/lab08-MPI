# SE4060-MPI-Part1
Starter Code MPI code

Exercise 1
Try the following program segment.

```c
// HelloMPI.c
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
```

Compile the program as follows

```mpiicpc -o job1 HelloMPI.c```

You can control the number of threads to be used by adding the following in the pbs file.


```
#PBS -l nodes=1
#PBS -l walltime=00:02:00
#PBS -l select=1
cat $PBS_NODEFILE
NPROC=2
cd $PBS_O_WORKDIR
MPISIZE=$NPROC
MPIPROG=`basename $PBS_JOBNAME .pbs`
echo 'Running MPI program' $MPIPROG 'on' $MPISIZE 'processes'
echo 'Started at' `date`
echo '--------------------------------------------------------------------------------'
(time mpirun -n $MPISIZE ./$MPIPROG) 2>&1
echo '--------------------------------------------------------------------------------'
echo 'Finished at' `date`
```

Run the program by submitting the job to the queue

```qsub job1.pbs```

Note: The above pbs script is written in such a way that the execution program should have the
same name as the pbs script, that is the executable file should be named as job1
