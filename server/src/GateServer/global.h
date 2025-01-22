#ifndef GLODEF_H
#define GLODEF_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <boost/filesystem.hpp> //boost文件系统，方便跨平台操作文件
#include <boost/property_tree/ptree.hpp> //boost读取ini文件
#include <boost/property_tree/ini_parser.hpp> //boost读取ini文件

//#include <hiredis/hiredis.h>

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <map>
#include <queue>
#include <iostream>
#include <unordered_map>
#include <jsoncpp/json/json.h>  //基本功能
#include <jsoncpp/json/value.h> //一个json类型的结构
#include <jsoncpp/json/reader.h> //字符串解析成json

#include "singleton.h"
#include "configmgr.h"
#define BOOST_ASIO_DISABLE_STD_CHRONO //强制使用boost.chrono
 


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = net::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes {
    Success = 0,
    Error_Json = 1001,  //Json解析错误
    RPCFailed = 1002,  //RPC请求错误
    VarifyExpired = 1003,  //验证码过期
    VarifyCodeErr = 1004,  //验证码错误
    UserExist = 1005,  //用户已存在
    PasswdErr = 1006,  //密码错误
    EmailNotMatch = 1007,  //邮箱不匹配
    PasswdUpFailed = 1008,  //密码更新失败
    PasswdInvalid = 1009,  //密码不合法
};

class Defer{
public:
    //接受一个lanmda表达式或者函数指针（可调用对象）
    Defer(std::function<void()> f):f_(f){}

    //析构函数执行传入的函数
    ~Defer(){
        f_();
    }
private:
    std::function<void()> f_;
};




#define CODEPREFIX "code_"

class ConfigMgr;
extern ConfigMgr gCfgMgr;

#endif // GLODEF_H