#ifndef LOST_H
#define LOST_H

#include <QWidget>
#include <QDialog>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "database/dbhelper.h"

namespace Ui {
class Lost;
}

class Lost : public QWidget
{
    Q_OBJECT

public:
    explicit Lost(QWidget *parent = 0);
    ~Lost();

private slots:
    void on_lost_clicked();

private:
    Ui::Lost *ui;
};

#endif // LOST_H
