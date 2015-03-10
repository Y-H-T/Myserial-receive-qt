#include "mainwindow.h"
#include <QApplication>
#include<QTextCodec>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);//QApplication类的对象
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("gbk"));
    MainWindow w;
    w.show();             //窗口界面显示
    return a.exec();      //exec()函数属于类QApplication，使其进入事件循环，可以执行的过程中接收各种指令
}
