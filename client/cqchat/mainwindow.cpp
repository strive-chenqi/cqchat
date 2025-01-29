#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QApplication::setStyle("Fusion"); // 其他样式，窗口的底部可能有一个阴影或类似区域，这是 macOS 的窗口样式特性。

    m_login_dlg = new LoginDialog(this);
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(m_login_dlg);
    //m_login_dlg->show();  //setCentralWidge能嵌入进来的话默认是有show的效果的，只要父窗口show了
    //m_reg_dlg->hide();   //注意：mac上需要hide一下，否则会造成两个dialog同时显示在mainwindow上




    connect(m_login_dlg, &LoginDialog::switchRegister,this, &MainWindow::slotSwitchReg); //该信号发送给MainWindow用来切换界面
    connect(m_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset); //连接登录界面忘记密码信号
}

MainWindow::~MainWindow()
{
    delete ui;

    /*
     * 调用 setCentralWidget设置中心窗口 时（slotSwitchReg中），QMainWindow 会接管该对象的所有权，负责在程序退出时释放内存。，导致重复删除
     *
     * 那没有调用setCentralWidget的又比较麻烦处理，所以在创建对象的时候最好就是指定父节点，利用对象树机制
     *
     * 注意：
     * 当将 m_login_dlg 和 m_reg_dlg 设置为 MainWindow 的子对象时，它们的默认行为是独立的窗口，因为 QDialog 默认是一个独立的窗口小部件（即顶层窗口），而不是嵌入式部件。
     *
     * 解决：
     * 1、setCentralWidget默认是有嵌入效果的，但是对于这里的dialog也只是放到中央区域，需要手动适应父窗口的布局才会表现出嵌入的效果（也只是效果上简单覆盖了而已，任然是两个单独的窗口）
     * 2、要嵌入到 MainWindow，需要修改窗口属性m_login_dlg->setWindowFlags(Qt::Widget);
     */


    // if(m_login_dlg){
    //     delete m_login_dlg;
    //     m_login_dlg = nullptr;
    // }

    // if(m_reg_dlg){
    //     delete m_reg_dlg;
    //     m_reg_dlg = nullptr;
    // }
}

void MainWindow::slotSwitchReg(){

    m_reg_dlg = new RegisterDialog(this);

    m_reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);


    //连接注册界面返回登录信号
    connect(m_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::slotSwitchLogin);
    setCentralWidget(m_reg_dlg);  //在调用 setCentralWidget() 设置新的中心窗口时自动删除之前的中心窗口部件


    // m_login_dlg->hide();
    // m_reg_dlg->show();

}

void MainWindow::slotSwitchLogin()
{
    m_login_dlg = new LoginDialog(this);
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);

    // m_reg_dlg->hide();
    // m_login_dlg->show();
    //连接登录界面注册信号（新实例创建后，旧的实例没有保留，之前的 connect（信号与槽的连接）就不会作用于新实例。）
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::slotSwitchReg);
    connect(m_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);

}

void MainWindow::SlotSwitchReset()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_reset_dlg = new ResetDialog(this);
    m_reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_reset_dlg);

    // _login_dlg->hide();
    // _reset_dlg->show();
    //注册返回登录信号和槽函数
    connect(m_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::slotSwitchLogin);
}

// void MainWindow::slotSwitchLogin2()
// {
//     m_login_dlg = new LoginDialog(this);
//     m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
//     setCentralWidget(m_login_dlg);

//     // m_reg_dlg->hide();
//     // m_login_dlg->show();
//     //连接登录界面注册信号（新实例创建后，旧的实例没有保留，之前的 connect（信号与槽的连接）就不会作用于新实例。）
//     connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::slotSwitchReg);

// }
