#ifndef HT1621B_CONFIG_H
#define HT1621B_CONFIG_H

/*
 * HT1621B共使用了3个引脚，分别是CS、WR、DATA
 * 此处用于配置这些引脚与MCU的引脚连接关系
 */
 
#define HT1621_GPIO_PORT GPIOB

#define HT1621_CS_Pin GPIO_PIN_10
#define HT1621_WR_Pin GPIO_PIN_11
#define HT1621_DATA_Pin GPIO_PIN_12


#endif // HT1621B_CONFIG_H
