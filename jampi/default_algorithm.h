#ifndef DEFAULT_ALGORITHM
#define DEFAULT_ALGORITHM

#include <iostream>

#include "SchedAlgorithm.h"

namespace parallel {

class CustomAlgorithm : public parallel::SchedAlgorithm { 

public: 
	CustomAlgorithm() { }

	template<class THREAD>
	THREAD* create_thread(int ctx) { 
		THREAD* thr = new THREAD; 
		return thr;
        
		return nullptr;
	} 

	template<class TASK>
	void execute(TASK& t, int ctx) { 
		parallel::Thread<TASK>* curr_thread = 
					dynamic_cast<parallel::Thread<TASK>*>(get_last_created_thread());
		curr_thread->operator()(t); 
	}
	
	template<class THREAD, class TASK>
	void execute(THREAD* thread, TASK& t, int ctx) { 
		thread->operator()(t);
	}
};


};

#endif
