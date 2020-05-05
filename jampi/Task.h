#ifndef _TASK_
#define _TASK_

#include <memory>
#include <tuple>
#include <utility>
#include <future>
#include <iostream>

#include "Callable.h"

namespace parallel {

template<int ...> struct seq {};

template<int N, int ...S> struct gens : gens<N-1, N-1, S...> {};

template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };


class TaskInterface {
public:
	virtual void operator()() = 0;
	virtual ~TaskInterface(){}
	
	TaskInterface() = default;
	TaskInterface(const TaskInterface&) = default;
	TaskInterface& operator = (const TaskInterface&) = default;

	TaskInterface(TaskInterface&&) = default;
	TaskInterface& operator = (TaskInterface&&) = default;
    
	void set_executed(bool e) {
		executed = e;
	}
    
	bool get_executed() {
		return executed;
	}
    
	void set_executor_id(std::string e_id) {
		executor_id = e_id;
	}
	
	std::string get_executor_id() {
		return executor_id;
	}
    
private:
	bool executed = false;
	std::string executor_id = "NOT";
};


template<class T>
class overload_tag { typedef T type; };


template
<
	typename ReturnType,
	typename ...Args
>
class Task : public virtual TaskInterface {
	
	std::unique_ptr< Callable<ReturnType, Args...> > function;
	
	std::tuple<Args...> arguments;

	std::promise<ReturnType> return_channel;

	template<int ...index, class T>
	void apply(seq<index...>, overload_tag<T>) { 
		if(function) 
			return_channel.set_value( (*function)(std::get<index>(arguments)...) );
	}

	template<int ...index>
	void apply(seq<index...>, overload_tag<void>) {
		if (function) { 
			(*function)(std::get<index>(arguments)...);
		}
	}

	public: 
		typedef ReturnType returnType_; 
		
		Task() : function(nullptr), arguments(), return_channel() { }
		
		~Task() {
			//std::cout << "~Task" << std::endl;
		}
		
		Task(const Task& t) : arguments(t.arguments) {
			Task& other    = const_cast<Task&>(t);
			function       = std::move(other.function);
			return_channel = std::move(other.return_channel); 
		}
		
		Task& operator = (const Task& t) {
			Task& other    = const_cast<Task&>(t); 
			arguments      = std::move(t.arguments);
			function       = std::move(other.function);
			return_channel = std::move(other.return_channel); 
			return *this; 
		}

		Task(Task&& other): function(std::move(other.function)),
			arguments(std::move(other.arguments)),
			return_channel(std::move(other.return_channel)){ }

		Task& operator = (Task&& other) {
			function       = std::move(other.function);
			arguments      = std::move(other.arguments);
			return_channel = std::move(other.return_channel);

			return *this;
		}

		template<class Fn>
		Task(Fn f, Args...args);

		template<class PtrToMember, class PtrToObject>
		Task(PtrToMember m, PtrToObject o, Args...args);

		void operator () () {
			apply(typename gens<sizeof...(Args)>::type(), overload_tag<ReturnType>{});
		}

		std::future<ReturnType> getTaskFuture() { 
			return return_channel.get_future();
		}
};

//Implementações da classe Task
template
<
	typename ReturnType,
	typename ...Args
>
template<class Fn>
Task<ReturnType, Args...>::Task(Fn f, Args...args) :
	function( new CallableImpl<Fn, ReturnType, Args...>(f) ), 
	arguments(std::tie(args...)) {}

template
<
	typename ReturnType,
	typename ...Args
>
template<class PtrToMember, class PtrToObject>
Task<ReturnType, Args...>::Task(PtrToMember m, PtrToObject o, Args...args) :
	function( new MemberCallableImpl<PtrToMember, PtrToObject, ReturnType, Args...>(m, o) ),
	arguments(std::tie(args...)) {}

} //namespace parallel

#endif
