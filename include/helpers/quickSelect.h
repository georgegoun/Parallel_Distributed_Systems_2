#ifndef QUICK_SELECT
#define QUICK_SELECT

#include <stdio.h>

void swap(double* a, double* b);
int partition(double arr[], int left, int right, int pIndex);
double quickselect(double arr[], int left, int right, int k);

#endif