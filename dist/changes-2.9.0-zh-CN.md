# WizNotePlus v2.9.0 更新日志

## 系统要求

预编译的 WizNotePlus 软件包基于 Qt 5.14.2 构建，最低系统需求如下：

* Linux 最低要求 GLIBC_2.17
* MacOS 最低要求 macOS 10.13
* Windows 最低要求 Windows 7

## 新的特征

* 增加鼠标中键关闭选项卡功能。
* 增加目录视图“显示子文件夹笔记”功能。#113 #157

## 已知问题

- Windows 平台外部编辑器 Typora 保存时，有时会出现 WizNote 不同步更新的状况。#88
- 外部编辑器或者单独编辑窗口保存时会导致主界面窗口被激活并在最上层显示。#141
- MacOS 下“显示系统托盘图标”设置会导致程序崩溃。#76
- 笔记标注在不同平台的客户端上会有位置偏差。