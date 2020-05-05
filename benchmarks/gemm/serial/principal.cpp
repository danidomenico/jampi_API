#include <sys/time.h>

#include "../jampi/gemm.hpp"

long wtime();

void gemm(float* matA, float* matB, float* matResult, int size);

int main(int argc, char** argv) {
	int size = 1024;
	long start_time, end_time;
	
	if(argc > 1)
		size = atoi(argv[1]);
	
	float* matA = new float[size*size]; 
	float* matB = new float[size*size]; 
	float* matResult = new float[size*size]; 
	
	fillMat(matA, size);
	fillMat(matB, size);
	
	start_time = wtime();
	gemm(matA, matB, matResult, size);
	end_time = wtime();
	double t = (double) end_time - start_time;
	
	printf("gemm;SERIAL;size: %d;bs: %d; threads: %d;time: %f\n", 
			 size, size, 1, t/1000000);
	
	delete[] matA;
	delete[] matB;
	delete[] matResult;
}

/* wtime */
long wtime() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec*1000000 + t.tv_usec;
}

/* matrix multiplication */
void gemm(float* matA, float* matB, float* matResult, int size) {
	for(int i = 0; i < size; ++i) {
		for(int j = 0; j < size; j++) {
			float dot = 0;
			for(int k = 0; k < size; k++)
				dot += matA[i*size+k] * matB[k*size+j];

			matResult[i*size+j] = dot;
		}
	}
}
