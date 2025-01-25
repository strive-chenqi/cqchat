#include "asioservicepool.h"
#include <iostream>
using namespace std;

//io_context不支持拷贝，一定要放在初始化列表里面，自动调用io_context的默认构造函数
//works里面每一个元素都是一个null_ptr
AsioIOServicePool::AsioIOServicePool(std::size_t size):ioContexts_(size), 
                                                        works_(size), nextIOService_(0){
    for (std::size_t i = 0; i < size; ++i) {
        //在使用 std::make_unique<Work>(_ioServices[i]) 时，并不会发生 io_context 的拷贝。相反，它会传递一个引用。
        works_[i] = std::unique_ptr<Work>(new Work(ioContexts_[i].get_executor()));
        
    }

    //遍历多个ioservice，创建多个线程，每个线程内部启动ioservice
    for (std::size_t i = 0; i < ioContexts_.size(); ++i) {
        threads_.emplace_back([this, i]() {
            ioContexts_[i].run();
            });
    }
}

AsioIOServicePool::~AsioIOServicePool() {
    stop();
    std::cout << "AsioIOServicePool destruct" << endl;
}

boost::asio::io_context& AsioIOServicePool::getIOService() {
    auto& service = ioContexts_[nextIOService_++];
    if (nextIOService_ == ioContexts_.size()) {
        nextIOService_ = 0;
    }
    return service;
}

void AsioIOServicePool::stop(){
    //因为仅仅执行work.reset并不能让iocontext从run的状态中退出
    //当iocontext已经绑定了读或写的监听事件后，还需要手动stop该服务。
    for (auto& work : works_) {
        //把服务先停止
        work->get_executor().context().stop();
        work.reset(); //回收work以及它内部的io_context
    }

    for (auto& t : threads_) {
        t.join();
    }
}