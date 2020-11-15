#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>
#include "tools/useraccount.h"
#include "database/dbhelper.h"

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0,int *IsLogin = nullptr);
    ~login();
    void remindReturnBook(QString UID);

    const int noLogin = 0;
    const int adminLogin = 1;
    const int userLogin = 2;

private slots:
    void on_login_btn_clicked();

private:
    Ui::login *ui;
    QString user;
    QString pwd;
    int remindTime;
    int *LogStatus;
    userAccount *account = new userAccount();
    QDate curDate;
};

#endif // LOGIN_H
