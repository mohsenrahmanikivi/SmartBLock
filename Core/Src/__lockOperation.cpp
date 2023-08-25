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


uint8_t __lockOperation( char* server, char* port, lockDataStruct *keys ){

	// Variables
	txDataStruct contractTx;
	txDataStruct unlockingTx;

//	char* owner_adr0 = (char*)keys->ownerXpub.address().c_str();
//	char* guest_adr0 = (char*)keys->guestXpub.address().c_str();
//	char* lock_adr0 = (char*)keys->lockXprv.address().c_str();
//

	Tx tx;

	char* TxInid=(char*)"6a24bb6ba8a2f601248e69e69746e2dd8493212182f302cd5977d1222190adbd";
	int TxInIndex= 0;
	int TxInfund=100000000*0.05956539;

	//min relay fee not met,  < 296 (code -26)

	int fee=1300;

	;

	char* GuestAdr=(char*)"1NRpjbkR4pjxS5H5XRREtkxuLrLn6ACh2D";
	char* nlock_Guest=(char*)"10";
	char* OwnerAdr=(char*)"1LzZJkQfz9ahY2SfetBHLcwyWmQRE9CwfU";
	char* nlock_Owner=(char*)"1792896796";



	_genTXContract (&tx, fee,
					TxInid, TxInIndex, TxInfund, keys->lockXprv,
					keys->lockXprv.address().c_str(),
					GuestAdr, nlock_Guest, OwnerAdr, nlock_Owner);
	printf("\nTx is=\n%s\r", tx.toString().c_str());





//
//	while(_search( (char*)server, (char*)port, owner_adr0, &contractTx)!= 1  ) {
//		HAL_Delay(5000);
//		cout<<"\n__lockOperationT--# Contract_TX is not found. Retry 5 seconds later....\r";
//	}

//	// Identify the guest from contractTX
//	 _checkGuestAdr( guest_adr0 ,  &contractTx );
//
//	//Wait for the unlocking TX
//	while(_search( (char*)server, (char*)port, lock_adr0, &unlockingTx)!= 1) {
//			HAL_Delay(5000);
//			cout<<"\n__lockOperationT--# Contract_TX is not found. Retry 5 seconds later....\r";
//		}
//
//	//run the unlock operation
//	//unlockOperation();
//
//	//establish the new contract
//
//	//sendContract();

	while(1);
return 1;
}


// write on disk
//		/***********************************Variables************************************/
//
//		FATFS FatFs; 	//Fatfs handle
//		FIL fil; 		//File handle
//		FRESULT fres; //Result after operations
//
//	/********************************FATFS Mounting**********************************/
//			//0- Unmount
//			f_mount(NULL, "", 0);
//			printf("\n_search--> UNMount the SDRAM");
//			//1=mount now
//			fres = f_mount(&FatFs, "", 1);
//			//3=Check mounting
//			if (fres != FR_OK) {	printf("_search--> f_mount error, Error Code=(%i)\r\n", fres); 	return 0;	}
//			printf("\n_search--> Mount   the SDRAM");
//			/************************************CHeck ***************************************/
//			char* fileName=(char*)"RTX.txt";
//			/********************************Write        **********************************/
//			fres = f_open(&fil, fileName, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
//			if(fres != FR_OK) {printf("\n_search--> f_open error (%i)\r\n", fres); return 0;  }
//
//			 //2-Write
//
//			 int res=f_puts(buff, &fil);
//			 if(fres != FR_OK) {	printf("\n_search--> f_write  error (%i)\r\n",res); return 0;   }
//
//			 //4-close
//			 f_close(&fil);
//			 /********************************Finish        **********************************/

//uint8_t _checkGuestAdr( char* guest_adr ,  txDataStruct* contractTx ){
//	bool guestAdrFound = 0;
//	bool ownerPubFound = 0;
//
//	for(int j=0; j< (int)contractTx->tx.outputsNumber; j++)	 {
//		guestAdrFound = false;
//		Script script;
//		char * Buff[256];
//		script = contractTx->tx.txOuts[j].scriptPubkey.serialize(Buff , sizeof(Buff));
//		uint64_t amount = contractTx->tx.txOuts[j].amount;
//		script.
//
//
//
//						}

//
//	for(int i=0; 	i< (int)contractTx.inputsNumber; i++)		{
//
//					found = false;
//			        scriptSig = tx.txIns[i].scriptSig;
//			        if(scriptSig.length() <= 1) {  Witness wit= tx.txIns[i].witness;
//
//			        cout<<"\r\n witness is = "<<wit.toString();
//			        }
//		        else if(scriptSig.length() > 1 && scriptSig.length() < sizeof(scriptSigBuf))
//			        {
//
//		//InLoop	        3- copy scriptSig  data to scriptSigBuf[]  bytes array
//							scriptSig.serialize(scriptSigBuf, sizeof(scriptSigBuf));
//		//InLoop			SCRIPT { SIGlen + SIG + PUBlen + PUB }
//		//InLoop			copy the SIGlen
//						scriptSigLen = scriptSigBuf[1];
//		//InLoop			reset the parsing
//							sigInTx.reset();
//		//InLoop			copy the SIG
//							sigInTx.parse(scriptSigBuf+2, scriptSigLen);
//		//InLoop			copy the PUBlen
//							pubInTxLen = scriptSigBuf[scriptSigLen+2];
//		//InLoop			reset the parsing
//							pubInTX.reset();
//		//InLoop			copy the PUB
//							pubInTX.parse(scriptSigBuf+scriptSigLen+3, pubInTxLen);
//
//			/******************************  Printing the result  ************************************/
//		#ifdef PRINT
//							cout<<"\r\n_TX_ENCODER_________________________________________________"<<"\r\n";
//							cout <<"\r\nTotal Inputs = "<<(int)tx.inputsNumber<<"\r\nInput numbers = "<<i+1<<"\r\n";
//							cout <<"\r\nScriptSig = \r\n"<<scriptSig.toString()<<"\r\n";
//							cout <<"\r\nInput Publickey: \r\n" << string(pubInTX) <<"\r\n";
//							cout <<"\r\nInput Signature: \r\n" << string(sigInTx) <<"\r\n";

//
//					return 1;
//}
