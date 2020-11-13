#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QLabel>
#include <QDebug>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include "pages/pages.h"
#include "database/dbhelper.h"
#include "tools/useraccount.h"
#include "tools/tools.h"
#include "tools/serialportthread.h"
#include "inc/m1356dll.h"

#define CURRENT_VERSION "基于RFID技术的图书管理系统 V1.0"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addWidgets(); //加载页面
    void handConnect();//信号插槽连接

    const int noLogin = 0;
    const int adminLogin = 1;
    const int userLogin = 2;

    struct Settings {
            QString name;
            qint32 baudRate;
            QString stringBaudRate;
            QSerialPort::DataBits dataBits;
            QString stringDataBits;
            QSerialPort::Parity parity;
            QString stringParity;
            QSerialPort::StopBits stopBits;
            QString stringStopBits;
            QSerialPort::FlowControl flowControl;
            QString stringFlowControl;
     };//缓存串口配置信息
     Settings settings() const;//返回常量形式的配置信息

private slots:
    void About(); //关于
    void LoginSys(); //登陆
    void ExitSys(); //退出
    void viewMainPage(); //显示主页
    void viewInfoTable(); //显示信息表
    void lostRecord(); //挂失
    void foundRecord(); //解除挂失
    void ViewRecordTable(); //查询借还书记录
    void BindBook(); //绑定图书
    void RegistorUser(); //用户注册
    void UnregistorUser(); //用户注销
    void BorrowBook(); //借书
    void ReBook(); //还书

    void fillPortsParameters();//填充串口参数
    void on_connect_clicked();
    void Disconnect(); //断开连接
    void onOperationError(QString msg); //串口发生错误时接收错误信息槽
    void onSendMessage(char *data, int frameLen);//串口消息发送槽
    void on_serialMsgreceived(QByteArray bytes);//串口消息接收槽

    void on_disconnect_clicked();

signals:
    void sendAction(QAction *action);
    void sendCardId(QString tagID);

protected:
    void closeEvent(QCloseEvent *event); //窗口关闭时调用

    int CheckLogin(); //检查是否已经登陆

    void updateSettings();//更新串口配置

private:
    Ui::MainWindow *ui;
    int IsLogin; //登录标志
    userAccount *account = new userAccount();
    QString LoginAccount;
    Settings currentSettings;//存储当前的配置信息
    SerialPortThread *serialPortThread; //自定义串口处理线程
    QSerialPort *serialPort; //串口类对象
    M1356Dll *m1356dll; //13.56M的库对象
    QString tagId;//卡号
};

#endif // MAINWINDOW_H
