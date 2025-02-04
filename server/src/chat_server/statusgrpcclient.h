#ifndef __STATUS_GRPC_CLIENT_H__
#define __STATUS_GRPC_CLIENT_H__

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "global.h"
#include "singleton.h"
#include "configmgr.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;
using message::LoginReq;
using message::LoginRsp;

class StatusConPool {
public:
   
    StatusConPool(size_t poolSize, std::string host, std::string port)
        : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
        for (size_t i = 0; i < poolSize_; ++i) {

            std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                grpc::InsecureChannelCredentials());

            connections_.push(StatusService::NewStub(channel));
        }
    }

    ~StatusConPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        Close();
        while (!connections_.empty()) {
            connections_.pop();
        }
    }

    std::unique_ptr<StatusService::Stub> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            if (b_stop_) {
                return true;
            }
            return !connections_.empty();
            });
        //如果停止则直接返回空指针
        if (b_stop_) {
            return  nullptr;
        }
        auto context = std::move(connections_.front());
        connections_.pop();
        return context;
    }

    void returnConnection(std::unique_ptr<StatusService::Stub> context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        connections_.push(std::move(context));
        cond_.notify_one();
    }

    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }

private:
    size_t poolSize_;
    std::string host_;
    std::string port_;
    std::atomic<bool> b_stop_;
    std::queue<std::unique_ptr<StatusService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

class StatusGrpcClient :public Singleton<StatusGrpcClient>
{
    friend class Singleton<StatusGrpcClient>;
public:
    ~StatusGrpcClient() {

    }
    GetChatServerRsp GetChatServer(int uid);  //获取聊天服务器
    LoginRsp Login(int uid, std::string token);  //登录

private:
    StatusGrpcClient();
    std::unique_ptr<StatusConPool> pool_;

};

#endif