#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "recordtablemodel.h"
#include <QMessageBox>
#include <QDebug>
/**************************************
 *作者: jianghj@up-tech.com
 *日期: 2016-09-30
 *描述: 125K演示程序主要代码,此处模拟的人员通道,进出需要刷卡,
 *      125K在实际应用中主要也是这个功能,比如小区的门禁卡.
 *      注意:人为主动刷卡,2.4G是被动刷卡
***************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->fillPortsParameters(ui->baudRateBox);//波特率填充
    this->serialPort = new QSerialPort(this);
    db = new Database(this);
    model = new RecordTableModel(this);
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    intValidator = new QIntValidator(0, 4000000,this);
    ui->btn_connect->setEnabled(true);
    ui->btn_refresh->setEnabled(true);
    ui->btn_disconnect->setEnabled(false);
    this->on_btn_refresh_clicked();
    m125dll = new M125Dll();
    //关联相关槽函数
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
}

MainWindow::~MainWindow()
{
    model->submitAll();
    delete model;
    delete db;
    delete m125dll;
    delete intValidator;
    delete serialPort;
    delete ui;
}
//Baudrate parameter init
void MainWindow::fillPortsParameters(QComboBox *box)
{
    box->clear();
    box->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    box->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    box->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    box->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    box->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    box->addItem(tr("Custom"));
}
/**
 * @brief MainWindow::on_btn_connect_clicked
 * 连接串口
 */
void MainWindow::on_btn_connect_clicked()
{
    QString name = ui->serialNameBox->currentText();
    QString baud = ui->baudRateBox->currentText().trimmed();
    if(baud.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), "波特率输入错误！");
        return ;
    }
    serialPort->setPortName(name);
    serialPort->setBaudRate(baud.toInt(),QSerialPort::AllDirections);
    if (serialPort->open(QIODevice::ReadWrite)) {
        ui->btn_connect->setEnabled(false);
        ui->btn_disconnect->setEnabled(true);
        ui->btn_refresh->setEnabled(false);
    } else {
        ui->btn_connect->setEnabled(true);
        ui->btn_refresh->setEnabled(true);
        ui->btn_disconnect->setEnabled(false);
        QMessageBox::warning(this,tr("提示"),tr("初始化%1失败！请检查串口是否已经被占用？").arg(name),QMessageBox::Yes);
    }
}
/**
 * @brief MainWindow::on_btn_disconnect_clicked
 * 断开连接
 */
void MainWindow::on_btn_disconnect_clicked()
{
    if(!serialPort->isOpen())
        return ;
    serialPort->close();
    ui->btn_connect->setEnabled(true);
    ui->btn_refresh->setEnabled(true);
    ui->btn_disconnect->setEnabled(false);
}
/**
 * @brief MainWindow::on_btn_refresh_clicked
 * 刷新按钮点击事件
 */
void MainWindow::on_btn_refresh_clicked()
{
    QStringList list = getSerialName();
    ui->serialNameBox->clear();
    ui->serialNameBox->addItems(list);
}
/**
 * @brief MainWindow::checkCustomBaudRatePolicy
 * @param idx combox被选中的索引值
 * 设置自定义波特率
 */
void MainWindow::checkCustomBaudRatePolicy(int idx)
{
    QComboBox *box = dynamic_cast<QComboBox*>(QObject::sender());
    bool isCustomBaudRate = !box->itemData(idx).isValid();
    box->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        box->clearEditText();
        box->setValidator(intValidator);
    }
}

/**
 * @brief MainWindow::readData
 * 读取串口数据
 */
void MainWindow::readData()
{    
    if(serialPort->bytesAvailable() < 5)
        return;
    QByteArray data = serialPort->readAll();
    if(m125dll->LF125K_FrameAnalysis((uint8 *)(data.data())) == 0)
    {
        QString tagId = CharStringtoHexString(tr(" "),data.data(),data.length());//获取卡号
        QString time = CurrentDateTime();//获取当前日期时间
        int index = model->findRecord(tagId);//查询指定卡号记录
        if(index >= 0 )
        {//存在记录
            QString recordTime = model->record(index).value(1).toString();//从记录读取刷卡时间

            QDateTime reTime = QDateTime::fromString(recordTime, "yyyy-MM-dd hh:mm:ss");//刷卡时间 
            QDateTime curTime = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");//当前时间

            QString status = model->record(index).value(2).toString();//读取进出状态
            int times = model->record(index).value(4).toInt();//读取刷卡次数
            if(status == tr("进"))
                model->addRecord(tagId, time, tr("出"), m_time.addSecs(reTime.secsTo(curTime)).toString("hh:mm:ss"), times+1);
            else
                model->addRecord(tagId, time, tr("进"), "00:00:00", 1);
        }
        else {
            model->addRecord(tagId, time, tr("进"), "00:00:00", 1);
        }
    }
}
/**
 * @brief MainWindow::handleError
 * @param error SerialPortError枚举类,详细请看SerialPortError的定义
 * 处理错误信息
 */
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serialPort->errorString());
        this->on_btn_disconnect_clicked();
    }
}
