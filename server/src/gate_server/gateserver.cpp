#include "connectionserver.h"
#include "redismgr.h"
#include "configmgr.h"

int main()
{
    auto& gCfgMgr = ConfigMgr::Inst();
    // ConfigMgr gCfgMgr = ConfigMgr();
    
    std::string gate_port_str = gCfgMgr["CServer"]["Port"];
    unsigned short gate_port = atoi(gate_port_str.c_str());

    try
    {
        //这个ioc跑在主线程里面，目前每个连接的连接、读写等都是通过这个ioc来调度的
        //并发能力不是很强
        /*
        1、用线程池（io_context_poll)，每个连接有一个独立的io_context，但是线程数目有限，不适合大量连接
        2、第二种方式是一个io_context跑在多个线程里面，这样可以利用多核，但是io_context不是线程安全的，需要加锁
        */
        net::io_context ioc{ 1 }; 

        
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM); //信号集
        //信号集处理
        signals.async_wait([&ioc](const boost::system::error_code& error, [[maybe_unused]] int signal_number) {

            if (error) {
                return;
            }
            ioc.stop();
        });


        std::make_shared<CServer>(ioc, gate_port)->start();
        std::cout << "Gate Server listen on port" << gate_port << std::endl;
        ioc.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}