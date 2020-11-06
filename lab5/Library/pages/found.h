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

namespace Ui {
class Found;
}

class Found : public QWidget
{
    Q_OBJECT

public:
    explicit Found(QWidget *parent = 0);
    ~Found();

private slots:
    void on_found_clicked();

private:
    Ui::Found *ui;
};

#endif // FOUND_H
