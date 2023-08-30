/*
 * _update.c
 *
 *  Created on: Oct 7, 2022
 *      Author: Administrator
 */


#include <_ESP8266_HAL.h>
#include <_search.h>
#include <___merkle.h>

//this function search to find the latest utxo related to the address and put it in th txDataStruct for further use
uint8_t _search(char* server, char* port, char* adr, txDataStruct* tx){

	//1-first check adr
	if (strlen(adr)== 43) {
		printf("\n_search--error Check the address in the _search() function");
		return 0;
	}

	//2- get txid and hight
	return _getUTXO( server, port, adr,tx);
}



uint8_t _getUTXO (char* server, char* port, char* adr, txDataStruct* tx){

	int rBuffSize=2048;
	char rBuff[rBuffSize];

	int size= 57+ strlen(adr);

	//1- prepare Request
	sprintf (rBuff, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
			"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
			"Content-Type: application/json\r\nContent-Length: %d\r\n\r\n"
			"{\"method\":\"scantxoutset\",\"params\":[\"start\",[\"addr(%s)\"]]}\r\n",server,port,size,(char *)adr);  // varibale are diffrent

	printf("\n_getUTXO--<info> UTXO request for : %s\r", (char *)adr);
	//3- send request and receive
	while(ATreceive_Timeout(rBuff, rBuffSize, (char *)"\"height\":", server, port, 50000) !=1){
		printf("\n_getUTXO--<error> no data received\r");
		return 0;
	}

	// Data cleaning
	int i=0;

	memset( tx->txid,'\0', 65);
	memset( tx->hight,'\0', 16);

	while (i < rBuffSize ){

		if (rBuff[i]=='t' && rBuff[i+1]=='x'  && rBuff[i+2]=='i'  &&  rBuff[i+3]=='d' )	{
			i=i+7;
			for(int j=0; j< 64 ; j++){ 	tx->txid[j]=rBuff[i]; 	i++; }
		}

		if (rBuff[i]=='h' && rBuff[i+1]=='e' && rBuff[i+2]=='i'  &&  rBuff[i+3]=='g' &&  rBuff[i+4]=='h' &&  rBuff[i+5]=='t')	{
			i=i+8;
			int j=0;
			while(rBuff[i]!= '}') 	{ 	tx->hight[j]=rBuff[i]; 	i++; j++; }
			break;
		}


		i++;
	}


	printf("\n_getUTXO--<info> txID=%s \r", tx->txid );
	printf("\n_getUTXO--<info> txHight=%s\r", tx->hight);
	/****************************************check data ****************************************/
	if ( atoi(tx->hight) <= 0)  {
	   printf("\n_getUTXO--<error> txHight is corrupted  \nReceived TxHight = %s\n Retry...\r", tx->hight);
	   return 0;
	}

	if ( strlen(tx->txid) != 64)  {
	   printf("\n_getUTXO--<error>  txid is corrupted  length=%d  \nReceived TxID  = %s\n Retry...\r", strlen(tx->txid), tx->txid);
	   return 0;
	}

	 /*****INPUT  hight ********************get block ID(HASH) *********************************/
	//0- counting digits
	 int digit=0;
	 int t=atoi(tx->hight);
	 do { t /= 10;   ++digit;  } while (t != 0);
	 //1- content len
	 int contentLen=40+digit;

	 //2- variable for receiving
	 int tmpSize=65+1; 									 //it has an extra quotation
	 char tmp[tmpSize];
	 tmp[tmpSize-1]='\0';
	 //3-prepare the request
	 memset(rBuff, '\0', rBuffSize);
	 sprintf (rBuff, "POST / HTTP/1.1\r\nHost: %s:%s\r\nAuthorization: Basic dXNlcjpwYXNzd29yZA==\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n{\"method\": \"getblockhash\", \"params\": [%d]}\r\n", server , port ,contentLen,atoi(tx->hight));
	 //4-send the request
	 while(receiveResult (rBuff, tmp, tmpSize, (char *)server, (char *)port) !=1)  HAL_Delay(1000);
	 //5-prepare the result
	 char blockHash[65];
	  memset(blockHash, '\0', 65);
	  for(int i=0; i<64; i++)  blockHash[i]=tmp[i+1]; //avalish qutation hast

	 /*  RESULT     blockHash[]   *//******************************************************/

	/***************************************check data *********************************/
	   if ( strlen(blockHash) != 64)  {printf("\n_getUTXO--<error>  blockHash is corrupted length=%d \n Received blockHash:%s \n Retry...\r", strlen(blockHash), blockHash );
	   return 0;
	   }
	/********************************************get TX ********************************************/
	//1- prepare Request
	   char buff2[1024];
	   memset(buff2,'\0',1024);

	memset(rBuff,'\0',rBuffSize);
	sprintf (buff2, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
	"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
	"Content-Type: application/json\r\nContent-Length: 185\r\n\r\n"
	"{\"method\":\"getrawtransaction\",\"params\":[\"%s\", false, \"%s\"]}\r\n", server, port, tx->txid , blockHash);

	//printf(buff);

	//3- send request and receive
	while(ATreceive (buff2 , (char*)"result\":\"", rBuff,  rBuffSize,  (uint8_t *)server, (uint8_t *)port) != 1)  HAL_Delay(1000);

	//4- prepare the receive data

	int k=0;
	while (rBuff[k]!='"' || k>rBuffSize) k++;
	if (k >rBuffSize) {
		printf("_getrawTX--> The transaction is longer than Buffer \nBufferSize:%d \r" , rBuffSize);
		return 0;
	}
	rBuff[k]='\0';

	tx->tx.fromString(rBuff);

	if (tx->tx.getStatus()!=0){
		printf("\n tx is corrupted parsing problem (PARSING_DONE=0)=%d\r", tx->tx.getStatus());
		printf("_getrawTX--> This is the raw TX:\n%s\r" , rBuff);
		return 0;
	}


	printf("_getrawTX--> Recieved a correct TX\r" );

	/***********************************Verify block id************************************/
	//check if the tx block id is as same as stored block ids(headers).

	//1 based on the hight, open the file and read the block id


		/***********************************FATFS Variables*******************************/

		FATFS FatFs; 			//Fatfs handle
		FIL fil; 				//File handle
		FRESULT fres; 			//Result after operations
		UINT bytesNUM;
		/***********************************Find the file**********************************/
		int hight=atoi(tx->hight);
		int first	= floor(hight/100);


		char folderName[32];
		long int f1000= floor(first/1000);
		sprintf(folderName,"HEADERS/%ld_%ld" , f1000*10000 , ((f1000+1)*10000)-1);
		memset(tmp, '\0', tmpSize);
		sprintf (tmp,"%s/%d.txt",folderName, first*100);


		/********************************FATFS Mounts the SD memory************************/
		//0- Unmount
		f_mount(NULL, "", 0);

		//1=mount now
		fres = f_mount(&FatFs, "", 1);
		//3=Check mounting
		if (fres != FR_OK) {	printf("\n_getUTXO f_mount error, Error Code=(%i)\r", fres); 	return 0;	}
		/*********************************FATFS Opens file*********************************/
		fres = f_open(&fil, tmp, FA_READ );
		if(fres != FR_OK) {printf("\n_getUTXO f_open error File Name=%s, Error Code=%d\r", tmp, fres); return 0;  }

		//00- number of characters per line
		int pointerToFile=0;   //Pointer to file from top of file to this address
		int chPerLine=0;

		for (int i=0 ; i < hight%100 ; i++){
			uint8_t digit2=0;
			int t2=hight;
			do { t2 /= 10;   ++digit2;  } while (t2 != 0);
			chPerLine=227+digit2;
			pointerToFile+= chPerLine;
			fres=f_lseek (&fil, pointerToFile);
			if(fres != FR_OK) { printf("\n_getUTXO--> f_lseek error, File Name=%s, Error Code=%d\r",tmp, fres); return 0; }
		}

		memset(rBuff, '\0', rBuffSize);
		fres = f_read(&fil, rBuff, chPerLine, &bytesNUM);
		if(fres != FR_OK){	printf("\n_getUTXO--> f_read error, File Name=%s, Error Code:%d\r",tmp, fres); return 0;}


		//printf("\n_getUTXO--> Block %d in %s :\n%s", hight, tmp,  rBuff);

		uint8_t digit2=0;
		int t2=hight;
		do { t2 /= 10;   ++digit2;  } while (t2 != 0);
		char HASH[64+1];
		HASH[64]='\0';

		for(int i=0; i<64; i++) HASH[i]=rBuff[i+digit2+1];
		if (strcmp(HASH, blockHash)==0)	printf("\n_getUTXO--> Block ID is verified") ;
		else{
			printf("\n_getUTXO--> Block ID is NOT verified" );
			return 0;
		}

		 //4-close
		 f_close(&fil);
		 /********************************FATFS Finish   **********************************/



	//2 compare the stored block id with download block id
	/***********************************GET the merkle proof******************************/
		 memset(rBuff, '\0', rBuffSize);
		 sprintf (rBuff, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
		 			"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
		 			"Content-Type: application/json\r\nContent-Length: 176\r\n\r\n"
		 			"{\"method\": \"gettxoutproof\", \"params\": [[\"%s\"],\"%s\"]}\r\n",server,port, tx->txid, blockHash);
		 //3- send request and receive
		 	while(ATreceive_Timeout (rBuff, rBuffSize , (char*)"result\":\"" , server , port , 30000) !=1)  HAL_Delay(1000);


		 	t2=0;
		 	while(rBuff[t2]!='"') t2++;
		 	rBuff[t2]='\0';
	/***********************************Verify tx id by merkle proof************************/
		 	if (merkle(rBuff,tx->txid)!=1) {
		 		printf("\n_getUTXO--> TX ID is NOT verified (merkle)" );
		 		return 0;

		 	}

		 	printf("\n_getUTXO--> TX ID is verified (merkle)" );



	return 1;


}



