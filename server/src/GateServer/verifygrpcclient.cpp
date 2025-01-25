#include "verifygrpcclient.h"
#include "configmgr.h"

RPConPool::RPConPool(size_t poolSize, std::string host, std::string port)
    : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {

    for (size_t i = 0; i < poolSize_; ++i) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host+":"+port,
            grpc::InsecureChannelCredentials());

        //这种是不行的，因为unique_ptr不能拷贝,要么stub那加上std::move
        //auto stub = VarifyService::NewStub(channel);
        //connections_.push(stub);
        connections_.push(VarifyService::NewStub(channel));
    }
}

RPConPool::~RPConPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    Close();
    while (!connections_.empty()) {
        connections_.pop();
    }
}

//消费者
std::unique_ptr<VarifyService::Stub> RPConPool::getConnection() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this] {
        if (b_stop_) {
            return true;
        }
        return !connections_.empty();
    });
    //如果停止则直接返回空指针
    if (b_stop_) {
        return nullptr;
    }
    auto context = std::move(connections_.front());
    connections_.pop();
    return context;
}

//生产者
void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (b_stop_) {
        return;
    }
    connections_.push(std::move(context));
    cond_.notify_one();
}

void RPConPool::Close() {
    b_stop_ = true;
    cond_.notify_all();
}


GetVarifyRsp VerifyGrpcClient::getVarifyCode(std::string email) {
    ClientContext context;
    GetVarifyRsp reply;
    GetVarifyReq request;

    request.set_email(email);

    auto stub_ = pool_->getConnection();
    //信使，客户端和服务端通信的
    Status status = stub_->GetVarifyCode(&context, request, &reply);

    if (status.ok()) {
        pool_->returnConnection(std::move(stub_));
        return reply;
    }
    else {
        pool_->returnConnection(std::move(stub_));
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}

VerifyGrpcClient::VerifyGrpcClient() {
    auto& gCfgMgr = ConfigMgr::Inst();
    std::string host = gCfgMgr["VerifyServer"]["Host"];
    std::string port = gCfgMgr["VerifyServer"]["Port"];
    pool_ = std::make_unique<RPConPool>(5, host, port);
}