#ifndef _LOGIC_SYSTEM_H_
#define _LOGIC_SYSTEM_H_

#include "singleton.h"
#include "global.h"
#include "data.h"
#include "csession.h"

typedef  function<void(shared_ptr<CSession>, const short &msg_id, const string &msg_data)> FunCallBack;
class LogicSystem:public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(shared_ptr < LogicNode> msg);
private:
	LogicSystem();
	void DealMsg();
	void RegisterCallBacks();
	void LoginHandler(shared_ptr<CSession> session, const short &msg_id, const string &msg_data);
	void SearchInfo(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	// void AddFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	// void AuthFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	// void DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	// bool isPureDigit(const std::string& str);
	// void GetUserByUid(std::string uid_str, Json::Value& rtvalue);
	// void GetUserByName(std::string name, Json::Value& rtvalue);
	// bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> &userinfo);
	// bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	// bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>> & user_list);
	std::thread _worker_thread;
	std::queue<shared_ptr<LogicNode>> _msg_que;
	std::mutex _mutex;
	std::condition_variable _consume;
	bool _b_stop;
	std::map<short, FunCallBack> _fun_callbacks;
	std::map<int, std::shared_ptr<UserInfo>> _users;
	std::mutex _users_mtx;
};

#endif