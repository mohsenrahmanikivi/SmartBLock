/*
 * _lockOperation.h
 *
 *  Created on: Aug 21, 2023
 *      Author: mrahm
 */

#ifndef INC___LOCKOPERATION_H_
#define INC___LOCKOPERATION_H_

#include <iostream>
#include <string.h>
#include <_backbone.h>

#include "stm32f4xx_hal.h"
#include <_search.h>
#include <_genTXContract.h>
#include <_ESP8266_HAL.h>
#include <_servoControl.h>


using namespace std;




uint8_t __lockOperation( char* server, char* port, lockDataStruct *keys, txinDataStruct *TXIN, int waitSeconds );
uint8_t _storeTxIN(char *TxInid, int TxInIndex,int TxInfund);
uint8_t _sendTx( char* server,char * port, Tx *tx);
uint8_t _storeScriptAdr(char * scriptAdr);
uint8_t _storePathIndex(char* index);
uint8_t _receiveFromTerminal(uint8_t *TEMP, int TEMP_SIZE);
uint8_t _storeRedeem(char* redeem);
uint8_t _checkGuestTX(txDataStruct* guestTX, lockDataStruct* keys, txinDataStruct* txin);
uint8_t _checkContractTX( txDataStruct* contractTX, lockDataStruct* keys) ;
void _removeSpaces(char * temp, int temp_size);






#endif /* INC___LOCKOPERATION_H_ */
