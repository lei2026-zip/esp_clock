#include "softwifi.h"

//**************************************************************************************************
bool autoConfig()
{
    WiFi.begin();                    // 默认连接保存的WIFI

    for (int i = 0; i < 10; i++)
    {
       char2Arr('W', 28, 0);
       char2Arr('i', 22, 0);
       char2Arr('-', 18, 0);
       char2Arr('F', 12, 0);
       char2Arr('i', 6, 0);

       refresh_display(); 
       
       if (WiFi.status() == WL_CONNECTED)
       {
          Serial.println("AutoConfig Success");
          Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
          Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
          clear_Display();
          char2Arr('O', 25, 0);
          char2Arr('K', 19, 0);
          char2Arr('!', 12, 0);
          char2Arr('!', 6, 0);
          refresh_display(); 

          Serial.println("WiFi connected");
          Serial.println(WiFi.localIP());
          Serial.println("Starting UDP");
          Udp.begin(LocalPort);
          Serial.print("Local port: ");
          Serial.println(Udp.localPort());
          return true;
       }
       else{
          Serial.print("AutoConfig Waiting......");
          Serial.println(WiFi.status());
          delay(1000);
       }
    }
    clear_Display();
    char2Arr('E', 25, 0);
    char2Arr('r', 19, 0);
    char2Arr('r', 12, 0);
    char2Arr('!', 6, 0);
    refresh_display(); 
    delay(1000);
    Serial.println("AutoConfig Faild!" );
    return false;
}
//**************************************************************************************************
void smartConfig()
{
    int i = 0;
    WiFi.mode(WIFI_STA);
    Serial.println("\r\nWait for Smartconfig");
    WiFi.beginSmartConfig();
    for (i = 0; i < 30; i++)
    {
       Serial.print(".");
       if(WiFi.smartConfigDone())
       {
          clear_Display();
          char2Arr('O', 25, 0);
          char2Arr('K', 19, 0);
          char2Arr('!', 12, 0);
          char2Arr('!', 6, 0);
          refresh_display(); 
          Serial.println("SmartConfig Success");
          Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
          Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
          WiFi.setAutoConnect(true);  // 设置自动连接
          
          Serial.println("WiFi connected");
          Serial.println(WiFi.localIP());
          Serial.println("Starting UDP");
          Udp.begin(LocalPort);
          Serial.print("Local port: ");
          Serial.println(Udp.localPort());
          delay(1000); 
          ESP.restart();
          break;
       }
       clear_Display();
       char2Arr('S', 29, 0);
       char2Arr('-', 23, -1);
       char2Arr('c', 17, 0);
       char2Arr('o', 12, 0);
       char2Arr('n', 6, 0);
       refresh_display(); 
       delay(1000);
    }
    if (i > 28)
    {
       clear_Display();
       char2Arr('R', 25, 0);
       char2Arr('T', 19, 0);
       char2Arr('C', 12, 0);
       char2Arr('!', 6, 0);
       refresh_display(); 
       delay(1000);
       Serial.println("SmartConfig Faild!" );
       Serial.println("Clock use RTC!" );
    }
}