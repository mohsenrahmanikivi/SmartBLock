/*
 * _wallet.cpp
 *
 *  Created on: Aug 30, 2022
 *      Author: Mohsen rahmanikivi
 */


#include <____uSmartLock.h>
#include <iostream>

#include <Bitcoin.h>
#include <_syncHeaders.h>
#include <_backbone.h>
#include <_ESP8266_HAL.h>
#include <_headerVerifier.h>
#include <__lockOperation.h>

using namespace std;


/* 1-NETWORK= {Testnet , Mainnet}  is configured in uBitcoin_conf.h default= Testnet
 * 2-fill the SD memory  with header folders with downloaded headers
 * 3-run the this app
 */


void ____uSmartLock(uint8_t* server, uint8_t* port,lockDataStruct* keys){
	// local variables
	int localHight=0, verifiedHight=0;
	char globalPreHASH[65];

	uint8_t ssid[16];
	uint8_t pass[16];

	uint8_t lastIndex;



	//	1-Loading
	cout<<"\nWALLET--The Minimal Wallet Starts Working\r";
	cout<<"\nWALLET--LOADING....\r";

	while(_readData( &localHight, keys, &lastIndex, ssid, pass, globalPreHASH, &verifiedHight)!=1)	{
		HAL_Delay(1000);
		NVIC_SystemReset();
	}

	// set type
		keys->lockXprv.type= P2PKH;

	cout<<"\nWALLET--############ STAT ###########\r";
	cout<<"\nWALLET--# (1=true 0=false) ";
	cout<<"\nWALLET--# Owner Xpub is found: "<<keys->ownerXpubIsFound;
	cout<<"\nWALLET--# Guest Xpub is found: "<<keys->guestXpubIsFound;
	cout<<"\nWALLET--# Lock  Xprv is found: "<<keys->lockXprvIsFound;
	cout<<"\nWALLET--# Last Local Hight   = "<<localHight;
	cout<<"\nWALLET--# Last Verified Hight= "<<verifiedHight;
	cout<<"\nWALLET--# Defined WIFI SSID  = "<<ssid;
	cout<<"\nWALLET--# Lock Address       = "<<keys->lockXprv.address().c_str();
	cout<<"\nWALLET--#############################\r";



	//1. Reset the wifi module
	int state=0;
	do{	cout<<"\nWALLET-- Reseting Wifi Module....\r";
		state = ESP_reset ();
	} while(state!=1);

	//2. connect to the network
	state=0;
	do{	cout<<"\nWALLET--# Connectiong to the WiFi....\r";
		state = ESP_Init((char*)ssid, (char*)pass);
	} while(state!=1);


	//3.1 Synchronize the headers

	cout<<"\nWALLET--# Synchronizing to the wallet....\r";
	while(_syncHeaders(server, port, localHight)!=1){};
	//3.2 Verify the headers
    while (_headerVerifier(localHight,globalPreHASH, verifiedHight)!=1){};
	cout<<"\nWALLET--# The wallet is updated ....\r";

	cout<<"\nWALLET--# The wallet is waiting to receive contract ....\r";
	 while (__lockOperation((char*)server , (char*)port, keys)!=1){};



//	 while(1);
	     }





