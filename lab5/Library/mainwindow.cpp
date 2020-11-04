#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "inc/m1356dll.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height()); //禁止拖动窗口
    this->setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint); // 禁止最大化按钮
    ui->logout->setDisabled(true);//未登陆禁用退出功能
    ui->admin->setDisabled(true);//未登陆禁用管理员操作
    ui->user->setDisabled(true);//未登陆禁用用户操作

    this->IsLogin = false;

    this->handConnect();
    this->addWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addWidgets()
{

}

void MainWindow::handConnect()
{
    connect(ui->about,SIGNAL(triggered(bool)),this,SLOT(About()));
}

/**
 * @brief MainWindow::About
 * 关于按钮点击事件
 */
void MainWindow::About()
{
    QString text = tr("软件版本:%1\r\n作者:%2\r\n描述:%3")
            .arg(CURRENT_VERSION)
            .arg(tr("huhan_h@163.com"))
            .arg(tr("基于射频识别技术，利用MySQL进行数据保存，利用13.56MHz的标签和读写器进行书籍与读者卡模拟"));
    QMessageBox::information(this,tr("帮助"),text,QMessageBox::Yes);
}

/**
 * @brief MainWindow::LoginSys
 *登陆按钮点击事件
 */
void MainWindow::LoginSys()
{

}

/**
 * @brief MainWindow::ExitSys
 * 退出按钮点击事件
 */
void MainWindow::ExitSys()
{

}

//窗口关闭响应事件
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox message;
    message.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    message.setWindowTitle(tr("温馨提示"));
    message.setIcon(QMessageBox::Warning);
    message.setText(tr("您确定要退出吗？"));
    if (message.exec()==QMessageBox::Yes)
    {
        //DB_Close();
    }
    else
    {
        event->ignore();
    }
}

/**
 * @brief MainWindow::CheckLogin
 * @return 如果已经登陆返回ture,否则false
 * 监测是否登陆
 */
bool MainWindow::CheckLogin()
{
    if(!IsLogin)
        QMessageBox::warning(this,tr("提示"),tr("您尚未登录，无权操作，请先登录！"),QMessageBox::Yes);
    return IsLogin;
}
