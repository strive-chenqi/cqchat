#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

signals:
    void switchRegister();
    void switchReset();
    void sig_connect_tcp(ServerInfo);
private slots:
    void initHead();

    void slot_forget_pwd();

    void on_login_btn_clicked();

    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);

    void slot_tcp_con_finish(bool bsuccess);
    void slot_login_failed(int);
private:
    void initHttpHandlers();

    bool checkUserValid();
    bool checkPwdValid();

    void addTipErr(TipErr te,QString tips);
    void delTipErr(TipErr te);

    void showTip(QString str,bool b_ok);
    bool enableBtn(bool enabled);

private:
    Ui::LoginDialog *ui;

    QMap<TipErr,QString> m_tip_errs;
    QMap<ReqId,std::function<void(QJsonObject)>>   _handlers;

    int _uid;
    QString _token;

};

#endif // LOGINDIALOG_H
