#ifndef _CSESSION_H_
#define _CSESSION_H_

#include "global.h"
#include "msgnode.h"

using namespace std;

class CServer;
class LogicSystem;

class CSession: public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& io_context, CServer* server);
	~CSession();
	tcp::socket& GetSocket();
	std::string& GetSessionId();
	void SetUserId(int uid);
	int GetUserId();

    // 启动会话
	void Start();

    // 发送消息
	void Send(char* msg,  short max_length, short msgid);
	void Send(std::string msg, short msgid);

    // 关闭会话
	void Close();

    // 共享自身
	std::shared_ptr<CSession> SharedSelf();

    // 异步读取消息
	void AsyncReadBody(int length);
	void AsyncReadHead(int total_len);
private:
	void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code& , std::size_t)> handler);
	void asyncReadLen(std::size_t  read_len, std::size_t total_len,
		std::function<void(const boost::system::error_code&, std::size_t)> handler);
	
	
	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);

private:
	tcp::socket _socket;
	std::string _session_id;
	char _data[MAX_LENGTH];
	CServer* _server;
	bool _b_close;       //是否解析完头部
	std::queue<shared_ptr<SendNode> > _send_que;
	std::mutex _send_lock;
	//收到的消息结构
	std::shared_ptr<RecvNode> _recv_msg_node;
	bool _b_head_parse;
	//收到的头部结构
	std::shared_ptr<MsgNode> _recv_head_node;
	int _user_uid;
};

class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);
private:
	shared_ptr<CSession> _session;
	shared_ptr<RecvNode> _recvnode;
};

#endif