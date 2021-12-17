#include "distributeByMedian.h"
#include "../helpers/eucDist.h"
#include "../helpers/removeElement.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void distributeByMedian(int my_id, int num_procs, int proc_data_length, double* dist_data, double median_value)
{
    double data_to_send[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    printf("\t1||ID: %d  Median Value: %.2lf  Data: ", my_id, median_value);
    for (int i = 0; i < proc_data_length; i++) {
        printf("%.2lf ", dist_data[i]);
    }
    if (my_id < num_procs / 2) {
        int counter = 0;
        for (int i = 0; i < proc_data_length; i++) {
            if (dist_data[i] >= median_value) {
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
        // Distribution

        // TODO: 0 to change to ~=num_procs

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
    } else {
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