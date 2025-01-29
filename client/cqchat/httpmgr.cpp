#include "httpmgr.h"

HttpMgr::HttpMgr()
{
    //此信号用来处理http请求完成后的操作(主要是再发一次信号区分是哪个模块完成）
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

HttpMgr::~HttpMgr()
{
}

void HttpMgr::postHttpReq(const QUrl& url, const QJsonObject& json, ReqId req_id, Modules mod)
{



    QByteArray data = QJsonDocument(json).toJson();  //传输都是字节流的形式
    //通过url构造请求，建一个HTTP POST请求，并设置请求头和请求体
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    QNetworkReply* reply = m_networkManager.post(request, data); //发送请求，并收到回复




    auto self = shared_from_this();

    //接受回包过程完成reply会发送finished信号，绑定信号与槽（这里使用lambda表达式）
    /*
     * 这里在lambda表达式中要使用到HttpMgr的数据，但是用this指针捕获是不安全的，在一些地方可能删除回收了HttpMgr的情况
     * 所以用了一个shared_from_this()，增加一个引用计数供lanbda表达式使用
     *
     */
    QObject::connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod]() {

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }

        QString res = reply->readAll();

        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);  //发送http请求完成信号

        reply->deleteLater(); //需要回收reply
        return;
    });
}


void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod) {
    if (mod == Modules::REGISTERMOD) {
        emit sig_reg_mod_finish(id, res, err);   //如果是注册模块的http请求就发送个注册模块完成的信号（err是哪种情况再在这个信号的槽函数中处理）
    }
    if (mod == Modules::RESETMOD){
        emit sig_reset_mod_finish(id,res,err);
    }

}
