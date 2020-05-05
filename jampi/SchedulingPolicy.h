#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

#include <mutex>
#include <atomic>
#include <type_traits>
#include <system_error>
#include <future>
#include <unordered_map> 
#include <iostream>
#include <vector>

#include "SchedAlgorithm.h"

namespace parallel {

template
<
	class Algorithm,
	class THREAD = void, 
	class AssertCompatible = 
		typename std::enable_if< 
			std::is_base_of<typename parallel::SchedAlgorithm, Algorithm>::value
		>::type
>
class SchedulingPolicy {
	
	Algorithm scheme;
	std::mutex thread_table_mutex;
	std::unordered_multimap<std::string, parallel::ThreadInterface*> thread_table;
	std::vector<TaskInterface*> task_table;

public:
	SchedulingPolicy() = default;
	SchedulingPolicy(const SchedulingPolicy&) = delete;
	SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;
	SchedulingPolicy(SchedulingPolicy&&) = delete;
	SchedulingPolicy& operator = (SchedulingPolicy&&) = delete;
	
	~SchedulingPolicy(){
		delete_thread_table();
		delete_task_table(true);
	}
	
	template<class TASK, class U = void>
	typename TASK::returnType_ spawn(TASK& t, int thread_id, int context = 0) {

		typedef typename std::conditional<
			std::is_same<void, THREAD>::value, U, THREAD>::type T;

		thread_table_mutex.lock();
		
		T* thr = scheme.template create_thread<T>(context); 

		std::string thread_id_str = generate_thread_id(PREFIX_THREAD, thread_id);
		if(thr) {
			thread_table.insert(std::make_pair(thread_id_str, thr));
			scheme.template add_created_thread(thr);
		}    

		TASK* tDin = new TASK(t);
		task_table.push_back(tDin);

		tDin->set_executed(true);
		tDin->set_executor_id(thread_id_str);
		
		if(! thr)
			thread_table_mutex.unlock();
		
		scheme.template execute<TASK>(*tDin, context); 
		
		if(thr)
			thread_table_mutex.unlock();
	}
	
	void sync(int thread_id) { 
		std::string thread_id_str = generate_thread_id(PREFIX_THREAD, thread_id);
		for(auto it = thread_table.begin(); it != thread_table.end(); ++it) {
			try {
				if(it->first == thread_id_str && it->second != nullptr) { 
					it->second->join();
				}
			} catch(std::system_error& se) { 
				std::cout << "Error sync (thread): " << it->first << std::endl;
			}
		}
		
		delete_thread_table(thread_id_str);
	} 
	
	template<class TASK, class U = void>
	typename TASK::returnType_ spawn_group(TASK& t, int group_id, int context = 0) {

		typedef typename std::conditional<
			std::is_same<void, THREAD>::value, U, THREAD>::type T;

		thread_table_mutex.lock();
		
		T* thr = scheme.template create_thread<T>(context); 

		std::string group_id_str = generate_thread_id(PREFIX_GROUP, group_id);
		if(thr) {
			thread_table.insert(std::make_pair(group_id_str, thr));
			scheme.template add_created_thread(thr);
		}    

		TASK* tDin = new TASK(t);
		task_table.push_back(tDin);

		tDin->set_executed(true);
		tDin->set_executor_id(group_id_str);
		
		if(! thr)
			thread_table_mutex.unlock();
		
		scheme.template execute<TASK>(*tDin, context); 
		
		thread_table_mutex.unlock(); 
		
		if(thr)
			thread_table_mutex.unlock();
	}
	
	void sync_group(int group_id) { 
		std::string group_id_str = generate_thread_id(PREFIX_GROUP, group_id);
		for(auto it = thread_table.begin(); it != thread_table.end(); ++it) {
			try {
				if(it->first == group_id_str && it->second != nullptr) { 
					it->second->join();
				}
			} catch(std::system_error& se) { 
				std::cerr << "Error sync_group | group_id: " << it->first << ": " << se.what() << std::endl;
			}
		}
		
		delete_thread_table(group_id_str);
	} 
	

private:
	const std::string PREFIX_GROUP  = "_group_";
	const std::string PREFIX_THREAD = "_thread_";
    
	//Debug method
	void print_thread_table() {
		if(thread_table.size() <= 0) {
			std::cout << "thread_table empty!" << std::endl;
			return;
		}
		
		for(auto it = thread_table.begin(); it != thread_table.end(); ++it)
			std::cout << it->first << ":" << it->second << "|";
		std::cout << std::endl;
	}
	
	std::string generate_thread_id(std::string prefix, int id) {
		return prefix + std::to_string(id);
	} 
    
	void delete_thread_table() {
		for(auto& it : thread_table)
			delete_thread(it.second);
	}
    
	void delete_thread_table(std::string thread_id) {
		std::unordered_multimap<std::string, parallel::ThreadInterface*>::iterator elem = 
				thread_table.find(thread_id);
		while(elem != thread_table.end()) {
			if(elem->first == thread_id) { 
				ThreadInterface* thread = elem->second;
				thread_table.erase(elem);
                
				delete_thread(thread);
                
				elem =  thread_table.find(thread_id);
			}
		}    
	}
    
	void delete_thread(ThreadInterface* thread) {
		try {
			if(thread != nullptr) { 
				thread->join();
				delete thread;
			}    
		} catch(std::system_error& se) { 
			std::cerr << "Error deleting thread: " << se.what() << std::endl;
		}
	}
   
   void delete_task_table(bool delete_all=false) {
		delete_task_table("", delete_all);
	}
   
	void delete_task_table(std::string executor_id, bool delete_all=false) {
		int i = 0;
		while(i < task_table.size()) {
			if( delete_all || 
				(task_table[i]->get_executor_id() == executor_id && task_table[i]->get_executed()) ) {
				try {
					if(task_table[i] != nullptr)
						delete task_table[i];
				} catch(std::system_error& se) { 
					std::cerr << "Error deleting task table | i=" << i << ": " << se.what() << std::endl;
				}    
						
				task_table.erase(task_table.begin() + i);
			} else {
				i++;
			}
		}
	}
};

};//namespace parallel
#endif
