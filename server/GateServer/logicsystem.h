#ifndef _LOGIC_SYSTEM_H_
#define _LOGIC_SYSTEM_H_

#include "global.h"

class HttpConnection;

typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem :public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem(){};

    bool handleGet(std::string, std::shared_ptr<HttpConnection>);
    bool handlePost(std::string, std::shared_ptr<HttpConnection>);
    void regGet(std::string, HttpHandler handler);
    void regPost(std::string, HttpHandler handler);
private:
    LogicSystem();

    std::map<std::string, HttpHandler> post_handlers_;
    std::map<std::string, HttpHandler> get_handlers_;
};

#endif
