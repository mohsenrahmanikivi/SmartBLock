/*
 * ESP8266_HAL.h
 *
 *  Created on: Apr 14, 2020
 *      Author: Controllerstech
 */

#ifndef INC__ESP8266_HAL_H_
#define INC__ESP8266_HAL_H_

#include <stdio.h>
#include <stdint.h>


int ESP_Init (char *SSID, char *PASSWD);
int ESP_reset (void);

int _getHeader (int hight,char* server, char* port);
int Server_Start (void);
int receive (char* send, char* receive,int receiveSize,char* server, char* port);
int test (char* command);
int receiveResult (char* send, char* receive,int receiveSize, char* server, char* port);

int receiveResultafter (char* send, char* receive,int receiveSize, char* getafter,char* server, char* port);

uint8_t ATsend (uint8_t* command);
uint8_t ATreceive (char* send,char * getafter, char* receive, uint8_t receiveSize, uint8_t * server, uint8_t * port);
uint8_t ATreceive_Timeout (char* buff,int buffSize, char* getafter, char * server, char * port, int timeout);
#endif /* INC__ESP8266_HAL_H_ */
