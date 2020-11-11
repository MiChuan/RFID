#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    ui->connect->setEnabled(true);//启用连接
    ui->disconnect->setDisabled(true);//禁用断开连接
    ui->stackedWidget->setCurrentIndex(0);

    this->IsLogin = noLogin;

    serialPortThread = new SerialPortThread(this);
    serialPort = new QSerialPort(this);
    m1356dll = new M1356Dll();

    this->handConnect();
    this->addWidgets();
    this->fillPortsParameters();
    this->updateSettings();
}

MainWindow::~MainWindow()
{
    delete m1356dll;
    delete serialPortThread;
    delete serialPort;
    delete account;
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
    Found *found = new Found(this,serialPortThread);
    connect(this,SIGNAL(sendCardId(QString)),found,SLOT(on_cardIdReceived(QString)));
    ui->stackedWidget->addWidget(found);//3
    ViewRecord *viewRecord = new ViewRecord(this);
    ui->stackedWidget->addWidget(viewRecord);//4
    Bind *bind = new Bind(this,serialPortThread);
    connect(this,SIGNAL(sendCardId(QString)),bind,SLOT(on_cardIdReceived(QString)));
    ui->stackedWidget->addWidget(bind);//5
    Registor *regist = new Registor(this,serialPortThread);
    connect(this,SIGNAL(sendCardId(QString)),regist,SLOT(on_cardIdReceived(QString)));
    ui->stackedWidget->addWidget(regist);//6
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
    connect(serialPortThread,SIGNAL(sendMsg(char*,int)),this,SLOT(onSendMessage(char*,int)));
    connect(serialPortThread,SIGNAL(wirteMsgError(QString)),this,SLOT(onOperationError(QString)));
    connect(serialPortThread,SIGNAL(receivedMsg(QByteArray)),this,SLOT(on_serialMsgreceived(QByteArray)));
}

/**
 * @brief MainWindow::settings
 * @return  串口配置信息
 * 作为一个常量返回当前串口配置
 */
MainWindow::Settings MainWindow::settings() const
{
    return currentSettings;
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

/**
 * @brief MainWindow::fillPortsParameters
 * 串口参数信息
 */
void MainWindow::fillPortsParameters()
{
    ui->com->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : "N/A")
             << (!manufacturer.isEmpty() ? manufacturer : "N/A")
             << (!serialNumber.isEmpty() ? serialNumber : "N/A")
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : "N/A")
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : "N/A");

        ui->com->addItem(list.first(), list);
    }
    ui->com->addItem(tr("NULL"));

    ui->rate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);//0
    ui->rate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);//1
    ui->rate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->rate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->rate->setCurrentIndex(1);
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
        serialPortThread->stopThread();
        this->Disconnect();
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

/**
 * @brief MainWindow::updateSettings
 * 更新串口配置
 */
void MainWindow::updateSettings()
{
    currentSettings.name = ui->com->currentText();

    currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->rate->itemData(ui->rate->currentIndex()).toInt());
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = QSerialPort::Data8;
    currentSettings.stringDataBits = "8";

    currentSettings.parity = QSerialPort::NoParity;
    currentSettings.stringParity = "None";

    currentSettings.stopBits = QSerialPort::OneStop;
    currentSettings.stringStopBits = "1";

    currentSettings.flowControl = QSerialPort::NoFlowControl;
    currentSettings.stringFlowControl = "None";
}

/**
 * @brief MainWindow::on_connect_clicked
 * 连接串口
 */
void MainWindow::on_connect_clicked()
{
    updateSettings();
    serialPort->setPortName(this->settings().name);//端口名
    serialPort->setBaudRate(this->settings().baudRate);//波特率
    serialPort->setDataBits(this->settings().dataBits);//比特位
    serialPort->setParity(this->settings().parity);//校验位
    serialPort->setStopBits(this->settings().stopBits);//停止位
    serialPort->setFlowControl(this->settings().flowControl);//流控
    if(serialPort->isOpen())
    {
        serialPort->close();
    }
    if(serialPort->open(QIODevice::ReadWrite))
    {
        ui->connect->setDisabled(true);//连接后禁用连接按钮
        ui->disconnect->setEnabled(true);
        serialPortThread->setSerialPort(serialPort);
        serialPortThread->setRetryTimes(RETRY_20);
        serialPortThread->start();
        uint16 frameLen;
        uint8 data[1];
        uint8 *p;
        data[0] = RC632_WORK_MODEL_15693;
        p = m1356dll->RC632_SendCmdReq(RC632_CMD_CONFIG_ISOTYPE,data,1);//设置为15693类型
        frameLen = BUILD_UINT16(p[0], p[1]);
        serialPortThread->writeData((char *)(p + 2 ),frameLen);
        ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6 success").arg(
                                       this->settings().name).arg(
                                       this->settings().baudRate).arg(
                                       this->settings().dataBits).arg(
                                       this->settings().parity).arg(
                                       this->settings().stopBits).arg(
                                       this->settings().flowControl));
    }else{
        QMessageBox::critical(this, tr("Error"), serialPort->errorString());
        ui->statusBar->showMessage(tr("Open error"));
    }
}

/**
 * @brief MainWindow::Disconnect
 * 与读卡器断开连接，关闭串口，停止读写线程
 */
void MainWindow::Disconnect()
{
    if(serialPort->isOpen())
    {
        serialPort->close();
        ui->connect->setEnabled(true);//启用连接
        ui->disconnect->setDisabled(true);//禁用断开连接
        ui->statusBar->showMessage(tr("Close Success"));
    }
}

/**
 * @brief MainWindow::onOperationError
 * @param msg 发送的命令
 * 当串口写入数据无响应时调用该方法
 */
void MainWindow::onOperationError(QString msg)
{
    QMessageBox::warning(this,tr("温馨提示"),msg,QMessageBox::Yes);
}
//显示发送消息
void MainWindow::onSendMessage(char *data, int frameLen)
{
    Q_UNUSED(data);
    Q_UNUSED(frameLen);
    //这里没用到,这里就是提供一个接口
}
/**
 * @brief MainWindow::on_serialMsgreceived
 * @param bytes 接收到的数据字节数组
 * 串口接收消息后的处理槽函数
 */
void MainWindow::on_serialMsgreceived(QByteArray bytes){
    M1356_RspFrame_t frame = m1356dll->M1356_RspFrameConstructor(bytes);
    if(frame.status.left(2) == "00")
    {
        if(frame.cmd.remove(" ") == "0010")//寻卡0x1000
        {
            tagId = frame.vdata.remove(" ");
            emit sendCardId(tagId);
        }
    }
    else
    {
        if(frame.cmd.remove(" ") == "0010")//寻卡
        {
            QMessageBox::warning(this,tr("温馨提示"),tr("寻卡失败，请调整卡与读卡器的距离后再试！"),QMessageBox::Yes);
        }
    }
}

void MainWindow::on_disconnect_clicked()
{
    this->Disconnect();
}
