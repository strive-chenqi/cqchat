#ifndef _ASIO_SERVICE_POOL_H_
#define _ASIO_SERVICE_POOL_H_


#include <vector>
#include <boost/asio.hpp>
#include "singleton.h"


class AsioIOServicePool:public Singleton<AsioIOServicePool>
{
    friend Singleton<AsioIOServicePool>;
public:
    using IOContext = boost::asio::io_context;
    using Work = boost::asio::executor_work_guard<IOContext::executor_type>;
    using WorkPtr = std::unique_ptr<Work>;

    ~AsioIOServicePool();
    AsioIOServicePool(const AsioIOServicePool&) = delete;
    AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;

    // 使用 round-robin 的方式返回一个 io_service
    IOContext& getIOService();
    void stop();
private:
    AsioIOServicePool(std::size_t size = 2/*std::thread::hardware_concurrency()返回cpu核数，这里觉得两个线程就够用了，所以写死的2*/);
    
    std::vector<IOContext> ioContexts_;
    std::vector<WorkPtr> works_;
    std::vector<std::thread> threads_;
    std::size_t                        nextIOService_; // 下一个要返回的 io_service 的索引
};

#endif