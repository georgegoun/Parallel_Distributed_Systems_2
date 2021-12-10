#include "pivotStart.h"
#include "../helpers/quickSort.h"
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int pivotStart(int my_id, int num_procs, int data_length, int* data)
{

    int seperator = 0;
    int pivot = 0;
    double* pro_data_distance = malloc(sizeof(double) * seperator);
    double median = 0;
    seperator = data_length / num_procs;
    if (my_id == 0) {
        printf("Parent: Hello world! I'm process %d out of %d processes\n",
            my_id, num_procs);

        int parent_data_length = data_length - (seperator * (num_procs - 1));
        int* parent_data = malloc(sizeof(int) * (parent_data_length));
        parent_data
            = &data[(seperator * (num_procs - 1))];

        double* all_distances = malloc(sizeof(double) * (data_length - parent_data_length));
        for (int i = 0; i < parent_data_length; i++) {
            printf("%d,", parent_data[i]);
        }
        printf("\n");

        // Pivot Parent

        srand(time(NULL));
        pivot = parent_data[(rand() % parent_data_length)];
        printf("pivot: %d \n", pivot);
        for (int i = 1; i < num_procs; i++) {
            MPI_Send(&pivot, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Recieving Distances

        for (int i = 1; i < num_procs; i++) {
            MPI_Recv(pro_data_distance, seperator, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // all_distances[(i - 1) * seperator] = *pro_data_distance;
            for (int j = 0; j < (seperator); j++) {
                all_distances[((i - 1) * seperator) + j] = pro_data_distance[j];
            }
        }
        int n = data_length - parent_data_length;

        for (int i = 0; i < n; i++) {
            printf(" %.2lf", all_distances[i]);
        }
        printf("\n");
        quickSort(all_distances, 0, n - 1);
        for (int i = 0; i < n; i++) {
            printf(" %.2lf", all_distances[i]);
        }
        if (n % 2 == 0) {
            median = (all_distances[n / 2] + all_distances[(n / 2) - 1]) / 2;
        } else {
            median = (all_distances[n / 2]);
        }

        printf("\n median number is %.2lf\n", median);
    } else {
        int* pro_data = malloc(sizeof(int) * seperator);
        pro_data = &data[(seperator * (my_id - 1))];

        // int* pro_data_distance = malloc(sizeof(int) * seperator);
        MPI_Recv(&pivot, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // TODO: convert numbers to double
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

        printf("Hello world! I'm process %d and my pivot %d\n",
            my_id, pivot);

        printf("ID: %d|| ", my_id);
        for (int i = 0; i < seperator; i++) {
            printf(" %d", pro_data[i]);
        }

        printf("\nDistances:");
        for (int i = 0; i < seperator; i++) {
            printf(" %.2lf", pro_data_distance[i]);
        }
        printf("\n");
        MPI_Send(pro_data_distance, seperator, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }
    return 0;
}