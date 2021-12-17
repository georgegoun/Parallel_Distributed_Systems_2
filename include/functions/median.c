#include "median.h"
#include "../helpers/quickSelect.h"
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

double median(int my_id, int num_procs, int data_length, int* data, double* dist_data)
{

    int seperator = 0;
    int pivot = 0;
    double* pro_data_distance = malloc((sizeof(double) * seperator));
    int err, len;
    char buffer[MPI_MAX_ERROR_STRING];

    MPI_Status status;
    MPI_Request r;

    double median = 0;
    seperator = (double)data_length / (double)num_procs;

    if (my_id == 0) {
        int* parent_data = malloc((sizeof(int) * (seperator)));
        parent_data = &data[(seperator * (num_procs - 1))];

        double* all_distances = malloc((sizeof(double) * (data_length)));

        // Pivot Parent

        srand(time(NULL));
        pivot = parent_data[(rand() % seperator)];

        //MPI_Barrier

        for (int i = 1; i < num_procs; i++) {
            err = MPI_Send(&pivot, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %i [%s] at %s:%i\n", i, buffer, __FILE__, __LINE__);
            }
        }
        //MPI_Request_free()

        // Parent Distances

        double* parent_data_distance
            = malloc((sizeof(double) * seperator));

        for (int i = 0; i < seperator; i++) {
            double euc_distance = 0;
            if (pivot > parent_data[i]) {
                euc_distance = sqrt((pivot * pivot) - (parent_data[i] * parent_data[i]));
                parent_data_distance[i] = euc_distance;
            } else {
                euc_distance = sqrt((parent_data[i] * parent_data[i]) - (pivot * pivot));
                parent_data_distance[i] = euc_distance;
            }
        }

        // Recieving All Distances

        // MPI_Barrier

        for (int i = 1; i < num_procs; i++) {
            MPI_Recv(pro_data_distance, seperator, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
            printf("ID: %i received data: pro_data_distance from src: %i / tag: %i\n", my_id, status.MPI_SOURCE, status.MPI_TAG);
            // MPI_Barrier(MPI_COMM_WORLD);
            //  all_distances[(i - 1) * seperator] = *pro_data_distance;
            for (int j = 0; j < (seperator); j++) {
                all_distances[((i - 1) * seperator) + j] = pro_data_distance[j];
            }
        }

        // MPI_Gather(&my_value, seperator, MPI_DOUBLE, buffer, seperator*num_procs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        for (int i = 0; i < seperator; i++) {
            all_distances[i + data_length - seperator] = parent_data_distance[i];
        }

        int n = data_length;

        if (n % 2 == 0) {
            median = (quickselect(all_distances, 0, n - 1, n / 2) + quickselect(all_distances, 0, n - 1, (n / 2) - 1)) / 2;
        } else {
            median = quickselect(all_distances, 0, n - 1, n / 2);
        }
        //| printf("median is: %.2lf\n", median);
        // for (int i = 0; i < n; i++) {
        //     printf(" %.2lf", all_distances[i]);
        // }
        // printf("\n");

        // MPI_Barrier
        for (int i = 1; i < num_procs; i++) {
            err = MPI_Send(&median, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %i [%s] at %s:%i\n", i, buffer, __FILE__, __LINE__);
            }
        }
        for (int i = 0; i < seperator; i++) {
            dist_data[i] = parent_data_distance[i];
        }

        // free(pro_data_distance);
        // free(parent_data);
        // free(all_distances);
        // free(parent_data_distance);
        return median;
    } else {

        int* pro_data = malloc((sizeof(int) * seperator));
        pro_data = &data[(seperator * (my_id - 1))];

        // int* pro_data_distance = malloc(sizeof(int) * seperator);
        MPI_Recv(&pivot, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("ID: %i received data: pivot from src: %i / tag: %i\n", my_id, status.MPI_SOURCE, status.MPI_TAG);

        // MPI_Barrier

        for (int i = 0; i < seperator; i++) {
            double euc_distance = 0;
            if (pivot > pro_data[i]) {
                euc_distance = sqrt((pivot * pivot) - (pro_data[i] * pro_data[i]));
                pro_data_distance[i] = euc_distance;
            } else {
                euc_distance = sqrt((pro_data[i] * pro_data[i]) - (pivot * pivot));
                pro_data_distance[i] = euc_distance;
            }
        }

        //| printf("Hello world! I'm process %d and my pivot %d\n",
        //     my_id, pivot);
        // printf("ID: %d|| ", my_id);
        // for (int i = 0; i < seperator; i++) {
        //     printf(" %d", pro_data[i]);
        // }

        //|printf("\nDistances:");
        // for (int i = 0; i < seperator; i++) {
        //     printf(" %.2lf", pro_data_distance[i]);
        // }
        //|printf("\n");

        err = MPI_Send(pro_data_distance, seperator, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        if (err) {
            MPI_Error_string(err, buffer, &len);
            printf("Error [%s] at %s:%i\n", buffer, __FILE__, __LINE__);
        }
        // MPI_Barrier

        // Recieve median

        MPI_Recv(&median, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);
        printf("ID: %i received data: %.2lf median from src: %i / tag: %i\n", my_id, median, status.MPI_SOURCE, status.MPI_TAG);
        for (int i = 0; i < seperator; i++) {
            dist_data[i] = pro_data_distance[i];
        }
        // free(pro_data);
        return median;
    }
    return 0;
}
