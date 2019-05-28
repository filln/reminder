#-------------------------------------------------
#
# Project created by QtCreator 2018-01-04T22:09:37

#//Reminder Copyright (C) 2018г. Кучеров Анатолий Александрович

#-------------------------------------------------

QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = reminder
TEMPLATE = app

win32: RC_ICONS = $$PWD/images/reminder.ico
win32:{
    VERSION = 1.5.0
#    QMAKE_TARGET_COMPANY = Название компании
    QMAKE_TARGET_PRODUCT =  Reminder
#    QMAKE_TARGET_DESCRIPTION = Программа-напоминалка
    QMAKE_TARGET_COPYRIGHT = https://ulvprog.wordpress.com/
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    listwindow.cpp \
    callrem.cpp \
    listhistwindow.cpp \
    trayrem.cpp \
    shortcutrem.cpp \
    changewindow.cpp

HEADERS += \
    mainwindow.h \
    listwindow.h \
    callrem.h \
    listhistwindow.h \
    trayrem.h \
    shortcutrem.h \
    changewindow.h

FORMS += \
        mainwindow.ui \
    listwindow.ui \
    callrem.ui \
    listhistwindow.ui \
    changewindow.ui

RESOURCES += \
    qrcrem.qrc
