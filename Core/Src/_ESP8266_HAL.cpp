


/*
 * ESP8266_HAL.c
 *
 *  Created on: Apr 14, 2020
 *      Author: Controllers  and edited by mohsen rahmanikivi
 */


#include <_ESP8266_HAL.h>
#include "stm32f4xx_hal.h"
#include "uartRingBufDMA.h"
#include "stdio.h"
#include "string.h"
#include "main.h"
#include <stdlib.h> // for atoi

extern UART_HandleTypeDef huart1;

#define wifi_uart &huart1
#define wifi_pin Wifi_Enable_Pin
#define wifi_gpio Wifi_Enable_GPIO_Port


char buffer[20];


static void uartSend (char *snd)
{
	HAL_UART_Transmit(wifi_uart, (uint8_t *) snd, strlen (snd), 1000);
}


int ESP_reset (void){
		Ringbuf_Init();

	/********* AT **********/
		uartSend((char *)"AT\r\n");
		if(isConfirmed(1000) != 1)
		{
			printf("\nESP_reset--<error> FAILED at AT\r");
			return 0;
		}
		printf("\nESP_reset--<info> AT OK\r");
		HAL_Delay (1000);

	/********* AT+RST *****reset*****/
		uartSend((char *)"AT+RST\r\n");
		if(isConfirmed(5000) != 1)
		{
			printf("\nESP_reset--<error> FAILED at AT+RST\r");
			return 0;
		}
		printf("\nESP_reset--<info> Reset OK\r");
		HAL_Delay (5000);

		return 1;

}

/************************************************************************************************/

int ESP_Init (char *SSID, char *PASSWD)
{
	//variables
	char cmd[80];


	// Initialized Ringbuffer
	Ringbuf_Init();


	printf("\nESP_Init--<info> WIFI module is ready\r");


	/********* AT **********/
	uartSend((char *)"AT\r\n");
	if(isConfirmed(1000) != 1)
	{
		printf("\nESP_Init--<error> FAILED at AT\r");
		return 0;
	}
	printf("\nESP_Init--<info> AT OK\r");
	HAL_Delay (1000);


	/********* AT+CWMODE=1 *****1: Station*****/
	uartSend((char *)"AT+CWMODE=1\r\n");
	if(isConfirmed(2000) != 1)
	{
		printf("\nESP_Init--<error> FAILED at CWMODE\r");
		return 0;
	}
	printf("\nESP_Init--<info> CW MODE 1");
	HAL_Delay (1000);

	/********* AT+CWJAP="SSID","PASSWD" **********/
	printf("\nESP_Init--<info> CONNECTING ssid=\"%s\"\r", SSID);
	sprintf (cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	uartSend(cmd);
	if(waitFor((char *)"GOT IP", 10000) != 1)
	{
		printf("\nESP_Init--<error> FAILED to GOT IP  SSID=\"%s\"\r", SSID);
		return 0;
	}

	printf("\nESP_Init--<info> CONNECTED  ssid=\"%s\"", SSID);

	Ringbuf_Reset();
	HAL_Delay (1000);

	/********* AT+CIFSR ****get IP******/
	uartSend((char *)"AT+CIFSR\r\n");
	if (waitFor((char *)"CIFSR:STAIP,\"", 5000) != 1)
	{
		printf("\nESP_Init--<error> FAILED to get the STATIC IP try 1\r");
		return 0;
	}
	if (copyUpto((char *)"\"",buffer, 3000) != 1)
	{
		printf("\nESP_Init--<error> FAILED to get the STATIC IP try 2\r");
		return 0;
	}
	if(isConfirmed(2000) != 1)
	{
		printf("\nESP_Init--<error> FAILED to get the STATIC IP try 3\r");
		return 0;
	}
	int len = strlen (buffer);
	buffer[len-1] = '\0';
	printf("\nESP_Init--<info> IP ADDRESS: %s\r", buffer);
	HAL_Delay (1000);

	uartSend((char *)"AT+CIPMUX=0\r\n");   //single connection
//	uartSend("AT+CIPMUX=1\r\n");	//multi connection
	if (isConfirmed(2000) != 1)
	{
		printf("\nESP_Init--<error> Failed at CIPMUX\r");
		return 0;
	}
	printf("\nESP_Init--<info> CIPMUX single connection\r");
//	printf("\nESP_Init--<info> CIPMUX multi connection\r");


/*Extera*/	printf("\nESP_Init--<info> Connected to the network ");

	return 1;

}

uint8_t ATsend (uint8_t* command){


	uartSend ((char *)command);

	if(isConfirmed(5000) != 1)
		  	{
		  		printf("\nATsend--<error> FAILD no OK after send\nESP--> sent COMMAND is:%s \r",command);
		  		char buffer[128];
		  		copyUpto ((char *)"ERROR", buffer, 1000 );
		  		printf ("\nATsend--<error> Debug:\n%S\r",(wchar_t*)buffer);
		  		HAL_Delay(1000);
		  		return 0;
		  	}
			return 1;
			}


//we use this also
int receiveResult (char* send, char* receive,int receiveSize,char* server, char* port){
		Ringbuf_Reset ();
		uint8_t command[256];
		memset(command,'\0',256);
		/******************************************TCP connection********************************************/

		sprintf ((char*)command, "AT+CIPSTART=\"TCP\",\"%s\",%s,60\r\n", server,port);

		while( ATsend(command) !=1)	{							// TRY AND RETRY TO ESTABLISH THE CONNECTION

			printf("\nreceiveResult--<error> Failed to make connection TRY AGAIN...\r");
			HAL_Delay(10);
		}

		/*********************************************SENDING********************************************/

		memset(command,'\0',256);     								    		//RESET THE COMMAND
		sprintf ((char*)command, "AT+CIPSEND=%d\r\n", strlen(send));			//	  AT+CIPSEND=<length>

		uartSend((char*)command);												/*send size*/

		if (waitFor((char*)">",100) != 1){												//WAIT FOR >
			printf("\nreceiveResult--<error> Failed to get \">\" after CIPSEND\r");
			return 0;
		}

		uartSend(send);													/*send command*/

		if (isConfirmed(100) != 1)	{
			printf("\nreceiveResult--<error> Failed to get \"OK\" after sending data\r");
			return 0;
		}


		if (waitFor((char*)"+IPD,", 3000)!=1){											//wait for +IPD
			printf("\nreceiveResult--<error> +IPD not found in the received data IN_3_Sec_TIMEOUT\r");
			return 0;
		}
		if (getAfter ((char*)"result\":", receiveSize, receive, 3000 )!=1){						//wait for result
			printf("\nreceiveResult--<error> RESULT not found in the received data 3_Sec_TIMEOUT\r");

			return 0;
		}
//		printf(receive);

		/******************************************Close connection********************************************/

			sprintf ((char*)command, "AT+CIPCLOSE\r\n");  // if disable HARDware fault occurs
			uartSend ((char*)command);

		return 1;


			}

int _getHeader (int hight,char* server, char* port){

//hash			/****input hight****************Download hash of block*******************************************/

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

//			 printf(send);
			/******************************************TCP connection********************************************/
			char command[256];
			memset(command,'\0',256);
			sprintf (command, "AT+CIPSTART=\"TCP\",\"%s\",%s,60\r\n", server,port);

			while( test(command) !=1)	{							// TRY AND RETRY TO ESTABLISH THE CONNECTION

				printf("\n_getHeader--<error> Failed to make connection try again...\r\n");
				HAL_Delay(10);
			}

			/*********************************************SENDING********************************************/

			memset(command,'\0',256);     								    //RESET THE COMMAND
			sprintf (command, "AT+CIPSEND=%d\r\n", strlen(send));			//	  AT+CIPSEND=<length>



			uartSend(command);												/*send size*/
			if (waitFor((char*)">",100) != 1){
				printf("\n_getHeader--<error>Failed to get \">\" after CIPSEND\r\n");
				return 0;
			}

			uartSend(send);													/*send command*/
			if (isConfirmed(100) != 1)	{	printf("\n_getHeader--<error>Failed to get \"OK\" after sending data\r"); 	return 0;}

//			printf(send);
			/**************************************RECEVING method 2*******************************************************/
			if (waitFor((char*)"+IPD,", 3000)!=1){
				printf("\n_getHeader--<error> +IPD_NOT_FOUND_IN_3_Sec_TIMEOUT\r");
				return 0;
			}
			if (getAfter ((char*)"result\":", receiveSize, receiveBlockHash, 5000 )!=1){						//wait for result
				printf("\n_getHeader--<error> receiveBlockHash_NOT_FOUND_IN_3_Sec_TIMEOUT\r");

				return 0;
			}
//			printf(receiveBlockHash);
			/**************************************prepare the result*******************************************************/
					 char blockHash[65];
					 blockHash[64]='\0';
					 for(int i=0; i<64; i++)  blockHash[i]=receiveBlockHash[i+1]; //avalish qutation hast


//header	/**************************************Download the header*******************************************************/

			 receiveSize=170+1; 									 //it has an extra quotation
			 char receiveBlockHeader[receiveSize];
			 receiveBlockHeader[receiveSize-1]='\0';
			 //1-prepare the request
			 sprintf (send, "POST / HTTP/1.1\r\nHost: %s:%s\r\nAuthorization: Basic dXNlcjpwYXNzd29yZA==\r\nContent-Type: application/json\r\nContent-Length: 115\r\n\r\n{\"method\": \"getblockheader\", \"params\": [\"%s\",false]}\r\n", server,port,blockHash);

			 /******************************************TCP connection********************************************/

			memset(command,'\0',256);
			sprintf (command, "AT+CIPSTART=\"TCP\",\"%s\",%s,60\r\n", server,port);

			while( test(command) !=1)	{							// TRY AND RETRY TO ESTABLISH THE CONNECTION

				printf("\n_getHeader--<error> Failed to make connection TRY AGAIN...\r");
				HAL_Delay(10);
			}

			/*********************************************SENDING********************************************/

			memset(command,'\0',256);     								    //RESET THE COMMAND
			sprintf (command, "AT+CIPSEND=%d\r\n", strlen(send));			//	  AT+CIPSEND=<length>



			uartSend(command);												/*send size*/
			if (waitFor((char*)">",100) != 1){
				printf("\n_getHeader--<error> Failed to get \">\" after CIPSEND\r");
				return 0;
			}

			uartSend(send);													/*send command*/
			if (isConfirmed(100) != 1)	{	printf("\n_getHeader--<error> Failed to get \"OK\" after sending data\r"); 	return 0;}


			/**************************************RECEVING method 2*******************************************************/
			if (waitFor((char*)"+IPD,", 3000)!=1){
				printf("\n_getHeader--<error> +IPD_NOT_FOUND_IN_3_Sec_TIMEOUT\r");
				return 0;
			}
			if (getAfter ((char*)"result\":", receiveSize, receiveBlockHeader, 5000 )!=1){						//wait for result
				printf("\n_getHeader--<error> receiveBlockHeader_NOT_FOUND_IN_3_Sec_TIMEOUT\r");

				return 0;
			}
//			printf(receiveBlockHeader);
			/**************************************prepare the result*******************************************************/
			 //5-prepare the result
			 char blockHeader[170+1];
			 memset(blockHeader,'\0',170);
			 int k=0;
			 while(receiveBlockHeader[k+1]!='\"'){
			 blockHeader[k]=receiveBlockHeader[k+1];
			 k++;
				 }

			/******************************************Close connection********************************************/

//				sprintf (command, "AT+CIPCLOSE\r\n");  // if disable HARDware fault occurs
//				uartSend (command);

			/******************************************Printing********************************************/
//				char hightchar[15];
//				itoa(hight,hightchar,10);
//				sprintf (command, "%s,%s,%s\r", hightchar,blockHash,blockHeader);
//				printf(command);


			return 1;
			}

int receiveResultafter (char* send, char* receive,int receiveSize, char* getafter,char* server, char* port){
		Ringbuf_Reset ();
		char command[256];
		memset(command,'\0',256);
		/******************************************TCP connection********************************************/

		sprintf (command, "AT+CIPSTART=\"TCP\",\"%s\",%s,60\r\n", server,port);

		do{
			HAL_Delay(10);
			uartSend (command);

		}while(isConfirmed(5000) != 1);



		/*********************************************SENDING********************************************/

		memset(command,'\0',256);     								    //RESET THE COMMAND
		sprintf (command, "AT+CIPSEND=%d\r\n", strlen(send));			//	  AT+CIPSEND=<length>

		uartSend(command);												/*send size*/

		if (waitFor((char*)">",100) != 1){									//WAIT FOR >
			printf("\nreceiveResultafter--<error> Failed to get \">\" after CIPSEND\r");
			return 0;
		}

		uartSend(send);													/*send command*/

		if (isConfirmed(100) != 1)	{
			printf("\nreceiveResultafter--<error> Failed to get \"OK\" after sending data\r");
			return 0;
		}


		if (waitFor((char*)"+IPD,", 15000)!=1){											//wait for +IPD
			printf("\nreceiveResultafter--<error> +IPD_NOT_FOUND_IN_5_Sec_TIMEOUT\r");
			return 0;
		}
		if (getAfter ((char*)"\"hex\":\"", receiveSize, receive, 10000 )!=1){						//wait for result
			printf("\nreceiveResultafter--<error> RESULT_NOT_FOUND_IN_3_Sec_TIMEOUT\r");

			return 0;
		}
//		printf(receive);

		/******************************************Close connection********************************************/

			sprintf (command, "AT+CIPCLOSE\r\n");  // if disable HARDware fault occurs
			uartSend (command);

		return 1;


			}


//int receiveBetween(char* send, char* receive,int receiveSize, char* getafter,char* server, char* port){
//		Ringbuf_Reset ();
//		char command[256];
//		memset(command,'\0',256);
//		/******************************************TCP connection********************************************/
//
//		sprintf (command, "AT+CIPSTART=\"TCP\",\"%s\",%s,60\r\n", server,port);
//
//		do{
//			HAL_Delay(10);
//			uartSend (command);
//
//		}while(isConfirmed(5000) != 1);
//
//
//
//		/*********************************************SENDING********************************************/
//
//		memset(command,'\0',256);     								    //RESET THE COMMAND
//		sprintf (command, "AT+CIPSEND=%d\r\n", strlen(send));			//	  AT+CIPSEND=<length>
//
//		uartSend(command);												/*send size*/
//
//		if (waitFor(">",100) != 1){									//WAIT FOR >
//			printf("\r\nFailed to get \">\" after CIPSEND\r\n");
//			return 0;
//		}
//
//		uartSend(send);													/*send command*/
//
//		if (isConfirmed(100) != 1)	{
//			printf("\r\nFailed to get \"OK\" after sending data\r\n");
//			return 0;
//		}
//
//		/*********************************************RECEIVING********************************************/
//
//
//		if (waitFor("+IPD,", 15000)!=1){											//wait for +IPD
//			printf("\r\n_+IPD_NOT_FOUND_IN_5_Sec_TIMEOUT\r\n");
//			return 0;
//		}
//
//
//		if (getDataFromBuffer ("\"hex\":\"",'\"', MainBuf,receive)!=1){						//wait for result
//			printf("\r\n_RESULT_NOT_FOUND_with_getDataFromBuffer\r\n");
//
//			return 0;
//		}
////		printf(receive);
//
//		/******************************************Close connection********************************************/
//
//			sprintf (command, "AT+CIPCLOSE\r\n");  // if disable HARDware fault occurs
//			uartSend (command);
//
//		return 1;
//
//
//			}

// in this we use getafer2
int receive (char* send, char* receive,int receiveSize,char* server, char* port){
		Ringbuf_Reset ();
		char command[256];
		memset(command,'\0',256);
		/******************************************TCP connection********************************************/

		sprintf (command, "AT+CIPSTART=\"TCP\",\"%s\",%s,60\r\n", server,port);

		while( test(command) !=1)	{							// TRY AND RETRY TO ESTABLISH THE CONNECTION

			printf("\nreceive--<error> Failed to make connection TRY AGAIN...\r");
			HAL_Delay(10);
		}

		/*********************************************SENDING********************************************/

		memset(command,'\0',256);     								    //RESET THE COMMAND
		sprintf (command, "AT+CIPSEND=%d\r\n", strlen(send));			//	  AT+CIPSEND=<length>

		uartSend(command);												/*send size*/

		if (waitFor((char*)">",100) != 1){									//WAIT FOR >
			printf("\nreceive--<error> Failed to get \">\" after CIPSEND\r");
			return 0;
		}

		uartSend(send);													/*send command*/

		if (isConfirmed(100) != 1)	{
			printf("\nreceive--<error> Failed to get \"OK\" after sending data\r");
			return 0;
		}


//		/**************************************RECEVING method 2*******************************************************/
		if (waitFor((char*)"+IPD,", 3000)!=1){											//wait for +IPD
			printf("\nreceive--<error> +IPD_NOT_FOUND_IN_3_Sec_TIMEOUT\r");
			return 0;
		}
		if (getAfter ((char*)"result\":\"", receiveSize, receive, 3000 )!=1){						//wait for result
			printf("\nreceive--<error> RESULT_NOT_FOUND_IN_3_Sec_TIMEOUT\r");

			return 0;
		}
//		printf(receive);

		/******************************************Close connection********************************************/

			sprintf (command, "AT+CIPCLOSE\r\n");  // if disable HARDware fault occurs
			uartSend (command);

		return 1;


			}

///////////////used
uint8_t ATreceive (char* send,char* getafter, char* receive, uint8_t receiveSize, uint8_t * server, uint8_t * port){

		Ringbuf_Reset ();
		uint8_t command[256];
		memset(command,'\0',256);
		/******************************************TCP connection********************************************/

		sprintf ((char *)command, "AT+CIPSTART=\"TCP\",\"%s\",%s,60\r\n", server,port);

		while( ATsend(command) !=1)	{											// TRY AND RETRY TO ESTABLISH THE CONNECTION

			printf("\nATreceive--<error>Failed to make connection TRY AGAIN...\r");
			HAL_Delay(10);
		}

		/*********************************************SENDING********************************************/

		memset(command,'\0',256);     								   			 //RESET THE COMMAND
		sprintf ((char *)command, "AT+CIPSEND=%d\r\n", strlen(send));			//	  AT+CIPSEND=<length>

		uartSend((char *)command);												/*send size*/

		if (waitFor((char*)">",1000) != 1){												//WAIT FOR >
			printf("\nATreceive--<error> Failed to get \">\" after CIPSEND\r");
			return 0;
		}

		uartSend(send);														/*send command*/

		if (isConfirmed(1000) != 1)	{
			printf("\nATreceive--<error> Failed to get \"OK\" after sending data\r");
			return 0;
		}

		/**************************************RECEVING method 2*******************************************************/
		if (waitFor((char*)"+IPD,", 1000)!=1){											//wait for +IPD
			printf("\nATreceive--<error> +IPD NOT FOUND IN 3 Sec TIMEOUT\r");
			return 0;
		}
		if (getAfter (getafter, receiveSize, receive, 1000 )!=1){						//wait for result
			printf("\nATreceive--<error> RESULT_NOT_FOUND_IN_3_Sec_TIMEOUT\r");

			return 0;
		}
//		printf(receive);

		/******************************************Close connection********************************************/

			sprintf ((char *)command, "AT+CIPCLOSE\r\n");  // if disable HARDware fault occurs
			uartSend ((char *)command);

		return 1;


			}

//uint8_t ATreceive_Timeout (char* buff,int buffSize, char* getafter, char * server, char * port, int timeout){
//
//		Ringbuf_Reset ();
//		uint8_t command[256];
//		memset(command,'\0',256);
//		/******************************************TCP connection********************************************/
//
//		sprintf ((char *)command, "AT+CIPSTART=\"TCP\",\"%s\",%s,60\r\n", server,port);
//
//		while( ATsend(command) !=1)	{											// TRY AND RETRY TO ESTABLISH THE CONNECTION
//
//			printf("\nATreceive_Timeout--<error> Failed to make connection TRY AGAIN...\r");
//			HAL_Delay(10);
//		}
//
//		/*********************************************SENDING********************************************/
//
//		memset(command,'\0',256);     								   			 //RESET THE COMMAND
//		sprintf ((char *)command, "AT+CIPSEND=%d\r\n", strlen(buff));			//	  AT+CIPSEND=<length>
//
//		uartSend((char *)command);												/*send size*/
//
//		if (waitFor((char*)">",100) != 1){												//WAIT FOR >
//			printf("\nATreceive_Timeout--<error> Failed to get \">\" after CIPSEND\r");
//			return 0;
//		}
//
//		uartSend(buff);														/*send command*/
//
//		if (isConfirmed(100) != 1)	{
//			printf("\nATreceive_Timeout--<error> Failed to get \"OK\" after sending data\r");
//			return 0;
//		}
//		/**************************************reset buffer*******************************************************/
//
//		memset(buff,'\0',buffSize);
//		/**************************************RECEVING method 2*******************************************************/
//		if (waitFor((char*)"+IPD,", timeout)!=1){											//wait for +IPD
//			printf("\nATreceive_Timeout--<error> +IPD NOT FOUND IN %d Sec TIMEOUT\r",timeout);
//			return 0;
//		}
//		if (getAfter (getafter, buffSize, buff, 3000 )!=1){						//wait for result
//			printf("\nATreceive_Timeout--<error> getafter() NOT FOUND IN 3 Sec TIMEOUT\r");
//
//			return 0;
//		}
////		printf(buff);
//
//		/******************************************Close connection********************************************/
//
//			sprintf ((char *)command, "AT+CIPCLOSE\r\n");  // if disable HARDware fault occurs
//			uartSend ((char *)command);
//
//		return 1;
//
//
//			}




