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
    ui->stackedWidget->setCurrentIndex(0);

    this->IsLogin = noLogin;

    this->handConnect();
    this->addWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addWidgets()
{
    Welcome *welcome = new Welcome(this);
    ui->stackedWidget->addWidget(welcome);//0
    ViewInfo *viewInfo = new ViewInfo(this);
    ui->stackedWidget->addWidget(viewInfo);//1
    Lost *lost = new Lost(this);
    ui->stackedWidget->addWidget(lost);//2
    Found *found = new Found(this);
    ui->stackedWidget->addWidget(found);//3
    ViewRecord *viewRecord = new ViewRecord(this);
    ui->stackedWidget->addWidget(viewRecord);//4
}

void MainWindow::handConnect()
{
    connect(ui->about,SIGNAL(triggered(bool)),this,SLOT(About()));
    connect(ui->login,SIGNAL(triggered(bool)),this,SLOT(LoginSys()));
    connect(ui->logout,SIGNAL(triggered(bool)),this,SLOT(ExitSys()));
    connect(ui->mainpage,SIGNAL(triggered(bool)),this,SLOT(viewMainPage()));
    connect(ui->viewall,SIGNAL(triggered(bool)),this,SLOT(viewInfoTable()));
    connect(ui->lost,SIGNAL(triggered(bool)),this,SLOT(lostRecord()));
    connect(ui->found,SIGNAL(triggered(bool)),this,SLOT(foundRecord()));
    connect(ui->viewrecord,SIGNAL(triggered(bool)),this,SLOT(ViewRecordTable()));
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
    login *loginpage = new login(this,&IsLogin);
    loginpage->setWindowTitle("用户登录");
    //loginpage->show();
    loginpage->exec();
    if(this->IsLogin == adminLogin) {
        ui->statusBar->showMessage(tr("管理员登录成功"));
        ui->login->setDisabled(true);
        ui->logout->setEnabled(true);
        ui->admin->setEnabled(true);
    }
    else if(this->IsLogin == userLogin) {
        ui->statusBar->showMessage(tr("用户登录成功"));
        ui->login->setDisabled(true);
        ui->logout->setEnabled(true);
        ui->user->setEnabled(true);
        LoginAccount = account->readConfig("/account/login_account");
        //qDebug() << LoginAccount;
    }
    else {
        ui->statusBar->showMessage(tr("登录失败"));
    }
    delete loginpage;
}

/**
 * @brief MainWindow::ExitSys
 * 退出按钮点击事件
 */
void MainWindow::ExitSys()
{
    this->IsLogin = noLogin;
    ui->login->setEnabled(true);
    ui->logout->setDisabled(true);
    ui->admin->setDisabled(true);//未登陆禁用管理员操作
    ui->user->setDisabled(true);//未登陆禁用用户操作
    ui->statusBar->showMessage(tr("退出登录"));
    account->writeConfig("/account/login_account","init");//重置保存的用户名
    ui->stackedWidget->setCurrentIndex(0);
}

/**
 * @brief MainWindow::viewMainPage
 * 显示welcome页面
 */
void MainWindow::viewMainPage()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->statusBar->showMessage("返回主页");
}

/**
 * @brief MainWindow::viewInfoTable
 * 显示信息表
 */
void MainWindow::viewInfoTable()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->statusBar->showMessage("显示信息表");
}

/**
 * @brief MainWindow::lostRecord
 * 挂失账户
 */
void MainWindow::lostRecord()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->statusBar->showMessage("挂失账户");
}

/**
 * @brief MainWindow::foundRecord
 * 解除挂失账户
 */
void MainWindow::foundRecord()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->statusBar->showMessage("解除挂失账户");
}

/**
 * @brief MainWindow::ViewRecordTable
 * 查询借还书记录
 */
void MainWindow::ViewRecordTable()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->statusBar->showMessage("查询借还书记录");
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
        account->writeConfig("/account/login_account","init");//重置保存的用户名
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
int MainWindow::CheckLogin()
{
    if(IsLogin == noLogin)
        QMessageBox::warning(this,tr("提示"),tr("您尚未登录，无权操作，请先登录！"),QMessageBox::Yes);
    return IsLogin;
}
