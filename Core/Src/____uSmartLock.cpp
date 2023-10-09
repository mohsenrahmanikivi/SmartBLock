/*

 *
 *  Created on: Aug 30, 2022
 *      Author: Mohsen rahmanikivi
 */

#include <____uSmartLock.h>

/* 1-NETWORK= {Testnet , Mainnet}  is configured in uBitcoin_conf.h default= Testnet
 * 2-fill the SD memory  with header folders with downloaded headers
 * 3-run the this app
 */


uint8_t ____uSmartLock(uint8_t* server, uint8_t* port,lockDataStruct* keys,txinDataStruct* TXIN ){
	// local variables

	int localHight=0, verifiedHight=0;
	char globalPreHASH[65];

	uint8_t ssid[16];
	uint8_t pass[16];

	uint8_t lastIndex;



	//	0-Loading
	cout<<"\n_uSmartLock--<info> The Minimal Wallet Starts Working\r";
	cout<<"\n_uSmartLock--<info> LOADING....\r";
	_servoLock();
	_exampleGen();


	while(_readData( &localHight, keys, &lastIndex, ssid, pass, globalPreHASH, &verifiedHight, TXIN)!=1)	{
		HAL_Delay(10000);
		NVIC_SystemReset();
	}

	//preprocessing,
	//add type for keys
			keys->lockXprv.type= P2PKH;
			keys->guestXpub.type= P2PKH;
			keys->ownerXpub.type= P2PKH;
	//define path
			sprintf(keys->inPath, "%s%d", keys->derivativePath, atoi(keys->index));
			sprintf(keys->outPath, "%s%d", keys->derivativePath, atoi(keys->index)+1);

			char tempPath[128];
			sprintf(tempPath, "%s%d", keys->derivativePath, 0);
			sprintf(keys->ownerAdr, "%s", keys->ownerXpub.derive(tempPath).address().c_str());



	cout<<"\n_uSmartLock--<info>-------------------- \r";
	cout<<"\n################ STAT #################\r";
	cout<<"\n# Derivative PATH : "<<keys->derivativePath;
	cout<<"\n# Index (inPut)   : "<<keys->index;
//	cout<<"\n# InPath-OutPath  : "<<keys->inPath<<"-"<<keys->outPath;
	cout<<"\n# Lock address    : "<<keys->lockXprv.derive(keys->inPath).address().c_str();
//	cout<<"\n# Lock address Out: "<<keys->lockXprv.derive(keys->outPath).address().c_str();
//	cout<<"\n# Script address  : "<<keys->scriptAdr;
	cout<<"\n# TXin ID         : "<<TXIN->id;
	cout<<"\n# TXin Index      : "<<TXIN->index;
	cout<<"\n# Fund (satoshi)  : "<<TXIN->fund;
	cout<<"\n# Local Hight     : "<<localHight;
	cout<<"\n# Verified Hight  : "<<verifiedHight;
	cout<<"\n# WIFI SSID:PSW   : "<<ssid;
	cout<<"\n# Owner Xpub----->\n"<<keys->ownerXpub.toString().c_str();
	cout<<"\n# Guest Xpub----->\n"<<keys->guestXpub.toString().c_str();
	cout<<"\n# Lock  Xpub----->\n"<<keys->lockXprv.xpub().toString().c_str();
	cout<<"\n#######################################\r";


	//1. Reset the Wi-Fi module
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
	cout<<"\n_uSmartLock--<info> Synchronizing ....\r";
	while(_syncHeaders(server, port, localHight)!=1){};

	//3.2 Verify the headers
    while (_headerVerifier(localHight,globalPreHASH, verifiedHight)!=1){};
	cout<<"\n_uSmartLock--<info> The wallet is updated ....\r";

	//4. Stop if data missing
	if(keys->ownerXpubIsFound == 0 || keys->guestXpubIsFound == 0  || keys->lockXprvIsFound == 0) {	cout<<"\n_uSmartLock--<Error> Check the Keys All should be TRUE (one/more are missing)\r";
		while(1);
	}else if(strlen(TXIN->id) != 64 || (TXIN->index)  < 0  || (TXIN->fund)  < 0) {cout<<"\n_uSmartLock--<Error> Check the TXin details. one/more are missing\r";
			while(1);
		}else if(strlen(keys->derivativePath) == 0 || atoi(keys->index)  < 0 )   {cout<<"\n_uSmartLock--<Error> Check the Derivative Path details. one/more are missing\r";
				while(1);
			}

	//5. SmarlLock operation
	cout<<"\n_uSmartLock--<info> SmartLock Operation is executed ....\r";
	 while (__lockOperation((char*)server , (char*)port, keys, TXIN, 10)!=1){};


	 return 1;
	     }





