#ifndef __LED_DISPLAY_H_
#define __LED_DISPLAY_H_

#include "max7219_driver.h"
#include "fontcode.h"

#ifdef anzMAX
extern unsigned short maxPosX;            
extern unsigned short LEDarr[anzMAX][8];                   
extern unsigned short helpArrMAX[anzMAX * 8];              
extern unsigned short helpArrPos[anzMAX * 8];          
#endif

//months
extern char M_arr[12][5];//days
extern char WT_arr[7][4];


void helpArr_init(void);
void clear_Display();
void rotate_90();
void refresh_display();
void char2Arr(unsigned short ch, int PosX, short PosY);
void char22Arr(unsigned short ch, int PosX, short PosY);



#endif