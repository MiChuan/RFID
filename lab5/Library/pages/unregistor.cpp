#include "unregistor.h"
#include "ui_unregistor.h"

Unregistor::Unregistor(QWidget *parent, SerialPortThread *serialPortThread) :
    QWidget(parent),
    ui(new Ui::Unregistor)
{
    ui->setupUi(this);
    ui->unreg->setDisabled(true);
    this->serialPortThread = serialPortThread;
    m1356dll = new M1356Dll();
    count = InitCount;
    this->handleConnect();
    CurOpt = InitOpt;
}

Unregistor::~Unregistor()
{
    delete ui;
}

void Unregistor::handleConnect()
{
    connect(serialPortThread,SIGNAL(receivedMsg(QByteArray)),this,SLOT(onDecodeFrame(QByteArray)));
}

void Unregistor::on_cardIdReceived(QString tagId)
{
    if(CurOpt != UnRegOpt){
        CurOpt = InitOpt;
        return;
    }
    QString userId = ui->userId->text();
    ui->cardId->setText(tagId);//填入识别到的卡号
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    //不存在的账户
    QString sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1' and CID = '%2' and USTATUS = 'Y';").arg(userId).arg(tagId);
    query.prepare(sql);
    query.exec();
    if(!query.next()){
        QMessageBox::critical(this,"警告","该账户不存在，不能注销");
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
    //未还书
    if(query.value("BNUM").toInt() < 5){
        QMessageBox::critical(this,"警告","该账户未还书，不能注销");
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
    //已激活的卡
    ui->unreg->setEnabled(true);//可以注销，启用注销按钮
    helper->closeDatabase();//关闭数据库
    return;
}

void Unregistor::onDecodeFrame(QByteArray bytes)
{
    if(CurOpt != UnRegOpt){
        CurOpt = InitOpt;
        return;
    }
    CurOpt = InitOpt;//执行终点
    M1356_RspFrame_t frame = m1356dll->M1356_RspFrameConstructor(bytes);
    if(frame.status.left(2) == "00")
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            QMessageBox::information(this,tr("提示"),tr("注销账户写卡成功！"),QMessageBox::Yes);
        }
    }
    else
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            QMessageBox::warning(this,tr("温馨提示"),tr("注销账户写卡失败，请调整卡与读卡器的距离后再试！"),QMessageBox::Yes);
        }
    }
}

void Unregistor::on_getId_clicked()
{
    CurOpt = UnRegOpt;//执行起点
    if(ui->userId->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入正确的用户编号");
        CurOpt = InitOpt;
        return;
    }

    if(!serialPortThread->serialPortIsOpen())
    {
        QMessageBox::critical(this,"警告","请先连接读卡器后再试！");
        CurOpt = InitOpt;
        return;
    }
    uint16 frameLen;
    uint8 *p;
    p = m1356dll->RC632_SendCmdReq(RC632_CMD_ISO15693_INVENTORY16,NULL,0);
    frameLen = BUILD_UINT16(p[0], p[1]);
    serialPortThread->writeData((char *)(p + 2 ),frameLen);
}

void Unregistor::on_unreg_clicked()
{
    if(CurOpt != UnRegOpt){
        CurOpt = InitOpt;
        return;
    }
    QString UID = ui->userId->text();
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    //标记账户为注销
    QString sql = QString("UPDATE USER_INFO SET USTATUS = 'S' WHERE UID = '%1' and USTATUS = 'Y';").arg(UID);
    query.prepare(sql);
    query.exec();
    sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1';").arg(UID);
    query.prepare(sql);
    query.exec();
    helper->closeDatabase();//关闭数据库

    if(!query.next()){
        QMessageBox::critical(this,"警告","写入数据库失败，请检查数据库连接后重新注销");
        CurOpt = InitOpt;
        return;
    }
    this->clearData();
}

void Unregistor::clearData()
{
    if(CurOpt != UnRegOpt){
        CurOpt = InitOpt;
        return;
    }
    QString CID = ui->cardId->text();
    QString NOP = "00000000";

    //构造用户编号写入0块的命令帧
    //需要将string中两个char转为quint8数组中一个8位的元素
    //16个字符的CID-->8个元素quint8数组，每个元素高低4位各代表一个字符
    //"FF"-->0xFF
    uint16 frameLen;
    uint8 *p;
    uint8* vdata1;
    quint8 * buf1;
    quint8 * buf2;
    vdata1 = new uint8[14];
    buf1=new quint8[8];
    buf2=new quint8[4];
    quint8 step;

    vdata1[0]=0x02;  //flag
    StringToHex(CID,buf1);
    memcpy(vdata1+1,buf1,8);    //write CID
    vdata1[9]=0x00;  //addr
    StringToHex(NOP,buf2);
    memcpy(vdata1+10,buf2,4);   //write block 0
    p = m1356dll->RC632_SendCmdReq(RC632_CMD_ISO15693_WRITE_SM,vdata1,14);
    frameLen = BUILD_UINT16(p[0], p[1]);
    serialPortThread->writeData((char *)(p + 2 ),frameLen);//清除卡内数据，块0

//    while(this->count > 0){
//        step = (quint8)(InitCount - this->count);
//        vdata1[9]=0x02+step;  //addr
//        p = m1356dll->RC632_SendCmdReq(RC632_CMD_ISO15693_WRITE_SM,vdata1,14);
//        frameLen = BUILD_UINT16(p[0], p[1]);
//        serialPortThread->writeData((char *)(p + 2 ),frameLen);//清除卡内数据，块0x02+step
//        --this->count;
//    }
}
