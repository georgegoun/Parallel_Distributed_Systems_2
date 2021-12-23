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
    int err, len;
    char buffer[MPI_MAX_ERROR_STRING];

    MPI_Status status;

    double median = 0;
    seperator = data_length / num_procs;

    double* pro_data_distance = malloc((sizeof(double) * seperator));

    if (my_id == 0) {
        int* parent_data = malloc((sizeof(int) * (seperator)));
        // parent_data = &data[seperator * my_id];
        for (int i = 0; i < seperator; i++) {
            parent_data[i] = data[i];
        }

        // Pivot Parent

        srand(time(NULL));
        pivot = parent_data[(rand() % seperator)];

        // MPI_Barrier

        for (int i = 1; i < num_procs; i++) {
            err = MPI_Send(&pivot, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %i [%s] at %s:%i\n", i, buffer, __FILE__, __LINE__);
            }
        }

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

        // Passing Parent Distances to All Distances
        for (int i = 0; i < seperator; i++) {
            dist_data[i] = parent_data_distance[i];
        }

        // Receiving All Distances

        // MPI_Barrier
        for (int i = 1; i < num_procs; i++) {
            MPI_Recv(pro_data_distance, seperator, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
            // printf("ID: %i received data: pro_data_distance from src: %i / tag: %i\n", my_id, status.MPI_SOURCE, status.MPI_TAG);
            //  MPI_Barrier(MPI_COMM_WORLD);
            for (int j = 0; j < seperator; j++) {
                dist_data[(i * seperator) + j] = pro_data_distance[j];
            }
        }

        int n
            = data_length;
        for (int i = 1; i < num_procs; i++) {
            err = MPI_Send(dist_data, data_length, MPI_DOUBLE, i, 3, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %i [%s] at %s:%i\n", i, buffer, __FILE__, __LINE__);
            }
        }
        double* testing = malloc(sizeof(double) * data_length);
        for (int i = 0; i < data_length; i++) {
            testing[i] = dist_data[i];
        }
        if (n % 2 == 0) {
            median = (quickselect(testing, 0, n - 1, (n / 2) - 1) + quickselect(testing, 0, n - 1, (n / 2))) / 2;
        } else {
            median = quickselect(testing, 0, n - 1, n / 2);
        }
        printf("testing: ");
        for (int i = 0; i < data_length; i++) {
            printf(" %.2lf", testing[i]);
        }
        printf("\n");

        for (int i = 1; i < num_procs; i++) {
            err = MPI_Send(&median, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &len);
                printf("Error %i [%s] at %s:%i\n", i, buffer, __FILE__, __LINE__);
            }
        }

        free(parent_data);
        free(parent_data_distance);
        return median;
    } else {

        int* pro_data = malloc((sizeof(int) * seperator));
        for (int i = 0; i < seperator; i++) {
            pro_data[i] = data[i + (my_id * seperator)];
        }

        MPI_Recv(&pivot, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

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

        err = MPI_Send(pro_data_distance, seperator, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        if (err) {
            MPI_Error_string(err, buffer, &len);
            printf("Error [%s] at %s:%i\n", buffer, __FILE__, __LINE__);
        }

        // Recieve median

        MPI_Recv(&median, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);

        MPI_Recv(dist_data, data_length, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, &status);

        return median;
    }
    return 0;
}
