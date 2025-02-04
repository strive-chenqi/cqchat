#ifndef GLODEF_H
#define GLODEF_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <boost/uuid/uuid.hpp>             // uuid 类
#include <boost/uuid/uuid_generators.hpp>  // 生成器
#include <boost/uuid/uuid_io.hpp>         // 流操作符

#include <boost/filesystem.hpp> //boost文件系统，方便跨平台操作文件
#include <boost/property_tree/ptree.hpp> //boost读取ini文件
#include <boost/property_tree/ini_parser.hpp> //boost读取ini文件

#include <hiredis/hiredis.h>

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <map>
#include <queue>
#include <sstream>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <jsoncpp/json/json.h>  //基本功能
#include <jsoncpp/json/value.h> //一个json类型的结构
#include <jsoncpp/json/reader.h> //字符串解析成json


#define BOOST_ASIO_DISABLE_STD_CHRONO //强制使用boost.chrono
 


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = net::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json解析错误
	RPCFailed = 1002,  //RPC请求错误
	VarifyExpired = 1003, //验证码过期
	VarifyCodeErr = 1004, //验证码错误
	UserExist = 1005,       //用户已经存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //更新密码失败
	PasswdInvalid = 1009,   //密码更新失败
	TokenInvalid = 1010,   //Token失效
	UidInvalid = 1011,  //uid无效
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







#define MAX_LENGTH  1024*2
//头部总长度
#define HEAD_TOTAL_LEN 4
//头部id长度
#define HEAD_ID_LEN 2
//头部数据长度
#define HEAD_DATA_LEN 2
#define MAX_RECVQUE  10000
#define MAX_SENDQUE 1000

enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005, //用户登陆
	MSG_CHAT_LOGIN_RSP = 1006, //用户登陆回包
	ID_SEARCH_USER_REQ = 1007, //用户搜索请求
	ID_SEARCH_USER_RSP = 1008, //搜索用户回包
	ID_ADD_FRIEND_REQ = 1009, //申请添加好友请求
	ID_ADD_FRIEND_RSP  = 1010, //申请添加好友回复
	ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
	ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
	ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
	ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
	ID_TEXT_CHAT_MSG_REQ = 1017, //文本聊天信息请求
	ID_TEXT_CHAT_MSG_RSP = 1018, //文本聊天信息回复
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
};


#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO  "nameinfo_"

#define CODEPREFIX "code_"

// class ConfigMgr;
// extern ConfigMgr gCfgMgr;

#endif // GLODEF_H