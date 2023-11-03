/*
 * _lockOperation.cpp
 *
 *  Created on: Aug 21, 2023
 *      Author: mrahm
 */


#include <__lockOperation.h>
/********Read data from terminal variable **********************************************/


	const size_t hal_temp_SIZE = 1024;
	uint8_t hal_temp[hal_temp_SIZE]; // to store received data from terminal


/***************************************************************************************/


uint8_t __lockOperation( char* server, char* port, lockDataStruct *keys, txinDataStruct *TXIN, int waitSeconds ){

	txDataStruct contractUTXO;
	txDataStruct scriptUTXO;
	txDataStruct txGuest;
	txDataStruct revokeUTXO;
	txGuest.tx.reset();
	uint8_t res;
	char scriptAddr[65];
	memset(scriptAddr, '\0', 65);
	char lockAddrInPath[65];
	sprintf(lockAddrInPath,"%s", keys->lockXprv.derive(keys->inPath).address().c_str());
	char revokeAddr[65];
	sprintf(revokeAddr, "%s", keys->lockXprv.derive(keys->outPath).address().c_str());

	bool validData = false;

	Tx newContractTx;
	int fee= 400;

	/*****Search for revokeTX******************************************/
	//0- Search for the revokeTX based on revoke address

	/*_addrCheck() Returns
	 * 0 = mal function
	 * 1 = address is incorrect or has a error
	 * 2 = address is correct but not UTXO
	 * 3 = address is correct and has UTXO/s
	 * this function just return the last UTXO it is NOT designe to return more the 1 UTXO.
	 */

	//{0} = mal function
	for ( res = _addrCheck(server, port, (char *)revokeAddr, &revokeUTXO) ; res == 0 ; ) {
						printf("\n__lockOperation--<error> Search operation Failed at _addrCheck(revokeAddr) \n");
						for(int i=0 ; i> waitSeconds ; i++){printf(".");HAL_Delay(500); printf("\b"); HAL_Delay(500); } // print waiting dots
						}

	//{3} = new RevokeTX is found
	if( res == 3 ){ 	printf("\n__lockOperation--<info> A RevokeTX is found \r");
						//download UTXO and verify
						while(_getAndVerifyTx(server, port, (char *)revokeAddr, &revokeUTXO) == 0 ){
								printf("\n__lockOperation--<error> Verifying the RevokeTx is failed. Try Again...\r");
								return 0;}

						//check detailes
						while(_checkRevokeTx( &revokeUTXO, keys, TXIN) == 0 ){
								printf("\n__lockOperation--<error> RevokeTx is NOT passed _checkRevokeTX().\r");
								return 0;}
						Tx refundTx;
						while(_refundTx(&refundTx,  keys, TXIN, fee)  == 0 ){
								printf("\n__lockOperation--<error> Generating RefundTX is NOT successful.\r");
								return 0;}

						while(_sendTx(server, port, &refundTx)  == 0 ){
								printf("\n__lockOperation--<error> Sending RefundTX is NOT sent successful.\r");
								return 0;}

						while(_pathUpdate(keys) == 0 ){
								printf("\n__lockOperation--<error> Derivative Path is NOT updated.\r");
								return 0;}

						printf("\n__lockOperation--<info> New iteration is established.\r");
						return 1;
						}

	//{1} = address is incorrect or has a error
	if( res == 1 ){ 	printf("\n__lockOperation--<error> Revoke address is incorrect or has an error.\r");
						return 0;}

	//{2} = address is correct but there is NOT any UTXO
	if( res == 2 ){ 	printf("\n__lockOperation--<info> A RevokeTX is NOT found.\r");
						}


	/*****Search for lock address's UTXO (contractTX)******************************************/
	//1- Search for the contract based on Lock address
	for ( res = _addrCheck(server, port, (char *)lockAddrInPath, &contractUTXO) ; res == 0 ; ) {
						printf("\n__lockOperation--<error> Search operation Failed at _addrCheck(contractAddr) \n");
						for(int i=0 ; i> waitSeconds ; i++){printf(".");HAL_Delay(500); printf("\b"); HAL_Delay(500); } // print waiting dots
						}

	//{1} = address is incorrect or has a error
	if(res == 1 ) { 	printf("\n__lockOperation--<error> Lock address is incorrect or has an error \r");
						return 0;	}

	//{2} = address is correct but there is NOT any UTXO

	if( res == 2) { 	printf("\n__lockOperation--<info> A new contractTX is NOT found \r");

						int resetCounter=0;
						do{		HAL_Delay(waitSeconds*1000);
								printf("\n__lockOperation--<info>  %d/20 tries to search.\r", resetCounter );
								res=_addrCheck(server, port, (char *)lockAddrInPath, &contractUTXO);
								resetCounter++;
								if( resetCounter > 20){ 	resetCounter=0;
															NVIC_SystemReset();
															}
								}while (res!=3);
					}

	//{3} = new contract is found
	if( res != 3){ 		return 0;}

	printf("\n__lockOperation--<info> A new ContractTX is  found \r");

	while(_getAndVerifyTx(server, port, (char *)lockAddrInPath, &contractUTXO) == 0 ){
		printf("\n__lockOperation--<error> downloading the ContractTx is faced unexpected error\r");
		return 0;
	}
	/******Save the Txin data from contractTX***************************************************/

	//2- Feed the TXDataStruct with data from contractTX (TxIn ID, Index, P2SH address )

	//ID
	strcpy(TXIN->id,(char *)contractUTXO.tx.txid().c_str());
	//Index
	for(uint8_t i=0; i<(uint8_t)contractUTXO.tx.outputsNumber; i++){
		if(contractUTXO.tx.txOuts[i].scriptPubkey.type() == P2PKH){
					TXIN->index=i;
					break;
				}
	}


	//Amount
	TXIN->fund=contractUTXO.tx.txOuts[TXIN->index].amount;

	//Store the NEW TxIN
	if(_storeTxIN(TXIN->id, TXIN->index, TXIN->fund) != 1)
		printf("\n__lockOperation--<error> A NEW (TXid, index, fund) is NOT saved\r");
	else printf("\n__lockOperation--<info> A NEW TxIN is extracted from contract and saved successfully \r");

	//Find and store the NEW script output and its address from contract tx
	int found=0;
	for(int i=0 ; i < (int)contractUTXO.tx.outputsNumber; i++) {
		if(contractUTXO.tx.txOuts[i].scriptPubkey.type()== P2SH) {
			sprintf(scriptAddr,"%s", contractUTXO.tx.txOuts[i].address().c_str());
			found= 1;
			if(_storeScriptAdr(scriptAddr) != 1) printf("\n__lockOperation--<error> The NEW script address which is found in the contract is NOT saved \r");
			else printf("\n__lockOperation--<info> A NEW script is exracted from the contract.\n__lockOperation--<info> script address is = %s \r", scriptAddr);

			break;
		}
	}
	if(!found){
					printf("\n__lockOperation--<error> A P2SH output is NOT found");
					return 0;
				}


	/********Check the contrctTX**************************************************************/

//	3- Check the contrct
	if(_checkContractTX( &contractUTXO, keys) !=1 ){
		printf("\n__lockOperation--<error> The contract is NOT valid.");
							return 0;
	}

	/****************************************************************************************/


	/*********Waiting for guest TX***********************************************************/
//	4- waiting for the unlock transaction

		printf("\n__lockOperation--<info> Please COPY then PASTE ___Guest transaction___ here. ");

		//0- Main receive loop while

		while(!validData){

		memset(hal_temp, '\0', hal_temp_SIZE);

		_receiveFromTerminal(hal_temp, hal_temp_SIZE,  120 ) ;
		_removeSpaces((char *)hal_temp, hal_temp_SIZE);

		txGuest.tx.reset();
		txGuest.tx.fromString((char *)hal_temp);

		//1- read transaction from user (guest) // if failed , wait to receive again
		if (!txGuest.tx){
			printf(
			"\n__lockOperation--<error> The data DOES NOT contain a correct transaction.\n"
			 "__lockOperation--<error> Received data is NOT correct.\r"
		   "\n----------------------------------------------------- DATA: \n%s\r"
		   "\n------------------------------------------------- Try Again \n", (char *)hal_temp);
			continue;
		}

		//2- check the guest transaction (unlock tx) spend the contract tx or not // if failed ,wait to receive again
		if( _checkGuestTX_send( &txGuest, keys, TXIN, server, port)!= 1){
			printf("\n__lockOperation--<error> The transaction is NOT passed _checkGuestTX_send()  \r");
			continue;

		}



		printf("\n__lockOperation--<info> The check and send operation are executed succefully.\r");
		validData = true;

		 }




	 /*******New contract generation*************************************************************/
	 //5- generate a new Contract / store the new TXin id, amount / store script address/ store the new path index



		if (_genTXContractFromScript (&newContractTx, fee, TXIN, keys) ==1 ){
			printf("\n__lockOperation--<info> Contract TX is correctly created \r");

				// send
			if(_sendTx( server, port, &newContractTx)==1){
				printf("\n__lockOperation--<info> Contract TX is correctly Sent \r");
				strcpy(TXIN->id,(char *)newContractTx.txid().c_str());

				for(int i=0 ; i < (int)newContractTx.outputsNumber ; i++){
					if (newContractTx.txOuts[i].scriptPubkey.type() == P2PKH ){
						TXIN->index=i;
						TXIN->fund= newContractTx.txOuts[i].amount;
						break;
					}
				}


				//store NEW TxIN
				if(_storeTxIN(TXIN->id, TXIN->index, TXIN->fund) != 1)
					printf("\n__lockOperation--<error> NEW (TXid, index, fund) is NOT saved\r");
				else printf("\n__lockOperation--<info> NEW TxIN saved successfully \r");

				//store NEW script address
				if(_storeScriptAdr((char *) newContractTx.txOuts[0].address().c_str()) != 1) {
					printf("\n__lockOperation--<error> NEW scriptAdr is NOT saved \r");
				}
				else {
					printf("\n__lockOperation--<info> NEW script address saved successfully scriptAdr= %s \r", newContractTx.txOuts[0].address().c_str());
					//Store out put path index/////
					char outPath_index[16];
					sprintf(outPath_index, "%d", atoi(keys->index)+1); // increase 1 as it would be OutPath for future use
					if(_storePathIndex(outPath_index) == 0) {
						printf("\n_storePathIndex--<error> Path_index save operation failed");
						return 0;
					}
					printf("\n__lockOperation--<info> Path_index is updated to: %s ", outPath_index);
					///////////////////////////////
				}
			}else{
				printf("\n__lockOperation--<error> TX send error tx=\n%s \r", newContractTx.toString().c_str());
			}
	}


		/******Unlock Operation**********************************************************************/
		//6- execute the unlock operation

		 _servoUnlockTimeout(25);



return 1;
}


/***FiniSH _lockOperation()*********************************************************************/
















/*** subfunctions *********************************************************************/



uint8_t _sendTx( char* server,char * port, Tx *tx){

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
		printf("\n_sendTx--<error> buffer overflow buffer size=1024 \r");
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

		printf("_sendTx--<error> Error Message : %s", buff+j );
		return 0;

		}else if (buff[i]=='"'){
			i++;
			buff[i+64]='\0';

			if (strcmp(tx->txid().c_str(), buff+i ) == 0){
				printf("\n_sendTx--<info> The TX Sucessfully is sent \n TXid is: %s\r", tx->txid().c_str());
				return 1;
				}else{
					printf("\n_sendTx--<error>  The TX is sent but txid is not the same\r");
					return 0;
					}



	}else
	{
		printf("\n_senddTx--<error>  corrupted received data\r");
		buff[1023]='\0';
		printf("\n_sendTx--<error> The data is = \n%s \r", buff);
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

uint8_t _storePathIndex(char* index){
	if( atoi(index) < 0){
		printf("\n__storePathIndex--<error> Path Index is corrupted/empty the Index is=%d\r",atoi(index));
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
	if (result != FR_OK) {	printf("\n__storePathIndex--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
	//1=mount now
	result = f_mount(&FatHand, "", 1);
	if (result != FR_OK) {	printf("\n__storePathIndex--<error><FATFS>  mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}


	/*********************Write the  "TXIN.txt"**************************/
	char * fname=(char *)"PATHINDEX.txt";

	 result=f_open(&FileHand, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
	 if(result != FR_OK){ printf("\n__storePathIndex--<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result); return 0;}

	 sprintf(buf,"%s\n", index);
	 f_puts((TCHAR*)buf, &FileHand);
	 f_close(&FileHand);


	 result=f_mount(NULL, "", 0);
	if (result != FR_OK) {	printf("\n__storePathIndex--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
return 1;
}

uint8_t _storeRedeem(char* redeem){
	if( strlen(redeem) < 0){
		printf("\n_storeRedeem--<error> Path Index is corrupted/empty the Index is=%d\r",strlen(redeem));
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
	if (result != FR_OK) {	printf("\n_storeRedeem--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
	//1=mount now
	result = f_mount(&FatHand, "", 1);
	if (result != FR_OK) {	printf("\n_storeRedeem--<error><FATFS>  mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}


	/*********************Write the  "TXIN.txt"**************************/
	char * fname=(char *)"REDEEM.txt";

	 result=f_open(&FileHand, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
	 if(result != FR_OK){ printf("\n_storeRedeem--<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result); return 0;}

	 sprintf(buf,"%s\n", redeem);
	 f_puts((TCHAR*)buf, &FileHand);
	 f_close(&FileHand);


	 result=f_mount(NULL, "", 0);
	if (result != FR_OK) {	printf("\n_storeRedeem--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
return 1;
}

uint8_t _checkGuestTX_send(txDataStruct* guestTX, lockDataStruct* keys, txinDataStruct* txin, char * server , char * port) {
		// 1- check inputs, it is spending our contract or not (Txin id = contract tx id)
		// 2- check outputs, if the change address belonge to the lock or not.
		// 3- check inputs, if the public key in the ScriptSIG belonge to the guest or not.
		uint8_t bufferSize= 200;
		uint8_t buffer[bufferSize];
		memset(buffer, '\0', bufferSize);
		size_t sigLen = 0;
		size_t pubLen = 0;

		size_t redeemLen = 0;
		PublicKey pubInTX;
		pubInTX.reset();
		Script redeemScript;
		redeemScript.reset();
		Script scriptSig = guestTX->tx.txIns[0].scriptSig;

		uint8_t tmp[32];
		memset(tmp, '\0', 32);
		char id[65];
		id[64]='\0';

		bool receivedFromTerminal=0 ;
		HDPublicKey pasteGuestXpub;
		/***Parsing *****************************************************************************/

		// check if there is a scripSIG or not
		if(scriptSig.length() < 1 || scriptSig.length() > sizeof(buffer)){
			printf("\n_checkGuestTX--<error> The TX DOES NOT have scriptSig or too long(buffer=360 ) ScriptSig=%d\r", scriptSig.length());
			return 0;
		}

		// copy scriptSig  data to buffer[]  bytes array
		scriptSig.serialize(buffer, sizeof(buffer));

		//SCRIPTSIG { SIGlen , SIG , PUBlen , PUB , redeemLen , redeem }
		sigLen = buffer[1];
		pubLen = buffer[1+sigLen+1];
		pubInTX.parse(buffer + sigLen +3, pubLen);

		//check there is any redeem script or not
		if( sigLen+pubLen+3 > scriptSig.length() ){
			printf("\n_checkGuestTX--<error> The TX DOES NOT have redeem script\r");
			return 0;
		}

		// parse the redeem script in tx
		redeemLen = buffer[1+sigLen+1+pubLen+1];

		redeemScript.push(buffer +1+sigLen+1+pubLen+1+1, redeemLen);
		Script P2SH_Script(redeemScript, P2SH );
		sprintf(keys->script,"%s",redeemScript.toString().c_str()+2); //+2 to skip two first charachter showing the len
		if (strlen(keys->script) < 100) {
			printf("\n_checkGuestTX--<error> The redeem script is wrong\r");
			printf("\n_checkGuestTX--<error> redeem script: %s", keys->script);
			return 0;
		}


		/***1- check TXin ID*****************************************************************************/
		//to little endian
		for(int i=0; i<32; i++) tmp[i] = guestTX->tx.txIns[0].hash[31-i];

		//to hex
		toHex(tmp, 32, id, 64);

		//compare
		if (strcmp(txin->id , (char *)id)!= 0 ) {
			printf("\n_checkGuestTX--<error> check 1/3 -The TX is NOT spending the current Contract TX \r"
					"In your Tx-fund tx Id = %s\r"
					"Contract Tx Id        = %s\r", id, txin->id);
			return 0;
		}

		/***2- check output address with lock address*********************************************************/
		char outAddr[40];
		strcpy (outAddr, guestTX->tx.txOuts[0].address().c_str());
		char desiredaddr[40];
		strcpy( desiredaddr,keys->lockXprv.derive(keys->inPath).address().c_str());

		if ( strcmp( outAddr, desiredaddr) != 0){
			printf("\n_checkGuestTX--<error> check 2/3 -The TX is NOT spending to the lock address (change output). \r"
			"In your Tx-change address  = %s\r"
			"desired changed address    = %s\r", outAddr , desiredaddr);
			return 0;
		}

/***send loop while(isSent)*****************************************************************************/
		bool isSent = 0;
		while(!isSent){
		/***3- check guest Pubkey in the TxIn*****************************************************************************/
////does guest xpub exist in the memory
		if ( (keys->guestXpubIsFound) == 1 ){
			printf("\n_checkGuestXpub--<info> Guest Xpub is found in the lock data storage.\r");
			PublicKey  guestpub = keys->guestXpub.derive(keys->inPath);
			if (pubInTX != guestpub ){
				printf("\n_checkGuestTX--<error> check 3/3 -The TX is NOT spending the desired guest address. \r"
				"The path is           = %s\r"
				"In your Txin address  = %s\r"
				"desired guest address = %s\r", keys->inPath, pubInTX.address().c_str(), guestpub.address().c_str());
			}
////or not so we ask user to provide the xpub
		}else{
			/******Get the Xpub from terminal and validate******/
			printf("\n_checkGuestXpub--<info> The Lock dosent have the Guest xpub(extended public key).\r");

			while(!receivedFromTerminal){
				printf("\n_checkGuestXpub--<info> Please COPY then PASTE ___Guest xpub___ here..\r");
				memset(buffer, '\0', bufferSize);
				_receiveFromTerminal((uint8_t *)buffer, bufferSize, 60) ;
				_removeSpaces((char *)buffer, bufferSize);

				pasteGuestXpub.reset();
				pasteGuestXpub.fromString((const char *)buffer);

				//check xpub is standard or not
				if(!pasteGuestXpub) {
					printf(   "\n_checkGuestXpub--<error> Received data is not a valid Xpub\n"
								 "_checkGuestXpub--<error> Received data is NOT correct.\r"
						    	"\n---------------------------------------------------- DATA: \n%s\r"
							   "\n------------------------------------------------- Try Again\n", (char *)buffer);
					continue;
				}

				//check the xpub belongs to guest or not
				pasteGuestXpub.type = P2PKH; //set the type
				if(pasteGuestXpub.derive(keys->inPath).address() != pubInTX.address())
				{
					printf("\n_checkGuestTX--<error> The entered Xpub is NOT the desired guest xpub( is not the same as the TXin pubkey in the Guest TX). \r"
					"the used path               = %s\r"
					"In your paste xpub, address = %s\r"
					"Desired guest address       = %s\r", keys->inPath, pasteGuestXpub.derive(keys->outPath).address().c_str(), pubInTX.address().c_str());
					continue;
				}
				receivedFromTerminal=1;
				//end while loop for reciving xpub from terminal
				}
				// end if check xpub from terminal
				}


				//send the transaction
				if(_sendTx( server, port, &guestTX->tx)!=1){
							printf("\n_checkGuestTX--<error> The transaction is NOT accepted by the network (mempool) \r");
							return 0;
					}else{
						printf("\n_checkGuestTX--<info> The transaction is accepted by the network (mempool) \r");

						isSent= 1;
					}

				//if _send is succeful
				if(_storeRedeem( (char*)redeemScript.toString().c_str()) != 1)
					printf("\n_checkGuestTX--<error> The redeemScript is NOT stored\r");
				else
					printf("\n_checkGuestTX--<info> The redeemScript is stored in the memory\r");

				if (receivedFromTerminal){
					if(_storeGXpub( (char *)pasteGuestXpub.toString().c_str()) != 1){
					printf("\n_checkGuestTX--<error> the guest Xpub is NOT stored.");
					}else printf("\n_checkGuestTX--<info> the guest Xpub is stored in the memory");
				}


//end of while(isSent)*****************************************************************/
				}
		//we need this for new contract
				strcpy(keys->lastUnlockTxId, guestTX->tx.txid().c_str());
			return 1;
}

uint8_t _checkContractTX( txDataStruct* contractTX, lockDataStruct* keys) {

	int bufferSize=360;
	uint8_t buffer[bufferSize];
	memset(buffer, '\0', bufferSize);
	size_t sigLen = 0;
	size_t pubLen = 0;
	PublicKey pubInput;
	pubInput.reset();



	//input public key should be belonged to the owner(0) or lock(index)
	// start checking the __Tx Inputs__ of the TX TXinputs[i]

	Script scriptSig = contractTX->tx.txIns[0].scriptSig;

	if(scriptSig.length() < 1 || scriptSig.length() > sizeof(buffer)){
		printf("\n_checkContractTX--<error> Contract TX is not valid or too long bufferSize= %d, ScriptSigSize= %d\r",bufferSize , scriptSig.length());
		return 0;
		}

	// copy scriptSig  data to buffer[]  bytes array
	scriptSig.serialize(buffer, sizeof(buffer));

	//SCRIPTSIG { SIGlen , SIG , PUBlen , PUB , redeemLen , redeem }
	sigLen = buffer[1];
	pubLen = buffer[sigLen+2];
	pubInput.parse(buffer + sigLen +3, pubLen);


	// check pub in tx is equal with owner or lock public key in the current index


	// (current contract) {input (index-1), output(index)  }
		memset(buffer, '\0', bufferSize);
		sprintf( (char *)buffer, "%s%d", keys->derivativePath, atoi(keys->index)-1);
	// generate the lock address
		sprintf( (char *)buffer, "%s", keys->lockXprv.derive((char *)buffer).address().c_str());

	///IsOwner?
			if (strcmp( keys->ownerAdr, pubInput.address().c_str()) == 0){
			sprintf(keys->index , "%d" , 1);
			printf("\n_checkContractTX--<info> The contract TX belonges to the OWNER. \r");

	///IsLock?
			}else if (strcmp( (char *)buffer, pubInput.address().c_str()) == 0){
			printf("\n_checkContractTX--<info> The contract TX belonges to the LOCK. \r");


	///else?
			}else{

			printf("\n_checkContractTX--<error> Contract TX is not valid. \r"
							"\nIn contract TX ,input address    = %s\r"
							"\nOwner address        = %s\r"
							"\nLock address        = %s\r"
							"\nDesired Path (owner index =0)= %s%s\r", pubInput.address().c_str(), keys->ownerAdr , (char *)buffer, keys->derivativePath, keys->index );

			return 0;

		}
//////////////////////////



		printf("\n_checkContractTX--<info> Received Contract TX is valid.\r");
		return 1;

}


uint8_t _receiveFromTerminal(uint8_t *TEMP, int TEMP_SIZE, int waitSeconds){

	extern UART_HandleTypeDef huart2; //bring uart2 from main.cpp
	extern DMA_HandleTypeDef hdma_usart2_rx;

	printf(   "\n  ____           _         _   _ _____ ____  _____ _ \n"
				" |  _ \\ __ _ ___| |_ ___  | | | | ____|  _ \\| ____| |\n"
				" | |_) / _` / __| __/ _ \\ | |_| |  _| | |_) |  _| | |\n"
				" |  __/ (_| \\__ \\ ||  __/ |  _  | |___|  _ <| |___|_|\n"
				" |_|   \\__,_|___/\\__\\___| |_| |_|_____|_| \\_\\_____(_)\n");


	printf("\n_receiveFromTerminal--<info> Paste HERE! Waiting ..");


	while(strlen((const char*)TEMP) == 0) {
				HAL_UARTEx_ReceiveToIdle_DMA(&huart2, TEMP , TEMP_SIZE);			//Receive data from terminal
				__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT); 					//disable half-recevied interrupt

				if (waitSeconds< 0) NVIC_SystemReset();

				HAL_Delay(500);
				printf(".");
				HAL_Delay(500);
				printf("\b");// waiting point

				printf("%03d",waitSeconds);
				fflush(stdout);
				HAL_Delay(500);
				printf("\b\b\b");
				fflush(stdout);
				waitSeconds--;
			}
return 1;
}

void _removeSpaces(char* TEMP, int TEMP_SIZE){

	char tx[1024];
	memset(tx, '\0', 1024);

	//data cleaning
	int j=0;
	for(int i = 0 ; i<(int)TEMP_SIZE && i<1024 ; i++ )	{if(TEMP[i] != ' ' &&  TEMP[i] != '\0' && TEMP[i] != '\r' &&  TEMP[i] != '\n' &&  TEMP[i] != '\t'){tx[j] = TEMP[i]; j++;}	}
	tx[j] = '\0';
	memset(TEMP, '\0', TEMP_SIZE);
	strcpy(TEMP, tx);

}

uint8_t _storeGXpub( char *GuestXpub){

	/***needs  MX_FATFS_Init(); in the main.c ***************************************/
	// variables for FatFs
	FATFS FatHand; 			//Fatfs handle
	FIL FileHand; 			//File handle
	FRESULT result; 		//Result after operations
	uint8_t bufSize=128;
	char buf[bufSize];
	/*********************************FATFS Mounting*********************************/
	//0- Unmount
	result=f_mount(NULL, "", 0);
	if (result != FR_OK) {	printf("\n_storeGXpub--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
	//1=mount now
	result = f_mount(&FatHand, "", 1);
	if (result != FR_OK) {	printf("\n_storeGXpub--<error><FATFS>  mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}


	/*********************Write the  "TXIN.txt"**************************/
	char * fname=(char *)"GXPUB.txt";

	 result=f_open(&FileHand, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
	 if(result != FR_OK){ printf("\n_storeGXpub--<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result); return 0;}

	 sprintf(buf,"%s\n", GuestXpub);
	 f_puts((TCHAR*)buf, &FileHand);
	 f_close(&FileHand);


	 result=f_mount(NULL, "", 0);
	if (result != FR_OK) {	printf("\n_storeGXpub--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}



	return 1;
}

uint8_t _checkRevokeTx(txDataStruct* revokeTx, lockDataStruct* keys, txinDataStruct* txin) {

		// 1- check inputs, it is spending our contract or not (Txin id = contract tx id).
		// if index = 1 there is not any contract and the data in the TxIn.txt is old for the previous iteration
		// so we need to skip this check

		// 3- check inputs, if the public key is in the ScriptSIG belonge to the owner or not.

		uint8_t bufferSize= 200;
		uint8_t buffer[bufferSize];
		memset(buffer, '\0', bufferSize);
		uint8_t tmp[32];
		memset(tmp, '\0', 32);
		char id[65];
		id[64]='\0';

		size_t sigLen = 0;
		size_t pubLen = 0;

		PublicKey pubInTX;
		Script scriptSig;
		 scriptSig.reset();
//		for(int i ; i < (int)revokeTx->tx.inputsNumber ; i ++) {
//
//			if( revokeTx->tx.txIns[i].scriptSig.type() == P2SH ) {
//				scriptSig = revokeTx->tx.txIns[i].scriptSig;
//				break;
//			}
//		}
		scriptSig = revokeTx->tx.txIns[0].scriptSig;

		/***Parsing the TX*****************************************************************************/

		// check if there is a scripSIG or not
		if(scriptSig.length() < 1 || scriptSig.length() > sizeof(buffer)){
			printf("\n_checkRevokeTx--<error> The TX DOES NOT have scriptSig or too long(buffer=360 ) ScriptSig=%d\r", scriptSig.length());
			return 0;
		}

		// copy scriptSig  data to buffer[]  bytes array
		scriptSig.serialize(buffer, sizeof(buffer));

		//SCRIPTSIG { SIGlen , SIG , PUBlen , PUB , redeemLen , redeem }
		sigLen = buffer[1];
		pubLen = buffer[1+sigLen+1];

		pubInTX.reset();
		pubInTX.parse(buffer + sigLen +3, pubLen);



		/***1- check TXin ID*****************************************************************************/
		//		check if index >1
		if ( atoi(keys->index) != 1 )
		{
			//to little endian
			for(int i=0; i<32; i++) tmp[i] = revokeTx->tx.txIns[0].hash[31-i];

			//to hex
			toHex(tmp, 32, id, 64);

			//compare
			if (strcmp(txin->id , (char *)id)!= 0 ) {
				printf("\n_checkRevokeTx--<error> check 1/2 -The TX is NOT spending the current Contract TX \r"
						"In your Tx-fund tx Id = %s\r"
						"Contract Tx Id        = %s\r", id, txin->id);
				return 0;
			}
		}


		/***3- check input pubkey with owner address*********************************************************/

		//IsOwner?
				if (strcmp( keys->ownerAdr, pubInTX.legacyAddress().c_str()) == 0){
				printf("\n_checkContractTx--<info> The Revoke TX belonges to the OWNER. \r");
				}else{
					printf("\n_checkContractTx--<error> Revoke TX is not valid. \r"
							"\nIn contract TX ,input address = %s\r"
							"\nDesire address(Owner)         = %s\r",
							pubInTX.address().c_str(), keys->ownerAdr );

				return 0;
				}
			return 1;
}


uint8_t _refundTx(Tx* tx, lockDataStruct* keys, txinDataStruct* TxIn_contract, int fee){
	// send the remaining fund in the latest contrat to the owner


	/**********1- txOut_change **************************/

	TxIn_contract->fund= TxIn_contract->fund - fee;
	TxOut txOut_Change( keys->ownerAdr , TxIn_contract->fund);
	if (!txOut_Change.isValid()){
		printf("\n_refundTx--<error> txOut_Change is NOT VALID. \r");
	}

	/**********2- txIns **************************/

	TxIn txInFromContract((const char *)TxIn_contract->id, TxIn_contract->index);

	/**********5- PrepareTX **************************/
	tx->addInput(txInFromContract);

	tx->addOutput(txOut_Change);


	tx->signInput( 0, keys->lockXprv.derive(keys->inPath));



return 1;
}

uint8_t _pathUpdate(lockDataStruct* keys){
	// m/0/index
	char iteration[6];
	sscanf(keys->derivativePath, "m/%s/\n",iteration );

	sprintf(keys->derivativePath, "m/%d/\n", atoi(iteration)+1);
	sprintf(keys->index, "%d\n", 1);

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
		if (result != FR_OK) {	printf("\n_PathUpdate--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
		//1=mount now
		result = f_mount(&FatHand, "", 1);
		if (result != FR_OK) {	printf("\n_PathUpdate--<error><FATFS>  mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}


		/*********************Write the  "PATH.txt"**************************/
		char * fname=(char *)"PATH.txt";

		 result=f_open(&FileHand, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
		 if(result != FR_OK){ printf("\n_PathUpdate--<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result); return 0;}

		 sprintf(buf,"%s\n", keys->derivativePath);
		 f_puts((TCHAR*)buf, &FileHand);
		 f_close(&FileHand);
		/*********************Write the  "PATHINDEX.txt"**************************/
		fname=(char *)"PATHINDEX.txt";

		 result=f_open(&FileHand, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
		 if(result != FR_OK){ printf("\n_PathUpdate--<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result); return 0;}

		 sprintf(buf,"%d\n", 1);
		 f_puts((TCHAR*)buf, &FileHand);
		 f_close(&FileHand);


		 result=f_mount(NULL, "", 0);
		if (result != FR_OK) {	printf("\n_PathUpdate--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}

		printf("\n_PathUpdate--<info><FATFS> Updationg Deravative Path is succesfull new path= %s",keys->derivativePath);

		return 1;

}

