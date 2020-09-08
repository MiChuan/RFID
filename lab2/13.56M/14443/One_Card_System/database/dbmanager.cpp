#include "dbmanager.h"
/**************************************
 *作者: jianghj@up-tech.com
 *日期: 2016-09-20
 *描述: 数据库管理者,用于初始化数据库,关闭数据库等
***************************************/
DBManager::DBManager(QObject *parent) : QObject(parent)
{
    if(this->createDB(DATABASE_NAME))
    {
        if(!this->tableExist(TABLE_NAME_PERSON))
        {
            PersonTableModel p;
            p.createTable();
        }
        if(!this->tableExist(TABLE_NAME_RECORD))
        {
            RecordTableModel r;
            r.createTable();
        }
        if(!this->tableExist(TABLE_NAME_REGISTER))
        {
            RegisterTableModel r;
            r.createTable();
        }
        if(!this->tableExist(TABLE_NAME_WRITEOFF))
        {
            WriteOffTableModel w;
            w.createTable();
        }
        if(!this->tableExist(TABLE_NAME_ADMIN))
        {
            AdminTableModel a;
            a.createTable();
        }
        if(!this->tableExist(TABLE_NAME_RECHARGE))
        {
            RechargeTableModel r;
            r.createTable();
        }
    }
}
/**
 * @brief RecordTableModel::createDB
 * @param dbName 数据库名称
 * 用于创建数据库
 */
bool DBManager::createDB(const QString &dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if(db.open()){
        qDebug()<<dbName<<" create success";
        return true;
    }
    else{
        qDebug()<<dbName<<" create failed!";
        return false;
    }
}
/**
 * @brief DBManager::tableExist
 * @param tableName 表名
 * @return  如果存在返回true，否则返回false
 * 用于判断表是否存在
 */
bool DBManager::tableExist(const QString &tableName)
{
    int count = 0 ;
    QString sqlText = QObject::tr("select count(*) from sqlite_master where type='table' and name='%1'").arg(tableName);
    QSqlQuery query;
    query.exec(sqlText);
    if(query.next())
        count = query.value(0).toInt();
    if(count > 0)
        return true;
    else
        return false;
}
/**
 * @brief DBManager::getTableNames
 * @return  数据库表集合
 * 获取数据库所有表的表名
 */
QStringList DBManager::getTableNames()
{
    QStringList tables;
    QString sqlText = QObject::tr("select name from sqlite_master where type='table' order by name;");
    QSqlQuery query;
    query.exec(sqlText);
    while(query.next())
    {
        tables << query.value(0).toString();
    }
    return tables;
}
/**
 * @brief DBManager::dbClose
 * 关闭数据库
 */
void DBManager::dbClose()
{
    if(db.isOpen())
        db.close();
}

static DBManager *dbManager;
/**
 * @brief DB_Init
 * 创建数据库管理对象实体，初始化数据库
 */
void DB_Init()
{
    dbManager = new DBManager();
}
/**
 * @brief DB_Close
 * 关闭数据库
 */
void DB_Close()
{
    dbManager->dbClose();
}
/**
 * @brief getTableNames
 * @return QStringList型的表名集合
 * 获取数据库所有表的表名
 */
QStringList getTableNames()
{
    return dbManager->getTableNames();
}
