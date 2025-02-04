#include "mysqlmgr.h"


MysqlMgr::~MysqlMgr() {

}

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    return dao_.RegUser(name, email, pwd);
}

MysqlMgr::MysqlMgr() {
}

bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email) {
    return dao_.CheckEmail(name, email);
}

bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& pwd) {
    return dao_.UpdatePwd(name, pwd);
}

bool MysqlMgr::checkPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo) {
    return dao_.checkPwd(name, pwd, userInfo);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
	return dao_.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(std::string name)
{
	return dao_.GetUser(name);
}
