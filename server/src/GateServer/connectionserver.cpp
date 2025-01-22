#include "connectionserver.h"
#include "httpconnection.h"
// #include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :
    acceptor_(ioc, tcp::endpoint(tcp::v4(), port)),
    ioc_(ioc),
    socket_(ioc) {  
    std::cout << "CServer constructor" << std::endl;
}

void CServer::start(){
    auto self = shared_from_this();

    //改用线程池
    /*
    这里不是指来一个连接创建一个线程，当前的实现已经利用了 boost::asio 提供的异步 I/O 功能，这本质上就是一种 I/O 多路复用的实现
    但是我们可以通过线程池来管理多个io_context提高并发能力，每个连接尽可能地分配到独立的io_context，但是线程数目有限，不适合大量连接
    */
    // auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
    // std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);

    //有自己独立的一个io_context，单独用来接受新连接，每来一个新连接从池子里面取一个io_context，创建一个新的HttpConnection
    //池子里的两个io_context只单独处理读写
    acceptor_.async_accept(socket_, [self](beast::error_code ec) {
        try {
            //出错则放弃这个连接，继续监听新链接
            if (ec) {
                self->start();
                return;
            }

            //处理新链接，创建HpptConnection类管理新连接
            //socket不提供拷贝构造，所以要么移动要么传引用，但是传引用的话acceptor_.async_accep底层会改变每一个连接数据到这个socket，所以用移动
            std::make_shared<HttpConnection>(std::move(self->socket_))->start(); //监听读写
            // new_con->Start();
            
            self->start();
        }
        catch (std::exception& exp) {
            std::cout << "exception is " << exp.what() << std::endl;
            self->start();
        }
    });

}