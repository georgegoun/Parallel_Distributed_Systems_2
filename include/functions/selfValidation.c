#include "selfValidation.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int selfValidation(int my_id, int num_procs, int proc_data_length, double* proc_data)
{
    MPI_Status status;
    int err, len;
    char buffer[MPI_MAX_ERROR_STRING];

    double min = proc_data[0];
    double max = proc_data[0];

    for (int i = 0; i < proc_data_length; i++) {
        if (min > proc_data[i]) {
            min = proc_data[i];
        } else if (max < proc_data[i]) {
            max = proc_data[i];
        }
    }
    // printf("%d: Max: %.2lf Min: %.2lf\n", my_id, max, min);
    double max_recv = 0;

    if (my_id >= 0 && my_id < num_procs) {
        if (my_id < num_procs - 1) {
            err = MPI_Send(&max, 1, MPI_DOUBLE, my_id + 1, 0, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
            }
            // printf("MYID: %d Max Sent: %.2lf\n", my_id, max);
        }
        if (my_id > 0) {
            MPI_Recv(&max_recv, 1, MPI_DOUBLE, my_id - 1, 0, MPI_COMM_WORLD, &status);
            // printf("MYID: %d Max Recv: %.2lf Min: %.2lf\n", my_id, max_recv, min);
        }
        if (my_id > 0) {
            if (min < max_recv) {
                perror("Error: Self Validation");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 1;
}
