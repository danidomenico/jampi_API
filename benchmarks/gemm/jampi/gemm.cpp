#include "gemm.hpp"

void fillMat(float* mat, int size) {
	srand(time(NULL));
	for(int i=0; i<size; i++) 
		for(int j=0; j<size; j++)
			//mat[i*size + j] = i + j;
			mat[i*size + j] = (rand() % size) + ((rand() % size) * 0.11111);
}

void printMat(float* mat, int size) {
	for(int i=0; i<size; i++) {
		for(int j=0; j<size; j++)
			 printf("%.2f ", mat[i*size + j]);
		printf("\n");
	}
}
