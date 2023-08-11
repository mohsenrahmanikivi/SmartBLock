/*
 * _sendTx.c
 *
 *  Created on: May 24, 2022
 *      Author: Administrator
 */
#include "_transmitTX.h"
#include "main.h"
#include <stdio.h>
#include "_ESP8266_HAL.h"
#include <string.h>
#include "stm32f4xx_hal.h"

void _getUnSpend(char* adr,char* server, char* port){

	//0- Define variable
	char buff[1024];
	memset(buff,'\0',1024);

	//1- prepare Request
	sprintf (buff, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
			"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
			"Content-Type: application/json\r\nContent-Length: 91\r\n\r\n"
			"{\"method\":\"scantxoutset\",\"params\":[\"start\",[\"addr(%s)\"]]}\r\n",server,port,(char *)adr);  // varibale are diffrent

	//3- send request and receive
	while(ATreceive_Timeout(buff, 1024, "result\":", server, port, 20000) !=1) HAL_Delay(1000);

	//preparing data

	printf(buff);
}

void _sendTX (char* srv,char * prt,char* rawtx, int size){

	char server[15];
	memset(server,'\0',15);
	strncpy(server, srv, strlen(srv));
	char port[6];
	memset(port,'\0',6);
	strncpy(port, prt, strlen(prt));
	char rawTX[512];
	strncpy(rawTX, rawtx, strlen(rawtx));

	//0- Define variable
	char send[1024];
	memset(send,'\0',1024);
	int contentSize=45+size;
	//1- prepare Request
	sprintf (send, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
			"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
			"Content-Type: application/json\r\nContent-Length: %d\r\n\r\n"
			"{\"method\":\"sendrawtransaction\",\"params\":[\"%s\"]}\r\n",server,port,contentSize,rawtx);  // varibale are diffrent
	printf(send);

	//2- prepare Respond variable
	int receiveSize=250;
	char receive[receiveSize];
//	 	receiveBlockCount[receiveSize-1]='\0';

	//3- send request and receive
	while(receiveResult (send, receive,receiveSize,server,port)!=1)  HAL_Delay(1000);

	//4- prepare the receive data


	printf(receive);



}

void _getTX(char* srv,char * prt,char * txid, char * Blockid){

	char server[15];
	memset(server,'\0',15);
	strncpy(server, srv, strlen(srv));
	char port[5];
	memset(port,'\0',5);
	strncpy(port, prt, strlen(prt));
	char TXhash[64];
	strncpy(TXhash, txid, 64);
	char Blockhash[64];
	strncpy(Blockhash, Blockid, 64);


	//0- Define variable
	char send[1024];
	memset(send,'\0',1024);

	//1- prepare Request


	sprintf (send, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
							"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
							"Content-Type: application/json\r\nContent-Length: 182\r\n\r\n"
							"{\"method\":\"getrawtransaction\",\"params\":[\"%s\", false,\"%s\"]}\r\n",server,port,txid,Blockid);


//	myprintf(send);
	//2- prepare Respond variable
	int receiveSize=250;
	char receive[receiveSize];
//	 	receiveBlockCount[receiveSize-1]='\0';
	//3- send request and receive
	while(receiveResult (send, receive,receiveSize,server,port)!=1)  HAL_Delay(1000);
	//4- prepare the receive data


//	myprintf(receive);



}


void _getTX2(char* srv,char * prt,char * txId, char * blockId){


	//0- Define variable
	char send2[1024];
	memset(send2,'\0',1024);

	//1- prepare Request
	sprintf (send2, "POST / HTTP/1.1\r\nHost: %s:%s\r\n"
	"Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n"
	"Content-Type: application/json\r\nContent-Length: 185\r\n\r\n"
	"{\"method\":\"getrawtransaction\",\"params\":[\"%s\", false, \"%s\"]}\r\n", srv,prt,txId,blockId);

	printf(send2);
	//2- prepare Respond variable
	int recSize=512;
	char recData[recSize];
//	 	receiveBlockCount[receiveSize-1]='\0';
	//3- send request and receive
	while(receive(send2,recData,recSize,srv,prt)!=1)  HAL_Delay(1000);
	//4- prepare the receive data


	printf(recData);



}




