#include "quickSelect.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// void swap(double* a, double* b)
// {
//     double t = *a;
//     *a = *b;
//     *b = t;
// }
// double partition(double arr[], int low, int high)
// {
//     double pivot = arr[high]; // pivot
//     int i = (low - 1); // Index of smaller element and indicates the right position of pivot found so far

//     for (int j = low; j <= high - 1; j++) {
//         // If current element is smaller than the pivot
//         if (arr[j] < pivot) {
//             i++; // increment index of smaller element
//             swap(&arr[i], &arr[j]);
//         }
//     }
//     swap(&arr[i + 1], &arr[high]);
//     return (i + 1);
// }

// void quickSort(double arr[], int low, int high)
// {
//     if (low < high) {
//         /* pi is partitioning index, arr[p] is now
//         at right place */
//         double pi = partition(arr, low, high);

//         // Separately sort elements before
//         // partition and after partition
//         quickSort(arr, low, pi - 1);
//         quickSort(arr, pi + 1, high);
//     }
// }

//asdsadsadsadas

// // Utility function to swapping of element
// void swap(double* a, double* b)
// {
//     double temp = *a;
//     *a = *b;
//     *b = temp;
// }

// // Returns the correct position of
// // pivot element
// int Partition(double arr[], int l, int r)
// {
//     int lst = arr[r], i = l, j = l;
//     while (j < r) {
//         if (arr[j] < lst) {
//             swap(&arr[i], &arr[j]);
//             i++;
//         }
//         j++;
//     }
//     swap(&arr[i], &arr[r]);
//     return i;
// }

// // Picks a random pivot element between
// // l and r and partitions arr[l..r]
// // around the randomly picked element
// // using partition()
// int randomPartition(double arr[], int l,
//     int r)
// {
//     int n = r - l + 1;
//     srand(time(NULL));
//     int pivot = rand() % n;
//     swap(&arr[l + pivot], &arr[r]);
//     return Partition(arr, l, r);
// }

// // Utility function to find median
// void MedianUtil(double arr[], int l, int r, int k, int& a, int& b)
// {

//     // if l < r
//     if (l <= r) {

//         // Find the partition index
//         int partitionIndex
//             = randomPartition(arr, l, r);

//         // If partition index = k, then
//         // we found the median of odd
//         // number element in arr[]
//         if (partitionIndex == k) {
//             b = arr[partitionIndex];
//             if (a != -1)
//                 return;
//         }

//         // If index = k - 1, then we get
//         // a & b as middle element of
//         // arr[]
//         else if (partitionIndex == k - 1) {
//             a = arr[partitionIndex];
//             if (b != -1)
//                 return;
//         }

//         // If partitionIndex >= k then
//         // find the index in first half
//         // of the arr[]
//         if (partitionIndex >= k)
//             return MedianUtil(arr, l,
//                 partitionIndex - 1,
//                 k, a, b);

//         // If partitionIndex <= k then
//         // find the index in second half
//         // of the arr[]
//         else
//             return MedianUtil(arr,
//                 partitionIndex + 1,
//                 r, k, a, b);
//     }

//     return;
// }

// // Function to find Median
// double findMedian(double arr[], int n)
// {
//     double ans;
//     int a = -1, b = -1;

//     // If n is odd
//     if (n % 2 == 1) {
//         MedianUtil(arr, 0, n - 1,
//             n / 2, a, b);
//         ans = b;
//     }

//     // If n is even
//     else {
//         MedianUtil(arr, 0, n - 1,
//             n / 2, a, b);
//         ans = (a + b) / 2;
//     }
//     return ans;
// }

////sadasdsaddssa

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

    // elements less than the pivot will be pushed to the left of `pIndex`;
    // elements more than the pivot will be pushed to the right of `pIndex`;
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
// (i.e., left <= k <= right). The search space within the array is
// changing for each round – but the list is still the same size.
// Thus, `k` does not need to be updated with each round.
double quickselect(double arr[], int left, int right, int k)
{
    // If the array contains only one element, return that element
    if (left == right) {
        //printf("Inside function calling 1st bracket value: %.2lf\n", arr[left]);

        return arr[left];
    }

    // select `pIndex` between left and right
    int pIndex = left + rand() % (right - left + 1);

    pIndex = partition(arr, left, right, pIndex);

    // The pivot is in its final sorted position
    if (k == pIndex) {
        //printf("Inside function calling 2nd bracket value: %.2lf\n", arr[k]);

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