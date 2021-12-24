#include "quickSelect.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(double* a, double* b)
{
    double t = *a;
    *a = *b;
    *b = t;
}

int partition(double arr[], int left, int right, int pIndex)
{
    // pick `pIndex` as a pivot from the array
    double pivot = arr[pIndex];

    // Move pivot to end
    swap(&arr[pIndex], &arr[right]);

    // elements < pivot, will be pushed to the left of `pIndex`;
    // elements > pivot, will be pushed to the right of `pIndex`;
    // equal elements can go either way
    pIndex = left;

    // each time we find an element less than or equal to the pivot, `pIndex`
    // is incremented, and that element would be placed before the pivot.
    for (int i = left; i < right; i++) {
        if (arr[i] <= pivot) {
            swap(&arr[i], &arr[pIndex]);
            pIndex++;
        }
    }

    // move pivot to its final place
    swap(&arr[pIndex], &arr[right]);

    // return `pIndex` (index of the pivot element)
    return pIndex;
}

// Returns the k'th smallest element in the list within `left…right`
double quickselect(double arr[], int left, int right, int k)
{
    // If the array contains only one element, return that element
    if (left == right) {
        // printf("Inside function calling 1st bracket value: %.2lf\n", arr[left]);

        return arr[left];
    }

    // select `pIndex` between left and right
    int pIndex = left + rand() % (right - left + 1);

    pIndex = partition(arr, left, right, pIndex);

    // The pivot is in its final sorted position
    if (k == pIndex) {
        // printf("Inside function calling 2nd bracket value: %.2lf\n", arr[k]);

        return arr[k];

    }

    // if `k` is less than the pivot index
    else if (k < pIndex) {
        return quickselect(arr, left, pIndex - 1, k);
    }

    // if `k` is more than the pivot index
    else {
        return quickselect(arr, pIndex + 1, right, k);
    }
}