#ifndef MYSQLDAO_H
#define MYSQLDAO_H

#include "global.h"
#include <cppconn/connection.h>
#include <mysql_driver.h>        //mysql驱动
#include <mysql_connection.h>    //mysql连接
#include <cppconn/driver.h>     //mysql驱动
#include <cppconn/exception.h>  //mysql异常
#include <cppconn/resultset.h>   //mysql结果集
#include <cppconn/statement.h>   //mysql语句
#include <cppconn/prepared_statement.h> //mysql预处理语句



//加一个过期检测
class SqlConnection {
public:
    //sql::Connection就是安装的C++connector原生的连接
    SqlConnection(sql::Connection* con, int64_t last_use_time) : con_(con), last_use_time_(last_use_time) {}
    std::unique_ptr<sql::Connection> con_;
    int64_t last_use_time_; // 上次使用时间, 用于超时检测
};

class MySqlPool {
public:
    MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize)
        : url_(url), user_(user), pass_(pass), schema_(schema), poolSize_(poolSize), b_stop_(false){
        try {
            for (int i = 0; i < poolSize_; ++i) {
                sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance(); //获取mysql驱动
                auto con = driver->connect(url_, user_, pass_); //连接数据库
                con->setSchema(schema_); //设置数据库
                auto currentTime = std::chrono::system_clock::now().time_since_epoch();   //获取当前时间
                long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count(); //转换为秒
                pool_.push(std::unique_ptr<SqlConnection>(new SqlConnection(con, timestamp))); //将连接加入连接池
                
                // std::unique_ptr<sql::Connection> con(driver->connect(url_, user_, pass_));
                // con->setSchema(schema_);
                // pool_.push(std::move(con));
            }
            check_thread_ = std::thread([this] {
                while (!b_stop_) {
                    checkConnection();
                    std::this_thread::sleep_for(std::chrono::seconds(10));
                }
            });
            check_thread_.detach();
        }
        catch (sql::SQLException& e) {
            // 处理异常
            std::cout << "mysql pool init failed" << std::endl;
        }
    }

    void checkConnection() {
        std::lock_guard<std::mutex> guard(mutex_);
        int poolSize = pool_.size();
        auto currentTime = std::chrono::system_clock::now().time_since_epoch();
        long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
        for (int i = 0; i < poolSize; ++i) {
            auto con = std::move(pool_.front());
            pool_.pop();

            //不管有没有异常，都要把连接放回去
            //类似于go语言的defer，后面不论那条分支return了，}结束之前都会执行
            //c++就可以利用RAII的思想自己实现（析构的时候执行）
            Defer defer([&] {         
                pool_.push(std::move(con));
            });

            if (timestamp - con->last_use_time_ < 5) { //如果连接最近使用过,跳过检查
                continue;
            }

            //大于一定的时间会有断开的风险，执行简单查询测试连接是否有效
            try {
               std::unique_ptr<sql::Statement> stmt(con->con_->createStatement()); //创建一个语句
                stmt->executeQuery("SELECT 1"); //执行查询
                con-> last_use_time_ = timestamp; //更新连接使用时间
                std::cout << "execute timer alive query,cur is" << timestamp << std::endl;
            }
            catch (sql::SQLException& e) {
                std::cout << "Error keeping connection alive" << e.what() << std::endl;
                //重新建立连接并替换旧的链接
                sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
                auto* new_con = driver->connect(url_, user_, pass_);
                new_con->setSchema(schema_);
                con->con_.reset(new_con);
                con->last_use_time_ = timestamp;

            }
        }
    }

    std::unique_ptr<SqlConnection> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { 
            if (b_stop_) {
                return true;
            }        
            return !pool_.empty(); 
        });

        if (b_stop_) {
            return nullptr;
        }
        std::unique_ptr<SqlConnection> con(std::move(pool_.front()));
        pool_.pop();
        return con;
    }

    void returnConnection(std::unique_ptr<SqlConnection> con) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        pool_.push(std::move(con));
        cond_.notify_one();
    }

    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }

    ~MySqlPool() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!pool_.empty()) {
            pool_.pop();
        }
    }

private:
    std::string url_;
    std::string user_;
    std::string pass_;
    std::string schema_;
    int poolSize_;
    std::queue<std::unique_ptr<SqlConnection>> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic<bool> b_stop_;
    //过期检测线程
    std::thread check_thread_;
};


struct UserInfo{
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};


class MysqlDao
{
public:
    MysqlDao();
    ~MysqlDao();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
private:
    std::unique_ptr<MySqlPool> pool_;
};

#endif // MYSQLDAO_H