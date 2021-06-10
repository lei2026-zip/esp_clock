#ifndef __DS3231MZ_H_
#define __DS3231MZ_H_

#include <Wire.h>
#include <time.h>
#define DS3231_ADDRESS 0x68
#define secondREG  0x00
#define minuteREG  0x01
#define hourREG  0x02
#define WTREG  0x03                  //weekday
#define dateREG  0x04
#define monthREG  0x05
#define yearREG  0x06
#define alarm_min1secREG  0x07
#define alarm_min1minREG  0x0
#define alarm_min1hrREG  0x09
#define alarm_min1dateREG  0x0A
#define alarm_min2minREG  0x0B
#define alarm_min2hrREG  0x0C
#define alarm_min2dateREG  0x0D
#define controlREG  0x0E
#define statusREG  0x0F
#define ageoffsetREG  0x10
#define tempMSBREG  0x11
#define tempLSBREG  0x12
#define _24_hour_format  0
#define _12_hour_format  1
#define AM  0
#define PM  1

#define SDA     5      // Pin sda (I2C)
#define SCL     4      // Pin scl (I2C)

extern tm *tt, ttm;

typedef struct Dtime {
    unsigned short sek1, sek2, sek12, min1, min2, min12, std1, std2, std12;
    unsigned short tag1, tag2, tag12, mon1, mon2, mon12, jahr1, jahr2, jahr12, WT;
} DateTime;

extern DateTime MEZ;

void rtc_init(unsigned char sda, unsigned char scl);
unsigned char dec2bcd(unsigned char x);
unsigned char bcd2dec(unsigned char x);
unsigned char rtc_Read(unsigned char regaddress);
void rtc_Write(unsigned char regaddress, unsigned char value);
unsigned char rtc_sekunde();
unsigned char rtc_minute();
unsigned char rtc_stunde();
unsigned char rtc_wochentag();
unsigned char rtc_tag();
unsigned char rtc_monat();
unsigned char rtc_jahr();
void rtc_sekunde(unsigned char sek);
void rtc_minute(unsigned char min);
void rtc_stunde(unsigned char std);
void rtc_wochentag(unsigned char wt);
void rtc_tag(unsigned char tag);
void rtc_monat(unsigned char mon);
void rtc_jahr(unsigned char jahr);
void rtc_set(tm* tt);
float rtc_temp();
void rtc2mez();



#endif
