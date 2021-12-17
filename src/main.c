#include "../include/functions/distributeByMedian.h"
#include "../include/functions/median.h"
#include "../include/helpers/PowerOfTwo.h"
//#include "../include/helpers/quickSort.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int data[] = { 2, 5, 1, 7, 9, 1, 2, 6, 8, 3, 9, 1, 5, 2, 6, 3, 7, 0, 1, 9, 4, 8, 2, 1, 4, 9, 0, 2, 3, 7, 0, 4 };
    int data_length
        = sizeof(data) / sizeof(data[0]);
    int ierr, num_procs, my_id;

    double median_value = 0;

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

    double* dist_data = malloc(sizeof(double) * data_length);
    median_value
        = median(my_id, num_procs, data_length, data, dist_data);
    // printf("ID: %d......", my_id);
    // for (int i = 0; i < data_length; i++) {
    //     printf("%.2lf ", dist_data[i]);
    // }
    // printf("\n");
    //distributeByMedian(my_id, num_procs, proc_data_length, dist_data, median_value);
    free(dist_data);
    ierr
        = MPI_Finalize();

    return 0;
}