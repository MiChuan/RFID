#ifndef FOUND_H
#define FOUND_H

#include <QWidget>
#include <QDialog>
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
class Found;
}

class Found : public QWidget
{
    Q_OBJECT

public:
    explicit Found(QWidget *parent = 0, SerialPortThread *serialPortThread = 0);
    ~Found();
    void handleConnect();//处理连接

private slots:
    void on_found_clicked();

    void on_cardIdReceived(QString tagId);//卡号接收槽

    void on_getId_clicked();
    void onDecodeFrame(QByteArray bytes);//串口接收到信息后解码输出槽

private:
    Ui::Found *ui;
    M1356Dll *m1356dll;//13.56MHz模块库的类

    SerialPortThread *serialPortThread;//串口线程
};

#endif // FOUND_H
