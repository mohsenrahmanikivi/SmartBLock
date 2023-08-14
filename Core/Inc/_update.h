/*
 * _update.h
 *
 *  Created on: Oct 7, 2022
 *      Author: Administrator
 */

#ifndef INC__UPDATE_H_
#define INC__UPDATE_H_


uint8_t _update(char* fileName, lockDataStruct* keys, char* server, char* port);

uint8_t _getUnSpendTXid(char* adr,char* server, char* port, char* rBuff, int rBuffSize);


#endif /* INC__UPDATE_H_ */
