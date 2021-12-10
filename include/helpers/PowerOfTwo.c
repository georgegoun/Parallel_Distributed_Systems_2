#include "PowerOfTwo.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

bool PowerOfTwo(int n)
{
    return (ceil(log2(n)) == floor(log2(n)));
}