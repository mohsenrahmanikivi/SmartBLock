/*
 * _wallet.cpp
 *
 *  Created on: Aug 30, 2022
 *      Author: Mohsen rahmanikivi
 */


#include <iostream>

#include <Bitcoin.h>
#include <_syncHeaders.h>
#include <_uSmartLock.h>
#include <_backbone.h>
#include <_ESP8266_HAL.h>
#include <_headerVerifier.h>
#include <_search.h>

/* 1-NETWORK= {Testnet , Mainnet}  is configured in uBitcoin_conf.h default= Testnet
 * 2-fill the SD memory  with header folders with downloaded headers
 * 3-run the this app
 */




using namespace std;


void __uSmartLock(uint8_t* bitcoreServerIP, uint8_t* bitcoreServerPort,lockDataStruct* keys){
	// local variables
	int localHight=0, verifiedHight=0;
	char globalPreHASH[65];

	uint8_t ssid[16];
	uint8_t pass[16];
	uint8_t* server=bitcoreServerIP;
	uint8_t* port=bitcoreServerPort;

	uint8_t lastIndex;



	//	1-Loading
	cout<<"\nWALLET--The Minimal Wallet Starts Working\r";
	cout<<"\nWALLET--LOADING....\r";

	while(_readData( &localHight, keys, &lastIndex, ssid, pass, globalPreHASH, &verifiedHight)!=1)	{
		HAL_Delay(1000);
		NVIC_SystemReset();
	}

	cout<<"\nWALLET--############ STAT ###########\r";
	cout<<"\nWALLET--# (1=true 0=false) ";
	cout<<"\nWALLET--# Owner Xpub is found: "<<keys->ownerXpubIsFound;
	cout<<"\nWALLET--# Guest Xpub is found: "<<keys->guestXpubIsFound;
	cout<<"\nWALLET--# Lock  Xprv is found: "<<keys->lockXprvIsFound;
	cout<<"\nWALLET--# Last Local Hight   = "<<localHight;
	cout<<"\nWALLET--# Last Verified Hight= "<<verifiedHight;
	cout<<"\nWALLET--# Defined WIFI SSID  = "<<ssid;
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


	//3. Synchronize the headers

	cout<<"\nWALLET--# Synchronizing to the wallet....\r";
	while(_syncHeaders(server, port, localHight)!=1){};


	//4. Verify the headers
     while (_headerVerifier(localHight,globalPreHASH, verifiedHight)!=1){};





     //5-Get transactions + verify
     txDataStruct tx_l_0;

	cout<<"\nWALLET--# Update the walletl's data....\r";
	char* adr_l_0=(char*)"mjyUHuKL36uW7mT2Kk5mbLhcrt18E58NtQ";

	while(_search( (char*)server, (char*)port, adr_l_0, &tx_l_0)!= 1) cout<<"\nWALLET--# Update not found/existed....\r";




	//	6-Parsing and Execute the operation


//	Tx tx;
//
//	tx.fromString("02000000018110a4c548c97fc4bd284c05abe082b1f1152db9da68d931485e1b00136f5531010000006a47304402"
//		"206fcdebbd20631a9043bbcfbb2e856c06217af85e9f5a6c6ee7b357a92eca0e240220761cfce8b0cb52f8976bcf59eae59762c"
//		"30c77687f6f4c449b7c52376572f889012103f542e635511cbb4745f4d3979b6da6c6dd4c37a5b128e46b4cb53366f342d52bfd"
//		"ffffff015f380000000000001976a91430e45bd40b7e70957cb6e2afaa2e3874b9a54ea288ac2bbe2500");
//	cout<<"\nTX id :.\r"<<tx.txid();
//
//	printf("\n tx status (PARSING_DONE=0)=%d\r", tx.getStatus());
//	printf("\n tx segwit=%d\r", tx.isSegwit());
//	printf("\n tx valid=%d\r", tx.isValid());
//	printf("\n tx txid=%s\r", tx.txid().c_str());
	 while(1);
	     }





