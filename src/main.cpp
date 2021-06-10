
#include <SPI.h>
#include <Ticker.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "led_display.h"
#include "DS3231MZ.h"
#include "DallasTemperature.h"
#include "ws2812b.h"
#include "softwifi.h"
#include "eps_flash.h"
 


char ssid[] = "";                    // your network SSID (name)
char pass[] = "";                    // your network password

unsigned int z_PosX = 0;                            
unsigned int d_PosX = 0;                            
bool f_tckr1s = false;
bool f_tckr50ms = false;
bool f_tckrhalfmins = false;
unsigned long epoch = 0;                   // local port to listen for UDP packets

uint8_t sleep_flag = 0;
uint8_t clock_flag = 1;


const char* ntpServerName = "time1.aliyun.com";
const int NTP_PACKET_SIZE = 48;                     // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];                 // buffer to hold incoming and outgoing packets
IPAddress timeServerIP;                            

// The object for the Ticker
Ticker tckr;

//temperature ds18b20 
#define ONE_WIRE_BUS 0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float now_temperature;


//***********读取网络时钟***************************************************************************************
tm* connectNTP() { //if response from NTP was succesfull return *tm else return a nullpointer
    WiFi.hostByName(ntpServerName, timeServerIP);
    Serial.println(timeServerIP);
    Serial.println("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(timeServerIP, 123); //NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
    delay(1000);                 // wait to see if a reply is available
    int cb = Udp.parsePacket();
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    // now convert NTP time into everyday time:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    epoch = secsSince1900 - seventyYears + 3600*8 + 2; //+2000ms Verarbeitungszeit
    //epoch=epoch-3600*6; // difference -6h = -6* 3600 sec)
    time_t t;
    t = epoch;
    tm* tt;
    tt = localtime(&t);
    Serial.println(epoch);
    Serial.println(asctime(tt)); 
    if (cb == 48)
        return (tt);
    else
        return (NULL);
}

//**************************************************************************************************
void timer50ms() {
    static unsigned int cnt50ms = 0;
    f_tckr50ms = true;
    cnt50ms++;
    if (cnt50ms%20 == 0) {
        f_tckr1s = true; // 1 sec
    }
    if (cnt50ms/600 ==1){  //30 sec
        f_tckrhalfmins = true;
        cnt50ms = 0;
    }
}

//**************************************************************************************************
//
//The setup function is called once at startup of the sketch
void setup() {
    // Add your initialization code here
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);
    Serial.begin(115200);
    SPI.begin(); 
    helpArr_init();
    max7219_init();
    rtc_init(SDA, SCL);
    sensors.begin(); //temperature  begin
    clear_Display();
    refresh_display(); //take info into LEDarr
    tckr.attach(0.05, timer50ms);    // every 50 msec
    ReLoadData();
    if(running_flag&0x8){
        sleep_flag = 1;
    }else{
        sleep_flag = 0;
    }
    if(running_flag&0x2){
        clock_flag = 1;
    }else{
        clock_flag = 0;
    }
    //////////////////////////////////
    if (!autoConfig())
    {
       smartConfig();
    }
    ///////////////////////////////////
    tm* tt;
    tt = connectNTP();
    if (tt != NULL)
        rtc_set(tt);
    else
        Serial.println("no timepacket received");
   //   Serial.end();
     led_init();   //初始化background led
     show_background();
}


//**************************************************************************************************
// The loop function is called in an endless loop
void loop() {
    //Add your repeated code here
    unsigned int sek1 = 0, sek2 = 0, min1 = 0, min2 = 0, std1 = 0, std2 = 0;
    unsigned int sek11 = 0, sek12 = 0, sek21 = 0, sek22 = 0;
    unsigned int min11 = 0, min12 = 0, min21 = 0, min22 = 0;
    unsigned int std11 = 0, std12 = 0, std21 = 0, std22 = 0;
    signed int x = 0; //x1,x2;
    signed int y = 0, y1 = 0, y2 = 0, y3=0;
    bool updown = false;
    unsigned int sc1 = 0, sc2 = 0, sc3 = 0, sc4 = 0, sc5 = 0, sc6 = 0;
    bool f_scrollend_y = false;
    unsigned int f_scroll_x = false;

    z_PosX = maxPosX;
    d_PosX = -8;
    //  x=0; x1=0; x2=0;
    sensors.requestTemperatures();
    now_temperature = sensors.getTempCByIndex(0); //获取温度值
    refresh_display();
    updown = true;
    if (updown == false) {
        y2 = -9;
        y1 = 8;
    }
    if (updown == true) { //scroll  up to down
        y2 = 8;
        y1 = -8;
    }
    while (true) {
     
        //show();
        yield();  //让线程回到可运行状态
        if(socket_listening()!=0){
            if(RecBuffer[16]==0x01){
                send_hand();
                //do something
            }else if(RecBuffer[16]==0x02){
                running_flag = RecBuffer[17]; // 第一位 背光mod选择 第二位时钟开关选择 第三位 背光开关选择 第四位睡眠mod开关选择
                backgroun_brightness = RecBuffer[18];
                SetR = RecBuffer[19];
                SetG = RecBuffer[20];
                SetB = RecBuffer[21];
                SetHour_H =  RecBuffer[22];
                SetMin_H =  RecBuffer[23];
                SetSec_H = RecBuffer[24];
                SetHour_L = RecBuffer[25];
                SetMin_L =  RecBuffer[26];
                SetSec_L =  RecBuffer[27];
                SetBackground(SetR,SetG,SetB);
                SetBackgroundBrightness(backgroun_brightness);
                if(running_flag&0x8){
                    sleep_flag = 1;
                }else{
                    sleep_flag = 0;
                }
                if(running_flag&0x2){
                    clock_flag = 1;
                }else{
                    clock_flag = 0;
                }
            }else if(RecBuffer[16]==0x03){
                 Serial.println("Start Write Flash!");
                 WriteFlash();
                 send_hand();
            }
        }
        if ( MEZ.std12==0 && MEZ.min12==20 && MEZ.sek12==0 ) //syncronisize RTC every day 00:20:00
        { 
            clear_Display();
            delay(500);
            ESP.restart();
        }
        if (f_tckr1s == true)        // flag 1sek
        {
            rtc2mez();
            sek1 = MEZ.sek1;
            sek2 = MEZ.sek2;
            min1 = MEZ.min1;
            min2 = MEZ.min2;
            std1 = MEZ.std1;
            std2 = MEZ.std2;
            y = y2;                 //scroll updown
            sc1 = 1;
            sek1++;
            if (sek1 == 10) {
                sc2 = 1;
                sek2++;
                sek1 = 0;
            }
            if (sek2 == 6) {
                min1++;
                sek2 = 0;
                sc3 = 1;
            }
            if (min1 == 10) {
                min2++;
                min1 = 0;
                sc4 = 1;
            }
            if (min2 == 6) {
                std1++;
                min2 = 0;
                sc5 = 1;
            }
            if (std1 == 10) {
                std2++;
                std1 = 0;
                sc6 = 1;
            }
            if ((std2 == 2) && (std1 == 4)) {
                std1 = 0;
                std2 = 0;
                sc6 = 1;
            }

            sek11 = sek12;
            sek12 = sek1;
            sek21 = sek22;
            sek22 = sek2;
            min11 = min12;
            min12 = min1;
            min21 = min22;
            min22 = min2;
            std11 = std12;
            std12 = std1;
            std21 = std22;
            std22 = std2;
            f_tckr1s = false;
            if (MEZ.sek12 == 45)
                f_scroll_x = true;//滚动开关
            Serial.printf("\r\nR:%d G:%d B:%d",SetR,SetG,SetB);
            Serial.printf("\r\nset:%d now:%d set:%d now:%d set:%d now:%d",SetHour_L,MEZ.std12,SetMin_L,MEZ.min12,SetSec_L,MEZ.sek12);
            if(sleep_flag){
                if((SetHour_L == MEZ.std12)&&(SetMin_L == MEZ.min12)&&(SetSec_L >= MEZ.sek12)){
                    running_flag &= 0xfD;
                    WriteFlash();
                    clock_flag = 0;
                }else if((SetHour_H == MEZ.std12)&&(SetMin_H == MEZ.min12)&&(SetSec_H >= MEZ.sek12)){
                    running_flag |= 0x2;
                    WriteFlash();
                    clock_flag = 1;
                }
            }
        } // end 1s
        if (f_tckr50ms == true) {
            f_tckr50ms = false;
            if(clock_flag){
                show_background();
                if (f_scroll_x == true) {
                    z_PosX++;
                    d_PosX++;
                    if (d_PosX == 101)
                        z_PosX = 0;
                    if (z_PosX == maxPosX) {
                        f_scroll_x = false;
                        d_PosX = -8;
                    }
                }
                if (sc1 == 1) {
                    if (updown == 1)
                        y--;
                    else
                        y++;
                y3 = y;
                if (y3 > 0) {
                    y3 = 0;
                    }     
                char22Arr(48 + sek12, z_PosX - 27, y3);
                char22Arr(48 + sek11, z_PosX - 27, y + y1);
                    if (y == 0) {
                        sc1 = 0;
                        f_scrollend_y = true;
                    }
                }
                else
                    char22Arr(48 + sek1, z_PosX - 27, 0);

                if (sc2 == 1) {
                    char22Arr(48 + sek22, z_PosX - 23, y3);
                    char22Arr(48 + sek21, z_PosX - 23, y + y1);
                    if (y == 0)
                        sc2 = 0;
                }
                else
                char22Arr(48 + sek2, z_PosX - 23, 0);

                if (sc3 == 1) {
                    char2Arr(48 + min12, z_PosX - 18, y);
                    char2Arr(48 + min11, z_PosX - 18, y + y1);
                    if (y == 0)
                        sc3 = 0;
                }
                else
                    char2Arr(48 + min1, z_PosX - 18, 0);

                if (sc4 == 1) {
                    char2Arr(48 + min22, z_PosX - 13, y);
                    char2Arr(48 + min21, z_PosX - 13, y + y1);
                    if (y == 0)
                        sc4 = 0;
                }
                else
                    char2Arr(48 + min2, z_PosX - 13, 0);

                char2Arr(':', z_PosX - 10 + x, 0);

                if (sc5 == 1) {
                    char2Arr(48 + std12, z_PosX - 4, y);
                    char2Arr(48 + std11, z_PosX - 4, y + y1);
                    if (y == 0)
                        sc5 = 0;
                }
                else
                    char2Arr(48 + std1, z_PosX - 4, 0);

                if (sc6 == 1) {
                    char2Arr(48 + std22, z_PosX + 1, y);
                    char2Arr(48 + std21, z_PosX + 1, y + y1);
                    if (y == 0)
                        sc6 = 0;
                }
                else
                    char2Arr(48 + std2, z_PosX + 1, 0);

                char2Arr(' ', d_PosX+5, 0);        //day of the week
                char2Arr(WT_arr[MEZ.WT][0], d_PosX - 1, 0);        //day of the week
                char2Arr(WT_arr[MEZ.WT][1], d_PosX - 7, 0);
                char2Arr(WT_arr[MEZ.WT][2], d_PosX - 13, 0);
                char2Arr(WT_arr[MEZ.WT][3], d_PosX - 19, 0);
                char2Arr(48 + MEZ.tag2, d_PosX - 24, 0);           //day
                char2Arr(48 + MEZ.tag1, d_PosX - 30, 0);
                char2Arr(M_arr[MEZ.mon12 - 1][0], d_PosX - 39, 0); //month
                char2Arr(M_arr[MEZ.mon12 - 1][1], d_PosX - 43, 0);
                char2Arr(M_arr[MEZ.mon12 - 1][2], d_PosX - 49, 0);
                char2Arr(M_arr[MEZ.mon12 - 1][3], d_PosX - 55, 0);
                char2Arr(M_arr[MEZ.mon12 - 1][4], d_PosX - 61, 0);
                char2Arr('2', d_PosX - 68, 0);                     //year
                char2Arr('0', d_PosX - 74, 0);
                char2Arr(48 + MEZ.jahr2, d_PosX - 80, 0);
                char2Arr(48 + MEZ.jahr1, d_PosX - 86, 0);

                refresh_display(); //alle 50ms
                if (f_scrollend_y == true) {
                    f_scrollend_y = false;
                }
            }else{
                clear_Display();
            }
        } //end 50ms
        if(f_tckrhalfmins){
             f_tckrhalfmins=false;
              sensors.requestTemperatures();
              now_temperature = sensors.getTempCByIndex(0); //获取温度值
              Serial.printf("\r\ntemperature:%0.01f \r\n",now_temperature);
          }//end 30sec
        if (y == 0) {
            // do something else
        }
        //======================
        //======================
        }  //end while(true)
    //this section can not be reached
}
