#include <stdio.h>
#include <stdlib.h>

#include "../../jampi/default_algorithm.h"
#include "../../jampi/default_thread.h"
#include "../../jampi/KeyGenerator.h"
#include "../../jampi/SchedulingPolicy.h"

using namespace parallel;

void fib(int x, int* ret);
void fib2(int x, int* ret);

class ContextAlgorithm : public parallel::SchedAlgorithm { 
public: 
	ContextAlgorithm() { }

	template<class THREAD>
	THREAD* create_thread(int ctx) { 
		if(ctx % 2 != 0) { 
			THREAD* thr = new THREAD; 
			return thr;
		} 
		return nullptr; 
	} 

	template<class TASK>
	void execute(TASK& t, int ctx) { 
		if(ctx % 2 != 0) {
			parallel::Thread<TASK>* curr_thread = 
						dynamic_cast<parallel::Thread<TASK>*>(get_last_created_thread());
			curr_thread->operator()(t); 
		} else {
			t(); 
		}    
	}
};

using TASK = Task<void, int, int*>;
SchedulingPolicy<CustomAlgorithm, DefaultThread<TASK>> scheduler;
SchedulingPolicy<ContextAlgorithm, DefaultThread<TASK>> scheduler_ctx;

int main(int argc, char** argv) {
	int x;
	
	printf("Fib CustomAlgorithm: \n");
	fib(8, &x);
	printf("Result: %d\n\n\n", x);
	
	printf("Fib ContextAlgorithm: \n");
	fib2(8, &x);
	printf("Result: %d\n", x);
}

void fib(int x, int* ret) { 
	if(x > 2) { 
		int rl, rr;
		  
		TASK left(fib, x-1, &rl);
		TASK right(fib, x-2, &rr);
		 
		int ktxl = generate_key();
		int ktxr = generate_key();
		
		scheduler.spawn(left, ktxl);
		scheduler.spawn(right, ktxr); 
        
		scheduler.sync(ktxl);
		scheduler.sync(ktxr);
		  
		*ret = rl + rr;
		return;
	}
	*ret = 1; 
}

void fib2(int x, int* ret) { 
	if(x > 2) { 
		int rl, rr;
		  
		TASK left(fib2, x-1, &rl);
		TASK right(fib2, x-2, &rr);
		 
		int ktxl = generate_key();
		
		scheduler_ctx.spawn(left, ktxl, 1); //odd value
		scheduler_ctx.spawn(right, 0, 2); //even value
        
      //Sync tasks and compute the results
		scheduler_ctx.sync(ktxl);
		  
		*ret = rl + rr;
		return;
	}
	*ret = 1; 
}
