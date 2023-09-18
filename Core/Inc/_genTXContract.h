/*
 * _TXgenerator.h
 *
 *  Created on: Aug 25, 2023
 *      Author: mrahm
 */

#include <stdint.h>
#include <string.h>
#include <iostream>
#include <Bitcoin.h>
#include <math.h>
#include <_backbone.h>

using namespace std;

uint8_t _genTXContract (Tx* tx,
						int fee,
						txinDataStruct* TxIn_contract,
						lockDataStruct* keys,
						char* GuestSeq,
						char* OwnerSeq);
