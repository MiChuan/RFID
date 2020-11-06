#include "found.h"
#include "ui_found.h"

Found::Found(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Found)
{
    ui->setupUi(this);
}

Found::~Found()
{
    delete ui;
}

void Found::on_found_clicked()
{
    QString inputNum = ui->num->text();
    if(ui->num->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入正确账号");
        return;
    }
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    QString sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1' and STATUS = 'N';").arg(inputNum);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        sql = QString("UPDATE USER_INFO SET STATUS = 'Y' WHERE UID = '%1' and STATUS = 'N';").arg(inputNum);
        query.prepare(sql);
        query.exec();
        QMessageBox::information(this, "提示", "解除挂失成功");
        helper->closeDatabase();//关闭数据库
        return;
    }
    QMessageBox::critical(this,"警告","需要挂失的账户不存在");
}
