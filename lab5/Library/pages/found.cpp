#include "found.h"
#include "ui_found.h"

Found::Found(QWidget *parent, SerialPortThread *serialPortThread) :
    QWidget(parent),
    ui(new Ui::Found)
{
    ui->setupUi(this);
    ui->found->setDisabled(true);
    this->serialPortThread = serialPortThread;
    m1356dll = new M1356Dll();
}

Found::~Found()
{
    delete ui;
}

/**
 * @brief Found::handleConnect
 * 处理信号与插槽的连接关系
 */
void Found::handleConnect()
{
    connect(serialPortThread,SIGNAL(receivedMsg(QByteArray)),this,SLOT(onDecodeFrame(QByteArray)));
}

/**
 * @brief Found::on_found_clicked
 * 写入数据到数据库
 */
void Found::on_found_clicked()
{
    QString inputUser = ui->userId->text();
    QString inputCard = ui->cardId->text();
    if(ui->userId->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入正确账号");
        return;
    } else if(ui->cardId->text().isEmpty()){
        QMessageBox::critical(this,"警告","请读取卡号");
        return;
    }
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    QString sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1' and USTATUS = 'N';").arg(inputUser);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        sql = QString("UPDATE USER_INFO SET USTATUS = 'Y', CID = '%1' WHERE UID = '%2' and USTATUS = 'N';").arg(inputCard).arg(inputUser);
        query.prepare(sql);
        query.exec();
        QMessageBox::information(this, "提示", "解除挂失成功");
        helper->closeDatabase();//关闭数据库
        return;
    }
    QMessageBox::critical(this,"警告","需要解除挂失的账户不存在");
}

/**
 * @brief Found::on_cardIdReceived
 * @param tagId 卡号
 * 接收到卡号时调用
 */
void Found::on_cardIdReceived(QString tagId)
{
    QString inputUser = ui->userId->text();
    if(ui->userId->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入账号");
        return;
    }
    //构造用户编号写入0块的命令帧
    uint16 frameLen;
    uint8 *p;
    uint8* vdata1;
    vdata1 = new uint8[14];
    vdata1[0]=0x02;  //flag
    quint8 * buf1;
    buf1=new quint8[8];
    StringToHex(tagId,buf1);
    memcpy(vdata1+1,buf1,8);    //write CID
    vdata1[9]=0x00;  //addr
    quint8 * buf2;
    buf2=new quint8[4];
    StringToHex(inputUser,buf2);
    memcpy(vdata1+10,buf2,4);   //write UID
    p = m1356dll->RC632_SendCmdReq(RC632_CMD_ISO15693_WRITE_SM,vdata1,14);
    frameLen = BUILD_UINT16(p[0], p[1]);

    ui->cardId->setText(tagId);//填入识别到的卡号
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    //已激活的卡
    QString sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'Y';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        QMessageBox::critical(this,"警告","该卡已被激活，不能用于解除挂失");
        helper->closeDatabase();//关闭数据库
        return;
    }
    //已注销的卡
    sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'S';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        serialPortThread->writeData((char *)(p + 2 ),frameLen);//初始化卡，写入用户编号到块0
        ui->found->setEnabled(true);//可以挂失，启用挂失按钮
        helper->closeDatabase();//关闭数据库
        return;
    }
    //用户自己的卡(被挂失)
    sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1' and CID = '%2' and USTATUS = 'N';").arg(inputUser).arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        serialPortThread->writeData((char *)(p + 2 ),frameLen);//初始化卡，写入用户编号到块0
        ui->found->setEnabled(true);//可以挂失，启用挂失按钮
        helper->closeDatabase();//关闭数据库
        return;
    }
    //新卡
    sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'N';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(!query.next()){
        serialPortThread->writeData((char *)(p + 2 ),frameLen);//初始化卡，写入用户编号到块0
        ui->found->setEnabled(true);//可以挂失，启用挂失按钮
        helper->closeDatabase();//关闭数据库
        return;
    }
}

/**
 * @brief Found::on_getId_clicked
 * 点击识别按钮，识别卡号
 */
void Found::on_getId_clicked()
{
    QString inputUser = ui->userId->text();
    if(ui->userId->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入账号");
        return;
    }
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    QString sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1' and USTATUS = 'N';").arg(inputUser);
    query.prepare(sql);
    query.exec();
    if(!query.next()){
        QMessageBox::critical(this,"警告","需要解除挂失的账户不存在");
        helper->closeDatabase();//关闭数据库
        return;
    }


    if(!serialPortThread->serialPortIsOpen())
    {
        QMessageBox::critical(this,"警告","请先连接读卡器后再试！");
        return;
    }
    uint16 frameLen;
    uint8 *p;
    p = m1356dll->RC632_SendCmdReq(RC632_CMD_ISO15693_INVENTORY16,NULL,0);
    frameLen = BUILD_UINT16(p[0], p[1]);
    serialPortThread->writeData((char *)(p + 2 ),frameLen);
}

/**
 * @brief Found::onDecodeFrame
 * @param bytes 接收到的数据
 * 串口接收槽函数
 */
void Found::onDecodeFrame(QByteArray bytes)
{
    M1356_RspFrame_t frame = m1356dll->M1356_RspFrameConstructor(bytes);
    if(frame.status.left(2) == "00")
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            QMessageBox::information(this,tr("提示"),tr("解除挂失写卡成功！"),QMessageBox::Yes);
        }
    }
    else
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            QMessageBox::warning(this,tr("温馨提示"),tr("解除挂失写卡失败，请调整卡与读卡器的距离后再试！"),QMessageBox::Yes);
        }
    }
}
