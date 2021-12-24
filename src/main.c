#include "../include/functions/dataImportPivot.h"
#include "../include/functions/distributeByMedian.h"
#include "../include/functions/selfValidation.h"
#include "../include/helpers/PowerOfTwo.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    // MPI

    int ierr, num_procs, my_id;

    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (PowerOfTwo(num_procs) == 0) {
        if (my_id == 0) {
            printf("Please enter power of 2 processes\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            return 0;
        }
    }
    double* proc_data = NULL;
    int data_length = 0;

    dataImportPivot(my_id, num_procs, &proc_data, &data_length);
    distributeByMedian(my_id, num_procs, data_length, proc_data, 0, num_procs - 1);
    selfValidation(my_id, num_procs, data_length, proc_data);

    // Print every process data after exchange

    printf("_ID: %d\tData: ", my_id);
    for (int i = 0; i < (data_length); i++) {
        printf("%.2lf ", proc_data[i]);
    }
    printf("\n");

    ierr
        = MPI_Finalize();
    free(proc_data);
    return 0;
}