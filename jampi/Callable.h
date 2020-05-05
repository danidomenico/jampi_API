#ifndef _CALLABLE_
#define _CALLABLE_

namespace parallel {

template
<
	typename ReturnType,
	typename ...Args
>
class Callable {
	public:
		Callable(const Callable&) = default;
		Callable& operator = (const Callable&) = default;

		Callable() = default;
		Callable(Callable&&) = default;
		Callable& operator = (Callable&&) = default;
	
		virtual ReturnType operator()(Args... args) = 0;
		virtual Callable<ReturnType, Args...>* clone() const = 0;
		virtual ~Callable() {}
};


template
<
	typename Function,
	typename ReturnType,
	typename ...Args
>
class CallableImpl : public Callable<ReturnType, Args...> {
	Function callable;
	
	public:
		CallableImpl(Function f) : callable(f) {}
		
		~CallableImpl(){}
		
		CallableImpl(const CallableImpl& c) : callable(c.callable) {}
		
		// Assuming it is copy asignable
		CallableImpl& operator = (const CallableImpl& c) {
			callable = c.callable;
			return *this;
		}

		CallableImpl(CallableImpl&& other) : callable(std::move(other.callable)) {}

		//Move assignment operator
		CallableImpl& operator = (CallableImpl&& other) { 
			callable = std::move(other.callable);
			return *this;
		}

		ReturnType operator() (Args... args) {
			return (callable)(args...);
			//return ReturnType();
		}

		virtual Callable<ReturnType, Args...>* clone() const { 
			return new CallableImpl<Function, ReturnType, Args...>(*this); 
		}
};


template
<
	typename PtrToMember,
	typename PtrToObject,
	typename ReturnType,
	typename ...Args
>
class MemberCallableImpl : public Callable<ReturnType, Args...> {
	PtrToMember callable;
	PtrToObject obj;
	
	public:
		MemberCallableImpl(PtrToMember c, PtrToObject& o) : callable(c), obj(o) {}
		~MemberCallableImpl(){}

		MemberCallableImpl(const MemberCallableImpl& m) : callable(m.callable), obj(m.obj) {}

		MemberCallableImpl& operator = (const MemberCallableImpl& m) {
			obj = m.obj;
			callable = m.callable;
			return *this;
		}

		MemberCallableImpl(MemberCallableImpl&& other) : 
			callable(std::move(other.callable)), obj(std::move(other.obj)) { }

		MemberCallableImpl& operator= (MemberCallableImpl&& other) { 
			obj = std::move(other.obj);
			callable = std::move(other.callable);

			return *this;
		}

		virtual Callable<ReturnType, Args...>* clone() const { 
			return new MemberCallableImpl<PtrToMember,PtrToObject, ReturnType, Args...>(*this); 
		}

		ReturnType operator()(Args...args) {
			return ((*obj).*callable)(args...);
		}
};

};
#endif
