#-------------------------------------------------
#
# Project created by QtCreator 2024-10-12T01:12:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FloatingBall
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        aichat.cpp \
        main.cpp \
        screenshotSelector.cpp \
        widget.cpp

HEADERS += \
        aichat.h \
        screenshotSelector.h \
        widget.h

FORMS += \
        widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
QMAKE_INFO_PLIST = ./Info.plist


# 添加 QHotkey 源文件
SOURCES += \
    ./QHotkey/qhotkey.cpp \
   ./QHotkey/qhotkey_mac.cpp \

# 添加 QHotkey 头文件
HEADERS += \
    ./QHotkey/qhotkey.h \
    ./QHotkey/qhotkey_p.h

# 添加 QHotkey 包含路径
INCLUDEPATH +=./QHotkey

# 根据平台添加必要的库
win32: LIBS += -luser32
macx: LIBS += -framework Carbon
