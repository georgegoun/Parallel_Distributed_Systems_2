#include "../include/functions/dataImportPivot.h"
#include "../include/functions/distributeByMedian.h"
#include "../include/functions/selfValidation.h"
#include "../include/helpers/PowerOfTwo.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    // int data[] = { 20, 51, 12, 73, 94, 15, 26, 67, 88, 39, 90, 11, 52, 23, 64, 35, 76, 7, 18, 99, 40, 81, 22, 13, 44, 95, 6, 27, 38, 79, 0, 41 };
    // int data_length= sizeof(data) / sizeof(data[0]);
    int ierr, num_procs, my_id;

    // MPI

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
    selfValidation(my_id, num_procs, data_length / num_procs, proc_data);

    printf("_ID: %d Data: ", my_id);
    for (int i = 0; i < (data_length); i++) {
        printf("%.2lf ", proc_data[i]);
    }
    printf("\n");

    ierr
        = MPI_Finalize();
    free(proc_data);
    return 0;
}