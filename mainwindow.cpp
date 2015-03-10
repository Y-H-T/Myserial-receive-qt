#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include <QtCore>

int count1,count2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->closeMyComBtn->setEnabled(false); //开始“关闭串口”按钮不可用
    //串口参数的设置
    foreach(QextPortInfo info, QextSerialEnumerator::getPorts())             //串口名是自动检测的
            ui->portNameComboBox->addItem(info.portName);                    //检测完，自动获得计算机上的串口名
    //make sure user can input their own port name!
    ui->portNameComboBox->setEditable(true);

     //添加波特率的几个设置属性到组合框
    ui->baudRateComboBox->addItem("1200", BAUD1200);
    ui->baudRateComboBox->addItem("2400", BAUD2400);
    ui->baudRateComboBox->addItem("4800", BAUD4800);
    ui->baudRateComboBox->addItem("9600", BAUD9600);
    ui->baudRateComboBox->addItem("19200", BAUD19200);
    ui->baudRateComboBox->setCurrentIndex(3);        //默认波特率显示为9600

    ui->parityComboBox->addItem("NONE", PAR_NONE);   //添加校验位的几个设置属性到组合框
    ui->parityComboBox->addItem("ODD", PAR_ODD);
    ui->parityComboBox->addItem("EVEN", PAR_EVEN);

    ui->dataBitsComboBox->addItem("5", DATA_5);     //添加数据位的几个设置属性到组合框
    ui->dataBitsComboBox->addItem("6", DATA_6);
    ui->dataBitsComboBox->addItem("7", DATA_7);
    ui->dataBitsComboBox->addItem("8", DATA_8);
    ui->dataBitsComboBox->setCurrentIndex(3);       //默认数据位显示为8

    ui->stopBitsComboBox->addItem("1", STOP_1);      //添加停止位的几个设置属性到组合框
    ui->stopBitsComboBox->addItem("2", STOP_2);
\
    //定义一个新的串口对象并对其进行初始化,以事件驱动的方式打开
    PortSettings settings = {BAUD9600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    myCom = new QextSerialPort(ui->portNameComboBox->currentText(), settings, QextSerialPort::EventDriven);

    enumerator = new QextSerialEnumerator(this);
    enumerator->setUpNotifications();

    connect(ui->baudRateComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onBaudRateChanged(int)));
    connect(ui->parityComboBox,   SIGNAL(currentIndexChanged(int)), SLOT(onParityChanged(int)));
    connect(ui->dataBitsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onDataBitsChanged(int)));
    connect(ui->stopBitsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onStopBitsChanged(int)));

    connect(enumerator, SIGNAL(deviceDiscovered(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
    connect(enumerator, SIGNAL(deviceRemoved(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//串口参数的设置函数
void MainWindow::onBaudRateChanged(int idx)
{
    myCom->setBaudRate((BaudRateType)ui->baudRateComboBox->itemData(idx).toInt());
}
void MainWindow::onParityChanged(int idx)
{
    myCom->setParity((ParityType)ui->parityComboBox->itemData(idx).toInt());
}
void MainWindow::onDataBitsChanged(int idx)
{
    myCom->setDataBits((DataBitsType)ui->dataBitsComboBox->itemData(idx).toInt());
}
void MainWindow::onStopBitsChanged(int idx)
{
    myCom->setStopBits((StopBitsType)ui->stopBitsComboBox->itemData(idx).toInt());
}

void MainWindow::onPortAddedOrRemoved()  //串口名字检测函数
{
    QString current = ui->portNameComboBox->currentText();

    ui->portNameComboBox->blockSignals(true);//串口组合框此时被阻塞
    ui->portNameComboBox->clear();           //刷新串口组合框
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        ui->portNameComboBox->addItem(info.portName);

    ui->portNameComboBox->setCurrentIndex(ui->portNameComboBox->findText(current));
    ui->portNameComboBox->blockSignals(false);
}


void MainWindow::on_openMyComBtn_clicked()                      //串口串口工作之前必须先打开
{
    myCom->setPortName(ui->portNameComboBox->currentText());    //设置串口的新名字,把监测到的新串口名字添加进去
    myCom ->open(QIODevice::ReadWrite);                         //以可读写方式打开串口
    //信号和槽函数关联，只要串口缓冲区有数据就立马进行读串口操作，几乎同时进行
    connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));

    ui->openMyComBtn->setEnabled(false); //打开串口后“打开串口”按钮不可用
    ui->closeMyComBtn->setEnabled(true); //打开串口后“关闭串口”按钮可用

    ui->baudRateComboBox->setEnabled(false); //设置各个组合框不可用
    ui->dataBitsComboBox->setEnabled(false);
    ui->parityComboBox->setEnabled(false);
    ui->stopBitsComboBox->setEnabled(false);
    ui->portNameComboBox->setEnabled(false);

}

//*******************************************接收部分*****************************//
void MainWindow::readMyCom()                              //读取串口函数
{
    qDebug()<< "read: "<<myCom->bytesAvailable()<<"bytes";//我们输出每次获得的字节数
    QByteArray temp = myCom->readAll();                   //读取串口缓冲区的所有数据给临时变量temp
    ui->textBrowser1->insertPlainText(temp);


    //考虑是否可以利用定时器，设成槽函数，以一定间隔读入缓冲区数据到storeDate中，担心同步不太好的问题
    static QByteArray storeData;
    storeData.append(temp);

    for(int i=0;i<storeData.size()/28;i++)   //只有长度符合要求才会执行接下来的处理函数,
    {       
        decode(storeData.left(28));
        storeData.remove(0,28);            //得到剩下的字符串数组，指针还是指向最前面
        //**************************************
        count2++;
        ui->lineEdit10->clear();
        ui->lineEdit10->insert(QString::number(count2));

    }
    //************************************************************

}

void MainWindow::decode(QByteArray data)
{
    if(QString(data.left(4))=="aaaa")
    {
        data.remove(0,4);

    if(QString(data.right(4))=="ffff")
    {
        data.remove(20,24);

        ui->lineEdit1->clear();  //满足要求后才会清理上次的结果
        ui->lineEdit2->clear();
        ui->lineEdit3->clear();
        ui->lineEdit4->clear();
        ui->lineEdit5->clear();
        ui->lineEdit6->clear();
        ui->lineEdit7->clear();
        ui->lineEdit8->clear();

    ui->textBrowser2->insertPlainText(data);//分割后的字符

    count1++;
    ui->lineEdit9->clear();
    ui->lineEdit9->insert(QString::number(count1));

    QString data1=QString(data);

    QString temp1=data1.mid(0,4);
    QString temp2=data1.mid(4,4);
    QString temp3=data1.mid(8,4);
    QString temp4=data1.mid(12,4);

    QString temp5=data1.mid(16,1);
    QString temp6=data1.mid(17,1);
    QString temp7=data1.mid(18,1);
    QString temp8=data1.mid(19,1);

    ui->lineEdit1->insert(temp1);
    ui->lineEdit2->insert(temp2);
    ui->lineEdit3->insert(temp3);
    ui->lineEdit4->insert(temp4);

    ui->lineEdit5->insert(temp5);
    ui->lineEdit6->insert(temp6);
    ui->lineEdit7->insert(temp7);
    ui->lineEdit8->insert(temp8);

    }
    }
    else
    {
        return;
    }

}

/*void MainWindow::readMyCom()
{
    qDebug()<< "read: "<<myCom->bytesAvailable()<<"bytes";//我们输出每次获得的字节数
    QByteArray temp = myCom->readAll();                   //读取串口缓冲区的所有数据给临时变量temp
    ui->textBrowser1->insertPlainText(temp.toHex());//将串口的数据显示在窗口的文本浏览器中以十六进制的形式显示
    //考虑是否可以利用定时器，设成槽函数，以一定间隔读入缓冲区数据到storeDate中，担心同步不太好的问题
    static QByteArray storeData;
    storeData.append(temp);

    for(int i=0;i<storeData.size()/16;i++)   //只有长度符合要求才会执行接下来的处理函数,
    {
        decode(storeData.left(16));
        storeData.remove(0,16);            //得到剩下的字符串数组，指针还是指向最前面
    }
}

void MainWindow::decode(QByteArray data)
{

if(QString(data.left(2))==QByteArray::fromHex("aaaa"))
{
    data.remove(0,2);
if(QString(data.right(2))==QByteArray::fromHex("ffff"))
{
    data.remove(12,14);

    ui->lineEdit1->clear();  //满足要求后才会清理上次的结果
    ui->lineEdit2->clear();
    ui->lineEdit3->clear();
    ui->lineEdit4->clear();
    ui->lineEdit5->clear();
    ui->lineEdit6->clear();
    ui->lineEdit7->clear();
    ui->lineEdit8->clear();

ui->textBrowser2->insertPlainText(data.toHex());//分割后的字符

QByteArray temp1=data.mid(0,2);
QByteArray temp2=data.mid(2,2);
QByteArray temp3=data.mid(4,2);
QByteArray temp4=data.mid(6,2);

QByteArray temp5=data.mid(8,1);
QByteArray temp6=data.mid(9,1);
QByteArray temp7=data.mid(10,1);
QByteArray temp8=data.mid(11,1);

ui->lineEdit1->insert(temp1.toHex());
ui->lineEdit2->insert(temp2.toHex());
ui->lineEdit3->insert(temp3.toHex());
ui->lineEdit4->insert(temp4.toHex());

ui->lineEdit5->insert(temp5.toHex());
ui->lineEdit6->insert(temp6.toHex());
ui->lineEdit7->insert(temp7.toHex());
ui->lineEdit8->insert(temp8.toHex());

}
}
else
{
    return;
}*/


void MainWindow::on_closeMyComBtn_clicked()
{
    myCom->close();                         //关闭串口，该函数在win_qextserialport.cpp文件中定义
    ui->openMyComBtn->setEnabled(true);     //关闭串口后“打开串口”按钮可用
    ui->closeMyComBtn->setEnabled(false);   //关闭串口后“关闭串口”按钮不可用

    ui->baudRateComboBox->setEnabled(true); //设置各个组合框可用
    ui->dataBitsComboBox->setEnabled(true);
    ui->parityComboBox->setEnabled(true);
    ui->stopBitsComboBox->setEnabled(true);
    ui->portNameComboBox->setEnabled(true);
}
void MainWindow::on_ClearButton_clicked()
{
    ui->textBrowser1->clear();
    ui->textBrowser2->clear();
    ui->lineEdit1->clear();
    ui->lineEdit2->clear();
    ui->lineEdit3->clear();
    ui->lineEdit4->clear();
    ui->lineEdit5->clear();
    ui->lineEdit6->clear();
    ui->lineEdit9->clear();
    ui->lineEdit10->clear();
    count1=count2=0;
}
