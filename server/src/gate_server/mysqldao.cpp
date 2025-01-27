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