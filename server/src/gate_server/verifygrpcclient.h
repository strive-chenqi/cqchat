#ifndef VERIFYGRPCCLIENT_H
#define VERIFYGRPCCLIENT_H

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
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


//之前是单线程 io_context，boost::asio 会自动处理 I/O 操作的同步问题，不存在多线程问题
//现在是多线程各自有一个 io_context，那么各自访问grpc服务的时候就需要保证stub_的线程安全
//我们可以用一个连接池来管理多个stub_（每个rpc连接有一个stub_），每个io_context用不一样的stub_这样就可以保证线程安全
class RPConPool {
public:
    RPConPool(size_t poolSize, std::string host, std::string port);
    ~RPConPool();

    std::unique_ptr<VarifyService::Stub> getConnection();
    void returnConnection(std::unique_ptr<VarifyService::Stub> context);
    void Close();
private:
    size_t poolSize_;              
    std::string host_;             
    std::string port_;             
    std::atomic<bool> b_stop_;     

    //一把锁同时控制头尾，性能不高，可以换成两把锁分别控制头尾的push和pop，也可以用无锁队列
    std::queue<std::unique_ptr<VarifyService::Stub>> connections_; 

    std::mutex mutex_;
    std::condition_variable cond_;
};
  

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;
public:

    GetVarifyRsp getVarifyCode(std::string email);

private:
    VerifyGrpcClient();

    std::unique_ptr<RPConPool> pool_;
    

    //VarifyService::Stub 客户端用来进行 RPC (远程过程调用) 的主要接口。用来与验证服务器进行通信
    //std::unique_ptr<VarifyService::Stub> stub_;  //grpc通信信使，channel是grpc通信的通道
};
#endif // VERIFYGRPCCLIENT_H