#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QLabel>
#include <QDebug>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMessageBox>
#include "pages/pages.h"
//#include "tools/uhf_thread.h"
#include "database/dbhelper.h"
#include "tools/tools.h"

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

private slots:
    void About(); //关于
    void LoginSys(); //登陆
    void ExitSys(); //退出
    void viewMainPage(); //显示主页

signals:
    void sendAction(QAction *action);

protected:
    void closeEvent(QCloseEvent *event); //窗口关闭时调用

    int CheckLogin(); //检查是否已经登陆

private:
    Ui::MainWindow *ui;
    int IsLogin; //登录标志
};

#endif // MAINWINDOW_H
