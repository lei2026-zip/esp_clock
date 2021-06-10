#ifndef __MAX7219_DRIVER_H_
#define __MAX7219_DRIVER_H_

#include <SPI.h>

#define CS         15     // Pin cs  (SPI)
#define anzMAX     4      
const uint8 brightness = 0x00;  //亮度 0xx0-0xxf

void max7219_init(void);  //all MAX7219 init
void max7219_set_brightness(unsigned short br);  //brightness MAX7219 亮度

#endif