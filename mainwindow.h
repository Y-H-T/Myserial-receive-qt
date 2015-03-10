#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QMainWindow>
#include"qextserialport.h"
#include"qextserialenumerator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);
    ~MainWindow();
    void decode(QByteArray);  //接收数据解析函数

private slots:
    void readMyCom();
    void on_openMyComBtn_clicked();
    void on_closeMyComBtn_clicked();
    void on_ClearButton_clicked();

    void onBaudRateChanged(int idx);
    void onParityChanged(int idx);
    void onDataBitsChanged(int idx);
    void onStopBitsChanged(int idx);
    void onPortAddedOrRemoved();

private:
    Ui::MainWindow *ui;
    QextSerialPort *myCom;
    QextSerialEnumerator *enumerator;

};

#endif // MAINWINDOW_H

