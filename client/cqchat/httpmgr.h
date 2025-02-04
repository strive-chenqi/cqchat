#ifndef HTTPMGR_H
#define HTTPMGR_H

#include "singleton.h"
#include "global.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

//C++的crtp，能够继承以自己为类型的模版类
class HttpMgr :public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT

    /* 逻辑：获取一个HttpMgr单例
     * 1、调用GetInstance，执行static std::shared_ptr<T> instance(new T);
     * 2、这句代码里面会new一个HttpMgr，那么会先构造基类Singleton，所以要确保Singleton的构造函数那些是protected
     * 3、继续构造子类，为了单例子类构造函数为私有，为了在父类里面能调用子类构造函数，所以需要设置友元
     * 4、std::shared_ptr需要访问析构函数来正确地管理对象的生命周期，所以HttpMgr的析构函数要为public
     */


    friend class Singleton<HttpMgr>;
public:
    ~HttpMgr();

    //发送http请求
    void postHttpReq(const QUrl& url, const QJsonObject& json, ReqId req_id, Modules mod);

signals:

    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    void sig_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
    void sig_login_mod_finish(ReqId id, QString res, ErrorCodes err);

private slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

private:
    HttpMgr();




private:
    QNetworkAccessManager m_networkManager;
};

#endif // HTTPMGR_H
