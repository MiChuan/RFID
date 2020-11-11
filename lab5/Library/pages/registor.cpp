#include "registor.h"
#include "ui_registor.h"

Registor::Registor(QWidget *parent, SerialPortThread *serialPortThread) :
    QWidget(parent),
    ui(new Ui::Registor)
{
    ui->setupUi(this);
    ui->reg->setDisabled(true);
    this->serialPortThread = serialPortThread;
    m1356dll = new M1356Dll();
    if(ui->canview->isChecked()){
        ui->password->setEchoMode(QLineEdit::Password);//密码输入框设置
    } else{
        ui->password->setEchoMode(QLineEdit::Normal);
    }
}

Registor::~Registor()
{
    delete ui;
}

void Registor::handleConnect()
{
    connect(serialPortThread,SIGNAL(receivedMsg(QByteArray)),this,SLOT(onDecodeFrame(QByteArray)));
}

void Registor::on_cardIdReceived(QString tagId)
{
    QString userId = ui->userId->text();
    ui->cardId->setText(tagId);//填入识别到的卡号
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    //已存在的账户
    QString sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1';").arg(userId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        QMessageBox::critical(this,"警告","该账户已被注册，不能注册该账户");
        helper->closeDatabase();//关闭数据库
        return;
    }
    //已激活的卡
    sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'Y';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        QMessageBox::critical(this,"警告","该卡已被激活，不能用于注册账户");
        helper->closeDatabase();//关闭数据库
        return;
    }
    //已挂失的卡
    sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'N';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        QMessageBox::critical(this,"警告","该卡已被挂失，不能用于注册账户");
        helper->closeDatabase();//关闭数据库
        return;
    }
    //已绑定的卡
    sql = QString("SELECT * FROM BOOK_INFO WHERE BID = '%1';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        QMessageBox::critical(this,"警告","该卡已绑定图书，不能用于注册账户");
        helper->closeDatabase();//关闭数据库
        return;
    }
    //已注销的卡
    sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'S';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        ui->reg->setEnabled(true);//可以注册，启用注册按钮
        helper->closeDatabase();//关闭数据库
        return;
    }
    //新卡
    ui->reg->setEnabled(true);//可以注册，启用注册按钮
    helper->closeDatabase();//关闭数据库
}

void Registor::onDecodeFrame(QByteArray bytes)
{
    M1356_RspFrame_t frame = m1356dll->M1356_RspFrameConstructor(bytes);
    if(frame.status.left(2) == "00")
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            QMessageBox::information(this,tr("提示"),tr("注册账户写卡成功！"),QMessageBox::Yes);
        }
    }
    else
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            QMessageBox::warning(this,tr("温馨提示"),tr("注册账户写卡失败，请调整卡与读卡器的距离后再试！"),QMessageBox::Yes);
        }
    }
}

void Registor::on_getId_clicked()
{
    if(ui->userId->text().isEmpty() ||
       ui->name->text().isEmpty() ||
       ui->tele->text().isEmpty() ||
       ui->password->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入正确的用户信息");
        return;
    }

    if(ui->canview->isChecked()){
        ui->password->setEchoMode(QLineEdit::Password);//密码输入框设置
    } else{
        ui->password->setEchoMode(QLineEdit::Normal);
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

void Registor::on_reg_clicked()
{
    QString UID = ui->userId->text();
    QString CID = ui->cardId->text();
    QString UNAME = ui->name->text();
    QString TELEPHONE = ui->tele->text();
    QString PASSWORD = GetMD5String(ui->password->text());//MD5加密
    QString USTATUS = "Y";

    //构造用户编号写入0块的命令帧
    //需要将string中两个char转为quint8数组中一个8位的元素
    //16个字符的CID-->8个元素quint8数组，每个元素高低4位各代表一个字符
    //"FF"-->0xFF
    uint16 frameLen;
    uint8 *p;
    uint8* vdata1;
    vdata1 = new uint8[14];
    vdata1[0]=0x02;  //flag
    quint8 * buf1;
    buf1=new quint8[8];
    StringToHex(CID,buf1);
    memcpy(vdata1+1,buf1,8);    //write CID
    vdata1[9]=0x00;  //addr
    quint8 * buf2;
    buf2=new quint8[4];
    StringToHex(UID,buf2);
    memcpy(vdata1+10,buf2,4);   //write UID
    p = m1356dll->RC632_SendCmdReq(RC632_CMD_ISO15693_WRITE_SM,vdata1,14);
    frameLen = BUILD_UINT16(p[0], p[1]);

    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    //写入图书信息到数据库
    QString sql = QString("INSERT INTO USER_INFO(UID,CID,UNAME,TELEPHONE,PASSWORD,USTATUS) "
                          "VALUES('%1','%2','%3','%4','%5','%6');").arg(UID).arg(CID).arg(UNAME).arg(TELEPHONE).arg(PASSWORD).arg(USTATUS);
    query.prepare(sql);
    query.exec();
    sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1';").arg(UID);
    query.prepare(sql);
    query.exec();
    helper->closeDatabase();//关闭数据库

    if(!query.next()){
        QMessageBox::critical(this,"警告","写入数据库失败，请检查数据库连接后重新注册");
        return;
    }
    //写入数据库成功
    serialPortThread->writeData((char *)(p + 2 ),frameLen);//初始化卡，写入用户编号到块0
}
