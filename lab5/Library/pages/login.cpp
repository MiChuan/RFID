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
}

login::~login()
{
    delete ui;
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
        } else {
            QMessageBox::critical(this,"警告","用户不存在");
            return;
        }
    }

    this->close();
    return;
}
