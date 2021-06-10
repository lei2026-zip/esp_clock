#ifndef __UDP_SOCKET_H_
#define __UDP_SOCKET_H_

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include "string.h"
#include "ws2812b.h"
#include "eps_flash.h"

#define RECEIVE_LENGHT 500 //接收buff长度

extern IPAddress LocalIp ;
extern uint16_t LocalPort;
extern IPAddress RemoteIp ;
extern uint16_t RemotePort;
extern uint8_t* RecBuffer;
extern uint8_t* SendBuffer;

extern WiFiUDP Udp;

uint8_t socket_listening();
void send_hand();


#endif
