#ifndef PARALLEL_SCHEDALGORITHM
#define PARALLEL_SCHEDALGORITHM

#include <vector>
#include "Thread.h"

namespace parallel {

class SchedAlgorithm {  
	private:
		std::vector<parallel::ThreadInterface*> available_threads;    
        
    public:
		template<class THREAD>
		THREAD* create_thread(int ctx) { return nullptr; }
		
		template<class TASK>
		void execute(TASK& t, int ctx) { }
		
		template<class THREAD>
		void add_created_thread(THREAD* thread) {
			available_threads.push_back(thread);
		}
		
		virtual ~SchedAlgorithm() = default;
        
	protected:
		parallel::ThreadInterface* get_last_created_thread() {
			if(available_threads.size() > 0)
				return available_threads[available_threads.size()-1];
			return nullptr;
		}
};

};

#endif
