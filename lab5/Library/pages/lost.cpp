#include "lost.h"
#include "ui_lost.h"

Lost::Lost(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Lost)
{
    ui->setupUi(this);
}

Lost::~Lost()
{
    delete ui;
}

void Lost::on_lost_clicked()
{
    QString inputNum = ui->num->text();
    if(ui->num->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入正确账号");
        return;
    }
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    QString sql;
    if(ui->method->currentText() == "用户编号"){
        sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1' and STATUS = 'Y';").arg(inputNum);
        query.prepare(sql);
        query.exec();
        if(query.next()){
            sql = QString("UPDATE USER_INFO SET STATUS = 'N' WHERE UID = '%1' and STATUS = 'Y';").arg(inputNum);
            query.prepare(sql);
            query.exec();
            QMessageBox::information(this, "提示", "挂失成功");
            helper->closeDatabase();//关闭数据库
            return;
        }
        QMessageBox::critical(this,"警告","该账户未激活");
    }
    else if(ui->method->currentText() == "用户卡号"){
        sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and STATUS = 'Y';").arg(inputNum);
        query.prepare(sql);
        query.exec();
        if(query.next()){
            sql = QString("UPDATE USER_INFO SET STATUS = 'N' WHERE CID = '%1' and STATUS = 'Y';").arg(inputNum);
            query.prepare(sql);
            query.exec();
            QMessageBox::information(this, "提示", "挂失成功");
            helper->closeDatabase();//关闭数据库
            return;
        }
        QMessageBox::critical(this,"警告","该账户未激活");
    }
}
