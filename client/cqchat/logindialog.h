#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

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
private slots:
    void slot_forget_pwd();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
