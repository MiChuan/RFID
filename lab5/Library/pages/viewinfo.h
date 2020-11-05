#ifndef VIEWINFO_H
#define VIEWINFO_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QModelIndex>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QDate>
#include "database/dbhelper.h"

namespace Ui {
class ViewInfo;
}

class ViewInfo : public QWidget
{
    Q_OBJECT

public:
    explicit ViewInfo(QWidget *parent = 0);
    ~ViewInfo();

private slots:
    void on_userbtn_clicked();

    void on_borrowbtn_clicked();

    void on_bookbtn_clicked();

private:
    Ui::ViewInfo *ui;
    QSqlQueryModel *model;
};

#endif // VIEWINFO_H
