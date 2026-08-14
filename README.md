# 华中科技大学物联网工程射频识别实验（HUST-IOT-RFID-LAB）

[![C++](https://img.shields.io/badge/C%2B%2B-Qt-blue.svg)](https://www.qt.io/)
[![RFID](https://img.shields.io/badge/RFID-LF%2FHF%2FUHF-8A2BE2.svg)](https://en.wikipedia.org/wiki/Radio-frequency_identification)
[![MySQL](https://img.shields.io/badge/DB-MySQL-4479A1.svg)](https://www.mysql.com/)
[![SQLite](https://img.shields.io/badge/DB-SQLite-003B57.svg)](https://www.sqlite.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![GitHub stars](https://img.shields.io/github/stars/MiChuan/RFID?style=social)](https://github.com/MiChuan/RFID/stargazers)

本项目基于 MIT License 开源发布，欢迎在遵守许可证条款的前提下自由使用、修改与再发布。

## 目录导航

- [项目简介](#introduction)
- [实验列表](#experiments)
- [目录结构](#structure)
- [环境要求](#requirements)
- [快速开始](#quick-start)
- [常见问题](#faq)
- [开源声明与版权归属](#open-source-statement--copyright)
- [其他说明](#notes)

<a id="introduction"></a>
## 项目简介

本仓库为华中科技大学物联网工程专业射频识别（RFID）课程实验集合，覆盖 **低频（125K）**、**高频（13.56M，ISO14443A / ISO15693）** 与 **超高频（900M）** 三类频段读写器的应用开发，并以"图书馆管理系统"作为综合实验，完整体现 RFID 技术在识别与业务管理中的应用。

全部实验均基于 Qt（C++）开发，采用厂商 SDK 动态库与串口通信完成读写器交互，可独立编译运行，适合 RFID 课程学习、课程设计与二次开发参考。

<a id="experiments"></a>
## 实验列表

| 实验 | 频段 / 协议 | 实验内容 |
|------|------------|----------|
| lab1 | 低频 LF-125K | 低频读写器实验：125K 读写器 Qt 示例程序（M125KDemo），实现卡片识别与记录展示 |
| lab2 | 高频 ISO14443A | 高频读写器实验：一卡通系统（One_Card_System），覆盖注册、充值、消费、注销、修改密码、数据表查询与数据导出 |
| lab3 | 高频 ISO15693 | 高频读写器实验：图书管理原型，实现图书管理、借还操作与用户管理（SQLite） |
| lab4 | 超高频 UHF-900M | 超高频读写器实验：900M 模块应用开发，附模块 API 说明与实验案例指导文档 |
| lab5 | 综合实验 | 图书馆管理系统：以 RFID 电子标签标识读者与图书，Qt + MySQL 实现完整的借阅管理（详见 [lab5/Library/readme.md](lab5/Library/readme.md)） |

<a id="structure"></a>
## 目录结构

```text
RFID/
├── lab1/
│   └── 125K/M125KDemo/            # 低频（LF-125K）读写器示例程序
│       ├── inc/ lib/              # M125Dll 头文件与静态库 / 动态库
│       ├── database.*             # 数据库封装（SQLite）
│       └── M125KDemo.pro          # qmake 工程文件
├── lab2/
│   └── 13.56M/14443/One_Card_System/  # 高频（ISO14443A）一卡通系统
│       ├── pages/                 # 登录、注册、充值、消费、注销等页面
│       ├── database/              # 数据库模型封装（SQLite）
│       ├── serialThread/          # 串口通信线程
│       ├── inc/ lib/              # M1356Dll 头文件与静态库 / 动态库
│       └── One_Card_System.pro    # qmake 工程文件
├── lab3/
│   └── 1356M/                     # 高频（ISO15693）图书管理原型
│       ├── booksmanage.*          # 图书管理
│       ├── borrow_return.*        # 借还操作
│       ├── usermanage.*           # 用户管理
│       ├── sqlite.*               # SQLite 封装
│       ├── uhf_thread.*           # 读写器线程
│       ├── inc/ lib/              # M1356Dll 头文件与静态库 / 动态库
│       └── 1356M.pro              # qmake 工程文件
├── lab4/
│   └── 900M/                      # 超高频（UHF-900M）模块应用
│       ├── DOC/                   # 900M 模块 API 说明与实验案例指导书
│       └── SRC/900M/              # 900M 示例程序（含 M900Dll）
├── lab5/
│   └── Library/                   # 综合实验：图书馆管理系统
│       ├── pages/                 # 登录、发卡、借还、挂失、查询等页面
│       ├── database/              # 数据库封装（MySQL）
│       ├── tools/                 # 串口线程、用户账号与配置
│       ├── inc/ lib/              # M1356Dll 头文件与静态库 / 动态库
│       ├── Library.pro            # qmake 工程文件
│       └── readme.md              # 项目说明
├── LICENSE                        # MIT 许可证
└── README.md                      # 项目说明（本文件）
```

<a id="requirements"></a>
## 环境要求

- Qt 5.x（qmake 构建，含 Qt SQL / Qt SerialPort 模块）
- Windows 环境（读写器厂商 SDK 动态库为 Win32 版本）
- 数据库：lab1 / lab2 / lab3 / lab4 使用 SQLite；lab5 使用 MySQL（需 QMYSQL 驱动）
- 硬件：对应频段的 RFID 读写器（125K / 13.56M / 900M）及配套电子标签

<a id="quick-start"></a>
## 快速开始

```bash
git clone https://github.com/MiChuan/RFID.git
cd RFID
```

1. 使用 Qt Creator 打开对应实验的 `.pro` 工程文件（如 `lab5/Library/Library.pro`）
2. 各实验所需读写器 SDK 动态库（`M125Dll` / `M1356Dll` / `M900Dll`）已随仓库提供于对应 `lib/` 目录
3. lab5 需在 `database/dbhelper.cpp` 中配置 MySQL 服务器地址、端口、库名与账号密码；其余实验使用 SQLite，无需额外配置
4. 连接读写器硬件后编译运行，按各实验界面进行操作

<a id="faq"></a>
## 常见问题

### 1. 编译报错，找不到 SDK 库

请确认对应实验的 `inc/` 与 `lib/` 目录完整（`M125Dll` / `M1356Dll` / `M900Dll` 的头文件与库文件），并检查 `.pro` 文件中的链接路径是否与本机一致。

### 2. 数据库连接失败

lab1–lab4 使用 SQLite，一般无需配置；lab5 使用 MySQL，请检查 `database/dbhelper.cpp` 中的主机、端口（默认 3306）、库名 `Library` 与账号密码，并确保 Qt 已启用 QMYSQL 驱动。

### 3. 读写器无响应

请检查读写器是否已连接串口、电子标签是否靠近天线，并确认使用的 SDK 动态库与读写器型号、频段匹配。

### 4. 各实验之间有什么关系

五个实验相互独立，lab1–lab4 分别覆盖低频、高频（两种协议）与超高频读写器的应用开发，lab5 为综合实验，将 RFID 读写能力与业务系统（图书馆借阅管理）结合。

<a id="open-source-statement--copyright"></a>
## 开源声明与版权归属

本项目基于 MIT License 开源发布（详见 [LICENSE](LICENSE)）。在保留原始署名与许可证声明的前提下，使用者可以自由复制、修改、分发、再发布，或用于个人学习与二次开发。

若将本项目代码用于课程设计、毕业设计展示或二次开发，建议保留原作者信息、仓库地址及许可说明，以尊重原始创作贡献。

<a id="notes"></a>
## 其他说明

- 本仓库为 RFID 课程实验代码集合，实验代码基于读写器厂商提供的 SDK 动态库开发，使用时请遵守厂商相关许可。
- 仓库中包含部分编译产物（DLL、可执行文件、中间文件等），可直接参考运行结果或按需清理。
- 各实验中的数据库连接参数与示例账号为实验环境配置，请按实际情况修改。
