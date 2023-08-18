


/*
 * ESP8266_HAL.c
 *
 *  Created on: Apr 14, 2020
 *      Author: Controllers  and edited by mohsen rahmanikivi
 */

#include "stdio.h"
#include "string.h"
#include <_ESP8266_HAL.h>
#include <_backbone.h> 		 //for txDataStruct
#include <_uartRingBufDMA.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdlib.h> 		// for atoi

extern UART_HandleTypeDef huart1;

#define wifi_uart &huart1
#define wifi_pin Wifi_Enable_Pin
#define wifi_gpio Wifi_Enable_GPIO_Port


char buffer[20];


static void uartSend (char *snd)
{
	HAL_UART_Transmit(wifi_uart, (uint8_t *) snd, strlen (snd), 1000);
}

uint8_t ATsend (uint8_t* command){


	uartSend ((char *)command);

	if(isConfirmed(5000) != 1)
		  	{
		  		printf("\nATsend--<error> FAILD no OK after send\nATsend--> sent COMMAND is:\n%s \r",command);
		  		char buffer[128];
		  		getAfter((char *)"ERROR", 128, buffer, 1000 );
		  		printf ("\nATsend--<error> Debug: ERROR\n%S\r",(wchar_t*)buffer);
		  		HAL_Delay(1000);
		  		return 0;
		  	}
			return 1;
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



/************************************************************************************************/

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


uint8_t ATreceive (char* send,char* getafter, char* receive, int receiveSize, uint8_t * server, uint8_t * port){

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

uint8_t ATreceive_Timeout (char* buff,int buffSize, char* getafter, char * server, char * port, int timeout){

		Ringbuf_Reset ();
		uint8_t command[256];
		memset(command,'\0',256);
		/******************************************TCP connection********************************************/

		sprintf ((char *)command, "AT+CIPSTART=\"TCP\",\"%s\",%s,60\r\n", server,port);

		while( ATsend(command) !=1)	{											// TRY AND RETRY TO ESTABLISH THE CONNECTION

			printf("\nATreceive_Timeout--<error> Failed to make connection TRY AGAIN...\r");
			HAL_Delay(10);
		}

		/*********************************************SENDING********************************************/

		memset(command,'\0',256);     								   			 //RESET THE COMMAND
		sprintf ((char *)command, "AT+CIPSEND=%d\r\n", strlen(buff));			//	  AT+CIPSEND=<length>

		uartSend((char *)command);												/*send size*/

		if (waitFor((char*)">",100) != 1){												//WAIT FOR >
			printf("\nATreceive_Timeout--<error> Failed to get \">\" after CIPSEND\r");
			return 0;
		}

		uartSend(buff);														/*send command*/

		if (isConfirmed(100) != 1)	{
			printf("\nATreceive_Timeout--<error> Failed to get \"OK\" after sending data\r");
			return 0;
		}
		/**************************************reset buffer*******************************************************/

		memset(buff,'\0',buffSize);
		/**************************************RECEVING method 2*******************************************************/
		if (waitFor((char*)"+IPD,", timeout)!=1){											//wait for +IPD
			printf("\nATreceive_Timeout--<error> +IPD NOT FOUND IN %d Sec TIMEOUT\r",timeout);
			return 0;
		}
		if (getAfter (getafter, buffSize, buff, 3000 )!=1){						//wait for result
			printf("\nATreceive_Timeout--<error> getafter() NOT FOUND IN 3 Sec TIMEOUT\r");

			return 0;
		}
//		printf(buff);

		/******************************************Close connection********************************************/

			sprintf ((char *)command, "AT+CIPCLOSE\r\n");  // if disable HARDware fault occurs
			uartSend ((char *)command);

		return 1;


			}


//uint8_t _getrawTX(char* srv,char * prt,char * txId, char * hight, txDataStruct* tx){
//
//
//	 /*****INPUT  hight ******************** HASH *********************************/
//	//0- counting digits
//	 int digit=0;
//	 int t=atoi(hight);
//	 do { t /= 10;   ++digit;  } while (t != 0);
//	 //1- content len
//	 int contentLen=40+digit;
//
//	 //2- variable for receiving
//	 int receiveSize=65+1; 									 //it has an extra quotation
//	 char receiveBlockHash[receiveSize];
//	 receiveBlockHash[receiveSize-1]='\0';
//	 //3-prepare the request
//	 char buff[1024];
//	 sprintf (buff, "POST / HTTP/1.1\r\nHost: %s:%s\r\nAuthorization: Basic dXNlcjpwYXNzd29yZA==\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n{\"method\": \"getblockhash\", \"params\": [%d]}\r\n", srv , prt ,contentLen,atoi(hight));
//	 //4-send the request
//	 while(receiveResult (buff, receiveBlockHash, receiveSize, (char *)srv, (char *)prt) !=1)  HAL_Delay(1000);
//	 //5-prepare the result
//	 char blockHash[65];
//	  memset(blockHash, '\0', 65);
//	  for(int i=0; i<64; i++)  blockHash[i]=receiveBlockHash[i+1]; //avalish qutation hast
//
//	 /*  RESULT     blockHash[]   *//******************************************************/
//
//	/********************************************get TX ********************************************/
//	//1- prepare Request
//	memset(buff,'\0',1024);
//	sprintf (buff, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
//	"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
//	"Content-Type: application/json\r\nContent-Length: 185\r\n\r\n"
//	"{\"method\":\"getrawtransaction\",\"params\":[\"%s\", false, \"%s\"]}\r\n", srv, prt, txId, blockHash);
//
//	printf(buff);
//
//
//	//3- send request and receive
//	while(ATreceive_Timeout (buff, 1024 , (char*)"result\":\"" , srv , prt , 10000) !=1)  HAL_Delay(1000);
//
//	//4- prepare the receive data
//	int i=0;
//	while (buff[i]!='"'){ i++;}
//	buff[i]='\0';
//
//	printf("_getrawTX--> This is the raw TX:\n%s\r" , buff);
//
//	return 1;
//
//}

