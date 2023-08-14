/*
 * _update.c
 *
 *  Created on: Oct 7, 2022
 *      Author: Administrator
 */


#include <iostream>
#include <Bitcoin.h>
#include "stm32f4xx_hal.h"

#include <_backbone.h>
#include <_ESP8266_HAL.h>
#include "_update.h"
#include "fatfs.h"


using namespace std;

/* Key structure
	keys[0].key; //root
	keys[1].key; //Change,0
	keys[2].key; //Receive,0
	keys[3].key; //Change,1
	keys[4].key; //Receive,1
*/



uint8_t _update(char* fileName, lockDataStruct* keys, char* server, char* port){

	HDPrivateKey prv=keys->lockXprv;

	string adrRcv0=prv.address();

	//get utxo of address
//	char * adr=(char*)adrRcv0.c_str();
	char * adr=(char*)"tb1q4n5j4nfn5c6jc5drw6vl3h7lm77dldefr3897k";
	//0- Define variable
		char buff[512];
		memset(buff,'\0',512);
		while(_getUnSpendTXid(adr, server, port, buff, 512)!= 1){ };


	// write on disk
			/***********************************Variables************************************/

			FATFS FatFs; 	//Fatfs handle
			FIL fil; 		//File handle
			FRESULT fres; //Result after operations

		  	/********************************FATFS Mounting**********************************/
			//0- Unmount
			f_mount(NULL, "", 0);
			printf("\nUPDT--> UNMount the SDRAM");
			//1=mount now
			fres = f_mount(&FatFs, "", 1);
			//3=Check mounting
			if (fres != FR_OK) {	printf("UPDT--> f_mount error (%i)\r\n", fres); 	while(1);	}
			printf("\nUPDT--> Mount   the SDRAM");
			/********************************Write        **********************************/
			fres = f_open(&fil, fileName, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
			if(fres != FR_OK) {printf("\nUPDT--> f_open error (%i)\r\n", fres); return 0;  }

			 //2-Write

			 int res=f_puts(buff, &fil);
			 if(fres != FR_OK) {	printf("\nUPDT--> f_write  error (%i)\r\n",res); return 0;   }

			 //4-close
			 f_close(&fil);
			 /********************************Finish        **********************************/


	printf("\nUPDATE-> Wallet Update Successfully.");
	return 1;
}

struct UTXO {

};

uint8_t _getUnSpendTXid(char* adr,char* server, char* port, char* rBuff, int rBuffSize){


	int size= 57+ strlen(adr);

	//1- prepare Request
	sprintf (rBuff, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
			"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
			"Content-Type: application/json\r\nContent-Length: %d\r\n\r\n"
			"{\"method\":\"scantxoutset\",\"params\":[\"start\",[\"addr(%s)\"]]}\r\n",server,port,size,(char *)adr);  // varibale are diffrent

	printf("\n_getUnSpend--<info> UTXO request for : %s\r", (char *)adr);
	//3- send request and receive
	while(ATreceive_Timeout(rBuff, rBuffSize, (char *)"\"height\":", server, port, 30000) !=1){
		printf("\n_getUnSpend--<error> no data received\r");
		return 0;
	}

	int i=0;
	char txhight[16];
	memset( txhight,'\0', 16);

	while(rBuff[i] != ',') {
		txhight[i]=rBuff[i];
		i++;
	}
	while (rBuff[i]!='t' || rBuff[i+1]!='x'  || rBuff[i+2]!='i'  ||  rBuff[i+3]!='d' )	{
		i++;

		if(i > rBuffSize) {
			printf("\n_getUnSpend--<info> txID=NOT FOUND\r");
			return 0;
		}

	}
i=i+7;
	char txID[65];
	txID[64]='\0';
	for(int j=0; j< 64 ; j++){
		txID[j]=rBuff[i];
		i++;

	}

	printf("\n_getUnSpend--<info> txID=%s \r", txID );
	printf("\n_getUnSpend--<info> txHight=%s\r", txhight);




return 1;

}


