#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include <QStyle>
#include <QRegularExpression>
#include <memory>
#include <iostream>
#include <mutex>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
#include <QSettings>
#include <QFile>

#include <QLoggingCategory>

// #define QRC_QSS(x) ":/Resources/QSS/" + x + ".css"
// #define INI_PATH(f) QApplication::applicationDirPath() + "/ini/" + f
// #define APP_ICON(i) ":/Resources/MainWindow/app/" + i + ".png"


/**
 * @brief repolish 用来刷新qss
 */
extern std::function<void(QWidget*)> repolish;

/**
 * @brief xorString  异或字符串，简单加密用
 */
extern std::function<QString(QString)> xorString;

// enum ButtonType
// {
//     MIN_CLOSE_BUTTON = 0,    //��С�����رհ�ť
//     MIN_MAX_CLOSE_BUTTON,    //��С������󻯼��رհ�ť
//     CLOSE_BUTTON             //���йرհ�ť
// };


/*
 * 现在都是异步请求，加Reqid req_id,Modules mod区分请求的id和模块，异步返回了才能做出区分
 */
//功能id
enum ReqId {
    ID_GET_VARIFY_CODE = 1001, //获取验证吗
    ID_REG_USER = 1002, //注册用户
    ID_RESET_PWD = 1003, //重置密码
};

//功能id所属于的模块
enum Modules {
    REGISTERMOD = 0,  //注册模块
    RESETMOD = 1,     //忘记密码模块
};

//功能码
enum ErrorCodes {
    SUCCESS = 0,
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2, //网络错误
};

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum ClickLbState{
    Normal = 0,
    Selected = 1
};

extern QString gate_url_prefix;

#endif // GLOBAL_H
