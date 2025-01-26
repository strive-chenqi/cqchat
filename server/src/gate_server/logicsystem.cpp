#include "logicsystem.h"
#include "httpconnection.h"
#include "verifygrpcclient.h"
// #include "redismgr.h"
// #include "mysqlmgr.h"

void LogicSystem::regGet(std::string url, HttpHandler handler) {
    get_handlers_.insert(make_pair(url, handler));
}

void LogicSystem::regPost(std::string url, HttpHandler handler) {
    post_handlers_.insert(make_pair(url, handler));
}

LogicSystem::LogicSystem() {
    regGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->response_.body()) << "receive get_test req";
        int i = 0;
        for (auto& elem : connection->get_params_) {
            i++;
            beast::ostream(connection->response_.body()) << "param" << i << " key is " << elem.first;
            beast::ostream(connection->response_.body()) << ", " <<  " value is " << elem.second << std::endl;
        }
    });

    regPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        //HTTP 请求体的数据转换为字符串
        auto body_str = boost::beast::buffers_to_string(connection->request_.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        

        connection->response_.set(http::field::content_type, "text/json");

        Json::Value root;     //响应
        Json::Reader reader;  //JSON 解析器对象
        Json::Value src_root; //解析后的请求
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString(); 
            beast::ostream(connection->response_.body()) << jsonstr;
            return true;
        }


        if (!src_root.isMember("email")) {
            std::cout << "email field is missing!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->response_.body()) << jsonstr;
            return true;
            // std::cout << "Failed to parse JSON data!" << std::endl;
            // root["error"] = ErrorCodes::Error_Json;
            // std::string jsonstr = root.toStyledString(); //将 root 对象转换为一个人类可读的 JSON 字符串
            // beast::ostream(connection->response_.body()) << jsonstr;
            // return true;
        }
        auto email = src_root["email"].asString(); //将JSON中的email字段转换为std::string类型，asString() 方法已经在内部处理了 JSON 值到 std::string 的转换（反序列化）

        //调用grpc验证服务
        auto rsp = VerifyGrpcClient::GetInstance()->getVarifyCode(email);

        std::cout << "email is " << email << std::endl;
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        root["code"] = rsp.code();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->response_.body()) << jsonstr;
        return true;
    });

    // regPost("/register", [](std::shared_ptr<HttpConnection> connection) {
    //     auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
    //     std::cout << "receive body is " << body_str << std::endl;
    //     connection->response_.set(http::field::content_type, "text/json");
    //     Json::Value root;
    //     Json::Reader reader;
    //     Json::Value src_root;
    //     bool parse_success = reader.parse(body_str, src_root);
    //     if (!parse_success) {
    //         std::cout << "Failed to parse JSON data!" << std::endl;
    //         root["error"] = ErrorCodes::Error_Json;
    //         std::string jsonstr = root.toStyledString();
    //         beast::ostream(connection->response_.body()) << jsonstr;
    //         return true;
    //     }

        
    //     auto email = src_root["email"].asString(); //获取 email 字段的值
    //     auto name = src_root["user"].asString(); //获取 name 字段的值
    //     auto pwd = src_root["passwd"].asString(); //获取 passwd 字段的值
    //     auto confirm = src_root["confirm"].asString(); //获取 confirm 字段的值

    //     if(pwd != confirm) {
    //         root["error"] = ErrorCodes::PasswdErr;
    //         std::string jsonstr = root.toStyledString();
    //         beast::ostream(connection->response_.body()) << jsonstr;
    //         return true;
    //     }

    //     //先查找redis中email对应的验证码是否合理
    //     std::string  varify_code;
    //     bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX+src_root["email"].asString(), varify_code);
    //     if (!b_get_varify) {
    //         std::cout << " get varify code expired" << std::endl;
    //         root["error"] = ErrorCodes::VarifyExpired;
    //         std::string jsonstr = root.toStyledString();
    //         beast::ostream(connection->response_.body()) << jsonstr;
    //         return true;
    //     }

    //     if (varify_code != src_root["varifycode"].asString()) {
    //         std::cout << " varify code error" << std::endl;
    //         root["error"] = ErrorCodes::VarifyCodeErr;
    //         std::string jsonstr = root.toStyledString();
    //         beast::ostream(connection->response_.body()) << jsonstr;
    //         return true;
    //     }


    //     //查找数据库判断用户是否已经存在，mysql中处理
    //     int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd);
    //     if (uid == 0 || uid == -1) {
    //         std::cout << " user or email exist" << std::endl;
    //         root["error"] = ErrorCodes::UserExist;
    //         std::string jsonstr = root.toStyledString();
    //         beast::ostream(connection->response_.body()) << jsonstr;
    //         return true;
    //     }

    //     root["error"] = 0;
    //     root["email"] = email;
    //     root["uid"] = uid;
    //     root ["user"]= name;
    //     root["passwd"] = pwd;
    //     root["confirm"] = confirm;
    //     root["varifycode"] = src_root["varifycode"].asString();
    //     std::string jsonstr = root.toStyledString();
    //     beast::ostream(connection->response_.body()) << jsonstr;
    //     return true;
    // });
}

bool LogicSystem::handleGet(std::string path, std::shared_ptr<HttpConnection> con) {
    if (get_handlers_.find(path) == get_handlers_.end()) {
        return false;
    }

    get_handlers_[path](con);
    return true;
}

bool LogicSystem::handlePost(std::string path, std::shared_ptr<HttpConnection> con) {
    if (post_handlers_.find(path) == post_handlers_.end()) {
        return false;
    }

    post_handlers_[path](con);
    return true;
}