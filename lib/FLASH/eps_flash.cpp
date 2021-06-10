#include "eps_flash.h"

const uint32_t FLASH_ADDR = SPI_FLASH_SEC*SPI_FLASH_SEC_SIZE;

uint8_t SetHour_H = 0;  //开启时间
uint8_t SetSec_H = 0;
uint8_t SetMin_H = 0;
uint8_t SetHour_L = 0;  //睡眠时间
uint8_t SetSec_L = 0;
uint8_t SetMin_L = 0;


//需四字节对齐
void WriteFlash(){
    // uint16_t i,len=lenght/4;
     uint32 buf[FLASH_SIZE/4]={0};
    // for(i=0;i<lenght;i<len;i++){
    //     buf[i] =(data[i*4+3]<<24)|(data[i*4+2]<<16)|(data[i*4+1]<<8)|data[i*4];
    // }
     buf[0] =  running_flag + (backgroun_brightness<<8);
     buf[1] = SetR | (SetG<<8) | (SetB<<16);
     buf[2] = SetHour_H | (SetMin_H<<8) | (SetSec_H<<16);
     buf[3] = SetHour_L | (SetMin_L<<8) | (SetSec_L<<16);
     spi_flash_erase_sector(SPI_FLASH_SEC);   //写入之前先擦除扇区
     SpiFlashOpResult result;
     result = spi_flash_write(FLASH_ADDR,buf,FLASH_SIZE);
     if(result!=SPI_FLASH_RESULT_OK){
         Serial.println("Flash Write Error!");
     }else{
          Serial.println("Flash Write Ok!");
     }
}

void ReLoadData(){
    uint32_t buf[FLASH_SIZE/4];
    spi_flash_read(FLASH_ADDR,buf,FLASH_SIZE);
    Serial.println("buff:");
    Serial.println(buf[0]);
    SetR = buf[1]&0xff;
    SetG =  (buf[1]>>8)&0xff;
    SetB =  (buf[1]>>16)&0xff;
    running_flag = buf[0]&0xff;
    backgroun_brightness = (buf[0]>>8)&0xff;
    SetHour_H = buf[2]&0xff;
    SetMin_H =  (buf[2]>>8)&0xff;
    SetSec_H =  (buf[2]>>16)&0xff;
    SetHour_L = buf[3]&0xff;
    SetMin_L =  (buf[3]>>8)&0xff;
    SetSec_L =  (buf[3]>>16)&0xff;
}