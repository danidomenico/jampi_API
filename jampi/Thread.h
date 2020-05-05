#ifndef _THREAD_
#define _THREAD_

#include <type_traits>
#include <future>

#include "Task.h"

namespace parallel {

class ThreadInterface {
public:
	virtual ~ThreadInterface() {} 
	virtual void join() = 0;
	
	ThreadInterface() = default;
	ThreadInterface(const ThreadInterface&) = default;
	ThreadInterface(ThreadInterface&&) = default;
	ThreadInterface& operator = (const ThreadInterface&) = default;
	ThreadInterface& operator = (ThreadInterface&&) = default;
};

template<class TASK>
class Thread : public virtual ThreadInterface {
public:
	
	virtual void operator()(TASK&) = 0;
	virtual ~Thread(){}

	Thread() = default;
	Thread(const Thread&) = default;
	Thread(Thread&&) = default;
	Thread& operator = (const Thread&) = default;
	Thread& operator = (Thread&&) = default;
};

};

#endif
