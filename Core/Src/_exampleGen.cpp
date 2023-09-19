/*
 * _exampleGen.cpp
 *
 *  Created on: Aug 11, 2023
 *      Author: mrahm
 */

#include <Bitcoin.h>
#include <_exampleGen.h>
#include "___merkle.h"




void _exampleGen(){

	char* guestMn=(char *)"chef often point exist boring patient image thrive sea joke pet swift";
	char* ownerMn=(char *)"ability interest lock fluid arm search library theory ritual amount hedgehog family";

	printf("\nexampleGen--<info> _exampleGen is checking the keys.\r");





	/***needs  MX_FATFS_Init(); in the main.c ***************************************/
	// variables for FatFs
	FATFS FatH; 			//Fatfs handle
	FIL FH;
	FILINFO FileInfo; //File handle
	FRESULT res; 		//Result after operations
	FRESULT result;
	HDPrivateKey guestXprv, ownerXprv;

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
	result = f_stat(fname, &FileInfo);
		switch (result) {
		 case FR_OK:

				 printf("\n_exampleGen--<info> \"%s\"is found.\r", fname);


				 break;
		 case FR_NO_FILE:
				 printf("\n_exampleGen--<info> \"%s\"is not found.\r", fname);

					guestXprv.fromMnemonic(guestMn, "");
					ownerXprv.fromMnemonic(ownerMn, "");

					res=f_open(&FH, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
					if(res!= FR_OK) 	  printf("\n_keyGenerator--<error><FATFS> \"%s\" Create file error, Error Code=%d \r",fname, res);


					sprintf(buf,"%s\n", "\n_exampleGen--<info> -----------OWNER KEYS----------------------\r" );	f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220);
					//Write data in file
					sprintf(buf,"\nOwner Xprv    :%s\r", ownerXprv.toString().c_str() );						f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220);

					sprintf(buf,"\nOwner Xpub    :%s\r", ownerXprv.xpub().toString().c_str() );				f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220);

					sprintf(buf,"\nOwner Address :%s\r", ownerXprv.xpub().address().c_str() );						f_puts((TCHAR*)buf, &FH);
						printf("%s", buf); memset(buf, '\0',220);

					sprintf(buf,"\nOwner WIF :%s\r", ownerXprv.wif().c_str() ); 							f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220); ;

					sprintf(buf,"%s\n", "\n_exampleGen--<info> -----------GUEST KEYS----------------------\r" ); 	 					f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220);

					sprintf(buf,"\nGuest Xprv :%s\r", guestXprv.toString().c_str() ); 	 											    	f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220);

					sprintf(buf,"\nGuest Xpub    :%s\r",  guestXprv.xpub().toString().c_str()); 											f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220);

					sprintf(buf,"\nGuest Address :%s\r",  guestXprv.xpub().address().c_str()); 											f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220);

					sprintf(buf,"\nGuest WIF     :%s\r", guestXprv.wif().c_str()); 														f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220);

					sprintf(buf,"%s\n", "\nexampleGen--<info> _exampleGen Finish working \r"); 	 									    	f_puts((TCHAR*)buf, &FH);
					printf("%s", buf); memset(buf, '\0',220);

					res=f_close(&FH);
					if(res!= FR_OK) 	  printf("\n_exampleGen--<error><FATFS> \"%s\" Close file error, Error Code=%d \r",fname, res);





				 break;
		 default:
				 printf("\n_exampleGen--<error> <FATFS> f_stat error file name=%s, Error Code: (%i)\r",fname , result);
				 break;
		}


	f_mount(NULL, "", 0);
}
