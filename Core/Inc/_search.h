/*
 * _update.h
 *
 *  Created on: Oct 7, 2022
 *      Author: Administrator
 */

#ifndef INC__SEARCH_H_
#define INC__SEARCH_H_

#include <iostream>
#include <Bitcoin.h>
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "math.h"

#include <_backbone.h>

using namespace std;


uint8_t _addrCheck (char* server, char* port, char* adr, txDataStruct* utxo);
uint8_t _getAndVerifyTx(char* server, char* port, char* adr, txDataStruct* tx);
uint8_t _getUTXO(char* server, char* port, char* adr,txDataStruct* tx);


#endif /* INC__SEARCH_H_ */
