#include "login.h"
#include "ui_login.h"
#include <QMessageBox>

login::login(QWidget *parent,int *IsLogin) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height()); //禁止拖动窗口
    this->setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint); // 禁止最大化按钮
    ui->password->setEchoMode(QLineEdit::Password);//密码输入框设置
    QWidget::setTabOrder(ui->username, ui->password);
    QWidget::setTabOrder(ui->password, ui->login_btn);
    this->LogStatus = IsLogin;
    this->user = account->readConfig("/account/first_account");
    this->pwd = account->readConfig("/password/first_password");
    this->remindTime = account->readConfig("/remind/time").toInt();
    curDate = QDate::currentDate();
}

login::~login()
{
    delete ui;
}

/**
 * @brief login::remainReturnBook
 * 提醒还书
 */
void login::remindReturnBook(QString UID)
{
    QString DDL;
    QString BID;
    QString  ReTime;
    QDate ddline;
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query,query1;
    helper->openDatabase();//链接数据库
    QString sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1';").arg(UID);
    query.prepare(sql);
    query.exec();
    if(query.next() && query.value("BNUM").toInt() < 5){//可借数量 4,3,2,1,0
        int count = 5 - query.value("BNUM").toInt();//已借数量 1,2,3,4,5
        QString BNUM = query.value("BNUM").toString();
        qDebug() << "UID: " << UID << " BNUM: " << BNUM;//debug
        sql = QString("SELECT * FROM RECORD WHERE UID = '%1';").arg(UID);
        query.prepare(sql);
        query.exec();
        if(query.last()){//定位到最新的借还记录
            BID = query.value("BID").toString();
            sql = QString("SELECT * FROM BOOK_INFO WHERE BID = '%1';").arg(BID);
            query1.prepare(sql);
            query1.exec();
            if(query1.next() && query1.value("BSTATUS").toString() == "N"){//最新记录的书未还
                count--;
                DDL = query.value("DDL").toString();//截止日期
                ddline = QDate::fromString(DDL,"yyyy-MM-dd");
                qint64 gap = curDate.daysTo(ddline);//今天到截止日期的时间
                ReTime = QString::number(gap);
                qDebug() << "BID: " << BID << " DDL: " << DDL << "gap: " << gap;//debug
                if(gap <= this->remindTime){
                    QString text = tr("书标签号:%1\r\n截止日期:%2\r\n剩余时间:%3天")
                            .arg(BID)
                            .arg(DDL)
                            .arg(ReTime);
                    QMessageBox::warning(this,tr("还书提示"),text,QMessageBox::Yes);
                }
            }
            //检查目前所有借的书
            while(query.previous() && count > 0){//向前一条记录
                BID = query.value("BID").toString();
                sql = QString("SELECT * FROM BOOK_INFO WHERE BID = '%1';").arg(BID);
                query1.prepare(sql);
                query1.exec();
                if(query1.value("BSTATUS").toString() == "N"){//记录的书未还
                    count--;
                    DDL = query.value("DDL").toString();//截止日期
                    ddline = QDate::fromString(DDL,"yyyy-MM-dd");
                    qint64 gap = curDate.daysTo(ddline);//今天到截止日期的时间
                    ReTime = QString::number(gap);
                    if(gap <= this->remindTime){
                        QString text = tr("书标签号:%1\r\n截止日期:%2\r\n剩余时间:%3")
                                .arg(BID)
                                .arg(DDL)
                                .arg(ReTime);
                        QMessageBox::warning(this,tr("还书提示"),text,QMessageBox::Yes);
                    }
                }
            }
        }
    }
    helper->closeDatabase();//关闭数据库
}

/**
 * @brief login::on_login_btn_clicked
 * 登陆按钮点击事件
 */
void login::on_login_btn_clicked()
{
    /*用户名密码不为空*/
    if(ui->username->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入用户名后再登录");
        return;
    }
    if(ui->password->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入密码后再登录");
        return;
    }
    QString inputName = ui->username->text();
    QString inputPwd = account->GetMD5String(ui->password->text());

    if(ui->who->currentText() == "管理员"){
        /*用户存在*/
        if(inputName != this->user){
            QMessageBox::critical(this,"警告","用户不存在");
            return;
        }

        /*检查密码*/
        if(inputPwd != this->pwd){
             QMessageBox::critical(this,"警告","密码错误");
             return;
        }
        *(this->LogStatus) = adminLogin;
    }
    else if(ui->who->currentText() == "用户"){
        DBHelper *helper = DBHelper::getInstance();
        QSqlQuery query;
        helper->openDatabase();//链接数据库
        QString sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1';").arg(inputName);
        query.prepare(sql);
        query.exec();
        helper->closeDatabase();//关闭数据库
        if(query.next()){
            if(query.value("PASSWORD").toString() != inputPwd){
                QMessageBox::critical(this,"警告","密码错误");
                return;
            }
            if(query.value("USTATUS").toString() != "Y"){
                QMessageBox::critical(this,"警告","账户未激活");
                return;
            }
            *(this->LogStatus) = userLogin;
            account->writeConfig("/account/login_account",inputName);//保存登录用户名
            this->remindReturnBook(inputName);//提醒还书
        } else {
            QMessageBox::critical(this,"警告","用户不存在");
            return;
        }
    }

    this->close();
    return;
}
