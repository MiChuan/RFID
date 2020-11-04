#ifndef DBHELPER_H
#define DBHELPER_H
#include <QSqlDatabase>
#include <QtSql/QSqlTableModel>


class DBHelper
{
public:
    static DBHelper *getInstance();
    bool openDatabase();
    bool closeDatabase();

private:
    static DBHelper *instance;
    DBHelper();
    QSqlDatabase db;
};
#endif // DBHELPER_H
