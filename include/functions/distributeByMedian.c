#include "distributeByMedian.h"
#include "../helpers/eucDist.h"
#include "../helpers/removeElement.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void distributeByMedian(int my_id, int num_procs, int data_length, double* dist_data, double median_value)
{

    int proc_data_length = data_length / num_procs;
    double* proc_dist_data = malloc(sizeof(double) * proc_data_length);
    for (int i = 0; i < proc_data_length; i++) {
        proc_dist_data[i] = dist_data[my_id * proc_data_length + i];
    }

    // +1 because first element is the counter of how many elements to be exchanged
    double* data_to_send = malloc(sizeof(double) * (proc_data_length + 1));

    // printf("\t1||ID: %d  Median Value: %.2lf  Data: ", my_id, median_value);
    // for (int i = 0; i < proc_data_length; i++) {
    //     printf("%.2lf ", dist_data[i]);
    // }
    if (my_id < num_procs / 2) {
        int counter = 0;
        printf("ID: %d  Median Value: %.2lf\nData: ", my_id, median_value);
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }

        printf("Before\n");
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }
        printf("\n");
        for (int i = 0, j = 1; i < proc_data_length; i++) {
            // TODO remove -1
            if (proc_dist_data[i] >= median_value - 1) {
                data_to_send[j] = proc_dist_data[i];
                counter++;
                // Remove elements from array without keeping the same order
                removeElement(proc_dist_data, i, proc_data_length);
                proc_data_length--;
                i--;
                j++;
                // TODO: DATA SEND EVERY PROCESS HOW MANY TO CHANGE
            }
        }
        printf("After\n");
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }
        printf("\n");

        data_to_send[0] = counter;
        printf("\nData to send: ");
        if (counter)
            printf("%.2lf (counter) ", data_to_send[0]);
        for (int i = 1; i < counter + 1; i++) {
            printf("%.2lf ", data_to_send[i]);
        }
        printf("\n");

        // Distribution
    }
    // TODO: 0 to change to ~=num_procs
    /*
        if (my_id == 0) {
        double** trading_values = NULL;
        // trading_values[my_id] = data_to_send;
        for (int i = 4; i < num_procs; i++) {
            int size = 0;
            MPI_Recv(&size, 1, MPI_INT, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            double* trading_value = malloc(sizeof(double) * (size));
            MPI_Recv(trading_value, size, MPI_DOUBLE, i, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            trading_values[i - 4] = trading_value;
        }
        printf("\n\n");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < (int)trading_values[i][0] + 1; j++) {
                printf("%.2lf ", trading_values[i][j]);
            }
            printf("\n");
        }
        }
        */
    else {
        int counter = 0;
        printf("ID: %d  Median Value: %.2lf\nData: ", my_id, median_value);
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }

        printf("Before\n");
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }
        printf("\n");
        for (int i = 0, j = 1; i < proc_data_length; i++) {
            // TODO remove -1
            if (proc_dist_data[i] < median_value + 1) {
                data_to_send[j] = proc_dist_data[i];
                counter++;
                // Remove elements from array without keeping the same order
                removeElement(proc_dist_data, i, proc_data_length);
                proc_data_length--;
                i--;
                j++;
                // TODO: DATA SEND EVERY PROCESS HOW MANY TO CHANGE
            }
        }
        printf("After\n");
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }
        printf("\n");
        data_to_send[0]
            = counter;
        printf("\nData to send: ");
        if (counter)
            printf("%.2lf (counter) ", data_to_send[0]);
        for (int i = 1; i < counter + 1; i++) {
            printf("%.2lf ", data_to_send[i]);
        }
        printf("\n");

        // Distribution
    }
}

/*else {
        int counter = 0;
        for (int i = 0; i < proc_data_length; i++) {
            if (dist_data[i] < median_value) {
                data_to_send[counter + 1] = dist_data[i];
                counter++;

                // Remove elements from array without keeping the same order

                removeElement(dist_data, i, proc_data_length);
                proc_data_length--;
                i--;
                // TODO: DATA SEND EVERY PROCESS HOW MANY TO CHANGE
            }
        }
        data_to_send[0] = counter;
        printf("\t2||ID: %d  Data: ", my_id);
        for (int i = 0; i < counter + 1; i++) {
            printf("%.2lf ", data_to_send[i]);
        }
        printf("\t3||ID: %d  Median Value: %.2lf  Data: ", my_id, median_value);
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", dist_data[i]);
        }
    }
    printf("\n");

    // Because first number is the size value

    int size = (int)dist_data[0] + 1;
    MPI_Send(&size, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
    MPI_Send(dist_data, size, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD);
}
// array remove elements | send elements and counter for every process

// leader proccess recieves all the elements in 2d array first element counter and then values
*/