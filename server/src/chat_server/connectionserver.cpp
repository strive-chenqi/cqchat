#include "connectionserver.h"
// #include "httpconnection.h"
#include "asioservicepool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :
    acceptor_(ioc, tcp::endpoint(tcp::v4(), port)),
    ioc_(ioc) {  
    std::cout << "connectionserver constructor,listen on port " << port << std::endl;
    startAccept();
}

// void CServer::start(){
//     auto self = shared_from_this();

//     //改用池
//     auto& io_context = AsioIOServicePool::GetInstance()->getIOService();
//     std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);

//     //有自己独立的一个io_context，单独用来接受新连接，每来一个新连接从池子里面取一个io_context，创建一个新的HttpConnection
//     //池子里的两个io_context只单独处理读写
//     acceptor_.async_accept(new_con->getSocket(), [self, new_con](beast::error_code ec) {
//         try {
//             //出错则放弃这个连接，继续监听新链接
//             if (ec) {
//                 self->start();
//                 return;
//             }

//             //处理新链接，创建HpptConnection类管理新连接
//             //socket不提供拷贝构造，所以要么移动要么传引用，但是传引用的话acceptor_.async_accep底层会改变每一个连接数据到这个socket，所以用移动
//             //std::make_shared<HttpConnection>(std::move(self->socket_))->start(); //监听读写
//             new_con->start();
            
//             self->start();
//         }
//         catch (std::exception& exp) {
//             std::cout << "exception is " << exp.what() << std::endl;
//             self->start();
//         }
//     });

// }

void CServer::startAccept() {
    auto &io_context = AsioIOServicePool::GetInstance()->getIOService();
    shared_ptr<CSession> new_session = make_shared<CSession>(io_context, this);
    acceptor_.async_accept(new_session->GetSocket(), std::bind(&CServer::handleAccept, this, new_session, std::placeholders::_1));
}

void CServer::handleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& error){
    if (!error) {
        new_session->Start();
        lock_guard<mutex> lock(mutex_sessions_);
        sessions_.insert(make_pair(new_session->GetSessionId(), new_session));
    }
    else {
        cout << "session accept failed, error is " << error.what() << endl;
    }

    startAccept();
}

void CServer::clearSession(std::string uuid) {
	
	// if (sessions_.find(uuid) != sessions_.end()) {
	// 	//ƳûsessionĹ
	// 	UserMgr::GetInstance()->RmvUserSession(_sessions[uuid]->GetUserId());
	// }

	// {
	// 	lock_guard<mutex> lock(_mutex);
	// 	_sessions.erase(uuid);
	// }
    std::lock_guard<std::mutex> lock(mutex_sessions_);
    sessions_.erase(uuid);
	
}