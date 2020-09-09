#include "recordtablemodel.h"
/**************************************
 *作者: jianghj@up-tech.com
 *日期: 2016-09-20
 *描述: 记录表的model
***************************************/
RecordTableModel::RecordTableModel(QObject *parent) : QSqlTableModel(parent)
{
    tableName = "person_Records";
    //header<<QObject::trUtf8("卡号")<< QObject::trUtf8("时间")<< QObject::trUtf8("状态");
    header<<QObject::trUtf8("卡号")<< QObject::trUtf8("刷卡时间")<< QObject::trUtf8("状态")<<QObject::trUtf8("停留时间")<<QObject::trUtf8("刷卡次数");
    if(!this->tableExist(tableName))
        this->createTable();
    this->setTable(tableName);
    this->select();
}
/**
 * @brief DBManager::tableExist
 * @param tableName 表名
 * @return  如果存在返回true，否则返回false
 * 用于判断表是否存在
 */
bool RecordTableModel::tableExist(const QString &tableName)
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
 * @brief RecordTableModel::createTable
 * @param tableName 数据块表名称
 * 用于创建刷卡记录表
 */
void RecordTableModel::createTable()
{
    QSqlQuery query;
    QString str;
    str  = tr("create table ") + tableName + tr(" ( ");
    str += header.at(0) + tr(" varchar not null, ");//ID
    str += header.at(1) + tr(" varchar, ");//Time
    str += header.at(2) + tr(" varchar, ");//Status
    str += header.at(3) + tr(" varchar, ");//Duration
    str += header.at(4) + tr(" varchar) ");//Times
    qDebug()<<"Sql: " << str.toUtf8().data();
    bool ret = query.exec(str);
    if(ret == true){
        qDebug()<<tableName<<QObject::tr(" table create success");
    }
    else{
        qDebug()<<tableName<<QObject::tr(" table create failed");
    }
}

/**
 * @brief RecordTableModel::findRecord
 * @param tagId 卡号
 * @return 如果成功返回该卡所在的行,否则返回-1
 * 根据卡号查找记录
 */
int RecordTableModel::findRecord(const QString &tagId)
{
    for(int row=rowCount()-1; 0 <= row; --row){//从最新的记录开始反向查找
        if(data(index(row, 0)).toString() == tagId)
            return row;
    }
    return -1;
}
/**
 * @brief RecordTableModel::deleteByTagId
 * @param tagId 卡号
 * @return 如果成功返回true，否则false
 * 根据卡号删除记录
 */
bool RecordTableModel::deleteByTagId(const QString &tagId)
{
    for(int row=0; row<rowCount(); row++){
        if(data(index(row, 0)).toString() == tagId)
            removeRow(row);
    }
    return submitAll();
}
/**
 * @brief RecordTableModel::updateRecord
 * @param row 待更新记录所在行
 * @param tagId 卡号
 * @param time 当前的时间
 * @param status 当前的状态
 * @param duration 停留时间
 * @param times 刷卡次数
 * 更新记录
 */
void RecordTableModel::updateRecord(int row,QString &tagId, QString &time, QString status, QString duration, int times)
{
    QSqlRecord record;//也可以直接使用sql语句进行,但是sql语句更容易写错
    record.append(QSqlField(header.at(0), QVariant::String));
    record.append(QSqlField(header.at(1), QVariant::String));
    record.append(QSqlField(header.at(2), QVariant::String));
    record.append(QSqlField(header.at(3), QVariant::String));
    record.append(QSqlField(header.at(4), QVariant::String));
    record.setValue(0, QVariant(tagId));
    record.setValue(1, QVariant(time));
    record.setValue(2, QVariant(status));
    record.setValue(3, QVariant(duration));
    record.setValue(4, QVariant(times));//利用QVariant构造函数从int构造对象
    //setData(index(row,0),tagId);
    //setData(index(row,1),time);
    //setData(index(row,2),status);
    this->insertRecord(row, record);
}
/**
 * @brief RecordTableModel::addRecord
 * @param tagId 卡号
 * @param time 当前的时间
 * @param status 当前的状态
 * @param duration 停留时间
 * @param times 刷卡次数
 * 添加记录
 */
int RecordTableModel::addRecord(QString &tagId, QString &time, QString status, QString duration, int times)
{
    QSqlRecord record;//也可以直接使用sql语句进行,但是sql语句更容易写错
    record.append(QSqlField(header.at(0), QVariant::String));
    record.append(QSqlField(header.at(1), QVariant::String));
    record.append(QSqlField(header.at(2), QVariant::String));
    record.append(QSqlField(header.at(3), QVariant::String));
    record.append(QSqlField(header.at(4), QVariant::String));
    record.setValue(0, QVariant(tagId));
    record.setValue(1, QVariant(time));
    record.setValue(2, QVariant(status));
    record.setValue(3, QVariant(duration));
    record.setValue(4, QVariant(times));
    insertRecord(-1, record);//插入表末尾
    return rowCount();
}
/**
 * @brief RecordTableModel::clearRecord
 * @return 如果成功返回true，否则false
 * 删除数据库记录
 */
bool RecordTableModel::clearRecord()
{
    for(int row=0; row < rowCount(); ++row){
        removeRow(row);
    }
    return submitAll();
}
