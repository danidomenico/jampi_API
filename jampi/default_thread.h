#ifndef DEFAULT_THREAD
#define DEFAULT_THREAD

#include <thread>
#include "Thread.h"

template<class TASK>
class DefaultThread : public virtual parallel::Thread<TASK> { 
	std::thread cpu;

public:
	DefaultThread() = default;
	~DefaultThread() = default;
		
	DefaultThread(const DefaultThread& t) = delete;
	
	DefaultThread& operator = (const DefaultThread& t) = delete;

	DefaultThread(DefaultThread&& t): cpu(std::move(t.cpu)) { }
	
	DefaultThread& operator = (DefaultThread&& t) { 
		cpu = std::move(t.cpu);
		return *this;
	}

	void operator()(TASK& t) { 
		cpu = std::thread(std::ref(t));
	}
	
	void join() {
		if(cpu.joinable()) { 
			cpu.join();
		}
	}
	
}; 

#endif
