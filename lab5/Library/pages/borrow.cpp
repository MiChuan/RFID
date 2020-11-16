#include "borrow.h"
#include "ui_borrow.h"

Borrow::Borrow(QWidget *parent, SerialPortThread *serialPortThread) :
    QWidget(parent),
    ui(new Ui::Borrow)
{
    ui->setupUi(this);
    ui->borrow->setDisabled(true);
    ui->getuserId->setDisabled(true);
    this->serialPortThread = serialPortThread;
    m1356dll = new M1356Dll();
    this->flag = Init;
    this->curDate = QDate::currentDate();
    this->handleConnect();
    CurOpt = InitOpt;
}

Borrow::~Borrow()
{
    delete ui;
}

void Borrow::handleConnect()
{
    connect(serialPortThread,SIGNAL(receivedMsg(QByteArray)),this,SLOT(onDecodeFrame(QByteArray)));
}

void Borrow::on_cardIdReceived(QString tagId)
{
    if(CurOpt != BorOpt){
        CurOpt = InitOpt;
        return;
    }
    if(this->flag == BookCard){
        ui->bookId->setText(tagId);//填入识别到的卡号
        DBHelper *helper = DBHelper::getInstance();
        QSqlQuery query;
        helper->openDatabase();//链接数据库
        //未绑定图书的卡
        QString sql = QString("SELECT * FROM BOOK_INFO WHERE BID = '%1';").arg(tagId);
        query.prepare(sql);
        query.exec();
        if(!query.next()){
            QMessageBox::critical(this,"警告","该卡未绑定图书");
            this->flag = Init;
            helper->closeDatabase();//关闭数据库
            CurOpt = InitOpt;
            return;
        }
        //可借出的书
        sql = QString("SELECT * FROM BOOK_INFO WHERE BID = '%1' and BSTATUS = 'Y';").arg(tagId);
        query.prepare(sql);
        query.exec();
        if(query.next()){
            ui->getuserId->setEnabled(true);
            ui->ISBN->setText(query.value("ISBN").toString());
            ui->bookname->setText(query.value("BNAME").toString());
            ui->author->setText(query.value("AUTHOR").toString());
            ui->press->setText(query.value("PRESS").toString());
            ui->pressdate->setText(query.value("PTIME").toString());
            ui->duration->setText(query.value("DURATION").toString());

//            QStringList datenow = this->curDate.toString("yyyy-MM-dd").split("-");
            int dur = ui->duration->text().toInt();//可借时长
//            int year = QString(datenow.at(0)).toInt();
//            int month = QString(datenow.at(1)).toInt();
//            int day = QString(datenow.at(2)).toInt();

            QDate d = curDate;
            d = d.addDays(dur);//借书时间加上可借时长为截止日期

            QString dateDDL = d.toString("yyyy-MM-dd");
            ui->ddl->setText(dateDDL);
            helper->closeDatabase();//关闭数据库
            return;
        }
        //已借出的书
        QMessageBox::critical(this,"警告","该卡绑定的图书已借出");
        this->flag = Init;
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
    else if(this->flag == UserCard){
        ui->userId->setText(tagId);//填入识别到的卡号
        DBHelper *helper = DBHelper::getInstance();
        QSqlQuery query;
        helper->openDatabase();//链接数据库
        //已挂失的卡
        QString sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'N';").arg(tagId);
        query.prepare(sql);
        query.exec();
        if(query.next()){
            QMessageBox::critical(this,"警告","该卡已被挂失，不能用于借书");
            this->flag = Init;
            helper->closeDatabase();//关闭数据库
            CurOpt = InitOpt;
            return;
        }
        //已注销的卡
        sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'S';").arg(tagId);
        query.prepare(sql);
        query.exec();
        if(query.next()){
            QMessageBox::critical(this,"警告","该卡已被注销，不能用于借书");
            this->flag = Init;
            helper->closeDatabase();//关闭数据库
            CurOpt = InitOpt;
            return;
        }
        //已激活的卡
        sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'Y';").arg(tagId);
        query.prepare(sql);
        query.exec();
        if(query.next()){
            this->flag = Init;
            ui->canbor->setText(query.value("BNUM").toString());
            if(query.value("BNUM").toInt() == 0){
                QMessageBox::critical(this,"警告","借书已满5本，请还书后再借书");
                helper->closeDatabase();//关闭数据库
                CurOpt = InitOpt;
                return;
            }
            ui->borrow->setEnabled(true);
            ui->uid->setText(query.value("UID").toString());
            helper->closeDatabase();//关闭数据库
            return;
        }
        //非有效用户卡
        QMessageBox::critical(this,"警告","该卡非有效用户卡，不能用于借书");
        this->flag = Init;
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
}

void Borrow::onDecodeFrame(QByteArray bytes)
{
    M1356_RspFrame_t frame = m1356dll->M1356_RspFrameConstructor(bytes);
    if(frame.status.left(2) == "00")
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            if(CurOpt != BorOpt){
                CurOpt = InitOpt;
                return;
            }
            CurOpt = InitOpt;//执行终点
            QMessageBox::information(this,tr("提示"),tr("借书写卡成功！"),QMessageBox::Yes);
        }
    }
    else
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            if(CurOpt != BorOpt){
                CurOpt = InitOpt;
                return;
            }
            CurOpt = InitOpt;//执行终点
            QMessageBox::warning(this,tr("温馨提示"),tr("借书写卡失败，请调整卡与读卡器的距离后再试！"),QMessageBox::Yes);
        }
    }
}

void Borrow::on_getuserId_clicked()
{
    CurOpt = BorOpt;//执行起点
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
    this->flag = UserCard;
}

void Borrow::on_getbookId_clicked()
{
    CurOpt = BorOpt;//执行起点
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
    this->flag = BookCard;
}

void Borrow::on_borrow_clicked()
{
    if(CurOpt != BorOpt){
        CurOpt = InitOpt;
        return;
    }
    QString BTIME = this->curDate.toString("yyyy-MM-dd");
    QString UID = ui->uid->text();
    QString CID = ui->userId->text();
    QString BID = ui->bookId->text();
    QString DDL = ui->ddl->text();
    QString BNUM = ui->canbor->text();//5,4,3,2,1
    int save = 6 - BNUM.toInt();//1,2,3,4,5
    QString SAVEBLOCK = QString::number(save);
    QString RID;

    //构造借书记录编号写入用户卡的命令帧
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
    vdata1[9]=(uint8)save;  //addr

    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    //生成记录编号
    QString sql = QString("SELECT * FROM RECORD;");
    query.prepare(sql);
    query.exec();
    if(query.last()){
        RID = query.value("RID").toString();
        int r = RID.toInt() + 1;
        RID = QString::number(r);
        quint8 * buf2;
        buf2=new quint8[4];
        StringToHex(RID,buf2);
        memcpy(vdata1+10,buf2,4);   //write RID
        p = m1356dll->RC632_SendCmdReq(RC632_CMD_ISO15693_WRITE_SM,vdata1,14);
        frameLen = BUILD_UINT16(p[0], p[1]);
    }

    //更新用户信息
    sql = QString("UPDATE USER_INFO SET BNUM = %1 WHERE UID = '%2';").arg(QString::number(BNUM.toInt()-1)).arg(UID);
    query.prepare(sql);
    query.exec();
    //检查写入情况
    sql = QString("SELECT * FROM USER_INFO WHERE UID = '%1' AND BNUM = %2;").arg(UID).arg(QString::number(BNUM.toInt()-1));
    query.prepare(sql);
    query.exec();
    if(!query.next()){
        QMessageBox::critical(this,"警告","写入数据库失败，请检查数据库连接后重新借书");
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
    //更新图书信息到数据库
    sql = QString("UPDATE BOOK_INFO SET BSTATUS = 'N', SAVEBLOCK = '%1' WHERE BID = '%2';").arg(SAVEBLOCK).arg(BID);
    query.prepare(sql);
    query.exec();
    //检查写入情况
    sql = QString("SELECT * FROM BOOK_INFO WHERE BID = '%1' AND BSTATUS = 'N';").arg(BID);
    query.prepare(sql);
    query.exec();
    if(!query.next()){
        QMessageBox::critical(this,"警告","写入数据库失败，请检查数据库连接后重新借书");
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
    //写入借书记录到数据库
    sql = QString("INSERT INTO RECORD(RID,UID,BID,BTIME,DDL) "
                          "VALUES('%1','%2','%3','%4','%5');").arg(RID).arg(UID).arg(BID).arg(BTIME).arg(DDL);
    query.prepare(sql);
    query.exec();
    //检查写入情况
    sql = QString("SELECT * FROM RECORD WHERE RID = '%1';").arg(RID);
    query.prepare(sql);
    query.exec();
    if(!query.next()){
        QMessageBox::critical(this,"警告","写入数据库失败，请检查数据库连接后重新借书");
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
    //写入数据库成功,写入借书记录到用户卡
    serialPortThread->writeData((char *)(p + 2 ),frameLen);//写入借书记录编号
    helper->closeDatabase();//关闭数据库
}
