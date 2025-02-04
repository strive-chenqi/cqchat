#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "global.h"


template <typename T>
class Singleton
{
public:
	static std::shared_ptr<T>  GetInstance(){
		static std::shared_ptr<T> instance(new T);
		return instance;
	}
	
	void printAddress() {
		std::shared_ptr<T> instance = GetInstance();
		std::cout << "singleton address: " << instance.get() << std::endl;
	}

protected:
	Singleton() = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;
	virtual ~Singleton() {
		std::cout << "singleton destruct" << std::endl;
	}
	
};

#endif
