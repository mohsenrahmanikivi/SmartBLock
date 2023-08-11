/*
 * _backbone.h
 *
 *  Created on: Sep 2, 2022
 *      Author: Administrator
 */

#ifndef INC__BACKBONE_H_
#define INC__BACKBONE_H_

#include <Bitcoin.h>

/****************global variable**********************/
struct lockDataStruct {

	HDPublicKey ownerXpub;
	HDPublicKey guestXpub;
	HDPrivateKey lockXprv;
	bool ownerXpubIsFound= false;
	bool guestXpubIsFound= false;
	bool lockXprvIsFound= false;

};
/****************global variable**********************/



uint8_t _readData(int* cnt,lockDataStruct* keys, uint8_t* index,uint8_t* ssid,uint8_t* password,char* GlobalpreHASH, int* verifiedHight);
uint8_t _mKeyGenerator(lockDataStruct* keys);



#endif /* INC__BACKBONE_H_ */