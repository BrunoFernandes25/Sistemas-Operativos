#include "vector.h"
#include <time.h>

int vector[MAX];			//definimos aqui

void init_vector(){
	srand(time(NULL));
	for(int i =0; i< MAX; i++){
		vector[i]=rand()%MAX;
	}
}

void print_vector(){
	for(int i =0; i< MAX; i++){
		printf("| %d ", vector[i]);
	}
	printf("|\n");

}

int count_needle(int needle){		//num_ocorr de needle

	int counter=0;
	for(int i=0; i<MAX; i++){
		if(vector[i]==needle){
			counter++;
		}
	}

    return counter;

}