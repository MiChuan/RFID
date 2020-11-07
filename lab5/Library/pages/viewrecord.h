#ifndef VIEWRECORD_H
#define VIEWRECORD_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QModelIndex>
#include <QDebug>
#include <QString>
#include "database/dbhelper.h"
#include "tools/useraccount.h"

namespace Ui {
class ViewRecord;
}

class ViewRecord : public QWidget
{
    Q_OBJECT

public:
    explicit ViewRecord(QWidget *parent = 0);
    ~ViewRecord();

private slots:
    void on_borrowbtn_clicked();

private:
    Ui::ViewRecord *ui;
    userAccount *account = new userAccount();
    QString LoginAccount;
    QSqlQueryModel *model;
};

#endif // VIEWRECORD_H
