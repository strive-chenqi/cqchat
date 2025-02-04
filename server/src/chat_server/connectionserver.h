#ifndef _CSERVER_H_
#define _CSERVER_H_

#include "global.h"
#include "csession.h"

using namespace std;

class CServer:public std::enable_shared_from_this<CServer>
{
public:
    CServer(boost::asio::io_context& ioc, unsigned short& port);
    //void start();   //gate server 用的
    void clearSession(std::string session_id);
private:
    void handleAccept(std::shared_ptr<CSession> session, const boost::system::error_code& error);
    void startAccept();  //chat server 用的
private:
    tcp::acceptor  acceptor_;
    net::io_context& ioc_; 
    std::map<std::string, std::shared_ptr<CSession>> sessions_;
    short port_;
    std::mutex mutex_sessions_;
    //tcp::socket socket_;    
};

#endif