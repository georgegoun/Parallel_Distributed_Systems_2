#include "../include/functions/pivotStart.h"
#include "../include/helpers/PowerOfTwo.h"
//#include "../include/helpers/quickSort.h"
#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int data[] = { 2, 5, 1, 7, 9, 1, 2, 6, 8, 3, 9, 1, 5, 2, 6, 3, 7, 0, 1, 9, 4, 8, 2, 1, 4, 9, 0, 2, 3, 7, 0, 4 };
    int data_length
        = sizeof(data) / sizeof(data[0]);
    int ierr, num_procs, my_id;
    //MPI

    ierr = MPI_Init(&argc, &argv);

    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (PowerOfTwo(num_procs) == 0) {
        if (my_id == 0) {
            printf("Please enter power of 2 processes\n");
            return 0;
        }
    }

    pivotStart(my_id, num_procs, data_length, data);

    ierr
        = MPI_Finalize();
    return 0;
}