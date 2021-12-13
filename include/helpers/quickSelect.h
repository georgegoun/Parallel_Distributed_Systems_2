#ifndef QUICK_SELECT
#define QUICK_SELECT


#include <stdio.h>

// void swap(double* a, double* b);
// double partition(double arr[], int low, int high);
// void quickSort(double arr[], int low, int high);

void swap(double* a, double* b);
// int Partition(double arr[], int l, int r);
// int randomPartition(double arr[], int l,int r);
// void MedianUtil(double arr[], int l, int r, int k, int &a, int &b);
// double findMedian(double arr[], int n);
int partition(double arr[], int left, int right, int pIndex);
double quickselect(double arr[], int left, int right, int k);

#endif