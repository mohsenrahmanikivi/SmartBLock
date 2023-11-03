/*
 * _backbone.h
 *
 *  Created on: Sep 2, 2022
 *      Author: Administrator
 */

#ifndef INC__BACKBONE_H_
#define INC__BACKBONE_H_

#include <iostream>
#include "fatfs.h"
#include <Bitcoin.h>
#include <stdio.h>

using namespace std;

typedef string String;


/****************global variable**********************/
struct lockDataStruct {
	//Keys which are read from file.txt in SD memory
	HDPublicKey ownerXpub;
	HDPublicKey guestXpub;
	HDPrivateKey lockXprv;
	bool ownerXpubIsFound= false;
	bool guestXpubIsFound= false;
	bool lockXprvIsFound= false;
	//senario owner address consistency
	char ownerAdr[65]={"-1"};
	//P2SH address
	char scriptAdr[65]={"-1"};
	char script[128]={"-1"};

	//Path
	/* derivative path are used to derive keys form
	 * inputs/outputs. Derivative Path needs Index
	 * to be completed.
	 */

	char index[127]={"-1"};
	char derivativePath[127];
	//please fill these variables before use
	char inPath[127];
	char outPath[127];

	char lastUnlockTxId[65];
};

struct txDataStruct {
	char id[65];
	char hight[16];
	uint8_t index;
	int amount;
	Tx tx;
};

struct txinDataStruct {
	char id[65];
	int index = -1;
	int fund = -1;

};
/****************global variable**********************/



uint8_t _readData(int* cnt,lockDataStruct* keys, uint8_t* index,uint8_t* ssid,uint8_t* password,char* GlobalpreHASH, int* verifiedHight, txinDataStruct* TXIN);
uint8_t _mKeyGenerator(lockDataStruct* keys);



#endif /* INC__BACKBONE_H_ */
