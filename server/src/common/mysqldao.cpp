#include "mysqldao.h"

MysqlDao::MysqlDao() {
  auto &cfg = ConfigMgr::Inst();
  const auto &host = cfg["Mysql"]["Host"];
  const auto &port = cfg["Mysql"]["Port"];
  const auto &pwd = cfg["Mysql"]["Passwd"];
  const auto &schema = cfg["Mysql"]["Schema"];
  const auto &user = cfg["Mysql"]["User"];
  pool_.reset(new MySqlPool(host + ":" + port, user, pwd, schema, 5));
}

MysqlDao::~MysqlDao() { pool_->Close(); }

int MysqlDao::RegUser(const std::string &name, const std::string &email,
                      const std::string &pwd) {
  auto con = pool_->getConnection();
  try {
    //这里其实也可以用def
    if (con == nullptr) {
      return false;
    }

    // 准备调用存储过程，带参数（存储过程），@result是输出参数
    // 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
    std::unique_ptr<sql::PreparedStatement> stmt(con->con_->prepareStatement("CALL reg_user(?,?,?,@result)"));
    // 设置输入参数
    stmt->setString(1, name);
    stmt->setString(2, email);
    stmt->setString(3, pwd);

    // 执行存储过程
    stmt->execute();

    // 如果存储过程设置了会话变量或有其他方式获取输出参数的值，你可以在这里执行SELECT查询来获取它们
    // 例如，如果存储过程设置了一个会话变量@result来存储输出结果，可以这样获取：
    std::unique_ptr<sql::Statement> stmtResult(con->con_->createStatement()); //不带参数(语句)
    std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result")); //取出会话变量（输出参数）
    if (res->next()) {
      int result = res->getInt("result");
      std::cout << "Result: " << result << std::endl;
      pool_->returnConnection(std::move(con));
      return result;
    }
    pool_->returnConnection(std::move(con));
    return -1;
    
  } catch (sql::SQLException &e) {
    pool_->returnConnection(std::move(con));
    std::cerr << "SQLException: " << e.what();
    std::cerr << " (MySQL error code: " << e.getErrorCode();
    std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    return -1;
  }
}

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email) {
    auto con = pool_->getConnection();
    try {
        if (con == nullptr) {
            return false;
        }

        // 准备查询语句
        std::unique_ptr<sql::PreparedStatement> pstmt(con->con_->prepareStatement("SELECT email FROM user WHERE name = ?"));

        // 绑定参数
        pstmt->setString(1, name);

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 遍历结果集
        while (res->next()) {
            std::cout << "Check Email: " << res->getString("email") << std::endl;
            if (email != res->getString("email")) {
                pool_->returnConnection(std::move(con));
                return false;
            }
            pool_->returnConnection(std::move(con));
            return true;
        }
        // 添加默认返回值，当没有找到匹配的记录时
        pool_->returnConnection(std::move(con));
        return false;
    }
    catch (sql::SQLException& e) {
        pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newpwd) {
    auto con = pool_->getConnection();
    try {
        if (con == nullptr) {
            pool_->returnConnection(std::move(con));
            return false;
        }

        // 准备查询语句
        std::unique_ptr<sql::PreparedStatement> pstmt(con->con_->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));

        // 绑定参数
        pstmt->setString(1, newpwd);
        pstmt->setString(2, name);

        // 执行更新
        int updateCount = pstmt->executeUpdate();

        std::cout << "Updated rows: " << updateCount << std::endl;
        pool_->returnConnection(std::move(con));
        return true;
    }
    catch (sql::SQLException& e) {
        pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::checkPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo) {
    auto con = pool_->getConnection();
    Defer defer([this, &con]() {
        pool_->returnConnection(std::move(con));
        });

    try {
        if (con == nullptr) {
            return false;
        }

        // 准备SQL语句
        std::unique_ptr<sql::PreparedStatement> pstmt(con->con_->prepareStatement("SELECT * FROM user WHERE email = ?"));
        pstmt->setString(1, name); // 将username替换为你要查询的用户名

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
    
        std::string origin_pwd = "";
        // 遍历结果集
        while (res->next()) {
            origin_pwd = res->getString("pwd");
            // 输出查询到的密码
            std::cout << "Password: " << origin_pwd << std::endl;
            break;
        }
        
        if (pwd != origin_pwd) {
            return false;
        }
        userInfo.name = name;
        userInfo.email = res->getString("email");
        userInfo.uid = res->getInt("uid");
        userInfo.pwd = origin_pwd;
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}


std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});

	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->con_->prepareStatement("SELECT * FROM user WHERE uid = ?"));
		pstmt->setInt(1, uid); // 将uid替换为你要查询的uid

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		// 遍历结果集
		while (res->next()) {
			user_ptr.reset(new UserInfo);
			user_ptr->pwd = res->getString("pwd");
			user_ptr->email = res->getString("email");
			user_ptr->name= res->getString("name");
			// user_ptr->nick = res->getString("nick");
			// user_ptr->desc = res->getString("desc");
			// user_ptr->sex = res->getInt("sex");
			// user_ptr->icon = res->getString("icon");
			user_ptr->uid = uid;
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(std::string name)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});

	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->con_->prepareStatement("SELECT * FROM user WHERE name = ?"));
		pstmt->setString(1, name); // 将uid替换为你要查询的uid

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		// 遍历结果集
		while (res->next()) {
			user_ptr.reset(new UserInfo);
			user_ptr->pwd = res->getString("pwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			// user_ptr->nick = res->getString("nick");
			// user_ptr->desc = res->getString("desc");
			// user_ptr->sex = res->getInt("sex");
			user_ptr->uid = res->getInt("uid");
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}
