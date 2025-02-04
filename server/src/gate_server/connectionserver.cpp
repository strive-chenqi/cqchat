#include "connectionserver.h"
#include "httpconnection.h"
#include "asioservicepool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :
    acceptor_(ioc, tcp::endpoint(tcp::v4(), port)),
    ioc_(ioc) {  
    std::cout << "connectionserver constructor,listen on port " << port << std::endl;
}

void CServer::start(){
    auto self = shared_from_this();

    //改用池
    auto& io_context = AsioIOServicePool::GetInstance()->getIOService();
    std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);

    //有自己独立的一个io_context，单独用来接受新连接，每来一个新连接从池子里面取一个io_context，创建一个新的HttpConnection
    //池子里的两个io_context只单独处理读写
    acceptor_.async_accept(new_con->getSocket(), [self, new_con](beast::error_code ec) {
        try {
            //出错则放弃这个连接，继续监听新链接
            if (ec) {
                self->start();
                return;
            }

            //处理新链接，创建HpptConnection类管理新连接
            //socket不提供拷贝构造，所以要么移动要么传引用，但是传引用的话acceptor_.async_accep底层会改变每一个连接数据到这个socket，所以用移动
            //std::make_shared<HttpConnection>(std::move(self->socket_))->start(); //监听读写
            new_con->start();
            
            self->start();
        }
        catch (std::exception& exp) {
            std::cout << "exception is " << exp.what() << std::endl;
            self->start();
        }
    });

}

