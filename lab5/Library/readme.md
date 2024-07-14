# 图书馆管理系统
## 应用技术
C++、Qt、MySQL、RFID
## 项目描述
用RFID电子标签标识读者和书籍，Qt实现管理员和读者不同的GUI和事务功能，管理员负责发卡、销卡及图书绑定标签，读者可挂失、解挂、借还图书、查询借书明细。
## 主要工作
- 基于ISO15693串口通讯协议的电子标签和13.56M读写器实现；
- 采用Client / Server架构，客户端提供图形化操作界面，云服务器部署MySQL数据库；
- 继承QThread、QSerialPort实现串口操作线程，封装QtSql相关函数提供数据库操作接口；
- 静态链接封装串口协议的库文件，串口线程利用信号槽机制，发送数据到主线程。