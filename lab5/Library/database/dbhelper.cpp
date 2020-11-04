#include "dbhelper.h"
#include <QDebug>
#include <QApplication>
DBHelper *DBHelper::instance = NULL;

DBHelper::DBHelper()
{
    db = QSqlDatabase::addDatabase("QMYSQL");   //数据库驱动类型为MySQL Server
    db.setHostName("47.106.172.87");                        //选择主机
    db.setPort(3306);                                   //选择端口
    db.setDatabaseName("Library");
    db.setUserName("root");                               //登录用户
    db.setPassword("512013");                              //密码
}

bool DBHelper:: openDatabase()
{
   if(!db.open())                                      //打开数据库
    {
        qDebug()<<"database open fail!";
        return false;                                   //打开失败
    }
    else
    {
        qDebug()<<"database open success!";
    }return true;
}

bool DBHelper:: closeDatabase(){
    db.close();
    db.removeDatabase("QMYSQL");

    qDebug()<<"database close success!\n";
    return true;
}

//单例模式，保证只有一个数据库实例
DBHelper *DBHelper::getInstance()
{
    if(instance == NULL)
    {
        instance = new DBHelper();
    }
    return instance;
}
