#include "udp_socket.h"

IPAddress LocalIp ;
uint16_t LocalPort = 8080;
IPAddress RemoteIp;
uint16_t RemotePort;
uint8_t *RecBuffer = 0;
uint8_t *SendBuff;

WiFiUDP Udp;

const char* username = "Hui-clock";
const char* password = "jdkcFuw5ln1JsT4d";  //16位识别码


uint8_t socket_listening(){
   // if there's data available, read a packet
   int packetSize = Udp.parsePacket();
   if(packetSize){
    uint8_t buff[50]={0};
    uint16_t i=0;
    // read the packet into packetBufffer
    Udp.read(buff,50);
    
    for(i=0;i<16;i++){
       if(buff[i]!=password[i]){
          return 0;
       }
    }
      // send a reply, to the IP address and port that sent us the packet we received
      RemoteIp = Udp.remoteIP();
      RemotePort = Udp.remotePort();
      RecBuffer = buff;
      return RecBuffer[16];
    }
    return 0;
   }
void send_hand(){
    uint8_t buff[50]={0};
    memcpy(buff,password,16);
    buff[16] = strlen(username);
    memcpy(&buff[17],username,19);
    
    buff[36] = 0x01;
    buff[37] = running_flag; 
    buff[38] = backgroun_brightness;
    buff[39] = SetR;
    buff[40] = SetG;
    buff[41] = SetB;
    buff[42] = SetHour_H;
    buff[43] = SetMin_H;
    buff[44] = SetSec_H;
    buff[45] = SetHour_L;
    buff[46] = SetMin_L;
    buff[47] = SetSec_L;

    Serial.println("Sended Hand !");
  
    Udp.beginPacket(RemoteIp, RemotePort); 
    Udp.write(buff, 50);
    Udp.endPacket();

}