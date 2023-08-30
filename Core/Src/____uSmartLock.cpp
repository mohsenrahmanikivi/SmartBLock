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


void ____uSmartLock(uint8_t* server, uint8_t* port,lockDataStruct* keys,txinDataStruct* TXIN ){
	// local variables

	int localHight=0, verifiedHight=0;
	char globalPreHASH[65];

	uint8_t ssid[16];
	uint8_t pass[16];

	uint8_t lastIndex;



	//	1-Loading
	cout<<"\n_uSmartLock--<info> The Minimal Wallet Starts Working\r";
	cout<<"\n_uSmartLock--<info> LOADING....\r";

	while(_readData( &localHight, keys, &lastIndex, ssid, pass, globalPreHASH, &verifiedHight, TXIN)!=1)	{
		HAL_Delay(1000);
		NVIC_SystemReset();
	}

	//m / purpose' / coin_type' / account' / change / index
		keys->lockXprv.type= P2PKH;

	cout<<"\n_uSmartLock--<info>-------------------- \r";
	cout<<"\n################ STAT #################\r";
	cout<<"\n# (1=true 0=false) ";
	cout<<"\n# Owner Xpub is found: "<<keys->ownerXpubIsFound;
	cout<<"\n# Guest Xpub is found: "<<keys->guestXpubIsFound;
	cout<<"\n# Lock  Xprv is found: "<<keys->lockXprvIsFound;
	cout<<"\n# Lock   address is  = "<<keys->lockXprv.derive(keys->P2PK_Path).address().c_str();
	cout<<"\n# Script address is  = "<<keys->scriptAdr;
	cout<<"\n# TXin ID = "<<TXIN->id;
	cout<<"\n# TXin Index         = "<<TXIN->index;
	cout<<"\n# Fund (in satoshi)  = "<<TXIN->fund;
	cout<<"\n# Last Local Hight   = "<<localHight;
	cout<<"\n# Last Verified Hight= "<<verifiedHight;
	cout<<"\n# Defined WIFI SSID  = "<<ssid;
	cout<<"\n#######################################\r";



	//1. Reset the wifi module
	int state=0;
	do{	cout<<"\n_uSmartLock--<info> Reseting Wi-fi Module....\r";
		state = ESP_reset ();
	} while(state!=1);

	//2. connect to the network
	state=0;
	do{	cout<<"\n_uSmartLock--<info> Connecting to the WiFi....\r";
		state = ESP_Init((char*)ssid, (char*)pass);
	} while(state!=1);


	//3.1 Synchronize the headers

	cout<<"\n_uSmartLock--<info> Synchronizing to the wallet....\r";
	while(_syncHeaders(server, port, localHight)!=1){};
	//3.2 Verify the headers
    while (_headerVerifier(localHight,globalPreHASH, verifiedHight)!=1){};
	cout<<"\n_uSmartLock--<info> The wallet is updated ....\r";

	cout<<"\n_uSmartLock--<info> The wallet is waiting to receive contract ....\r";
	 while (__lockOperation((char*)server , (char*)port, keys, TXIN)!=1){};



//	 while(1);
	     }





