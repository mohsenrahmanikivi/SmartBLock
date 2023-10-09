/*
 * _lockOperation.cpp
 *
 *  Created on: Aug 21, 2023
 *      Author: mrahm
 */


#include <__lockOperation.h>

/********Read data from terminal variable **********************************************/


const size_t TEMP_SIZE = 1024;
uint8_t TEMP[TEMP_SIZE]; // to store received data from terminal

extern UART_HandleTypeDef huart2; //bring uart2 from main.cpp
extern DMA_HandleTypeDef hdma_usart2_rx;

	/***************************************************************************************/

uint8_t __lockOperation( char* server, char* port, lockDataStruct *keys, txinDataStruct *TXIN, int waitSeconds ){


	txDataStruct contractUTXO;
	txDataStruct scriptUTXO;
	txDataStruct txGuest;
	txGuest.tx.reset();
	uint8_t res;
	char scriptAddr[65];
	memset(scriptAddr, '\0', 65);
	const char* lockAddrInPath = keys->lockXprv.derive(keys->inPath).address().c_str();

	/*****Search ******************************************************************************/
	//1- Search for the contract based on Lock address
	for ( res = _addrCheck(server, port, (char *)lockAddrInPath, &contractUTXO) ; res == 0 ; ) {
		printf("\n__lockOperation--<error> Lock Operation Failed \n");
		for(int i=0 ; i> waitSeconds ; i++){printf(".");HAL_Delay(500); printf("\b"); HAL_Delay(500); } // waiting point
		}
	//1 = address is incorrect or has a error
	if(res == 1 )
		{ 	printf("\n__lockOperation--<error> Error: Lock address is incorrect or has an error \r");
			return 0;
		}
	//2 = address is correct but not there is not any UTXO
	if( res == 2)
		{ 	printf("\n__lockOperation--<info> A new contract is NOT found \r");
			printf("\n__lockOperation--<info> Searching for a new contract...\r");
			do
			{	HAL_Delay(waitSeconds*1000);
				res=_addrCheck(server, port, (char *)lockAddrInPath, &contractUTXO);
			}while (res!=3);
		}
	if( res != 3){ return 0;}

	//3 = new contract is found
	printf("\n__lockOperation--<info> A new ContractTX is  found \r");

	while(_getAndVerifyTx(server, port, (char *)lockAddrInPath, &contractUTXO) == 0 ){
		printf("\n__lockOperation--<error> downloading the ContractTx is faced unexpected error\r");
		return 0;
	}
	/****************************************************************************************/

	//2- Feed the TXDataStruct with data from contractTX (TxIn ID, Index, P2SH address )

	//ID
	strcpy(TXIN->id,(char *)contractUTXO.tx.txid().c_str());
	//Index
	for(size_t i=0; i>contractUTXO.tx.outputsNumber; i++) 	if(strcmp(contractUTXO.tx.txOuts[i].address().c_str(), lockAddrInPath )== 0)	TXIN->index=i;
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


	/****************************************************************************************/

//	3- Check the contrct
	if(_checkContractTX( &contractUTXO, keys) !=1 ){
		printf("\n__lockOperation--<error> The contract is NOT valid.");
							return 0;
	}






	/****************************************************************************************/
	//4- waiting for the unlock transaction
		printf("\n__lockOperation--<info> Please COPY then PASTE Guest transaction here. ");
		printf("\n  _____          _          _    _ ______ _____  ______   _ \n"
				 " |  __ \\        | |        | |  | |  ____|  __ \\|  ____| | |\n"
				 " | |__) |_ _ ___| |_ ___   | |__| | |__  | |__) | |__    | |\n"
				 " |  ___/ _` / __| __/ _ \\  |  __  |  __| |  _  /|  __|   | |\n"
				 " | |  | (_| \\__ \\ ||  __/  | |  | | |____| | \\ \\| |____  |_|\n"
			     " |_|   \\__,_|___/\\__\\___|  |_|  |_|______|_|  \\_\\______| (_)\n");




		//0- Main receive loop while(!receive)
		bool receive = false;
while(!receive){
		printf("\n__lockOperation--<info> Paste HERE! Waiting ..");
		memset(TEMP, '\0', TEMP_SIZE);

		_receiveFromTerminal(TEMP, TEMP_SIZE) ;
		_removeSpaces((char *)TEMP, TEMP_SIZE);
		txGuest.tx.fromString((char *)TEMP);

		//1- read transaction from user (guest) // if failed , wait to receive again
		if (!txGuest.tx){
			printf("\n__lockOperation--<error> The data DOES NOT contain a correct transaction.\n"
					 "--Received data is----\\/\n%s\n"
					 "----------------------/\\\n", (char *)TEMP);
			receive = false;

		}else {

		//1-2- check the guest transaction (unlock tx) spend the contract tx or not // if failed ,wait to receive again
		if( _checkGuestTX( &txGuest, keys, TXIN)!= 1){
			printf("\n__lockOperation--<error> The transaction is NOT match with the Contract \r");
			receive = false;

		}else{
		//1-2-3- send transaction to the network //wait to receive again
		if(_sendTx( server, port, &txGuest.tx)!=1){
				printf("\n__lockOperation--<error> The transaction is NOT accepted by the network (mempool) \r");
				receive = false;
		}else{
			printf("\n__lockOperation--<info> The transaction is ACCEPTED by the network (mempool) \r");
			receive = true;
		//1-2-3-end
		}
		//1-2-end
		}

		//1-end
		}

		//0- while(!receive) end
		 }

	/****************************************************************************************/
	//5- execute the unlock operation

	 _servoUnlockTimeout(25);




	 /****************************************************************************************/
	 //6- generate a new Contractn



#ifdef generateContract
	//6- prepare the new contract and send it
	Tx tx;
	int fee= 350;
	char* GuestSeq=(char*)"1";
	char* OwnerSeq=(char*)"1";

	if (_genTXContract (&tx,
						fee,
						TXIN,
						keys,
						GuestSeq,
						OwnerSeq)==1 ){
			printf("\n__lockOperation--<info> Contract TX is correctly created \r");

				// send
			if(_sendTx( server, port, &tx)==1){
				printf("\n__lockOperation--<info> Contract TX is correctly Sent \r");
				strcpy(TXIN->id,(char *)tx.txid().c_str());
				TXIN->index=2;
				TXIN->fund= TXIN->fund - (2*fee);

				//store NEW TxIN
				if(_storeTxIN(TXIN->id, TXIN->index, TXIN->fund) != 1)
					printf("\n__lockOperation--<error> NEW (TXid, index, fund) is NOT saved\r");
				else printf("\n__lockOperation--<info> NEW TxIN saved successfully \r");

				//store NEW script address
				if(_storeScriptAdr((char *) tx.txOuts[0].address().c_str()) != 1) {
					printf("\n__lockOperation--<error> NEW scriptAdr is NOT saved \r");
				}
				else {
					printf("\n__lockOperation--<info> NEW script address saved successfully scriptAdr= %s \r", tx.txOuts[0].address().c_str());
					//Store out put path index/////
					char outPath_index[16];
					sprintf(outPath_index, "%d", atoi(keys->index)+1); // increase 1 as it would be OutPath for future use
					if(_storePathIndex(outPath_index) == 0) {
						printf("\n_storePathIndex--<error> Path_index save operation failed");
						return 0;
					}
					printf("\n__lockOperation--<info> Path_index is updated to: %s and stored", outPath_index);
					///////////////////////////////
				}
			}else{
				printf("\n__lockOperation--<error> TX send error tx=\n%s \r", tx.toString().c_str());
			}
	}
#endif




return 1;
}


/***FiniSH _lockOperation()*********************************************************************/
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
				printf("\n_sendTx--<info>  TXid is: %s\r", tx->txid().c_str());
				return 1;
				}else{
					printf("\n_sendTx--<error>  TX is sent but txid is not the same\r");
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

uint8_t _checkGuestTX(txDataStruct* guestTX, lockDataStruct* keys, txinDataStruct* txin) {
	// check it is spending our contract or not
	//in input , redemm should be correct
	//when the in input ,public key should be belonged to the guest
	//in output, address should be to the lock
	uint8_t buffer[200];
	memset(buffer, '\0', 200);
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

	for(int i=0; i<32; i++){
		tmp[i] = guestTX->tx.txIns[0].hash[31-i];
	    }
	char id[65];
	id[64]='\0';
	toHex(tmp, 32, id, 64);
	if (strcmp(txin->id , (char *)id)!= 0 ) {
		printf("\n_checkGuestTX--<error> The TX is NOT spending the current Contract TX \r"
				"In your Tx-fund tx Id = %s\r"
				"Contract Tx Id        = %s\r", id, txin->id);
		return 0;
	}


	// check ScriptSIG
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


   		// check pub in tx is equal with guest(index) pub
   		PublicKey guestPub=keys->guestXpub.derive(keys->outPath);
   		if (pubInTX != guestPub){
   			printf("\n_checkGuestTX--<error> The transaction DOES NOT belong to a valid guest\r"
   					"In your TX-guest address = %s\r"
   					"Desired guest address    = %s\r"
   					"Desired guest path       = %s\r", pubInTX.address().c_str(), guestPub.address().c_str(), keys->outPath);
   			return 0;
   		}

   		//check there is any redeem script or not
   		if( sigLen+pubLen+3 > scriptSig.length() ){
   	   		printf("\n_checkGuestTX--<error> The TX DOES NOT have redeem script\r");
   	   		return 0;
   	   	}

   		// check redeem in tx
   		redeemLen = buffer[1+sigLen+1+pubLen+1];



		redeemScript.push(buffer +1+sigLen+1+pubLen+1+1, redeemLen);

		Script P2SH_Script(redeemScript, P2SH );

		sprintf(keys->script,"%s",P2SH_Script.toString().c_str());

		char * x=(char *)P2SH_Script.address().c_str();

		if( strcpy(x, keys->scriptAdr ) !=0){
			printf("\n_checkGuestTX--<error> The transaction DOES NOT have valid redeemScript\r"
					"In your TX-Script address = %s\r"
					"Desired Script address    = %s\r", P2SH_Script.address().c_str(), keys->scriptAdr);
			return 0;
		}

		if(_storeRedeem( (char*)redeemScript.toString().c_str()) != 1){
			printf("\n_checkGuestTX--<error> The redeemScript is NOT stored\r");
		}

		return 1;
}

uint8_t _checkContractTX( txDataStruct* contractTX, lockDataStruct* keys) {

	//input public key should be belonged to the owner
	uint8_t buffer[360];
	memset(buffer, '\0', 360);

	size_t sigLen = 0;
	size_t pubLen = 0;


	PublicKey pubInTX;
	pubInTX.reset();


	Script scriptSig = contractTX->tx.txIns[0].scriptSig;

	if(scriptSig.length() < 1 || scriptSig.length() > sizeof(buffer)){
		printf("\n_checkGuestTX--<error> Contract TX is not valid or too long(buffer=360 ) ScriptSig=%d\r", scriptSig.length());
		return 0;
	}

		// copy scriptSig  data to buffer[]  bytes array
		scriptSig.serialize(buffer, sizeof(buffer));

		//SCRIPTSIG { SIGlen , SIG , PUBlen , PUB , redeemLen , redeem }

		sigLen = buffer[1];
		pubLen = buffer[sigLen+2];
		pubInTX.parse(buffer + sigLen +3, pubLen);


		// check pub in tx is equal with guest(index) pub

		if (strcmp(pubInTX.address().c_str(), keys->ownerAdr) != 0){
			printf("\n_checkGuestTX--<error> Contract TX is not valid, Owner of the Tx is not valid. \r"
					"\nIn your TX ,Owner address= %s\r"
					"\nDesired Owner address    = %s\r", pubInTX.address().c_str(), keys->ownerAdr );
			return 0;
		}

		return 1;

}



uint8_t _receiveFromTerminal(uint8_t *TEMP, int TEMP_SIZE){

	while(strlen((const char*)TEMP) == 0) {
				HAL_UARTEx_ReceiveToIdle_DMA(&huart2, TEMP , TEMP_SIZE);			//Receive data from terminal
				__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT); 					//disable half-recevied interrupt
				HAL_Delay(500);
				printf(".");
				HAL_Delay(500);
				printf("\b");// waiting point
			}
return 1;
}

void _removeSpaces(char* temp, int temp_size){

	char tx[1024];
	memset(tx, '\0', 1024);

	//data cleaning
	int j=0;
	for(int i = 0 ; i<(int)TEMP_SIZE && i<1024 ; i++ )	{if(TEMP[i] != ' ' &&  TEMP[i] != '\0' && TEMP[i] != '\r' &&  TEMP[i] != '\n' &&  TEMP[i] != '\t'){tx[j] = TEMP[i]; j++;}	}
	tx[j] = '\0';
	memset(temp, '\0', temp_size);
	strcpy(temp, tx);

}

