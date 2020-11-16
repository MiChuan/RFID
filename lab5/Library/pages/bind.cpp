#include "bind.h"
#include "ui_bind.h"

Bind::Bind(QWidget *parent, SerialPortThread *serialPortThread) :
    QWidget(parent),
    ui(new Ui::Bind)
{
    ui->setupUi(this);
    ui->bindcard->setDisabled(true);
    this->serialPortThread = serialPortThread;
    m1356dll = new M1356Dll();
    this->handleConnect();
    CurOpt = InitOpt;
}

Bind::~Bind()
{
    delete ui;
}

void Bind::handleConnect()
{
    connect(serialPortThread,SIGNAL(receivedMsg(QByteArray)),this,SLOT(onDecodeFrame(QByteArray)));
}

void Bind::on_cardIdReceived(QString tagId)
{
    if(CurOpt != BindOpt){
        CurOpt = InitOpt;
        return;
    }
    ui->cardId->setText(tagId);//填入识别到的卡号
    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    //已激活的卡
    QString sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'Y';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        QMessageBox::critical(this,"警告","该卡已被激活，不能用于绑定图书");
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
    //已挂失的卡
    sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'N';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        QMessageBox::critical(this,"警告","该卡已被挂失，不能用于绑定图书");
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
    //已绑定的卡
    sql = QString("SELECT * FROM BOOK_INFO WHERE BID = '%1';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        QMessageBox::critical(this,"警告","该卡已被绑定，不能用于绑定图书");
        helper->closeDatabase();//关闭数据库
        CurOpt = InitOpt;
        return;
    }
    //已注销的卡
    sql = QString("SELECT * FROM USER_INFO WHERE CID = '%1' and USTATUS = 'S';").arg(tagId);
    query.prepare(sql);
    query.exec();
    if(query.next()){
        ui->bindcard->setEnabled(true);//可以绑定，启用绑定按钮
        helper->closeDatabase();//关闭数据库
        return;
    }
    //新卡
    ui->bindcard->setEnabled(true);//可以绑定，启用绑定按钮
    helper->closeDatabase();//关闭数据库
}

void Bind::onDecodeFrame(QByteArray bytes)
{

}

void Bind::on_bindcard_clicked()
{
    qDebug() <<"bind0";
    qDebug() << CurOpt;
    if(CurOpt != BindOpt){
        CurOpt = InitOpt;
        return;
    }
    CurOpt = InitOpt;//执行终点
    QString BID = ui->cardId->text();
    QString ISBN = ui->ISBN->text();
    QString bookname = ui->bookname->text();
    QString author = ui->author->text();
    QString press = ui->press->text();
    QString pressdate = ui->pressdate->text();
    QString duration = ui->duration->text();
    QString BSTATUS = "Y";

    DBHelper *helper = DBHelper::getInstance();
    QSqlQuery query;
    helper->openDatabase();//链接数据库
    //写入图书信息到数据库
    QString sql = QString("INSERT INTO BOOK_INFO(BID,ISBN,BNAME,AUTHOR,PRESS,PTIME,DURATION,BSTATUS) "
                          "VALUES('%1','%2','%3','%4','%5','%6',%7,'%8');").arg(BID).arg(ISBN).arg(bookname).arg(author).arg(press).arg(pressdate).arg(duration).arg(BSTATUS);
    query.prepare(sql);
    query.exec();
    //写入数据库成功
    sql = QString("SELECT * FROM BOOK_INFO WHERE BID = '%1';").arg(BID);
    query.prepare(sql);
    query.exec();
    helper->closeDatabase();//关闭数据库
    if(!query.next()){
        QMessageBox::critical(this,"警告","写入数据库失败，请检查数据库连接后重新绑定");
        CurOpt = InitOpt;
        return;
    }
}

void Bind::on_getId_clicked()
{
    CurOpt = BindOpt;//执行起点
    if(ui->ISBN->text().isEmpty() ||
       ui->bookname->text().isEmpty() ||
       ui->author->text().isEmpty() ||
       ui->press->text().isEmpty()){
        QMessageBox::critical(this,"警告","请输入正确的书籍信息");
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
