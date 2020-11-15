#ifndef UNREGISTOR_H
#define UNREGISTOR_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "database/dbhelper.h"
#include "tools/tools.h"
#include "tools/serialportthread.h"
#include "inc/m1356dll.h"

namespace Ui {
class Unregistor;
}

class Unregistor : public QWidget
{
    Q_OBJECT

public:
    explicit Unregistor(QWidget *parent = 0, SerialPortThread *serialPortThread = 0);
    ~Unregistor();
    void handleConnect();//处理连接

private slots:
    void on_cardIdReceived(QString tagId);//卡号接收槽

    void onDecodeFrame(QByteArray bytes);//串口接收到信息后解码输出槽

    void on_getId_clicked();

    void on_unreg_clicked();

private:
    void clearData();

private:
    const int InitCount = 18;//2-19块
    Ui::Unregistor *ui;
    M1356Dll *m1356dll;//13.56MHz模块库的类

    SerialPortThread *serialPortThread;//串口线程
    int count; //记录写卡次数
    const int InitOpt = 0;
    const int UnRegOpt = 1;
    int CurOpt;
};

#endif // UNREGISTOR_H
