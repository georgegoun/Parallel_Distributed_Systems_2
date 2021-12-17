#include "eucDist.h"
#include <math.h>
#include <stdio.h>

double eucDist(double x, double y)
{
    if (x > y) {
        return sqrt((x * x) - (y * y));
    } else {
        return sqrt((y * y) - (x * x));
    }
}