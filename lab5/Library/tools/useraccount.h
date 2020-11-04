#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QSettings>
#include <QCryptographicHash>

class userAccount
{
public:
    userAccount();
    ~userAccount();

    void writeConfig(QString key, QString value);//写入配置文件
    QString readConfig(QString key);//读取配置文件
    QString GetMD5String(QString src);//将原字符串进行MD5编码

private:
    QString filePath = "F:\\QTproject\\RFID_Design\\MQTT\\AnalyzeData\\config.ini";
};

#endif // USERACCOUNT_H
