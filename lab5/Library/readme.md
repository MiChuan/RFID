# 图书馆管理系统

[![C++](https://img.shields.io/badge/C%2B%2B-Qt-blue.svg)](https://www.qt.io/)
[![MySQL](https://img.shields.io/badge/DB-MySQL-4479A1.svg)](https://www.mysql.com/)
[![RFID](https://img.shields.io/badge/RFID-ISO15693%2F13.56M-8A2BE2.svg)](https://en.wikipedia.org/wiki/ISO/IEC_15693)
[![Arch](https://img.shields.io/badge/Arch-Client%2FServer-0078D6.svg)](https://en.wikipedia.org/wiki/Client%E2%80%93server_model)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](../../LICENSE)

本项目所属仓库基于 MIT License 开源发布，欢迎在遵守许可证条款的前提下自由使用、修改与再发布。

## 目录导航

- [项目简介](#introduction)
- [功能特性](#features)
- [技术架构](#architecture)
- [目录结构](#structure)
- [环境要求](#requirements)
- [快速开始](#quick-start)
- [常见问题](#faq)
- [开源声明与版权归属](#open-source-statement--copyright)
- [其他说明](#notes)

<a id="introduction"></a>
## 项目简介

图书馆管理系统是 RFID 课程综合实验项目，利用 RFID 电子标签标识读者与图书，基于 Qt（C++）实现管理员与读者两种角色的图形化界面与事务功能。管理员负责读者发卡、销卡以及图书与标签绑定；读者可完成挂失、解挂、借书、还书与借阅明细查询等操作。

系统采用 Client / Server 架构，客户端提供图形化操作界面，云服务器部署 MySQL 数据库，并通过 13.56M（ISO15693）读写器与电子标签交互。

<a id="features"></a>
## 功能特性

### 管理员端

- **读者发卡 / 销卡**：为读者发放 RFID 借书卡，支持注销
- **图书绑定**：将 RFID 电子标签与图书信息绑定
- **读者信息查看**：查看读者及图书绑定信息

### 读者端

- **借书 / 还书**：基于 RFID 标签快速完成借还操作
- **挂失 / 解挂**：借书卡丢失后可挂失，找回后解挂
- **借阅明细查询**：查询个人借阅记录
- **还书提醒**：按配置时间自动提醒待还图书（提醒时间可在 `tools/config.ini` 配置）

### 系统特性

- 基于 ISO15693 串口通讯协议，通过 13.56M 读写器读写电子标签
- Client / Server 架构，MySQL 数据库部署于云服务器
- 继承 `QThread`、`QSerialPort` 实现串口操作线程，通过信号槽机制与主线程通信
- 封装 QtSql 相关函数提供统一数据库操作接口，账号密码以 MD5 形式存储

<a id="architecture"></a>
## 技术架构

```text
13.56M 读写器 / RFID 电子标签（ISO15693）
        │ 串口通讯（QSerialPort 线程）
        ▼
Qt 客户端（Library）
 ├─ pages/      界面页面（登录 / 发卡 / 借还 / 挂失 / 查询等）
 ├─ tools/      串口线程、用户账号、配置
 └─ database/   DBHelper 数据库封装（QMYSQL，单例模式）
        │ QtSql
        ▼
云服务器 MySQL（数据库 Library）
```

<a id="structure"></a>
## 目录结构

```text
Library/
├── database/            # 数据库封装（dbhelper，QMYSQL 单例）
├── inc/                 # 13.56M 读写器 SDK 头文件（m1356dll）
├── lib/                 # 13.56M 读写器 SDK 静态库 / 动态库（M1356Dll）
├── pages/               # 界面页面（登录、注册、发卡、借还、挂失、解挂、查询、欢迎等）
├── tools/               # 串口线程、用户账号、工具函数与配置文件
├── images/              # 界面图标资源
├── main.cpp             # 程序入口
├── mainwindow.cpp/h/ui  # 主窗口
├── pics.qrc             # 资源文件
├── Library.pro          # qmake 工程文件
└── readme.md            # 项目说明（本文件）
```

<a id="requirements"></a>
## 环境要求

- Qt 5.x（含 Qt SQL、Qt SerialPort 模块）
- MySQL 5.x 及 Qt QMYSQL 驱动（云服务器部署）
- 13.56M ISO15693 读写器与电子标签，配套 M1356Dll（`inc/`、`lib/` 已随仓库提供）
- Windows 环境（SDK 动态库为 Win32 版本）

<a id="quick-start"></a>
## 快速开始

1. 使用 Qt Creator 打开 `Library.pro` 工程
2. 配置数据库：修改 `database/dbhelper.cpp` 中的主机、端口（默认 3306）、库名（`Library`）与账号密码，并确保 Qt 的 QMYSQL 驱动可用
3. 构建并运行：管理员账号信息维护于 `tools/config.ini`（密码以 MD5 存储）
4. 连接 13.56M 读写器，通过串口线程完成标签读取与绑定操作

<a id="faq"></a>
## 常见问题

### 1. 编译报错，找不到 M1356Dll

请确认 `inc/` 与 `lib/` 目录存在且完整，`Library.pro` 中的 `LIBS += -L$$PWD/lib/ -lM1356Dll` 链接配置正确。

### 2. 数据库连接失败

请检查 `database/dbhelper.cpp` 中的服务器地址、端口、库名与账号密码，并确认 Qt 已正确配置 QMYSQL 驱动（缺少驱动会提示 "driver not loaded"）。

### 3. 读写器无响应

请检查读写器是否已连接、串口参数是否匹配，并确认电子标签位于 13.56M 天线感应范围内。

### 4. 登录失败

登录账号信息维护于 `tools/config.ini`，密码以 MD5 形式存储，请核对账号与密码是否匹配。

<a id="open-source-statement--copyright"></a>
## 开源声明与版权归属

本项目所属仓库基于 MIT License 开源发布（详见仓库根目录 [LICENSE](../../LICENSE)）。在保留原始署名与许可证声明的前提下，使用者可以自由复制、修改、分发、再发布，或用于个人学习与二次开发。

若将本项目代码用于课程设计、毕业设计展示或二次开发，建议保留原作者信息、仓库地址及许可说明，以尊重原始创作贡献。

<a id="notes"></a>
## 其他说明

- 本项目为 RFID 课程综合实验，数据库连接参数与示例账号为实验环境配置，请按实际情况修改，勿提交真实凭据。
- 实验代码基于读写器厂商提供的 SDK 动态库（M1356Dll）开发，使用时请遵守厂商相关许可。
- 系统以 Qt 页面 + 串口线程 + 数据库封装组织代码，适合学习 Qt 多线程、串口通信与 RFID 应用集成。
