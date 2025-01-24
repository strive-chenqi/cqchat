#ifndef VERIFYGRPCCLIENT_H
#define VERIFYGRPCCLIENT_H

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "global.h"
#include "singleton.h"

// gRPC 通道类，用于创建与服务器的连接
using grpc::Channel;        
// gRPC 状态类，用于表示 RPC 调用的结果状态
using grpc::Status;
// gRPC 客户端上下文类，用于控制 RPC 调用的行为
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;


// //之前是单线程 io_context，boost::asio 会自动处理 I/O 操作的同步问题，不存在多线程问题
// //现在是多线程 io_context，需要保证stub_的线程安全
// //我们可以用一个连接池来管理多个stub_（每个rpc连接有一个stub_），每个io_context用不一样的stub_这样就可以保证线程安全
// class RPConPool {
// public:
//     RPConPool(size_t poolSize, std::string host, std::string port)
//         : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {

//         for (size_t i = 0; i < poolSize_; ++i) {

//             std::shared_ptr<Channel> channel = grpc::CreateChannel(host+":"+port,
//                 grpc::InsecureChannelCredentials());

//             //这种是不行的，因为unique_ptr不能拷贝,要么stub那加上std::move
//             //auto stub = VarifyService::NewStub(channel);
//             //connections_.push(stub);
//             connections_.push(VarifyService::NewStub(channel));
//         }
//     }

//     ~RPConPool() {
//         std::lock_guard<std::mutex> lock(mutex_);
//         Close();
//         while (!connections_.empty()) {
//             connections_.pop();
//         }
//     }

//     //类似于消费者
//     std::unique_ptr<VarifyService::Stub> getConnection() {
//         std::unique_lock<std::mutex> lock(mutex_);
//         cond_.wait(lock, [this] {
//             if (b_stop_) {
//                 return true;
//             }
//             return !connections_.empty();
//             });
//         //如果停止则直接返回空指针
//         if (b_stop_) {
//             return  nullptr;
//         }
//         auto context = std::move(connections_.front());
//         connections_.pop();
//         return context;
//     }

//     //类似于生产者
//     void returnConnection(std::unique_ptr<VarifyService::Stub> context) {
//         std::lock_guard<std::mutex> lock(mutex_);
//         if (b_stop_) {
//             return;
//         }
//         connections_.push(std::move(context));
//         cond_.notify_one();
//     }

//     void Close() {
//         b_stop_ = true;
//         cond_.notify_all();
//     }

// private:
//     std::atomic<bool> b_stop_;

//     size_t poolSize_;
//     std::string host_;
//     std::string port_;

//     //一把锁同时控制头尾，性能不高，可以换成两把锁分别控制头尾的push和pop，也可以用无锁队列
//     std::queue<std::unique_ptr<VarifyService::Stub>> connections_; 

//     std::mutex mutex_;
//     std::condition_variable cond_;
// };

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;
public:

    GetVarifyRsp getVarifyCode(std::string email) {
        ClientContext context;
        GetVarifyRsp reply;
        GetVarifyReq request;

        request.set_email(email);

        //auto stub_ = pool_->getConnection();
        //信使，客户端和服务端通信的

        Status status = stub_->GetVarifyCode(&context, request, &reply);

        if (status.ok()) {
            //pool_->returnConnection(std::move(stub_));
            return reply;
        }
        else {
            //pool_->returnConnection(std::move(stub_));
            reply.set_error(ErrorCodes::RPCFailed);
            return reply;
        }
    }

private:
    //std::unique_ptr<RPConPool> pool_;

    VerifyGrpcClient(){
        std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
        stub_ = VarifyService::NewStub(channel);
    }
        
    //改为池子
    // std::shared_ptr<Channel> channel = grpc::CreateChannel("192.168.119.134:50051", grpc::InsecureChannelCredentials());
    // stub_ = VarifyService::NewStub(channel);

        
    

    //VarifyService::Stub 客户端用来进行 RPC (远程过程调用) 的主要接口。用来与验证服务器进行通信
    std::unique_ptr<VarifyService::Stub> stub_;  //grpc通信信使，channel是grpc通信的通道
};
#endif // VERIFYGRPCCLIENT_H