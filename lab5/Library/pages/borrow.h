#ifndef BORROW_H
#define BORROW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>
#include "database/dbhelper.h"
#include "tools/tools.h"
#include "tools/serialportthread.h"
#include "inc/m1356dll.h"

namespace Ui {
class Borrow;
}

class Borrow : public QWidget
{
    Q_OBJECT

public:
    explicit Borrow(QWidget *parent = 0, SerialPortThread *serialPortThread = 0);
    ~Borrow();
    void handleConnect();//处理连接

private slots:
    void on_cardIdReceived(QString tagId);//卡号接收槽

    void onDecodeFrame(QByteArray bytes);//串口接收到信息后解码输出槽

    void on_getuserId_clicked();

    void on_getbookId_clicked();

    void on_borrow_clicked();

private:
    Ui::Borrow *ui;
    M1356Dll *m1356dll;//13.56MHz模块库的类

    SerialPortThread *serialPortThread;//串口线程
    const int Init = 0;
    const int BookCard = 1;
    const int UserCard = 2;
    int flag = 0;
    QDate curDate;
    const int InitOpt = 0;
    const int BorOpt = 1;
    int CurOpt;
};

#endif // BORROW_H
