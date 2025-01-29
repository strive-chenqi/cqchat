#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <global.h>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();
signals:
    void sigSwitchLogin();

private slots:
    void on_get_code_clicked();
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

    void on_sure_btn_clicked();

    void on_return_btn_clicked();

    void on_cancel_btn_clicked();

private:
    void showTip(QString str,bool ok);
    void initHttpHandlers();  //初始化一些处理器（哪个模块的哪个id完成的回调）

    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVarifyValid();

    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);

    void ChangeTipPage();


private:
    Ui::RegisterDialog *ui;

    QMap<TipErr, QString> _tip_errs;

    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;  //id和回调（可调用对象）的映射

    QTimer* _countdown_timer;
    int _countdown;
};

#endif // REGISTERDIALOG_H
