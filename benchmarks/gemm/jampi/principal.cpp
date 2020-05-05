#include <sys/time.h>

#include "gemm.hpp"

#include "../../../jampi/for.h"
#include "../../../jampi/default_algorithm.h"
#include "../../../jampi/default_thread.h"
#include "../../../jampi/SchedulingPolicy.h"

using namespace parallel;


long wtime();

void gemm(float* matA, float* matB, float* matResult, int size, int chunk);

int main(int argc, char** argv) {
	int size = 1024;
	int threads = 1;
	int block_size = size;
	long start_time, end_time;
	
	if(argc > 1)
		size = atoi(argv[1]);
	
	if(argc > 2)
		block_size = atoi(argv[2]);
	
	if(argc > 3)
		threads = atoi(argv[3]);
	
	float* matA = new float[size*size]; 
	float* matB = new float[size*size]; 
	float* matResult = new float[size*size]; 
	
	fillMat(matA, size);
	fillMat(matB, size);
	
	start_time = wtime();
	gemm(matA, matB, matResult, size, block_size);
	end_time = wtime();
	double t = (double) end_time - start_time;
	
	printf("gemm;THREAD;size: %d;bs: %d;threads: %d;time: %f\n", 
			 size, block_size, threads, t/1000000);
	
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
void gemm(float* matA, float* matB, float* matResult, int size, int chunk) {
	parallel::For<CustomAlgorithm>(parallel::Range(0, size), chunk,
		[&](parallel::Range& r) -> void {
			for(auto i = r.begin(); i != r.end(); ++i) {
				for(int j = 0; j < size; j++) {
					float dot = 0;
					for(int k = 0; k < size; k++)
						dot += matA[i*size+k] * matB[k*size+j];

					matResult[i*size+j] = dot;
				}
			}
		} //Fim lambda
	);
}
