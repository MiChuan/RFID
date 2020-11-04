#include "useraccount.h"
#include <QDebug>

userAccount::userAccount()
{

}

userAccount::~userAccount()
{

}

void userAccount::writeConfig(QString key, QString value)
{
    //Qt中使用QSettings类读写ini文件
    //QSettings构造函数的第一个参数是ini文件的路径,第二个参数表示针对ini文件,第三个参数可以缺省
    QSettings *configIniWrite = new QSettings(filePath, QSettings::IniFormat);
    //向ini文件中写入内容,setValue函数的两个参数是键值对
    //向ini文件的key写入内容value
    configIniWrite->setValue(key, value);
    //写入完成后删除指针
    delete configIniWrite;
}

QString userAccount::readConfig(QString key)
{
    QSettings *configIniRead = new QSettings(filePath, QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    QString Result = configIniRead->value(key).toString();
    //打印得到的结果
    qDebug() << Result;
    //读入入完成后删除指针
    delete configIniRead;
    return Result;
}

/**
 * @param src 用户输入的字符串
 * @return MD5字符串
 * MD5加密
 */
QString userAccount::GetMD5String(QString src)
{
    QString result;
    QByteArray ba;
    ba = QCryptographicHash::hash(src.toLatin1(), QCryptographicHash::Md5);
    result.append(ba.toHex());
    qDebug() << result;
    return result;
}
