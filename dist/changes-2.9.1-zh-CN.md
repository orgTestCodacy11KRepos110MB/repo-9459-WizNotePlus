# WizNotePlus v2.9.1 更新日志

## 系统要求

预编译的 WizNotePlus 软件包基于 Qt 5.14.2 构建，最低系统需求如下：

* Linux 最低要求 GLIBC_2.17
* MacOS 最低要求 macOS 10.13
* Windows 最低要求 Windows 7

## 问题修复

* 修复编辑器工具栏按钮图标不更新的问题。#192 by @notplus
* 修复 Linux 端系统托盘图标消息通知的错误行为。#194
* 修复外部编辑器或者单独笔记窗口保存时主窗口被激活从而打断编辑体验的问题。#141
* 修复单独编辑窗口关闭时自动保存流程没有触发内容变更信号的问题。#141
* 修复外部编辑器启动和保存时的一些问题。#199
* 修复无法渲染 lite/markdown 类型笔记的问题。#198
* 修复内部编辑器 “丢弃修改” 功能失效的问题。

## 已知问题

* Windows 平台外部编辑器 Typora 保存时，有时会出现 WizNote 不同步更新的状况。#88
* MacOS 下“显示系统托盘图标”设置会导致程序崩溃。#76
* 笔记标注在不同平台的客户端上会有位置偏差。
* 单独笔记窗口在关闭时，偶尔会无法自动保存笔记内容。