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

using namespace std;


uint8_t _genTXContract (Tx* tx, int fee,
						char* TxInid,int TxInIndex, int TxInfund, HDPrivateKey TxInPrivateKey,
						const char* changeAddr,
						char* GuestAdr, char* nlock_Guest, char* OwnerAdr,char* nlock_Owner);
