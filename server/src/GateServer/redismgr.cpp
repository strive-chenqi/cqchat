#include "redismgr.h"
#include "configmgr.h"


RedisMgr::RedisMgr() : reply_(nullptr) {
    auto& gCfgMgr = ConfigMgr::Inst();
    auto host = gCfgMgr["Redis"]["Host"];
    auto port = gCfgMgr["Redis"]["Port"];
    auto pwd = gCfgMgr["Redis"]["Passwd"];
    pool_.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}

RedisMgr::~RedisMgr(){
    Close();
}



bool RedisMgr::Get(const std::string &key, std::string& value)
{
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

     reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "GET %s", key.c_str()));
     if (reply_.get() == NULL) {
        std::cout << "[ GET  " << key << " ] failed" << std::endl;
        return false;
    }

     if (reply_.get()->type != REDIS_REPLY_STRING) {
         std::cout << "[ GET  " << key << " ] failed" << std::endl;
         return false;
    }

     value = reply_.get()->str;

     std::cout << "Succeed to execute command [ GET " << key << "  ]" << std::endl;
     return true;
}

bool RedisMgr::Set(const std::string &key, const std::string &value){
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "SET %s %s", key.c_str(), value.c_str()));

    //如果返回NULL则说明执行失败
    if (NULL == reply_.get())
    {
        std::cout << "Execut command [ SET " << key << "  "<< value << " ] failure ! " << std::endl;
        return false;
    }

    //如果执行失败则释放连接
    if (!(reply_.get()->type == REDIS_REPLY_STATUS && (strcmp(reply_.get()->str, "OK") == 0 || strcmp(reply_.get()->str, "ok") == 0)))
    {
        std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;    
        return false;
    }

    //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
    // freeReplyObject(this->_reply);  单纯的reply_改为RedisReplyPtr   RAII思想
    std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
    return true;
}

bool RedisMgr::Auth(const std::string &password)
{
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "AUTH %s", password.c_str()));
    if (reply_.get()->type == REDIS_REPLY_ERROR) {
        std::cout << "认证失败" << std::endl;
        return false;
    }
    else {
        std::cout << "认证成功" << std::endl;
        return true;
    }
}

bool RedisMgr::LPush(const std::string &key, const std::string &value)
{
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str()));
    if (NULL == reply_.get())
    {
        std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        return false;
    }

    if (reply_.get()->type != REDIS_REPLY_INTEGER || reply_.get()->integer <= 0) {
        std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        return false;
    }

    std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
    return true;
}

bool RedisMgr::LPop(const std::string &key, std::string& value){
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "LPOP %s ", key.c_str()));
    if (reply_.get() == nullptr || reply_.get()->type == REDIS_REPLY_NIL) {
        std::cout << "Execut command [ LPOP " << key<<  " ] failure ! " << std::endl;
        return false;
    }
    value = reply_.get()->str;
    std::cout << "Execut command [ LPOP " << key <<  " ] success ! " << std::endl;
    return true;
}

bool RedisMgr::RPush(const std::string& key, const std::string& value) {
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str()));
    if (NULL == reply_.get())
    {
        std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        return false;
    }

    if (reply_.get()->type != REDIS_REPLY_INTEGER || reply_.get()->integer <= 0) {
        std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        return false;
    }

    std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
    return true;
}

bool RedisMgr::RPop(const std::string& key, std::string& value) {
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "RPOP %s ", key.c_str()));
    if (reply_.get() == nullptr || reply_.get()->type == REDIS_REPLY_NIL) {
        std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
        return false;
    }
    value = reply_.get()->str;
    std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
    return true;
}

bool RedisMgr::HSet(const std::string &key, const std::string &hkey, const std::string &value) {
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str()));
    if (reply_.get() == nullptr || reply_.get()->type != REDIS_REPLY_INTEGER ) {
        std::cout << "Execut command [ HSet " << key << "  " << hkey <<"  " << value << " ] failure ! " << std::endl;
        return false;
    }
    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
    return true;
}


// 专门用于处理二进制数据 适合存储二进制数据（如序列化的对象、图片、音频等）因为它可以正确处理包含空字符的数据
//char binary_data[] = {0x00, 0x01, 0x02, 0x03};
//redismgr.HSet("user:1", "binary_field", binary_data, 4);
bool RedisMgr::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    const char* argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;
    reply_ = RedisReplyPtr((redisReply*)redisCommandArgv(connect, 4, argv, argvlen));
    if (reply_.get() == nullptr || reply_.get()->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
        return false;
    }
    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
    return true;
}

std::string RedisMgr::HGet(const std::string &key, const std::string &hkey)
{
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        return "Get connection failed";
    }

    const char* argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();
    reply_ = RedisReplyPtr((redisReply*)redisCommandArgv(connect, 3, argv, argvlen));
    if (reply_.get() == nullptr || reply_.get()->type == REDIS_REPLY_NIL) {
        std::cout << "Execut command [ HGet " << key << " "<< hkey <<"  ] failure ! " << std::endl;
        return "";
    }

    std::string value = reply_.get()->str;
    std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
    return value;
}

bool RedisMgr::Del(const std::string &key)
{
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "DEL %s", key.c_str()));
    if (reply_.get() == nullptr || reply_.get()->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ Del " << key <<  " ] failure ! " << std::endl;
        return false;
    }
    std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
     return true;
}

bool RedisMgr::ExistsKey(const std::string &key)
{
    auto connect = pool_->getConnection();
    if(connect == nullptr){
        std::cout << "Get connection failed" << std::endl;
        return false;
    }

    reply_ = RedisReplyPtr((redisReply*)redisCommand(connect, "exists %s", key.c_str()));
    if (reply_.get() == nullptr || reply_.get()->type != REDIS_REPLY_INTEGER || reply_.get()->integer == 0) {
        std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
        return false;
    }
    std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
    return true;
}

void RedisMgr::Close()
{
    //一般是RedisMgr的析构先调，再才是成员变量的析构，也就是pool_的析构
    //这里不希望pool再析构的时候才去做通知，在redis做析构的时候就让pool去通知
    //等到pool真正回收的时候只做它自己的析构该做的事就行了，让析构功能更加单一化
    pool_->Close();  
}