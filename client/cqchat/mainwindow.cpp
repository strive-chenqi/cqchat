#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QApplication::setStyle("Fusion"); // 其他样式，窗口的底部可能有一个阴影或类似区域，这是 macOS 的窗口样式特性。

    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_login_dlg = new LoginDialog();
    setCentralWidget(m_login_dlg);
    //m_login_dlg->show();

    m_reg_dlg = new RegisterDialog(); //注意两个对象都没有制定父节点

    //该信号发送给MainWindow用来切换界面
    connect(m_login_dlg, &LoginDialog::switchRegister,this, &MainWindow::slotSwitchReg);
}

MainWindow::~MainWindow()
{
    delete ui;
    if(m_login_dlg){
        delete m_login_dlg;
        m_login_dlg = nullptr;
    }

    if(m_reg_dlg){
        delete m_reg_dlg;
        m_reg_dlg = nullptr;
    }
}

void MainWindow::slotSwitchReg(){
    setCentralWidget(m_reg_dlg);
    m_login_dlg->hide();
    //m_reg_dlg->show();
}
