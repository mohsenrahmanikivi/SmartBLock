/*
 * _syncHeaders.c
 *
 *  Created on: May 17, 2022
 *      Author: mrahmanikivi
 */


#include <_ESP8266_HAL.h>
#include <_syncHeaders.h>
#include <iostream>
#include "stdlib.h"
#include "stdio.h"  //sprintf
#include "string.h" //memset
#include "math.h"
#include "stdlib.h"  //atoi

#include "stm32f4xx_hal.h"
#include "fatfs.h"

#include "uartRingBufDMA.h"



using namespace std;


uint8_t _syncHeaders(uint8_t* server, uint8_t* port ,int localHight){

		printf("\nSYNCHRONIZING...");
		/***********************************Variables************************************/

		FATFS FatFs; 	//Fatfs handle
		FIL fil; 		//File handle
		FRESULT fres; //Result after operations
		UINT bytesNum;
		char buff512[512];
		memset(buff512,'\0',512);
		char buff16[16];
		memset(buff16,'\0',16);
		uint8_t flag=0;
	  	/********************************FATFS Mounting**********************************/
		//0- Unmount
		f_mount(NULL, "", 0);
		printf("\nSYNC--> UNMount the SDRAM");
		//1=mount now
		fres = f_mount(&FatFs, "", 1);
		//3=Check mounting
		if (fres != FR_OK) {	printf("f_mount error (%i)\r\n", fres); 	while(1);	}
		printf("\nSYNC--> Mount   the SDRAM");

		/*****INPUT  Established WIFI********************Get Hight **********************/

		//1- prepare Request
		sprintf (buff512, "POST / HTTP/1.1\r\nHost: %s:%s\r\nAuthorization: Basic dXNlcjpwYXNzd29yZA==\r\nContent-Type: application/json\r\nContent-Length: 41\r\n\r\n{\"method\": \"getblockcount\", \"params\": []}\r\n", server,port);


		//3- send request and receive
		while(ATreceive (buff512,(char *)"result\":", buff16, 16, server, port)!=1)  HAL_Delay(1000);

		//4- prepare the receive data

		memset(buff512,'\0',512);
		uint8_t i=0;
		while(buff16[i]!=',')  { buff512[i]=buff16[i];  i++;}
		buff512[i]='\0';

		int netHight= atoi(buff512);
		printf("\nSYNC--> Network Hight=%d",netHight);
		printf("\nSYNC--> Local   Hight=%d",localHight);
		/*  RESULT   int  hight    *//****************************************************/



		/*################################# Loop from first to last########################*/

		int last	= ceil(netHight/100);
		int first	= floor(localHight/100);

		/*START OF FOR
		*	A. START
		*		A.1 isNOTlast
		*		A.2 isLast
		*	A. END
		*	B. START
		*		B.1 isNotLast
		*		B.2 isLast
		*	B. END
		 * */


/*START OF FOR*/	for(first; first < last+1; first++)	{

					/*1- filename.txt**reset buff********/
					memset(buff512,'\0',512);

					memset(buff16,'\0',16);
					sprintf (buff16,"HEADERS/%d.txt",first*100);
					/************************************/

		 /***********************************A. START *****************************************/
/*If isNOTexist*/	if( f_open(&fil, buff16, FA_READ) != FR_OK)	{ 	f_close(&fil);

						if(f_open(&fil, buff16, FA_WRITE | FA_OPEN_APPEND ) != FR_OK) {	printf("\nSYNC--> f_open error in A isNOTexist %s\r\n",buff16); return 0;}

/*A.1 isNOTlast*/		if( ((first+1)*100)< netHight )	{
							for(int j=(first*100); j<(first+1)*100;j++)	{
								headerDownloader(j, buff512,server,port);
								if(f_write(&fil, buff512, strlen(buff512), &bytesNum) != FR_OK) {printf("\nSYNC--> f_write error A.1 isNOTlast %s hight=%d\r\n",buff16,j); return 0;}
								localHight=j;	}
							f_close(&fil);	}
/*A.2 isLast*/			else{

							for(int j=(first*100); j<netHight;j++)	{
								headerDownloader(j, buff512,server,port);
								if(f_write(&fil, buff512, strlen(buff512), &bytesNum) != FR_OK) {printf("\nSYNC--> f_write error A.2 isLast %s hight=%d\r\n",buff16,j); return 0;}
								localHight=j;		}
							f_close(&fil);		}
/*A. END */ 				}
		/************************************B. START *****************************************/
/*B. START */		else {
						//FILE.TXT IS OPEN NOW
/*If isExist*/			int size=f_size(&fil);
						f_close(&fil);




/*B.1 isNotLast */		if( ((first+1)*100)< netHight )	{
	/*check  */					if((size == 22890)||(size == 23000)||(size == 23100)||(size == 23200)||(size == 23300)||(size == 23400)){
									printf("\nSYNC--> SKIP %s",buff16);
/*show the next file  */			localHight=(first+1)*100;
									flag=1;
									continue;}
								else{ f_unlink (buff16);
									printf("\nSYNC--> Corrupted %s",buff16);
									if(f_open(&fil, buff16, FA_WRITE | FA_OPEN_APPEND ) != FR_OK) {	printf("\nSYNC--> f_open error B.1 isNotLast %s\r\n",buff16); return 0;}
									for(int j=(first*100); j<(first+1)*100;j++)	{
										headerDownloader(j, buff512,server,port);
										if(f_write(&fil, buff512, strlen(buff512), &bytesNum) != FR_OK) {printf("\nSYNC--> f_write error B.1 isNotLast %s hight=%d\r\n",buff16,j); return(0);}
										localHight=j;
										flag=1;}
								f_close(&fil);	}
/*B.2 isLast*/			}else{

							if(localHight<netHight){
/*protect doble data*/			if(flag) f_unlink (buff16);
								if(f_open(&fil, buff16, FA_WRITE | FA_OPEN_APPEND ) != FR_OK) {	printf("\nSYNC--> f_open error in B.2 isLast %s\r\n",buff16); return 0;}

/*Download the next till last*/		for(int a=localHight+1; a<netHight+1;a++){
									headerDownloader(a, buff512,server,port);
									if(f_write(&fil, buff512, strlen(buff512), &bytesNum) != FR_OK) {printf("\nSYNC--> f_write error B.2 isLast %s hight=%d\r\n",buff16,a); return 0;}
									localHight=a;	}
								f_close(&fil);	}
								}


/*B. END */					}


/*END OF FOR*/ }

		/***************Create and Write counter.txt **********/
	  	  //0- open or create coounter.txt
	  	  fres = f_open(&fil, "counter.txt", FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
	  	  if(fres != FR_OK) {printf("\nSYNC--> f_open counter.txt error (%i)\r\n", fres); return 0;  }

	  	  //2-Variable to store
	  	  memset(buff16,'\0',15);

	  	  //2-convert int to string
	  	  sprintf(buff16,"%d\r\n",localHight);
	  	  //2-Write
	  	  fres = f_write(&fil, buff16, strlen(buff16), &bytesNum);
	  	  if(fres != FR_OK) {	printf("\nSYNC--> f_write counter.txt  error (%i)\r\n",fres); return 0;   }

	  	  //4-close
	  	  f_close(&fil);
	  	  /*************************************************************************************/

// Unmount
f_mount(NULL, "", 0);
printf("\nSYNC--> UNMount the SDRAM");
printf("\nSYNC--> Synchronized : Local Hight= %d   !!!\n", localHight);
HAL_Delay(10000);
return 1;
}







void headerDownloader(int hight,char* header ,uint8_t* server, uint8_t* port)
{
		 /*****INPUT  hight ******************** HASH *********************************/
		//0- counting digits
		 int digit=0;
		 int t=hight;
		 do { t /= 10;   ++digit;  } while (t != 0);
		 //1- content len
		 int contentLen=40+digit;

		 //2- variable for receiving
		 int receiveSize=65+1; 									 //it has an extra quotation
		 char receiveBlockHash[receiveSize];
		 receiveBlockHash[receiveSize-1]='\0';
		 //3-prepare the request
		 char send[512];
		 sprintf (send, "POST / HTTP/1.1\r\nHost: %s:%s\r\nAuthorization: Basic dXNlcjpwYXNzd29yZA==\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n{\"method\": \"getblockhash\", \"params\": [%d]}\r\n", server,port,contentLen,hight);
		 //4-send the request
		 while(receiveResult (send, receiveBlockHash,receiveSize,(char *)server,(char *)port) !=1)  HAL_Delay(1000);
		 //5-prepare the result
		 char blockHash[65];
		 blockHash[64]='\0';
		 for(int i=0; i<64; i++)  blockHash[i]=receiveBlockHash[i+1]; //avalish qutation hast

		 /*  RESULT     blockHash[]   *//******************************************************/

		/*****INPUT  blockHash ******************  HEADER  ***********************************/

		 //0-variable for receiving
		 receiveSize=170+1; 									 //it has an extra quotation
		 char receiveBlockHeader[receiveSize];
		 receiveBlockHeader[receiveSize-1]='\0';
		 //1-prepare the request
		 sprintf (send, "POST / HTTP/1.1\r\nHost: %s:%s\r\nAuthorization: Basic dXNlcjpwYXNzd29yZA==\r\nContent-Type: application/json\r\nContent-Length: 115\r\n\r\n{\"method\": \"getblockheader\", \"params\": [\"%s\",false]}\r\n", server,port,blockHash);
		 //2-send the request
		 while(receiveResult (send, receiveBlockHeader,receiveSize,(char *)server,(char *)port)!=1)  HAL_Delay(1000);
		 //5-prepare the result
		 char blockHeader[170+1];
		 memset(blockHeader,'\0',170);
		 int k=0;
		 while(receiveBlockHeader[k+1]!='\"'){
		 blockHeader[k]=receiveBlockHeader[k+1];
		 k++;
		 	 }

		 /*  RESULT    blockHeader[]   *//***************************************************/

		 sprintf(header,"%d,%s,%s\r",hight,blockHash,blockHeader);
		 printf("\nSYNC--> %d",hight);


}
