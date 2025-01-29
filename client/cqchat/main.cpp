#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLoggingCategory::setFilterRules("MyModule.debug=true");

    QFile qss(":/style/stylesheet.qss");
    if( qss.open(QFile::ReadOnly))
    {
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug("Open failed");
    }

    /* 注意：这是返回当前可执行文件所在的目录路径（程序运行时的可执行文件路径）
     *
     * 在mac上，文件系统中看到的可执行程序，是.app后缀的，而macOS 的 .app 是一个特殊的目录结构（实际上是一个文件夹），其中的可执行文件通常位于 Contents/MacOS 子目录下
     * 所以这个会返回/Users/chenqi/ChenQi/04 projects/cqchat/client/cqchat/build/Qt_6_5_3_macos-Debug/bin/cqchat.app/Contents/MacOS
     *
     */
    QString app_path = QCoreApplication::applicationDirPath();

    //QString fileName = "config.ini";
    // QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName);

    QString config_path = QDir::cleanPath(app_path + "/../../../config.ini");

    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gate_host + ":" + gate_port;

    MainWindow w;
    w.show();



    return a.exec();
}

