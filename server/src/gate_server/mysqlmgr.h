#ifndef MYSQLMGR_H
#define MYSQLMGR_H

#include "mysqldao.h"

class MysqlMgr: public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
private:
    MysqlMgr();
    MysqlDao  dao_;
};

#endif // MYSQLMGR_H