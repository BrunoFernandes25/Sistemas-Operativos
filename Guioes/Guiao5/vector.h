#include <stdlib.h>
#include <stdio.h>

//vector
#define SIZE 100
#define MAX 10

extern int vector[MAX];             //colocamos extern para defenir vector noutro lugar

void init_vector();

void print_vector();

int count_needle(int needle);