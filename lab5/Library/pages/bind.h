#ifndef BIND_H
#define BIND_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include "database/dbhelper.h"
#include "tools/tools.h"
#include "tools/serialportthread.h"
#include "inc/m1356dll.h"

namespace Ui {
class Bind;
}

class Bind : public QWidget
{
    Q_OBJECT

public:
    explicit Bind(QWidget *parent = 0, SerialPortThread *serialPortThread = 0);
    ~Bind();
    void handleConnect();//处理连接

private slots:
    void on_cardIdReceived(QString tagId);//卡号接收槽

    void onDecodeFrame(QByteArray bytes);//串口接收到信息后解码输出槽

    void on_bindcard_clicked();

    void on_getId_clicked();

private:
    Ui::Bind *ui;
    M1356Dll *m1356dll;//13.56MHz模块库的类

    SerialPortThread *serialPortThread;//串口线程
    const int InitOpt = 0;
    const int BindOpt = 1;
    int CurOpt;
};

#endif // BIND_H
