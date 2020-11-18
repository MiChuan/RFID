#include "returnbook.h"
#include "ui_returnbook.h"

ReturnBook::ReturnBook(QWidget *parent, SerialPortThread *serialPortThread) :
    QWidget(parent),
    ui(new Ui::ReturnBook)
{
    ui->setupUi(this);
    ui->retbook->setDisabled(true);
    ui->getuserId->setDisabled(true);
    this->serialPortThread = serialPortThread;
    m1356dll = new M1356Dll();
    this->flag = Init;
    this->curDate = QDate::currentDate();
    this->handleConnect();
    CurOpt = InitOpt;
}

ReturnBook::~ReturnBook()
{
    delete ui;
}

void ReturnBook::handleConnect()
{
    connect(serialPortThread,SIGNAL(receivedMsg(QByteArray)),this,SLOT(onDecodeFrame(QByteArray)));
}

void ReturnBook::on_cardIdReceived(QString tagId)
{
    if(CurOpt != RetOpt){
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
        //已借出的书
        sql = QString("SELECT * FROM BOOK_INFO WHERE BID = '%1' and BSTATUS = 'N';").arg(tagId);
        query.prepare(sql);
        query.exec();
        if(query.next()){
            ui->getuserId->setEnabled(true);
            ui->bookname->setText(query.value("BNAME").toString());
            helper->closeDatabase();//关闭数据库
            return;
        }
        //未借出的书
        QMessageBox::critical(this,"警告","该卡绑定的图书未借出");
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
            QMessageBox::critical(this,"警告","该卡已被挂失，不能用于还书");
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
            QMessageBox::critical(this,"警告","该卡已被注销，不能用于还书");
            this->flag = Init;
            helper->closeDatabase();//关闭数据库
            CurOpt = InitOpt;
            return;
        }
        //已激活的卡
        qDebug() << "CID: " << tagId;//debug
        sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'Y';").arg(tagId);
        query.prepare(sql);
        query.exec();
        if(query.next()){
            QString UID = query.value("UID").toString();
            ui->uid->setText(UID);
            QString BNUM = query.value("BNUM").toString();
            qDebug() << "UID: " << UID << " BNUM: " << BNUM;//debug
            this->flag = Init;
            sql = QString("SELECT * FROM RECORD WHERE UID = '%1' AND BID = '%2';").arg(UID).arg(ui->bookId->text());
            query.prepare(sql);
            query.exec();
            if(query.last()){
                QString RID = query.value("RID").toString();
                QString RTIME = curDate.toString("yyyy-MM-dd");
                qDebug() << "RID: " << RID << " RTIME: " << RTIME;//debug
                ui->rid->setText(RID);
                sql = QString("UPDATE RECORD SET RTIME = '%1' WHERE RID = '%2';").arg(RTIME).arg(RID);
                query.prepare(sql);
                query.exec();
                sql = QString("UPDATE BOOK_INFO SET BSTATUS = 'Y' WHERE BID = '%1';").arg(ui->bookId->text());
                query.prepare(sql);
                query.exec();
                BNUM = QString::number(BNUM.toInt()+1);
                ui->canbor->setText(BNUM);
                sql = QString("UPDATE USER_INFO SET BNUM = '%1' WHERE UID = '%2';").arg(BNUM).arg(UID);
                query.prepare(sql);
                query.exec();
            }
            ui->retbook->setEnabled(true);
            helper->closeDatabase();//关闭数据库
            return;
        }
        //非有效用户卡
        QMessageBox::critical(this,"警告","该卡非有效用户卡，不能用于还书");
        this->flag = Init;
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
}

void ReturnBook::onDecodeFrame(QByteArray bytes)
{
    M1356_RspFrame_t frame = m1356dll->M1356_RspFrameConstructor(bytes);
    if(frame.status.left(2) == "00")
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            if(CurOpt != RetOpt){
                CurOpt = InitOpt;
                return;
            }
            CurOpt = InitOpt;
            QMessageBox::information(this,tr("提示"),tr("还书写卡成功！"),QMessageBox::Yes);
        }
    }
    else
    {
        if(frame.cmd.remove(" ") == "0610")//写卡0x1006
        {
            if(CurOpt != RetOpt){
                CurOpt = InitOpt;
                return;
            }
            CurOpt = InitOpt;
            QMessageBox::warning(this,tr("温馨提示"),tr("还书写卡失败，请调整卡与读卡器的距离后再试！"),QMessageBox::Yes);
        }
    }
}

void ReturnBook::on_getuserId_clicked()
{
    CurOpt = RetOpt;//执行起点
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

void ReturnBook::on_getbookId_clicked()
{
    CurOpt = RetOpt;//执行起点
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

void ReturnBook::on_retbook_clicked()
{
    if(CurOpt != RetOpt){
        CurOpt = InitOpt;
        return;
    }
    CurOpt = InitOpt;//执行终点
    QMessageBox::information(this,tr("提示"),tr("还书成功！"),QMessageBox::Yes);
}
