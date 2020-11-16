#include "serialportthread.h"
#include <qdebug.h>
/**************************************
 *作者: jianghj@up-tech.com
 *日期: 2016-09-20
 *描述: 单独对串口进行处理的线程,主要提供对串口的读写操作接口
***************************************/
SerialPortThread::SerialPortThread(QObject *parent) : QThread(parent)
{
    serialIsOpen = false;
    Stop = false;
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(onError()));
    connect(this,SIGNAL(hasResponse()),this,SLOT(stopTimer()));
}
SerialPortThread::~SerialPortThread()
{
}
/**
 * @brief SerialPortThread::stopTimer
 * 停止定时器
 */
void SerialPortThread::stopTimer()
{
    if(timer->isActive())
    {
        timer->stop();
    }
}
/**
 * @brief SerialPortThread::onError
 * 当发送出错时发送错误信号
 */
void SerialPortThread::onError()
{
    QString error = serialPort->errorString();
    if(error == "Unknown error")
        emit wirteMsgError("No Response!");
    else
        emit wirteMsgError(error);
    this->stopTimer();
}
/**
 * @brief SerialPortThread::stopThread
 * 停止串口线程
 */
void SerialPortThread::stopThread()
{
    this->stopTimer();
    this->Stop = true;
    this->destroyed();
}
/**
 * @brief SerialPortThread::setRetryTimes
 * @param times
 * 设置串口重复读取数据
 */
void SerialPortThread::setRetryTimes(ReadRetryTimes times)
{
    this->retryTimes = times;
}
/**
 * @brief SerialPortThread::writeData
 * @param data 要发送到串口的已经char转hex后的数据
 * @param frameLen 一帧数据的长度
 * @param echo 用于控制是否回显
 * 发送数据
 */
void SerialPortThread::writeData(char * data, int frameLen, bool echo)
{
    int count = 0 ;
    QString str = "SendMessage: ";
    //8bits一个字符转为4bits，ASCII码中0~F转为16进制表示
    //SendingMsg = CharStringtoHexString(tr(" "),data,frameLen);
    for(int i = 0; i < frameLen; ++i){
        str += QString("%1").arg(data[i]);
    }
    qDebug() << str;
    //if(echo) emit sendMsg(data,frameLen);

    while (frameLen > 0) {//写入串口，最多写15次
        frameLen -= serialPort->write(data,frameLen);
        count ++;
        if(count > 15)
            break;
    }
    if(!timer->isActive())
        timer->start(2000);//启动计时器，等待响应
    delete [] data;
}
/**
 * @brief SerialPortThread::serialPortIsOpen
 * @return  如果打开了返回true，否则返回false
 * 判断串口是否打开
 */
bool SerialPortThread::serialPortIsOpen()
{
  return serialIsOpen;
}
/**
 * @brief SerialPortThread::run
 * 线程实体run
 */
void SerialPortThread::run()
{
    int count = retryTimes;
    QByteArray bytes;
    //控制线程循环
    while (!Stop) {
        if(serialPort->isOpen())
        {
            if(serialPort->bytesAvailable() >= 4)
            {
                emit hasResponse();//接收到数据，发送信号停止计时器
                bytes = serialPort->readAll();//读取串口缓冲区数据
                if(bytes.at(0) != (char) 0xAA && bytes.at(1) != (char)0xBB)
                    continue;//校验帧头
                qint16 waitforReadLen = (qint16)((bytes.at(2) & 0x00FF) + ((bytes.at(3) & 0x00FF) << 8)) + 4;
                //4byte = sop(2byte)+len(2byte)
                while ((waitforReadLen - bytes.length()) > 0 && count -- > 0)
                {//还有未读取的数据
                    if(serialPort->bytesAvailable() == 0)
                        QThread::usleep(10);
                    else
                    {//读取后追加到缓冲区，设置读取次数
                        bytes += serialPort->readAll();
                        count = retryTimes;
                    }
                }
                count = retryTimes;
                for(int i = 0 ; i < bytes.length() ; i ++)
                {
                    if(bytes.at(i) == (char)0xAA && bytes.at(i+1) == (char)0x00)
                    {//去除为区分sof中0xAA和后续数据中0xAA而在后续数据中加入的0x00
                        bytes.remove(i + 1,1);
                    }
                }
                qDebug() << "RecMessage: "<< CharStringtoHexString(tr(" "),bytes.data(),bytes.length());;
                emit receivedMsg(bytes);
            }
        }
        QThread::msleep(20);
    }
}
/**
 * @brief SerialPortThread::setSerialPort
 * @param serialport
 * 设置串口实体类
 */
void SerialPortThread::setSerialPort( QSerialPort *serialport)
{
    this->serialPort = serialport;
    serialIsOpen = serialport->isOpen();
}

uint8 RC632_UartCalcFCS_Test( uint8 *msg_ptr, uint8 len )
{
    uint8 x;
    uint8 xorResult;
    xorResult = 0;
    for ( x = 0; x < len; x++, msg_ptr++ )
      xorResult = xorResult ^ *msg_ptr;
    return ( xorResult );
}

uint8* RC632_SendCmdReq_Test(uint16 cmd, const uint8 *data, uint16 len)
{
    uint8 *p;
    uint16 frameLen, uartdatalen;
    frameLen = len+9;   // 9 = sop(2)+len(2)+nc(2)+cmd(2)+fcs(1)
    uartdatalen = frameLen;
    for(uint16 i=0; i<len; i++)
        if(data[i] == 0xAA)  uartdatalen++;
    p = new uint8[uartdatalen+2];  // p[0] is used return frameLen
    p[0] = LO_UINT16(uartdatalen);
    p[1] = HI_UINT16(uartdatalen);     //framelen
    p[2] = LO_UINT16(RC632_FRAME_SOP);
    p[3] = HI_UINT16(RC632_FRAME_SOP); //BBAA
    p[4] = LO_UINT16(frameLen-4);
    p[5] = HI_UINT16(frameLen-4);      // 4 = sop(2)+len(2)
    p[6] = 0;
    p[7] = 0;                          // NC
    p[8] = LO_UINT16(cmd);
    p[9] = HI_UINT16(cmd);
    //memcpy(p+10, data, len);
    int k=0;
    for(int j=0; j<len; j++, k++){
        p[10+k] = data[j];
        if(data[j]==0xAA)  p[10+ ++k] = 0x00;
    }
    p[10+k] = RC632_UartCalcFCS_Test(p+5, uartdatalen-4);

    return p;
}
