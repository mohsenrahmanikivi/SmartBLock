/*
 * _exampleGen.cpp
 *
 *  Created on: Aug 11, 2023
 *      Author: mrahm
 */





#include <iostream>

#include <Bitcoin.h>
#include <_exampleGen.h>
#include "fatfs.h"


using namespace std;

void _exampleGen(){

	char* guestMn=(char *)"chef often point exist boring patient image thrive sea joke pet swift";
	char* ownerMn=(char *)"ability interest lock fluid arm search library theory ritual amount hedgehog family";

	printf("\nexampleGen--<info> _exampleGen Start working \r");
	HDPrivateKey guestXprv, ownerXprv;
	guestXprv.fromMnemonic(guestMn, "");
	ownerXprv.fromMnemonic(ownerMn, "");


	/***needs  MX_FATFS_Init(); in the main.c ***************************************/
	// variables for FatFs
	FATFS FatH; 			//Fatfs handle
	FIL FH; 			//File handle
	FRESULT res; 		//Result after operations


	char buf[220]; //each words has a maximum len 8 ch
	memset(buf, '\0',220);


	/*********************************FATFS Mounting*********************************/
	//0- Unmount
	res=f_mount(NULL, "", 0);
	if (res != FR_OK) 	printf("\n_exampleGen--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", res);
	//1=mount now
	res = f_mount(&FatH, "", 1);
	if (res != FR_OK) 	printf("\n_exampleGen--<error><FATFS>  mounting SD memory error, Error Code: (%i)\r", res);
	/*********************************FATFS Mounting*********************************/

	char* fname=(char *)"keygen.txt";

	res=f_open(&FH, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
	if(res!= FR_OK) 	  printf("\n_keyGenerator--<error><FATFS> \"%s\" Create file error, Error Code=%d \r",fname, res);



	//Write data in file
	sprintf(buf,"\nexampleGen--<info> Owner extended Privat key :\n%s\r\n", ownerXprv.toString().c_str() );				f_puts((TCHAR*)buf, &FH);
	printf("%s", buf); memset(buf, '\0',220);

	sprintf(buf,"\nexampleGen--<info> Owner extended Public Key :\n%s\r\n", ownerXprv.xpub().toString().c_str() );		f_puts((TCHAR*)buf, &FH);
	printf("%s", buf); memset(buf, '\0',220);

	sprintf(buf,"\nexampleGen--<info> Owner WIF file :\n%s\r\n", ownerXprv.wif().c_str() ); 							f_puts((TCHAR*)buf, &FH);
	printf("%s", buf); memset(buf, '\0',220); ;

	sprintf(buf,"%s\n", "\nexampleGen--<info> ---------------------------------\r" ); 	 								f_puts((TCHAR*)buf, &FH);
	printf("%s", buf); memset(buf, '\0',220);

	sprintf(buf,"\nexampleGen--<info> Owner extended Privat key :\n%s\r\n", guestXprv.toString().c_str() ); 	 		f_puts((TCHAR*)buf, &FH);
	printf("%s", buf); memset(buf, '\0',220);

	sprintf(buf,"\nexampleGen--<info> Owner extended Public Key :\n%s\r\n",  guestXprv.xpub().toString().c_str()); 		f_puts((TCHAR*)buf, &FH);
	printf("%s", buf); memset(buf, '\0',220);

	sprintf(buf,"\nexampleGen--<info> Owner WIF file :\n%s\r\n", guestXprv.wif().c_str()); 								f_puts((TCHAR*)buf, &FH);
	printf("%s", buf); memset(buf, '\0',220);

	sprintf(buf,"%s\n", "\nexampleGen--<info> _exampleGen Finish working \r"); 	 										f_puts((TCHAR*)buf, &FH);
	printf("%s", buf); memset(buf, '\0',220);

	res=f_close(&FH);
	if(res!= FR_OK) 	  printf("\n_keyGenerator--<error><FATFS> \"%s\" Close file error, Error Code=%d \r",fname, res);

	f_mount(NULL, "", 0);
}
