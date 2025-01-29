#ifndef SINGLETON_H
#define SINGLETON_H


#include "global.h"


template <typename T>
class Singleton
{
public:
    static std::shared_ptr<T>  GetInstance(){
        /*
        * 双重检查锁定存在问题，因为编译器可能会对代码进行优化，导致初始化顺序不正确
        if (m_pInstance == nullptr){
            lock_guard<mutex> lock(m_mutex);
            if (m_pInstance == nullptr){
                m_pInstance = new T();
                // 当程序通过调用 exit()或从 main 中返回时被调用.终止处理程序执行的顺序和注册时的顺序是相反的
                atexit(ReleaseInstance);
            }
        }
        return m_pInstance;
        */



        /*
        * 通过局部静态变量实现单例模式，是线程安全的，因为局部静态变量只会被初始化一次
        * 和下面其实是一样的，这种还多了要加一个私有成员变量static std::shared_ptr<T> m_pInstance
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            m_pInstance = std::shared_ptr<T>(new T());
            atexit(ReleaseInstance);
        });

        return m_pInstance;
        */


        //我们自己new T这样的话只要T的析构函数要暴露出来给shared_ptr用就可以了
        static std::shared_ptr<T> instance(new T);

        // std::make_shared<T>()的话要将T的构造函数和析构函数都设置为public，就失去了单例的意义
        //static std::shared_ptr<T> instance = std::make_shared<T>();

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



#endif // SINGLETON_H
