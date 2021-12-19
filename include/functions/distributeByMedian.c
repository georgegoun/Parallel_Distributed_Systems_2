#include "distributeByMedian.h"
#include "../helpers/eucDist.h"
#include "../helpers/removeElement.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void distributeByMedian(int my_id, int num_procs, int data_length, double* dist_data, double median_value)
{
    MPI_Status status;
    int err, len;
    char buffer[MPI_MAX_ERROR_STRING];

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
        printf("ID: %d  Median Value: %.2lf\nData:\t\t", my_id, median_value);
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }

        printf("\nBefore:\t\t");
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }
        printf("\n");
        for (int i = 0, j = 1; i < proc_data_length; i++) {
            // TODO remove -1
            if (proc_dist_data[i] > median_value) {
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
        printf("After:\t\t");
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }
        printf("\nCounter:\t%d", counter);

        data_to_send[0] = counter;
        printf("\nData to send: ");
        if (counter)
            printf("%.2lf (counter) ", data_to_send[0]);
        for (int i = 1; i < counter + 1; i++) {
            printf("%.2lf ", data_to_send[i]);
        }
        printf("\n");

        // Distribution

        // TODO: 0 and 1 to change to ~=num_procs
        // 0 to (num_procs/2)-1
        // 1 to num_procs
        if (my_id == 0) {
            double** all_data_to_send;
            all_data_to_send
                = malloc(sizeof(double*) * (num_procs / 2));

            all_data_to_send[0] = (double*)malloc((data_to_send[0] + 1) * sizeof(double));
            all_data_to_send[0] = data_to_send;

            // Captain receiving 2D array with all elements to be traded
            // rows->num_id
            // columns->[0]size, [size,...]elements
            for (int i = 1; i < num_procs / 2; i++) {
                double size = 0;

                MPI_Recv(&size, 1, MPI_DOUBLE, i, 4, MPI_COMM_WORLD, &status);
                all_data_to_send[i] = (double*)malloc((size + 1) * sizeof(double));
                MPI_Recv(all_data_to_send[i], size + 1, MPI_DOUBLE, i, 5, MPI_COMM_WORLD, &status);
            }
            printf("0: All Data Gathered: \n");
            for (int i = 0; i < num_procs / 2; i++) {
                for (int j = 0; j < all_data_to_send[i][0] + 1; j++) {
                    printf("%.2lf ", all_data_to_send[i][j]);
                }
                printf("\n");
            }

            // Sending Distribution 2D Array

            for (int i = 0; i < num_procs / 2; i++) {
                err = MPI_Send(&all_data_to_send[i][0], 1, MPI_DOUBLE, 4, 8, MPI_COMM_WORLD);
                if (err) {
                    MPI_Error_string(err, buffer, &len);
                    printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
                }
                err = MPI_Send(all_data_to_send[i], all_data_to_send[i][0] + 1, MPI_DOUBLE, 4, 9, MPI_COMM_WORLD);
                if (err) {
                    MPI_Error_string(err, buffer, &len);
                    printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
                }
            }

            // Receiving Distribution 2D Array

            double** distr_recv;
            distr_recv = malloc(sizeof(double*) * (num_procs / 2));
            for (int i = 0; i < num_procs / 2; i++) {
                double size = 0;
                MPI_Recv(&size, 1, MPI_DOUBLE, 4, 10, MPI_COMM_WORLD, &status);
                distr_recv[i] = (double*)malloc((size + 1) * sizeof(double));
                MPI_Recv(distr_recv[i], size + 1, MPI_DOUBLE, 4, 11, MPI_COMM_WORLD, &status);
            }
            printf("0: All Data Received: \n");
            for (int i = 0; i < num_procs / 2; i++) {
                for (int j = 0; j < distr_recv[i][0] + 1; j++) {
                    printf("%.2lf ", distr_recv[i][j]);
                }
                printf("\n");
            }
        }
        if (my_id > 0 && my_id < num_procs / 2) {
            err = MPI_Send(&data_to_send[0], 1, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
            }
            err = MPI_Send(data_to_send, data_to_send[0] + 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
            }
        }

    } else {
        int counter = 0;

        printf("ID: %d  Median Value: %.2lf\nData:\t\t", my_id, median_value);
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }

        printf("\nBefore:\t\t");
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }
        printf("\n");
        for (int i = 0, j = 1; i < proc_data_length; i++) {
            // TODO remove -1
            if (proc_dist_data[i] < median_value) {
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
        printf("After:\t\t");
        for (int i = 0; i < proc_data_length; i++) {
            printf("%.2lf ", proc_dist_data[i]);
        }
        printf("\nCounter:\t%d", counter);
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

        if (my_id == 4) {
            double** all_data_to_send;
            all_data_to_send
                = malloc(sizeof(double*) * (num_procs / 2));
            printf("\n\nok ko\n\n");
            all_data_to_send[0] = (double*)malloc((data_to_send[0] + 1) * sizeof(double));
            all_data_to_send[0] = data_to_send;

            // Captain receiving 2D array with all elements to be traded
            // rows->num_id
            // columns->[0]size, [size,...]elements
            for (int i = 1; i < num_procs / 2; i++) {
                double size = 0;
                MPI_Recv(&size, 1, MPI_DOUBLE, i + (num_procs / 2), 6, MPI_COMM_WORLD, &status);
                all_data_to_send[i] = (double*)malloc((size + 1) * sizeof(double));
                MPI_Recv(all_data_to_send[i], size + 1, MPI_DOUBLE, i + (num_procs / 2), 7, MPI_COMM_WORLD, &status);
            }
            printf("4: All Data Gathered: \n");
            for (int i = 0; i < num_procs / 2; i++) {
                for (int j = 0; j < all_data_to_send[i][0] + 1; j++) {
                    printf("%.2lf ", all_data_to_send[i][j]);
                }
                printf("\n");
            }

            // Receiving Distribution 2D Array

            double** distr_recv;
            distr_recv = malloc(sizeof(double*) * (num_procs / 2));
            for (int i = 0; i < num_procs / 2; i++) {
                double size = 0;
                MPI_Recv(&size, 1, MPI_DOUBLE, 0, 8, MPI_COMM_WORLD, &status);
                distr_recv[i] = (double*)malloc((size + 1) * sizeof(double));
                MPI_Recv(distr_recv[i], size + 1, MPI_DOUBLE, 0, 9, MPI_COMM_WORLD, &status);
            }
            printf("4: All Data Received: \n");
            for (int i = 0; i < num_procs / 2; i++) {
                for (int j = 0; j < distr_recv[i][0] + 1; j++) {
                    printf("%.2lf ", distr_recv[i][j]);
                }
                printf("\n");
            }

            //Sending Distribution 2D Array

            for (int i = 0; i < num_procs / 2; i++) {
                err = MPI_Send(&all_data_to_send[i][0], 1, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD);
                if (err) {
                    MPI_Error_string(err, buffer, &len);
                    printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
                }
                err = MPI_Send(all_data_to_send[i], all_data_to_send[i][0] + 1, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD);
                if (err) {
                    MPI_Error_string(err, buffer, &len);
                    printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
                }
            }
        }

        if (my_id > 4 && my_id < num_procs) {
            err = MPI_Send(&data_to_send[0], 1, MPI_DOUBLE, 4, 6, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
            }
            err = MPI_Send(data_to_send, data_to_send[0] + 1, MPI_DOUBLE, 4, 7, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
            }
        }
    }
}

// array remove elements | send elements and counter for every process

// leader proccess recieves all the elements in 2d array first element counter and then values
