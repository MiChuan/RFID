#ifndef RETURNBOOK_H
#define RETURNBOOK_H

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
class ReturnBook;
}

class ReturnBook : public QWidget
{
    Q_OBJECT

public:
    explicit ReturnBook(QWidget *parent = 0, SerialPortThread *serialPortThread = 0);
    ~ReturnBook();
    void handleConnect();//处理连接

private slots:
    void on_cardIdReceived(QString tagId);//卡号接收槽

    void onDecodeFrame(QByteArray bytes);//串口接收到信息后解码输出槽

    void on_getuserId_clicked();

    void on_getbookId_clicked();

    void on_retbook_clicked();

private:
    Ui::ReturnBook *ui;
    M1356Dll *m1356dll;//13.56MHz模块库的类

    SerialPortThread *serialPortThread;//串口线程
    const int Init = 0;
    const int BookCard = 1;
    const int UserCard = 2;
    int flag = 0;
    QDate curDate;
};

#endif // RETURNBOOK_H
