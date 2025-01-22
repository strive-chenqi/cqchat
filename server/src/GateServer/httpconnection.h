#ifndef _HTTP_CONNECTION_H_
#define _HTTP_CONNECTION_H_

#include "global.h"

class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;
public:
    HttpConnection(tcp::socket socket);

    void start();

    tcp::socket& getSocket() { return socket_; }

private:
    void handleRequest();     //处理请求

    void preParseGetParam();  //解析get参数 
    void writeResponse();     //写响应
    void checkDeadline();     //检查超时,像js这些封装比较好的语言有这个机制，c++需要自己写
       

private:
    tcp::socket  socket_;

    // http缓冲区 The buffer for performing reads.
    beast::flat_buffer  buffer_{ 8192 };

    // http请求 The request message.
    http::request<http::dynamic_body> request_;

    // http响应 The response message.
    http::response<http::dynamic_body> response_;

    // 定时器 The timer for putting a deadline on connection processing.
    //socket_.get_executor()获取socket的执行（调度）器，std::chrono::seconds(60)表示60秒
    net::steady_timer deadline_{socket_.get_executor(), std::chrono::seconds(60) };

    std::string get_url_;
    std::unordered_map<std::string, std::string> get_params_;  //get请求参数 键值对
};

#endif