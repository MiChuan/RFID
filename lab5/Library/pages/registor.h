#ifndef REGISTOR_H
#define REGISTOR_H

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
class Registor;
}

class Registor : public QWidget
{
    Q_OBJECT

public:
    explicit Registor(QWidget *parent = 0, SerialPortThread *serialPortThread = 0);
    ~Registor();
    void handleConnect();//处理连接

private slots:
    void on_cardIdReceived(QString tagId);//卡号接收槽

    void onDecodeFrame(QByteArray bytes);//串口接收到信息后解码输出槽

    void on_getId_clicked();

    void on_reg_clicked();

private:
    Ui::Registor *ui;
    M1356Dll *m1356dll;//13.56MHz模块库的类

    SerialPortThread *serialPortThread;//串口线程
};

#endif // REGISTOR_H
