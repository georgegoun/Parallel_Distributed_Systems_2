#include <stdio.h>

int main(int argc, char* argv[])
{
    int data[] = { 2, 5, 1, 7, 9, 1, 2, 6, 8, 3, 9, 1, 5, 2, 6, 3, 7, 0, 1, 9, 4, 8, 2, 1, 4, 9, 0, 2, 3, 7, 0, 4 };
    int data_length = sizeof(data) / sizeof(data[0]);
    printf("%d", data_length);
}