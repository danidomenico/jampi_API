#ifndef _FOR_
#define _FOR_

#include <type_traits>
#include "Task.h"
#include "SchedulingPolicy.h"
#include "Range.h"
#include "default_thread.h"
#include <iostream>

namespace parallel {

template
<
	class SCHED,
	template <typename> class ThreadType,
	typename Func
>
void for_helper(SCHED& policy , parallel::Range& r, int chunk, Func& f) { 

	int size = r.size(); 
	if( (size/chunk) > 1 ) {
		int shift = size / 2;

		parallel::Range left_range( r.begin(), r.begin() + shift );
		parallel::Range right_range( r.begin() + shift, r.end() );

		parallel::Task<void, SCHED&, parallel::Range& , int,Func&> 
			left(for_helper<SCHED, ThreadType, Func>, policy, left_range, chunk, f);
			
		parallel::Task<void, SCHED&, parallel::Range& , int,Func&> 
			right(for_helper<SCHED, ThreadType, Func>, policy, right_range, chunk, f);

		int context = shift + r.begin();
	       
		using TASK = decltype(left);
		policy.template spawn< TASK, ThreadType<TASK> >(left, context);
		policy.template spawn< TASK, ThreadType<TASK> >(right, context);

		policy.template sync(context);
	} else {
		parallel::Task<void, parallel::Range&> seq(f, r);
		seq();
	}
}

template
<
	typename SchedAlg,
	template <typename> class ThreadType = DefaultThread,
	typename Func
>
void For(parallel::Range& r, int chunk, const Func& f) { 

	if(chunk == 0 || chunk > r.size()) return; 
	
	parallel::SchedulingPolicy<SchedAlg> scheduler;

	for_helper<decltype(scheduler), ThreadType, typename std::remove_cv<Func>::type>
		(scheduler, r, chunk, const_cast<Func&>(f) ); 
}

template
<
	typename SchedAlg,
	template <typename> class ThreadType = DefaultThread,
	typename Func
>
void For(parallel::Range&& r, int chunk, const Func& f) { 

	if(chunk == 0 || chunk > r.size()) return; 

	parallel::SchedulingPolicy<SchedAlg> scheduler; 

	for_helper<decltype(scheduler), ThreadType, typename std::remove_cv<Func>::type>
		(scheduler, r, chunk, const_cast<Func&>(f)); 
}

};


#endif
