#include "viewrecord.h"
#include "ui_viewrecord.h"

ViewRecord::ViewRecord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewRecord)
{
    ui->setupUi(this);
    model = new QSqlQueryModel(ui->tableView);/**将数据模型与QTableView绑定**/
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//只读
}

ViewRecord::~ViewRecord()
{
    delete ui;
}

void ViewRecord::on_borrowbtn_clicked()
{
    LoginAccount = account->readConfig("/account/login_account");
    qDebug() << "ViewRecord: " <<LoginAccount;
    DBHelper *helper = DBHelper::getInstance();
    helper->openDatabase();
    QString sql = QString("select USER_INFO.UID as '用户编号',"
            "CID as '用户卡号',"
            "UNAME as '用户姓名',"
            "TELEPHONE as '电话',"
            "BOOK_INFO.BID as '书籍卡号',"
            "ISBN as '国际统一书号',"
            "BNAME as '书名',"
            "BTIME as '借书时间',"
            "DDL as '截止日期',"
            "RTIME as '还书时间'"
            " from USER_INFO,RECORD,BOOK_INFO"
            " where USER_INFO.UID = RECORD.UID"
            " and RECORD.BID = BOOK_INFO.BID"
            " and USER_INFO.UID = '%1';").arg(LoginAccount);
    model->setQuery(sql);
    ui->tableView->setModel(model);
    helper->closeDatabase();//关闭数据库
}
