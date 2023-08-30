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

using namespace std;

uint8_t __lockOperation( char* server, char* port, lockDataStruct *keys, txinDataStruct *TXIN );
uint8_t _storeTxIN(char *TxInid, int TxInIndex,int TxInfund);
uint8_t _sendContractTx( char* server,char * port, Tx *tx);
uint8_t _storeScriptAdr(char * scriptAdr);

#endif /* INC___LOCKOPERATION_H_ */
