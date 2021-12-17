#include "removeElement.h"

#include <stdio.h>
#include <stdlib.h>

void removeElement(double* arr, int pos, int length)
{
    arr[pos] = arr[length - 1];
    // for (int i = pos; i < length - 1; i++) {
    //     arr[i] = arr[i + 1];
    // }
    double* tmp
        = realloc(arr, (length - 1) * sizeof(double));
    if (tmp == NULL && length > 1) {
        /* No memory available */
        exit(EXIT_FAILURE);
    }
    arr = tmp;
}