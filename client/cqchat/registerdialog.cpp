#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);


    initHttpHandlers();


    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);

    //day11 设定输入框输入后清空字符串
    ui->err_tip->clear();

    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });

    connect(ui->pass_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });

    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });

    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this](){
        checkVarifyValid();
    });


    ui->pass_visible->setCursor(Qt::PointingHandCursor);
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);

    ui->pass_visible->SetState("unvisible","unvisible_hover","","visible",
                               "visible_hover","");

    ui->confirm_visible->SetState("unvisible","unvisible_hover","","visible",
                                  "visible_hover","");
    //连接点击事件

    connect(ui->pass_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->pass_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->pass_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->pass_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->confirm_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->confirm_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });


    // 创建定时器
    _countdown_timer = new QTimer(this);
    _countdown = 5;
    // 连接信号和槽
    connect(_countdown_timer, &QTimer::timeout, [this](){
        if(_countdown==0){
            _countdown_timer->stop();
            _countdown = 5;
            emit sigSwitchLogin();
            return;
        }
        _countdown--;
        auto str = QString("注册成功，%1 s后返回登录").arg(_countdown);
        ui->tip_lb->setText(str);
    });


}

RegisterDialog::~RegisterDialog()
{
    qDebug("destruct RegisterDlg");
    delete ui;
}

void RegisterDialog::on_get_code_clicked()
{
    if(!checkEmailValid()){
        return;
    }

    //发送http请求
    /*ui->err_tip->setText(QString::fromLocal8Bit("发送请求成功"));
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);*/
    QJsonObject json_obj;
    json_obj["email"] = ui->email_edit->text();
    HttpMgr::GetInstance()->postHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);
    return;

}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"),false);
        return;
    }

    // 发的时候需要序列化为QByteArray二进制字节流，解析需要反序列化为类对象
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());

    if (jsonDoc.isNull()) {
        showTip(tr("json解析错误"),false);
        return;
    }

    if (!jsonDoc.isObject()) {
        showTip(tr("json解析错误"),false);
        return;
    }

    QJsonObject jsonObj = jsonDoc.object(); //json文档转化成json对象
    _handlers[id](jsonObj);   //再根据不同的id进行处理

    return;
}

void RegisterDialog::showTip(QString str,bool ok){

    if(ok){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state", "err");
    }

    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

void RegisterDialog::initHttpHandlers()
{
    //获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject& jsonObj) {
        //注意：这些字段记得和服务器对应一下
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("获取验证码失败"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送至邮箱"),true);
        qDebug() << "email is " << email;
    });

    //注册用户回包逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug()<< "email is " << email ;

        ChangeTipPage();
    });

}


void RegisterDialog::on_sure_btn_clicked()
{
    if(!checkUserValid() || !checkEmailValid() || !checkPassValid() || !checkConfirmValid() || !checkVarifyValid()){
        return;
    }

    //发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = xorString(ui->pass_edit->text());
    json_obj["confirm"] = xorString(ui->confirm_edit->text());
    json_obj["varifycode"] = ui->varify_edit->text();
    HttpMgr::GetInstance()->postHttpReq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}

void RegisterDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void RegisterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }

    showTip(_tip_errs.first(), false);
}

void RegisterDialog::ChangeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);

    // 启动定时器，设置间隔为1000毫秒（1秒）
    _countdown_timer->start(1000);
}


bool RegisterDialog::checkUserValid()
{
    if(ui->user_edit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}


bool RegisterDialog::checkPassValid()
{

    auto pass = ui->pass_edit->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

bool RegisterDialog::checkConfirmValid()
{
    if(ui->confirm_edit->text() == ""){
        AddTipErr(TipErr::TIP_CONFIRM_ERR,"确认密码不能为空");
        return false;
    }

    if(ui->confirm_edit->text() != ui->pass_edit->text()){
        AddTipErr(TipErr::TIP_CONFIRM_ERR,"密码和确认密码不匹配");
        return false;
    }

    DelTipErr(TipErr::TIP_CONFIRM_ERR);
    return true;

}



bool RegisterDialog::checkEmailValid()
{
    auto email = ui->email_edit->text();

    if (email.isEmpty()) {
        AddTipErr(TipErr::TIP_EMAIL_ERR,"邮箱不能为空");
        return false;
    }

    QRegularExpression emailRegex(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}\b)");

    if (!emailRegex.match(email).hasMatch()) {
        AddTipErr(TipErr::TIP_EMAIL_ERR,"邮箱格式不正确");
        return false;
    }



    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkVarifyValid()
{
    auto pass = ui->varify_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void RegisterDialog::on_return_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}


void RegisterDialog::on_cancel_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

