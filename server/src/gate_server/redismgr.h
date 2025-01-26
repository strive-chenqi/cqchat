#ifndef _REDIS_MGR_H_
#define _REDIS_MGR_H_


#include "global.h"

class RedisReplyPtr {
public:
    explicit RedisReplyPtr(redisReply* reply) : reply_(reply) {}
    ~RedisReplyPtr() {
        if (reply_) {
            freeReplyObject(reply_);
        }
    }

    // 禁用拷贝构造和赋值操作
    RedisReplyPtr(const RedisReplyPtr&) = delete;
    RedisReplyPtr& operator=(const RedisReplyPtr&) = delete;

    // 允许移动构造和赋值操作
    RedisReplyPtr(RedisReplyPtr&& other) noexcept : reply_(other.reply_) {
        other.reply_ = nullptr;
    }
    RedisReplyPtr& operator=(RedisReplyPtr&& other) noexcept {
        if (this != &other) {
            if (reply_) {
                freeReplyObject(reply_);
            }
            reply_ = other.reply_;
            other.reply_ = nullptr;
        }
        return *this;
    }

    redisReply* get() const { return reply_; }

private:
    redisReply* reply_;
};



class RedisConPool {
public:
    RedisConPool(size_t poolSize, const char* host, int port, const char* pwd)
        : poolSize_(poolSize), host_(host), port_(port), pwd_(pwd), b_stop_(false){
        for (size_t i = 0; i < poolSize_; ++i) {
            auto* context = redisConnect(host, port);
            if (context == nullptr || context->err != 0) {
                if (context != nullptr) {
                    redisFree(context);
                }
                continue;
            }

            auto reply = RedisReplyPtr((redisReply*)redisCommand(context, "AUTH %s", pwd));
            if (reply.get()->type == REDIS_REPLY_ERROR) {
                std::cout << "认证失败" << std::endl;
                redisFree(context);
                
                continue;
            }

            std::cout << "认证成功" << std::endl;
            connections_.push(context);
        }

    }

    ~RedisConPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        // Close();   这个close放在redismgr的析构里面去处理了
        while (!connections_.empty()) {
            connections_.pop();
        }
    }

    redisContext* getConnection() {
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
        auto* context = connections_.front();
        connections_.pop();
        return context;
    }

    void returnConnection(redisContext* context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        connections_.push(context);
        cond_.notify_one();
    }

    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }

private:
    size_t poolSize_;
    const char* host_;
    int port_;
    const char* pwd_;
    std::atomic<bool> b_stop_;
    std::queue<redisContext*> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};




class RedisMgr: public Singleton<RedisMgr>, 
    public std::enable_shared_from_this<RedisMgr>
{
    friend class Singleton<RedisMgr>;
public:
    ~RedisMgr();
    //bool Connect(const std::string& host, int port); 改为了池子
    bool Auth(const std::string &password);

    bool Get(const std::string &key, std::string& value);
    bool Set(const std::string &key, const std::string &value);
    
    bool LPush(const std::string &key, const std::string &value);
    bool LPop(const std::string &key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key, std::string& value);

    bool HSet(const std::string &key, const std::string  &hkey, const std::string &value);
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string HGet(const std::string &key, const std::string &hkey);

    bool Del(const std::string &key);

    bool ExistsKey(const std::string &key);

    void Close();
private:
    RedisMgr();

    // redisContext* _connect; 改为连接池
    std::unique_ptr<RedisConPool> pool_;


    RedisReplyPtr reply_;
};

#endif