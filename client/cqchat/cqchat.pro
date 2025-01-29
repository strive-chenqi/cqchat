QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

ICON = cqchat.icns

DESTDIR = ./bin

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
CONFlG += sdk_no_version_check


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clickedlabel.cpp \
    global.cpp \
    httpmgr.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    registerdialog.cpp \
    resetdialog.cpp \
    timerbtn.cpp

HEADERS += \
    clickedlabel.h \
    global.h \
    httpmgr.h \
    logindialog.h \
    mainwindow.h \
    registerdialog.h \
    resetdialog.h \
    singleton.h \
    timerbtn.h

FORMS += \
    logindialog.ui \
    mainwindow.ui \
    registerdialog.ui \
    resetdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

DISTFILES += \
    config.ini

# 针对 macOS 系统的配置
macx:CONFIG(debug, debug | release) {
    # 指定要拷贝的配置文件路径为工程目录下的 config.ini
    TargetConfig = $${PWD}/config.ini
    # 指定目标输出目录
    OutputDir = $${OUT_PWD}/$${DESTDIR}
    # 执行 cp 命令，将配置文件拷贝到输出目录
    QMAKE_POST_LINK += cp -f \"$$TargetConfig\" \"$$OutputDir\"
}
