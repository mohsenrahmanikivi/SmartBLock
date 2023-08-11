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
	char * adr=(char*)adrRcv0.c_str();

	//0- Define variable
		char buff[1024];
		memset(buff,'\0',1024);
	_getUnSpend(adr, server, port, buff, 1024);

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

void _getUnSpend(char* adr,char* server, char* port, char* rBuff, int rBuffSize){


	int size= 57+ strlen(adr);

	//1- prepare Request
	sprintf (rBuff, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
			"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
			"Content-Type: application/json\r\nContent-Length: %d\r\n\r\n"
			"{\"method\":\"scantxoutset\",\"params\":[\"start\",[\"addr(%s)\"]]}\r\n",server,port,size,(char *)adr);  // varibale are diffrent
	printf(rBuff);

	//3- send request and receive
	while(ATreceive_Timeout(rBuff, rBuffSize, (char *)"result\":", server, port, 30000) !=1) HAL_Delay(1000);
	//4- preparing data




}


