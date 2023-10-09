/*
 *
 *
 *  Created on: Aug 30, 2022
 *      Author: Administrator
 */

#ifndef INC__WALLET_H_
#define INC__WALLET_H_

#include <_backbone.h>

#include <iostream>

#include <Bitcoin.h>
#include <_syncHeaders.h>
#include <_backbone.h>
#include <_ESP8266_HAL.h>
#include <_headerVerifier.h>
#include <__lockOperation.h>
#include <_servoControl.h>
#include <_exampleGen.h>

using namespace std;





uint8_t  ____uSmartLock(uint8_t* bitcoreServerIP, uint8_t* bitcoreServerPort, lockDataStruct* keys,txinDataStruct* TXIN);


#endif /* INC__WALLET_H_ */
