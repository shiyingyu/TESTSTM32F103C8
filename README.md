# TESTSTM32F103C8
A stm32f103c8 dev-board test project, with verbose comment to get easy start

本项目用于测试STM32F103C8核心板及基础组件的配置和使用，例程及详细注释可以作为开发参考。

## 主要模块

* 外部EEPROM
* 外部Flash
* 串口打印输出
* 以太网接口（W5500和ENC28J60）
* 4G连网
* 段码屏

## 段码屏的使用
所需文件

* ht1621b_config.h 在这个文件中修改段码屏驱动芯片的CS、WR、DATA管脚与MCU的连接关系
* ht1621b.h 在main.c中包含这个头文件
* ht1621b.c 将这个文件加入到工程

段码屏驱动程序会在初始化时自己初始化MCU的GPIO管脚，所以不需要在CubeMX中配置GPIO。
