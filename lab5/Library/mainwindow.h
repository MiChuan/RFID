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

private slots:
    void About(); //关于
    void LoginSys(); //登陆
    void ExitSys(); //退出

signals:
    void sendAction(QAction *action);

protected:
    void closeEvent(QCloseEvent *event); //窗口关闭时调用

    bool CheckLogin(); //检查是否已经登陆

private:
    Ui::MainWindow *ui;
    bool IsLogin; //登录标志
};

#endif // MAINWINDOW_H
