#include "distributeByMedian.h"
#include "../helpers/eucDist.h"
#include "../helpers/quickSelect.h"
#include "../helpers/removeElement.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void distributeByMedian(int my_id, int num_procs, int data_length, double* dist_data, double median_value, int low, int high)
{
    MPI_Status status;
    int err, len;
    char buffer[MPI_MAX_ERROR_STRING];

    int proc_data_length = data_length / num_procs;
    int proc_rec_length = data_length / num_procs;
    int proc_rec_data_length = (high - low + 1) * proc_rec_length;
    double* proc_dist_data = malloc(sizeof(double) * proc_data_length);

    for (int i = 0; i < proc_data_length; i++) {
        proc_dist_data[i] = dist_data[my_id * proc_data_length + i];
    }

    // +1 because first element is the counter of how many elements to be exchanged
    double* data_to_send = malloc(sizeof(double) * (proc_data_length + 1));

    if (my_id == 0) {
        double* testing = malloc(sizeof(double) * proc_rec_data_length);
        for (int i = 0; i < proc_rec_data_length; i++) {
            testing[i] = dist_data[i + (low * proc_rec_length)];
        }
        if (proc_rec_data_length % 2 == 0) {
            median_value = (quickselect(testing, 0, proc_rec_data_length - 1, (proc_rec_data_length / 2) - 1) + quickselect(testing, 0, proc_rec_data_length - 1, (proc_rec_data_length / 2))) / 2;
        } else {
            median_value = quickselect(testing, 0, proc_rec_data_length - 1, proc_rec_data_length / 2);
        }
        for (int i = low; i < high + 1; i++) {
            err = MPI_Send(&median_value, 1, MPI_DOUBLE, i, 100, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
            }
        }
    }
    MPI_Recv(&median_value, 1, MPI_DOUBLE, 0, 100, MPI_COMM_WORLD, &status);

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

        // Couunter of how many elements to receive
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

            int send_sizes = data_to_send[0];
            for (int i = 1; i < num_procs / 2; i++) {
                double size = 0;
                MPI_Recv(&size, 1, MPI_DOUBLE, i, 4, MPI_COMM_WORLD, &status);
                send_sizes += (int)size;
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
            int recv_sizes = 0;
            for (int i = 0; i < num_procs / 2; i++) {
                double size = 0;
                MPI_Recv(&size, 1, MPI_DOUBLE, 4, 10, MPI_COMM_WORLD, &status);
                recv_sizes += (int)size;

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

            // Database median ERROR check

            if ((int)recv_sizes != (int)send_sizes) {
                printf("Error in your database\n");
                exit(EXIT_FAILURE);
            }

            // Making Distribution among processes real

            double* distr_1d = malloc(sizeof(double) * (int)recv_sizes);
            for (int i = 0, k = 0; i < num_procs / 2; i++) {
                for (int j = 0; j < distr_recv[i][0]; j++, k++) {
                    distr_1d[k] = distr_recv[i][j + 1];
                }
            }
            for (int i = 0; i < recv_sizes; i++) {
                printf("%.2lf ", distr_1d[i]);
            }
            printf("\n");

            double** distr;
            distr = malloc(sizeof(double*) * (num_procs / 2));
            for (int i = 0, k = 0; i < num_procs / 2; i++) {
                distr[i] = (double*)malloc((all_data_to_send[i][0] + 1) * sizeof(double));
                distr[i][0] = all_data_to_send[i][0];
                for (int j = 0; j < all_data_to_send[i][0]; j++, k++) {
                    distr[i][j + 1] = distr_1d[k];
                }
            }
            printf("0: All Data Distribution: \n");
            for (int i = 0; i < num_procs / 2; i++) {
                for (int j = 0; j < distr[i][0] + 1; j++) {
                    printf("%.2lf ", distr[i][j]);
                }
                printf("\n");
            }

            // Distribution: Passing to processes the right values

            for (int i = 0; i < num_procs / 2; i++) {
                err = MPI_Send(distr[i], (int)distr[i][0] + 1, MPI_DOUBLE, i, 12, MPI_COMM_WORLD);
                if (err) {
                    MPI_Error_string(err, buffer, &len);
                    printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
                }
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

        // Distribution: All processes receive right values

        // TODO: CHANGE 0
        double* distr_arr = malloc(sizeof(double) * ((int)data_to_send[0] + 1));
        MPI_Recv(distr_arr, (int)data_to_send[0] + 1, MPI_DOUBLE, 0, 12, MPI_COMM_WORLD, &status);

        // Fitting the right values

        for (int i = 0; i < proc_data_length; i++) {
            dist_data[(my_id * (data_length / num_procs)) + i] = proc_dist_data[i];
        }
        for (int i = 0; i < distr_arr[0]; i++) {
            dist_data[(my_id * (data_length / num_procs)) + proc_data_length + i] = distr_arr[i + 1];
        }
        printf("END_ ID: %d  Median Value: %.2lf\nData:\t\t", my_id, median_value);
        for (int i = 0; i < data_length / num_procs; i++) {
            printf("%.2lf ", dist_data[(my_id * (data_length / num_procs)) + i]);
        }
        printf("\n");

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
            int recv_sizes = 0;
            for (int i = 0; i < num_procs / 2; i++) {
                double size = 0;
                MPI_Recv(&size, 1, MPI_DOUBLE, 0, 8, MPI_COMM_WORLD, &status);
                recv_sizes += (int)size;
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

            // Sending Distribution 2D Array

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

            // Making Distribution among processes real

            double* distr_1d = malloc(sizeof(double) * (int)recv_sizes);
            for (int i = 0, k = 0; i < num_procs / 2; i++) {
                for (int j = 0; j < distr_recv[i][0]; j++, k++) {
                    distr_1d[k] = distr_recv[i][j + 1];
                }
            }
            for (int i = 0; i < recv_sizes; i++) {
                printf("%.2lf ", distr_1d[i]);
            }
            printf("\n");

            double** distr;
            distr = malloc(sizeof(double*) * (num_procs / 2));
            for (int i = 0, k = 0; i < num_procs / 2; i++) {
                distr[i] = (double*)malloc((all_data_to_send[i][0] + 1) * sizeof(double));
                distr[i][0] = all_data_to_send[i][0];
                for (int j = 0; j < all_data_to_send[i][0]; j++, k++) {
                    distr[i][j + 1] = distr_1d[k];
                }
            }
            printf("4: All Data Distribution: \n");
            for (int i = 0; i < num_procs / 2; i++) {
                for (int j = 0; j < distr[i][0] + 1; j++) {
                    printf("%.2lf ", distr[i][j]);
                }
                printf("\n");
            }

            // Distribution: Passing to processes the right values

            for (int i = 0; i < num_procs / 2; i++) {
                err = MPI_Send(distr[i], (int)distr[i][0] + 1, MPI_DOUBLE, i + num_procs / 2, 13, MPI_COMM_WORLD);
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

        // Distribution: All processes receive right values

        // TODO: CHANGE 4
        double* distr_arr = malloc(sizeof(double) * ((int)data_to_send[0] + 1));
        MPI_Recv(distr_arr, (int)data_to_send[0] + 1, MPI_DOUBLE, 4, 13, MPI_COMM_WORLD, &status);

        // Fitting the right values

        for (int i = 0; i < proc_data_length; i++) {
            dist_data[(my_id * (data_length / num_procs)) + i] = proc_dist_data[i];
        }
        for (int i = 0; i < distr_arr[0]; i++) {
            dist_data[(my_id * (data_length / num_procs)) + proc_data_length + i] = distr_arr[i + 1];
        }
        printf("END_ ID: %d  Median Value: %.2lf\nData:\t\t", my_id, median_value);
        for (int i = 0; i < data_length / num_procs; i++) {
            printf("%.2lf ", dist_data[(my_id * (data_length / num_procs)) + i]);
        }
        printf("\n");
    }
}

// array remove elements | send elements and counter for every process

// leader proccess recieves all the elements in 2d array first element counter and then values
