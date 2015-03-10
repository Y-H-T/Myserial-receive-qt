#-------------------------------------------------
#
# Project created by QtCreator 2014-11-08T10:40:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mycom2jeishou
TEMPLATE = app
#包含第三方串口库到项目里
include("c:\qextserialport-1.2rc\src\qextserialport.pri")

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
