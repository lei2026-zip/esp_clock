#ifndef __SOFTWIFI_H_
#define __SOFTWIFI_H_

#include <ESP8266WiFi.h>
#include "led_display.h"
#include "udp_socket.h"

bool autoConfig();
void smartConfig(); 

#endif
