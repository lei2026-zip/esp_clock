#ifndef _ESP_FLASH_H_
#define _ESP_FLASH_H_

#include <ESP8266WiFi.h>
#include "ws2812b.h"

extern uint8_t SetHour_H;  //开启时间
extern uint8_t SetSec_H;
extern uint8_t SetMin_H;
extern uint8_t SetHour_L;  //睡眠时间
extern uint8_t SetSec_L;
extern uint8_t SetMin_L;


#define  SPI_FLASH_SEC         513    //0-1060  
//#define SPI_FLASH_SEC_SIZE      4096  //内部已定义
#define FLASH_SIZE    24     //byte
void WriteFlash();
void ReLoadData();

#endif