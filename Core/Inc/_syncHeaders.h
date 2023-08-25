/*
 * _syncHeaders.h
 *
 *  Created on: May 17, 2022
 *      Author: mrahmanikivi
 */

#ifndef INC__SYNCHEADERS_H_
#define INC__SYNCHEADERS_H_

#include <stdint.h>
#include <iostream>
#include "stdlib.h"
#include "stdio.h"  //sprintf
#include "string.h" //memset
#include "math.h"
#include "stdlib.h"  //atoi

#include "stm32f4xx_hal.h"
#include "fatfs.h"




using namespace std;

uint8_t _syncHeaders(uint8_t* server, uint8_t* port ,int blockCounter);
void headerDownloader(int hight,char* header ,uint8_t* server, uint8_t* port);




#endif /* INC__SYNCHEADERS_H_ */
