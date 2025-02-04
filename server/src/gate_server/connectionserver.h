#ifndef _CSERVER_H_
#define _CSERVER_H_

#include "global.h"

using namespace std;

class CServer:public std::enable_shared_from_this<CServer>
{
public:
    CServer(boost::asio::io_context& ioc, unsigned short& port);
    void start();   //gate server 用的
private:
    tcp::acceptor  acceptor_;
    net::io_context& ioc_; 
    //tcp::socket socket_;    
};

#endif