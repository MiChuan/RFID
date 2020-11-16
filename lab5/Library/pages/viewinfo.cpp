#include "viewinfo.h"
#include "ui_viewinfo.h"

ViewInfo::ViewInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewInfo)
{
    ui->setupUi(this);
    model = new QSqlQueryModel(ui->tableView);/**将数据模型与QTableView绑定**/
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//只读
    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//列宽等宽自适应
}

ViewInfo::~ViewInfo()
{
    delete ui;
}

void ViewInfo::on_userbtn_clicked()
{
    DBHelper *helper = DBHelper::getInstance();
    helper->openDatabase();
    QSqlQuery query;
    bool ret = query.exec("select * from USER_INFO;");
    while(query.next()){
        qDebug() << query.value(0).toString() << "|"
                 << query.value(1).toString() << "|"
                 << query.value(2).toString() << "|"
                 << query.value(3).toString() << "|"
                 << query.value(4).toString() << "|"
                 << query.value(5).toString() << "|"
                 << query.value(6).toString();
    }
    if(ret){
        qDebug()<<"select from USER_INFO success";
    } else {
        qDebug()<<query.lastError().text();
    }

    model->setQuery("select UID as '用户编号',"
                    "CID as '卡号',"
                    "UNAME as '姓名',"
                    "TELEPHONE as '电话',"
                    "USTATUS as '账户状态',"
                    "BNUM as '可借数量'"
                    " from USER_INFO;");
    ui->tableView->setModel(model);
    helper->closeDatabase();//关闭数据库
}

void ViewInfo::on_borrowbtn_clicked()
{
    DBHelper *helper = DBHelper::getInstance();
    helper->openDatabase();
    QSqlQuery query;
    bool ret = query.exec("select * from RECORD;");
    while(query.next()){
        qDebug() << query.value(0).toString() << "|"
                 << query.value(1).toString() << "|"
                 << query.value(2).toString() << "|"
                 << query.value(3).toString() << "|"
                 << query.value(4).toString();
    }
    if(ret){
        qDebug()<<"select from RECORD success";
    } else {
        qDebug()<<query.lastError().text();
    }

    model->setQuery("select UID as '用户编号',"
                    "BID as '书籍卡号',"
                    "BTIME as '借书时间',"
                    "DDL as '截止日期',"
                    "RTIME as '还书时间'"
                    " from RECORD;");
    ui->tableView->setModel(model);
    helper->closeDatabase();//关闭数据库
}

void ViewInfo::on_bookbtn_clicked()
{
    DBHelper *helper = DBHelper::getInstance();
    helper->openDatabase();
    QSqlQuery query;
    bool ret = query.exec("select * from BOOK_INFO;");
    while(query.next()){
        qDebug() << query.value(0).toString() << "|"
                 << query.value(1).toString() << "|"
                 << query.value(2).toString() << "|"
                 << query.value(3).toString() << "|"
                 << query.value(4).toString() << "|"
                 << query.value(5).toString() << "|"
                 << query.value(6).toString();
    }
    if(ret){
        qDebug()<<"select from BOOK_INFO success";
    } else {
        qDebug()<<query.lastError().text();
    }

    model->setQuery("select BID as '书籍卡号',"
                    "ISBN as '国际统一书号',"
                    "BNAME as '书名',"
                    "AUTHOR as '作者',"
                    "PRESS as '出版社',"
                    "PTIME as '出版时间',"
                    "DURATION as '可借时长',"
                    "BSTATUS as '是否可借'"
                    " from BOOK_INFO;");
    ui->tableView->setModel(model);
    helper->closeDatabase();//关闭数据库
}
