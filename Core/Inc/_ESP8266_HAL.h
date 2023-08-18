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
#include <_backbone.h> 		 //for txDataStruct


int ESP_Init (char *SSID, char *PASSWD);
int ESP_reset (void);


int receiveResult (char* send, char* receive,int receiveSize, char* server, char* port);


uint8_t ATsend (uint8_t* command);
uint8_t ATreceive (char* send,char* getafter, char* receive, int receiveSize, uint8_t * server, uint8_t * port);
uint8_t ATreceive_Timeout (char* buff,int buffSize, char* getafter, char * server, char * port, int timeout);
uint8_t _getrawTX(char* srv,char * prt,char * txId, char * hight, txDataStruct* tx);
#endif /* INC__ESP8266_HAL_H_ */
