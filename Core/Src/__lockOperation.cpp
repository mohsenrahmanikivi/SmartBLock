/*
 * _lockOperation.cpp
 *
 *  Created on: Aug 21, 2023
 *      Author: mrahm
 */


#include "stm32f4xx_hal.h"
#include <_search.h>
#include <__lockOperation.h>
#include <_genTXContract.h>
#include <_ESP8266_HAL.h>
	/*
	 * assumption :
	 * owner_xpub
	 *
	 * contract tx: wait to receive
	 * conditions: 	1- input: owner_adr or lock_adr
	 * 				2- output: owner_adr
	 * (input: owner_adr0       output: guest_adr0* 	||	 owner_adr0)
	 * *guest_adr0 forms the guest list.
	 *
	 *
	 * unlock tx:  wait to receive
	 * conditions: 	1- input: guest_adr (come from the guest list)
	 * 				2- output: lock_adr (come from the lock key)
	 * (input: guest_adr0  		output: lock_adr0)
	 *
	 *
	 * Establish new contract:
	 * conditions: 	1- input: lock_adr (come from the lock key)
	 * 				2- output: similar to the received contract with different time lock
	 * (input: lock_adr0       output: guest_adr0 	||	 owner_adr0)
	 *
	 */


uint8_t __lockOperation( char* server, char* port, lockDataStruct *keys, txinDataStruct *TXIN, int waitSeconds ){


	txDataStruct lockUTXO;
	txDataStruct scriptUTXO;
	uint8_t res;
	char scriptAddr[65];
	memset(scriptAddr, '\0', 65);
	const char * lockAddr= keys->lockXprv.derive(keys->P2PK_Path).address().c_str();
	/****************************************************************************************/
	//1- Search for the contract based on Lock address
	for ( res = _addrCheck(server, port, (char *)lockAddr, &lockUTXO) ; res == 0 ; ) {
		printf("\n__lockOperation--<error> Lock Operation Failed \n");
		for(int i=0 ; i< waitSeconds ; i++){HAL_Delay(1000); printf(".");} // waiting point
	}

	if(res == 1 ) //1 = address is incorrect or has a error
	{ 	printf("\n__lockOperation--<error> Error: Lock address is incorrect or has an error \r");
		return 0;
	}

	if( res == 2) //2 = address is correct but not there is not any UTXO
	{ 	printf("\n__lockOperation--<info> A new contract is NOT found \r");
		printf("\n__lockOperation--<info> Searching for a new contract...\r");
		do
		{	HAL_Delay(waitSeconds*1000);
			res=_addrCheck(server, port, (char *)lockAddr, &lockUTXO);
		}while (res!=3);
	}

	if( res != 3){ return 0;}

	printf("\n__lockOperation--<info> A new ContractTX is  found \r");
	while(_getAndVerifyTx(server, port, (char *)lockAddr, &lockUTXO) == 0 ){
		printf("\n__lockOperation--<error> downloading the ContractTx is faced unexpected error\r");
		return 0;
	}


	//2- feed the TxIn from contractTX
	//lockUTXO.tx.txOuts[0].scriptPubkey.type()
	strcpy(TXIN->id,(char *)lockUTXO.tx.txid().c_str());
	for(size_t i=0; i>lockUTXO.tx.outputsNumber; i++)
		if(strcmp(lockUTXO.tx.txOuts[i].address().c_str(), lockAddr )== 0)
			TXIN->index=i;
	TXIN->fund=lockUTXO.tx.txOuts[TXIN->index].amount;

		//store NEW TxIN
	if(_storeTxIN(TXIN->id, TXIN->index, TXIN->fund) != 1)
		printf("\n__lockOperation--<error> NEW (TXid, index, fund) is NOT saved\r");
	else printf("\n__lockOperation--<info> NEW TxIN saved successfully \r");

		//store NEW script address

	for(int i=0; i < (int)lockUTXO.tx.outputsNumber; i++) {
		if(lockUTXO.tx.txOuts[i].scriptPubkey.type()== P2SH) {
			sprintf(scriptAddr,"%s", lockUTXO.tx.txOuts[i].address().c_str());
			if(_storeScriptAdr(scriptAddr) != 1){
				printf("\n__lockOperation--<error> NEW scriptAdr is NOT saved \r");
			} else {
				printf("\n__lockOperation--<info> NEW script is = %s \r", scriptAddr);
				break;
			}

		}else{
		printf("\n__lockOperation--<error> No P2SH output found in the contract \r");
		return 0;
		}
	}

	//3- check the guest data and lock time data
//develop later

	//4- waiting for the unlock transaction

	for ( res = _addrCheck(server, port, (char *)scriptAddr, &scriptUTXO) ;	res == 0 ; )
		{	printf("\n__lockOperation--<error> Lock Operation Failed \r");
			HAL_Delay(10000);
		}
	if( res == 2){
			printf("\n__lockOperation--<info> script is already SPENT. \r");
			return 0;
	}
	if( res == 3) //2 = address is correct but not there is not any UTXO
	{ 		printf("\n__lockOperation--<info> script is UNSPENT. \r");
			printf("\n__lockOperation--<info> Searching the network again ..........\r");
			do{
 				for(int i=0 ; i< waitSeconds ; i++){HAL_Delay(1000); printf(".");} // waiting point
				res=_addrCheck(server, port, (char *)scriptAddr, &scriptUTXO);
			}while (res != 2);

		}
	if( res == 2){
		//verify TX and unlock
		printf("\n__lockOperation--<info> execute operation. \r");
		while(1);

	}
	//5- execute the unlock operation

	//6- prepare the new contract and send it



//		char* owner_adr0 = (char*)keys->ownerXpub.address().c_str();
//		char* guest_adr0 = (char*)keys->guestXpub.address().c_str();
//		char* lock_adr0 = (char*)keys->lockXprv.address().c_str();


//		Tx tx;
//		int fee= 350;

//		char* GuestAdr=(char*)keys->guestXpub.address().c_str();
//		char* nlock_Guest=(char*)"1792848796";
//		char* OwnerAdr=(char*)keys->ownerXpub.address().c_str();
//		char* nlock_Owner=(char*)"1792898796";





//	 make sure keys are loaded
//	if (_genTXContract (&tx, fee, TXIN->id, TXIN->index, TXIN->fund, keys->lockXprv.derive(keys->P2PK_Path), GuestAdr, nlock_Guest, OwnerAdr, nlock_Owner)==1 ){
//			printf("\n__lockOperation--<info> Contract TX is correctly created \r");
//			if(_sendContractTx( server, port, &tx)==1){
//				printf("\n__lockOperation--<info> Contract TX is correctly Sent \r");
//				strcpy(TXIN->id,(char *)tx.txid().c_str());
//				TXIN->index=2;
//				TXIN->fund= TXIN->fund - (2*fee);
//				//store NEW TxIN
//				if(_storeTxIN(TXIN->id, TXIN->index, TXIN->fund) != 1)
//					printf("\n__lockOperation--<error> NEW (TXid, index, fund) is NOT saved\r");
//				else printf("\n__lockOperation--<info> NEW TxIN saved successfully \r");
//				//store NEW script address
//				if(_storeScriptAdr((char *) tx.txOuts[0].address().c_str()) != 1)
//					printf("\n__lockOperation--<error> NEW scriptAdr is NOT saved \r");
//				else printf("\n__lockOperation--<info> NEW script adr saved successfully scriptAdr= %s \r", tx.txOuts[0].address().c_str());
//			}else{
//				printf("\n__lockOperation--<error> TX send error tx=\n%s \r", tx.toString().c_str());
//			}
//	}





return 1;
}

uint8_t _sendContractTx( char* server,char * port, Tx *tx){

	//0- Define variable
	int buffSize=1024;
	char buff[buffSize];
	memset(buff,'\0',buffSize);
	int contentSize= 45 + strlen(tx->toString().c_str());
	//1- prepare Request
	int res=sprintf (buff, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
			"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
			"Content-Type: application/json\r\nContent-Length: %d\r\n\r\n"
			"{\"method\":\"sendrawtransaction\",\"params\":[\"%s\"]}\r\n",server,port,contentSize,tx->toString().c_str());  // varibale are diffrent
	if(res < 0 || res>= buffSize){
		printf("\n_sendContractTx--<error> buffer overflow buffer size=1024 \r");
		return 0;
	}


	//3- send request and receive
	while(ATreceive_Timeout(buff, buffSize, (char *)"\"result\":", server, port, 5000)!=1)  HAL_Delay(1000);

	//4- check the receive data
	int i=0;
	if ( buff[i]=='n'){

		while( buff[i]!='"' || buff[i+1]!='m' || buff[i+2]!='e' || buff[i+3]!='s' || buff[i+8]!='"'   ){
			i++;
			if (i >= buffSize) break;
		}

		int j=i+11;
		i=i+11;
		while( buff[i]!='"'){
			i++;
			if (i >= buffSize) break;
			}
		buff[i]='\0';

		printf("_sendContractTx--<error> Error Message : %s", buff+j );
		return 0;

		}else if (buff[i]=='"'){
			i++;
			buff[i+64]='\0';

			if (strcmp(tx->txid().c_str(), buff+i ) == 0){
				printf("\n_sendContractTx--<info>  TXid is: %s\r", tx->txid().c_str());
				return 1;
				}else{
					printf("\n_sendContractTx--<error>  TX is sent but txid is not the same\r");
					return 0;
					}



	}else
	{
		printf("\n_sendContractTx--<error>  corrupted received data\r");
		buff[1023]='\0';
		printf("\n_sendContractTx--<error> The data is = \n%s \r", buff);
		return 0;

	}

}

uint8_t _storeTxIN(char *TxInid, int TxInIndex,int TxInfund){

	/***needs  MX_FATFS_Init(); in the main.c ***************************************/
			// variables for FatFs
			FATFS FatHand; 			//Fatfs handle
			FIL FileHand; 			//File handle
			FRESULT result; 		//Result after operations
			char buf[128];
			memset(buf, '\0',128);


			/*********************************FATFS Mounting*********************************/
			//0- Unmount
			result=f_mount(NULL, "", 0);
			if (result != FR_OK) {	printf("\n_storeTxIN--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
			//1=mount now
			result = f_mount(&FatHand, "", 1);
			if (result != FR_OK) {	printf("\n_storeTxIN--<error><FATFS>  mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}


			/*********************Write the  "TXIN.txt"**************************/
			char * fname=(char *)"TXIN.txt";

			 result=f_open(&FileHand, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
			 if(result != FR_OK){ printf("\n_storeTxIN--<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result); return 0;}

			 sprintf(buf,"%s,%d,%d\n",TxInid,TxInIndex,TxInfund );
			 f_puts((TCHAR*)buf, &FileHand);
			 f_close(&FileHand);


			 result=f_mount(NULL, "", 0);
			if (result != FR_OK) {	printf("\n_storeTxIN--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
return 1;
}


uint8_t _storeScriptAdr(char * scriptAdr){
	if(strlen(scriptAdr)== 64){
		printf("\n_storeScriptAdr--<error> Script address is corrupted/empty the length is=%d\r",strlen(scriptAdr));
		return 0;
	}
	/***needs  MX_FATFS_Init(); in the main.c ***************************************/
	// variables for FatFs
	FATFS FatHand; 			//Fatfs handle
	FIL FileHand; 			//File handle
	FRESULT result; 		//Result after operations
	char buf[128];
	memset(buf, '\0',128);


	/*********************************FATFS Mounting*********************************/
	//0- Unmount
	result=f_mount(NULL, "", 0);
	if (result != FR_OK) {	printf("\n_storeScriptAdr--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
	//1=mount now
	result = f_mount(&FatHand, "", 1);
	if (result != FR_OK) {	printf("\n_storeScriptAdr--<error><FATFS>  mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}


	/*********************Write the  "TXIN.txt"**************************/
	char * fname=(char *)"SCRIPTADR.txt";

	 result=f_open(&FileHand, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
	 if(result != FR_OK){ printf("\n_storeScriptAdr--<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result); return 0;}

	 sprintf(buf,"%s\n", scriptAdr);
	 f_puts((TCHAR*)buf, &FileHand);
	 f_close(&FileHand);


	 result=f_mount(NULL, "", 0);
	if (result != FR_OK) {	printf("\n_storeScriptAdr--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
return 1;
}

