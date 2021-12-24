#include "dataImportPivot.h"
#include "../helpers/PowerOfTwo.h"
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void dataImportPivot(int my_id, int num_procs, double** proc_data, int* data_length)
{
    int err, length;
    char buffer[MPI_MAX_ERROR_STRING];
    MPI_Status status;

    // read csv file and store in array
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    size_t read;

    fp = fopen("../data/iris.csv", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    // get number of lines in file

    int num_lines = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        num_lines++;
    }

    // get number of dimensions in first line delimited with comma

    int num_dimensions = 0;
    char* token;
    rewind(fp);
    read = getline(&line, &len, fp);
    token = strtok(line, ",");
    while (token != NULL) {
        num_dimensions++;
        token = strtok(NULL, ",");
    }

    // initiliaze array with num_lines rows and num_dimensions columns

    double** data = malloc((num_lines / num_procs) * sizeof(double*));
    for (int i = 0; i < (num_lines / num_procs); i++) {
        data[i] = malloc(num_dimensions * sizeof(double));
    }

    // read each element of the file

    rewind(fp);
    int row = 0;
    int counter = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (counter >= my_id * (num_lines / num_procs) && counter < (my_id + 1) * (num_lines / num_procs)) {
            token = strtok(line, ",");
            int col = 0;
            while (token != NULL) {
                data[row][col] = atof(token);
                token = strtok(NULL, ",");
                col++;
            }
            row++;
        }
        counter++;
    }

    if (PowerOfTwo(num_lines) == 0) {
        if (my_id == 0) {
            printf("False Dataset\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            exit(EXIT_FAILURE);
        }
    }
    *data_length = (num_lines / num_procs);

    // Captain Process

    if (my_id == 0) {
        double* pivot = malloc(sizeof(double) * num_dimensions);
        srand(time(NULL));
        pivot = data[(rand() % (*data_length))];

        for (int i = 0; i < num_procs; i++) {
            err = MPI_Send(pivot, num_dimensions, MPI_DOUBLE, i, 10, MPI_COMM_WORLD);
            if (err) {
                MPI_Error_string(err, buffer, &length);
                printf("Error %i [%s] at %s:%i\n", i, buffer, __FILE__, __LINE__);
            }
        }
        free(pivot);
    }

    // Processes receive pivot

    double* pivot_recv = malloc(sizeof(double) * num_dimensions);
    MPI_Recv(pivot_recv, num_dimensions, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);

    // Euclidean distance

    *proc_data = malloc((*data_length) * sizeof(double));
    for (int i = 0; i < (*data_length); i++) {
        double distance = 0;
        for (int j = 0; j < num_dimensions; j++) {
            distance += pow(data[i][j] - pivot_recv[j], 2);
        }
        (*proc_data)[i] = sqrt(distance);
    }

    // free memory

    free(data);
    free(line);
    fclose(fp);
}
