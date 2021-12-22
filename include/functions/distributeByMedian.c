#include "distributeByMedian.h"
#include "../helpers/eucDist.h"
#include "../helpers/quickSelect.h"
#include "../helpers/removeElement.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// 0: receive data for median number
// 1: announcing median number
// 2: sending data for exchange to captain
// 3: ---""---
// 4: sending to all processes thedistribution data
void distributeByMedian(int my_id, int num_procs, int data_length, double* proc_data, int low, int high)
{

    if (low == high) {
        return;
    }

    MPI_Status status;
    int err, len;
    char buffer[MPI_MAX_ERROR_STRING];

    int proc_data_length = data_length / num_procs;

    // Recursion variables

    int split = ((high - low) / 2) + low;
    int split_count = ((high - low) / 2);
    int rec_num_procs = high - low + 1;

    double median_value;

    // Distribution variables
    // +1 because first element is the counter of how many elements to be exchanged
    double* proc_data_to_send = malloc(sizeof(double) * (proc_data_length + 1));

    err = MPI_Send(proc_data, proc_data_length, MPI_DOUBLE, split, 0, MPI_COMM_WORLD);
    if (err) {
        MPI_Error_string(err, buffer, &len);
        printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
    }

    // Testing Data Before

    // printf("ID: %d Data Before: ", my_id);
    // for (int i = 0; i < proc_data_length; i++) {
    //     printf("%.2lf ", proc_data[i]);
    // }
    // printf("\n");

    // Captain Process announcing median number

    if (my_id == split) {
        // Gather data from processes to find median
        double* data_find_median = malloc(sizeof(double) * (rec_num_procs * proc_data_length));
        for (int i = low; i < high + 1; i++) {
            if (i == split) {
                for (int j = 0; j < proc_data_length; j++) {
                    data_find_median[j + ((my_id - low) * proc_data_length)] = proc_data[j];
                }
            } else {
                double* tmp = malloc(sizeof(double) * proc_data_length);
                MPI_Recv(tmp, proc_data_length, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
                for (int j = 0; j < proc_data_length; j++) {
                    data_find_median[j + ((i - low) * proc_data_length)] = tmp[j];
                }
                free(tmp);
            }
        }
        // Finding Median using quick select
        median_value
            = (quickselect(data_find_median, 0, (rec_num_procs * proc_data_length) - 1, ((rec_num_procs * proc_data_length) / 2) - 1)
                  + quickselect(data_find_median, 0, (rec_num_procs * proc_data_length) - 1, ((rec_num_procs * proc_data_length) / 2)))
            / 2;

        free(data_find_median);
        for (int i = low; i < high + 1; i++) {

            err = MPI_Send(&median_value, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
            }
        }
    }

    // Median value Received
    MPI_Recv(&median_value, 1, MPI_DOUBLE, split, 1, MPI_COMM_WORLD, &status);

    // Sending data to Captain
    int proc_data_median_length = proc_data_length;
    double* proc_data_median = malloc(sizeof(double) * proc_data_length);
    for (int i = 0; i < proc_data_length; i++) {
        proc_data_median[i] = proc_data[i];
    }
    int counter = 0;
    proc_data_to_send[0] = 0;
    if (my_id <= split) {
        for (int i = 0, j = 1; i < proc_data_median_length; i++) {
            if (proc_data_median[i] > median_value) {
                proc_data_to_send[j] = proc_data_median[i];
                counter++;
                // Remove elements from array without keeping the same order
                removeElement(proc_data_median, i, proc_data_median_length);
                proc_data_median_length--;
                i--;
                j++;
            }
        }
    } else {
        for (int i = 0, j = 1; i < proc_data_median_length; i++) {
            if (proc_data_median[i] < median_value) {
                proc_data_to_send[j] = proc_data_median[i];
                counter++;
                // Remove elements from array without keeping the same order
                removeElement(proc_data_median, i, proc_data_median_length);
                proc_data_median_length--;
                i--;
                j++;
            }
        }
    }

    // Testing Data After

    // printf("ID: %d Data After: ", my_id);
    // for (int i = 0; i < proc_data_median_length; i++) {
    //     printf("%.2lf ", proc_data_median[i]);
    // }
    // printf("\n");

    // Counter of how many elements stored and ready to received
    proc_data_to_send[0] = (double)counter;

    // Sending data to Captain
    err = MPI_Send(&proc_data_to_send[0], 1, MPI_DOUBLE, split, 2, MPI_COMM_WORLD);
    if (err) {
        MPI_Error_string(err, buffer, &len);
        printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
    }
    err = MPI_Send(proc_data_to_send, proc_data_to_send[0] + 1, MPI_DOUBLE, split, 3, MPI_COMM_WORLD);
    if (err) {
        MPI_Error_string(err, buffer, &len);
        printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
    }

    // Captain process receiving
    if (my_id == split) {
        int left_sizes = 0;
        int right_sizes = 0;
        double** distr_data = malloc(sizeof(double*) * (rec_num_procs));
        for (int i = low, j = 0; i < high + 1; i++, j++) {
            double size = 0;
            MPI_Recv(&size, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &status);
            // For checking later if left and right counter are the samee
            (i <= split) ? (left_sizes += (int)size)
                         : (right_sizes += (int)size);
            distr_data[j] = (double*)malloc((size + 1) * sizeof(double));
            MPI_Recv(distr_data[j], size + 1, MPI_DOUBLE, i, 3, MPI_COMM_WORLD, &status);
        }
        if (left_sizes != right_sizes) {
            printf("Error in your database\n");
            exit(EXIT_FAILURE);
        }

        // Testing Distribution Data

        // printf("Distr data: \n");
        // for (int i = 0; i < rec_num_procs; i++) {

        //     printf("ID_d: %d|| ", i);
        //     for (int j = 0; j < distr_data[i][0]; j++) {
        //         printf("%.2lf ", distr_data[i][j + 1]);
        //     }
        //     printf("\n");
        // }

        // Left - Right Distribution

        // left_sizes = right_sizes
        double* distr1d_left_arr
            = malloc(sizeof(double) * left_sizes);
        double* distr1d_right_arr = malloc(sizeof(double) * right_sizes);

        // distr_data[0] = size of the array
        for (int i = low, l = 0; i < split + 1; i++) {
            for (int j = 0; j < distr_data[i - low][0]; j++, l++) {
                distr1d_left_arr[l] = distr_data[i - low][j + 1];
            }
        }

        for (int i = split + 1, l = 0; i < high + 1; i++) {
            for (int j = 0; j < distr_data[i - low][0]; j++, l++) {
                distr1d_right_arr[l] = distr_data[i - low][j + 1];
            }
        }
        for (int i = low; i < high + 1; i++) {
            // Send the new data to left sided processes
            double* sending = malloc(sizeof(double) * distr_data[i - low][0]);
            if (i <= split) {
                // Send as many as can afford
                right_sizes -= (int)distr_data[i - low][0];
                for (int k = 0; k < distr_data[i - low][0]; k++) {
                    sending[k] = distr1d_right_arr[right_sizes + k];
                }

            } else {
                // Send as many as can afford
                left_sizes -= (int)distr_data[i - low][0];
                for (int k = 0; k < distr_data[i - low][0]; k++) {
                    sending[k] = distr1d_left_arr[left_sizes + k];
                }
            }

            err = MPI_Send(sending, distr_data[i - low][0], MPI_DOUBLE, i, 4, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %d [%s] at %s:%i\n", my_id, buffer, __FILE__, __LINE__);
            }
            free(sending);
        }
    }

    // Receiving new data

    double* proc_data_recv = malloc(sizeof(double) * (int)proc_data_to_send[0]);
    MPI_Recv(proc_data_recv, (int)proc_data_to_send[0], MPI_DOUBLE, split, 4, MPI_COMM_WORLD, &status);

    // proc_data = realloc(proc_data, sizeof(double) * proc_data_length);
    for (int i = 0; i < proc_data_median_length; i++) {
        proc_data[i] = proc_data_median[i];
    }
    for (int i = 0; i < (int)proc_data_to_send[0]; i++) {
        // proc_data[i + proc_data_length - (int)proc_data_median_length] = proc_data_recv[i];
        proc_data[i + proc_data_median_length] = proc_data_recv[i];
    }
    free(proc_data_recv);

    // Recursion call
    if (my_id <= split) {
        distributeByMedian(my_id, num_procs, data_length, proc_data, split - split_count, split);
    } else {
        distributeByMedian(my_id, num_procs, data_length, proc_data, split + 1, split + split_count + 1);
    }
}
