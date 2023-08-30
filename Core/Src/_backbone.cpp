/*
 * _backbone.cpp
 *
 *  Created on: Sep 2, 2022
 *      Author: Administrator
 */

#include <_backbone.h>
#include <Bitcoin.h>

TCHAR* defaultMnemonic=(TCHAR*)"extra joke genuine garbage boring hold system right all voice retreat treat";
/*
 * in case of Mnemonic NOT found
 * the app uses Default Mnemonic
 */

uint8_t _mKeyGenerator(lockDataStruct* keys ){

		/***needs  MX_FATFS_Init(); in the main.c ***************************************/
		// variables for FatFs
		FATFS FatHandIn; 			//Fatfs handle
		FIL FileHandIn; 			//File handle
		FRESULT result; 		//Result after operations
		FRESULT resultIn;
		FILINFO FileInfo;
		char mnemonic[220]; //each words has a maximum len 8 ch
		memset(mnemonic, '\0',220);


		/*********************************FATFS Mounting*********************************/
		//0- Unmount
		result=f_mount(NULL, "", 0);
		if (result != FR_OK) {	printf("\n_keyGenerator--<error> <FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
		//1=mount now
		result = f_mount(&FatHandIn, "", 1);
		if (result != FR_OK) {	printf("\n_keyGenerator--<error> <FATFS>  Mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}

		/*********************check the  "mnemonic.txt"**************************/
		/*if mnemonic.txt exist
		 *if mnemonic.txt dosenot exist 	ERROR
		 */
		const char *fname="MNEMONIC.txt";
		result = f_stat(fname, &FileInfo);

			switch (result) {
			 case FR_OK:
				 resultIn=f_open(&FileHandIn, fname, FA_READ);
				 if(resultIn != FR_OK) 	 { printf("\n_keyGenerator--<error> <FATFS> \"%s\" open error, Error Code=%d \r",fname, resultIn);  return 0;}

				 printf("\n_keyGenerator--<info> \"%s\" is found.\r", fname);
				 f_gets((TCHAR*)mnemonic, 220, &FileHandIn);

				 f_close(&FileHandIn);
			 	 break;
			 case FR_NO_FILE:
		         printf("\n_keyGenerator--<info> \"%s\" is not found.\r", fname);
		         resultIn= f_open(&FileHandIn, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
		         if(resultIn != FR_OK) 	 { printf("\n_keyGenerator--<error> Create file error, file name:\"%s\", Error Code=%d \r",fname, resultIn); return 0;}
		         strcpy(mnemonic,defaultMnemonic);
		         f_puts((TCHAR*)mnemonic, &FileHandIn);
		         f_close(&FileHandIn);
		         printf("\n_keyGenerator--<info>\"%s\" file is created.\r", fname);
		         printf("\n_keyGenerator--<info> Using Default Mnemonic:%s \r", mnemonic);
		         break;

			 default:
		         printf("\n_keyGenerator--<error><FATFS> f_stat error, Error Code: (%i)\r", result);
		         return 0;
			}

		keys->lockXprv.fromMnemonic(mnemonic, "");


		//we use mnemonic variable as a temp buffer
		memset(mnemonic, '\0',220);
		fname="XPRV.txt";

		resultIn=f_open(&FileHandIn, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
		if(resultIn!= FR_OK) 	 { printf("\n_keyGenerator--<error><FATFS> \"%s\" Create file error, Error Code=%d \r",fname, resultIn);  return 0;}
		sprintf(mnemonic,"%s\n", keys->lockXprv.toString().c_str());
		f_puts((TCHAR*)mnemonic, &FileHandIn);
		f_close(&FileHandIn);


		 //compare stored data with read data
		if( !(keys->lockXprv ) ) printf("\nREADDATA--<error> Lock's Xprv in invalid");
		char strtmp[140];
		sprintf( strtmp, "%s\n", keys->lockXprv.toString().c_str() );
		 if ( strcmp(strtmp ,  (TCHAR*)mnemonic ) == 0 ) keys->lockXprvIsFound=true;
		 else {
			 printf("\nREADDATA--<error> Lock's Xprv in the %s file error in saving.\r", fname);
			 printf("\nREADDATA--<error> Read data/Saved data  :\r%s\r%s\r ", (TCHAR*)mnemonic, strtmp);

			 keys->lockXprvIsFound=false;
		 }

		printf("\n_keyGenerator--<info> Generated Xprv:%s \r", mnemonic);

		result=f_mount(NULL, "", 0);
		if (result != FR_OK) {	printf("\n_keyGenerator--<error><FATFS> Un-mounting error, Error Code: (%i)\r", result); 	return 0; 	}
		return 1;
}



uint8_t _readData(int* cnt,lockDataStruct* keys, uint8_t* index,uint8_t* ssid,uint8_t* password,char* GlobalpreHASH, int* verifiedHight, txinDataStruct* TXIN){
	/* return 0 error
	 * return 1 complete function
	 */
	/***needs  MX_FATFS_Init(); in the main.c ***************************************/
	// variables for FatFs
	FATFS FatHand; 			//Fatfs handle
	FIL FileHand; 			//File handle
	FRESULT result; 		//Result after operations
	FRESULT result2; 		//Result after operations
	FILINFO FileInfo;
	uint8_t buff[140];
	char strtmp[128];
	memset(strtmp, '\0',220);
	memset(buff, '\0',220);
	HDPublicKey ownerXpub;

	int counter;
	int lastVerified;

	const char *fname="COUNTER.txt";


	printf("\n_readData--<info> Start reading the stored data\r");
	/*********************************FATFS Mounting*********************************/
	//0- Unmount
	result=f_mount(NULL, "", 0);
	if (result != FR_OK) {	printf("\n_readData--<error> <FATFS> Un-mounting SD memory error: (%i)\r\n", result); 	return 0; 	}
	//1=mount now
	result = f_mount(&FatHand, "", 1);
	if (result != FR_OK) {	printf("\n_readData--<error> <FATFS> mounting SD memory error: (%i)\r\n", result); 	return 0; 	}

	/*********************check the  "Counter.txt"**************************/
	/*if counter.txt exist 				then counter= data
	 *if counter.txt dose not exist 	then create	 counter.txt and put 0 in it and counter= 0
	 */
	result = f_stat(fname, &FileInfo);
	switch (result) {
	 case FR_OK:

			 printf("\n_readData--<info> \"%s\"is found.\r", fname);
			 if(FileInfo.fsize == 0  ) { counter=0;
			 }else{
				result2=f_open(&FileHand, fname, FA_READ);
				if(result2 != FR_OK ){ printf("\n_readData--<error> <FATFS> open file error file name=%s, Error Code=%d \r", fname, result2); return 0; }
				f_gets((TCHAR*)buff, 16, &FileHand);
				f_close(&FileHand);
				counter =(atoi((const char *)buff));
				//prevent to get negetive
				if(counter<0) counter=0;
			 }

			 printf("\n_readData--<info> Counter: %d \r", counter);

			 break;
	 case FR_NO_FILE:
			 printf("\n_readData--<info> \"%s\"is not found.\r", fname);
			 result2=f_open(&FileHand, fname , FA_CREATE_ALWAYS);
			 if(result2 != FR_OK){ printf("\n_readData--<error> <FATFS> create file error file name=%s, Error Code=%d \r", fname, result2); return 0;}
			 f_putc ('0', &FileHand);
			 f_close(&FileHand);
			 counter=0;
			// printf("\n_readData--<info> Counter: %d \r", counter);

			 break;
	 default:
			 printf("\n_readData--<error> <FATFS> f_stat error file name=%s, Error Code: (%i)\r",fname , result);
			 return 0;
	}

	/*********************check the  "XPRV.txt"**************************/
	/*if xprv.txt exist 			then fill lockDataStruct
	 *if xprv.txt dose not exist 	just create it
	 */
	int len=0;
	fname="XPRV.txt";
	memset(buff, '\0',140);
	result = f_stat(fname, &FileInfo);
	switch (result) {
	 case FR_OK:
			printf("\n_readData--<info> \"%s\"is found.\r", fname);
			result2=f_open(&FileHand, fname, FA_READ);
			if(result2 != FR_OK){ printf("\n_readData--<error> <FATFS> open file error file name=%s, Error Code=%d \r", fname, result2 ); return 0; }
			f_gets((TCHAR*)buff, 140, &FileHand);
			len=strlen((char*)buff);
			if (len !=112 && len !=111){
				printf("\n_readData--<info> XPRV in the file is corrupted/missed. the lengh is %d\r",len);
				printf("\n_readData--<info> mKeyGenerator starts generate XPRV\r");
				f_close(&FileHand);
				while(_mKeyGenerator(keys)!=1){};
				// need to mount again after runnig _mKeyGenerator() as this function unmount the SD memory
				result2 = f_mount(&FatHand, "", 1);
				if (result2 != FR_OK) {	printf("\n_readData--<error> <FATFS> mounting SD memory error: (%i)\r\n", result2); 	return 0; 	}
			   	break;
			}

			keys->lockXprv.fromString((TCHAR*)buff);

			 f_close(&FileHand);
			// printf("\nREADDATA--<info> XPRV in File is:%s\r", buff);
			 //compare stored data with read data
			if( !(keys->lockXprv ) ) printf("\n_readData--<error> Lock's Xprv in invalid");
			 sprintf( strtmp, "%s\n", keys->lockXprv.toString().c_str() );
			 if ( strcmp(strtmp ,  (TCHAR*)buff ) == 0 ) keys->lockXprvIsFound=true;
			 else {
				 printf("\n_readData--<error> Lock's Xprv in the %s file error in saving.\r", fname);
				 printf("\n_readData--<error> Read data/Saved data  :\r%s\r%s\r ", (TCHAR*)buff, strtmp);

				 keys->lockXprvIsFound=false;
			 }

			 break;
	 case FR_NO_FILE:
			 printf("\n_readData--<info> \"%s\" is not found.\r", fname);
			 printf("\n_readData--<info> mKeyGenerator starts generate XPRV\r");
			 // need to mount again after runnig _mKeyGenerator() as this function unmount the SD memory
			 while(_mKeyGenerator(keys)!=1){};
			 result2 = f_mount(&FatHand, "", 1);
			 if (result2 != FR_OK) {	printf("\n_readData--<error> <FATFS> mounting SD memory error: (%i)\r\n", result2); 	return 0; 	}
			 break;


	 default:
			 printf("\n_readData--<error> <FATFS> f_stat error file name=%s, Error Code: (%i)\r",fname , result);
			 return 0;
	}

	/*********************check the  "OXPUB.txt"**************************/
	/*if OXPUB.txt exist 			then fill lockDataStruct
	 *if OXPUB.txt dose not exist 	just create it
	 */
	fname="OXPUB.txt";
	memset(buff, '\0',140);
	result = f_stat(fname, &FileInfo);
	switch (result) {
	 case FR_OK:
			printf("\n_readData--<info> \"%s\"is found.\r", fname);
			result2= f_open(&FileHand, fname, FA_READ);
			if(result2!= FR_OK){ printf("\n_readData --<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result2); }
			f_gets((TCHAR*)buff, 140, &FileHand);
			len=strlen((char*)buff);
			if ( len !=112 && len !=111 ){

				printf("\n_readData--<info> Owner's Xpub in the file %s is corrupted/missed. lengh is=%d \r", fname, len);
				printf("\n_readData--<info> Please check the Owner's Xpub in the %s file in the SD memory and reset the system.\r", fname);
				f_close(&FileHand);
				break;
			}
			f_close(&FileHand);

			keys->ownerXpub.fromString((TCHAR*)buff);
			if( !(keys->ownerXpub ) ) printf("\n_readData--<error> Owner's Xpub in invalid");

			//compare stored data with read data
			 sprintf( strtmp, "%s\n", keys->ownerXpub.toString().c_str() );
			 if (strcmp(strtmp , (const char*) buff ) ==0) keys->ownerXpubIsFound=true;
			 else {
				 printf("\n_readData--<error> Owner's Xpub in the %s file error in saving.\r", fname);
				 printf("\n_readData--<error> Read data/Saved data  :\r%s\r%s\r ", buff, strtmp);

				 keys->ownerXpubIsFound=false;
			 }

			 break;

	 case FR_NO_FILE:
			 printf("\n_readData--<info> \"%s\" is not found.\r", fname);

			 printf("\n_readData--<info> Please create/put the Owner's Xpub in the %s file and save it in the SD memory and reset the system.\r", fname);


			 break;



	 default:
			 printf("\n_readData--<error> <FATFS> f_stat error file name=%s, Error Code: (%i)\r",fname , result);
			 break;
	}

	/*********************check the  "GXPUB.txt"**************************/
	/*if GXPUB.txt exist 			then fill lockDataStruct
	 *if GXPUB.txt dose not exist 	just create it
	 */
	fname="GXPUB.txt";
	memset(buff, '\0',140);
	result = f_stat(fname, &FileInfo);
	switch (result) {
	 case FR_OK:
			printf("\n_readData--<info> \"%s\"is found.\r", fname);
			result2= f_open(&FileHand, fname, FA_READ);
			if(result2 != FR_OK){ printf("\n_readData--<error> <FATFS> open file error file name=%s, Error Code=%d \r\n", fname, result2);  }
			f_gets((TCHAR*)buff, 140, &FileHand);
			len=strlen((char*)buff);
			if (len !=112 && len !=111 ){
				printf("\n_readData--<info> Guest's Xpub in the file %s is corrupted/missed. lengh is=%d\r", fname, len);
				printf("\n_readData--<info> Please check the Guest's Xpub in the %s file in the SD memory and reset the system.\r",fname);
				f_close(&FileHand);
				 break;
			}
			keys->guestXpub.fromString((TCHAR*)buff);
			//compare stored data with read data
			if( !(keys->guestXpub ) ) printf("\n_readData--<error> Guest's Xpub in invalid");
			sprintf( strtmp, "%s\n", keys->guestXpub.toString().c_str() );
			 if (strcmp(strtmp , (const char*) buff )==0) keys->guestXpubIsFound=true;
			 else {

				 printf("\n_readData--<error> Owner's Xpub in the %s file error in saving.\r", fname);
				 printf("\n_readData--<error> Read data/Saved data  :\r%s\r%s\r ", buff, strtmp);
				 keys->guestXpubIsFound=false;
			 }

			 f_close(&FileHand);

			 break;
	 case FR_NO_FILE:
			 printf("\n_readData--<info>\"%s\" is not found.\r", fname);

			 printf("\n_readData--<info> Please create/put the Guest's Xpub in the %s file and save it in the SD memory and reset the system.\r", fname);


			 break;



	 default:
			 printf("\n_readData--<error><FATFS> f_stat error file name=%s, Error Code: (%i)\r",fname , result);
			 break;
	}


		/*********************check the  "WIFI.txt"**************************/
		fname="WIFI.txt";
		result = f_stat(fname, &FileInfo);
		switch (result) {
			 case FR_OK:
				 printf("\n_readData--<info>\"%s\"is found. \r", fname);
				 result2=f_open(&FileHand, fname, FA_READ);
				 if(result2 != FR_OK){ printf("\n_readData--<error> <FATFS> open file error file name=%s, Error Code=%d \r", fname, result2); return 0;}
				 if(f_gets((TCHAR*)buff, 64, &FileHand)!=0){
					uint8_t i=0;
					while(buff[i]!=':') {
						ssid[i]=buff[i];
						i++;
					}
					ssid[i]='\0';
					i++; 											 //jump from ','
					uint8_t j=0;
					while(buff[i]!='\0'){
						password[j]=buff[i];
						i++;
						j++;
					}
					password[j]='\0';

					//printf("\n_readData--<info> SSID is %s:%s\r", ssid, password);
				}else {
					printf("\n_readData--<info>\"%s\" is empty or not readable.(Acceptable data style SSID:Password)\r", fname);
					return 0;
				}
				 f_close(&FileHand);

			 	 break;
		 case FR_NO_FILE:
		         printf("\n_readData--<info>\"%s\" is not found. Create a WIFI.txt file.(Acceptable data style SSID:Password)\r", fname);
		         return 0;

		         break;
		 default:
		         printf("\n_readData--<error><FATFS> f_stat error file name=%s, Error Code: (%i)\r",fname , result);
		         return 0;
		}
		/********************************************************************/
		/*********************check the  "VERIFIED.txt"**************************/
		fname="VERIFIED.txt";
		result = f_stat(fname, &FileInfo);
		switch (result) {
			case FR_OK:
				printf("\n_readData--<info>\"%s\"is found. \r", fname);
				result2 =f_open(&FileHand, fname, FA_READ);
				if(result2 != FR_OK){ printf("\n_readData--<error> <FATFS> open file error file name=%s, Error Code=%d \r", fname, result2);  return 0;}

				char TMP[16];
				if(f_gets((TCHAR*)buff, 140, &FileHand)!=0){
					uint8_t i=0;
					while(buff[i]!=',') {
						TMP[i]=buff[i];
						i++;
					}
					TMP[i]='\0';
					lastVerified=(atoi((const char *)TMP));
					i++; 											 //jump from ','
					uint8_t j=0;
					while(buff[i]!='\0'){
						GlobalpreHASH[j]=buff[i];
						i++;
						j++;
					}
					GlobalpreHASH[j]='\0';
				}else {
					printf("\n_readData--<info> \"%s\" is empty or not readable.\r ", fname);
					lastVerified=0;
				}
				 f_close(&FileHand);
				 break;
		 case FR_NO_FILE:
				 printf("\n_readData--<info> \"%s\" is not found.\r", fname);
				 lastVerified=0;

				 break;
		 default:
				 printf("\n_readData--<error><FATFS> f_stat error file name=%s, Error Code: (%i)\r",fname , result);
				 return 0;
		}
	/********************************************************************/
	/*********************check the  "TXIN.txt"**************************/
		fname="TXIN.txt";
		result = f_stat(fname, &FileInfo);
		memset(buff, '\0',140);
		switch (result) {
			 case FR_OK:
				 printf("\n_readData--<info>\"%s\"is found. \r", fname);
				 result2=f_open(&FileHand, fname, FA_READ);
				 if(result2 != FR_OK){ printf("\n_readData--<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result2); return 0;}
				 if(f_gets((TCHAR*)buff, 140, &FileHand)!=0){
					 uint8_t i=0;
					while(buff[i]!=',') { TXIN->id[i]=buff[i];
						i++;
						if(i>66){	printf("\n_readData--<info>\"%s\"is found. but the data structure is wrong (txid,index,fund)\r", fname);
							return 0;
						}
					}
					TXIN->id[i]='\0';
					i++; 				//jump from ','
					char tmp[16];
					memset(tmp, '\0', 16);
					uint8_t j=0;
					while(buff[i]!=',') {
						tmp[j]=buff[i];
						i++; 	j++;
						if(j>4){ printf("\n_readData--<info> \"%s\"is found. but the data structure is wrong (txid,index,fund)\r", fname);
						return 0;
						}
					}
					i++;				//jump from ','
					TXIN->index= atoi(tmp);

					memset(tmp, '\0', 16);
					j=0;
					while(buff[i]!='\n'){
						tmp[j]=buff[i];
						i++;
						j++;
						if(i > 140 || j > 16){
							printf("\n_readData--<info> \"%s\"is found. but the data structure is wrong (txid,index,fund)\r", fname);
							return 0;
						}
					}
					TXIN->fund= atoi(tmp);

				 }
				 else
				 {
					printf("\n_readData--<info> \"%s\" is empty or not readable.Data style-->TXID,index,fund(satoshi))\r", fname);
					return 0;
				}
				 f_close(&FileHand);

				 break;
		 case FR_NO_FILE:
				 printf("\n_readData--<info>\"%s\" is not found. Create a TXIN.txt file.(Data style-->TXID,index,fund(satoshi))\r", fname);
				 return 0;

				 break;
		 default:
				 printf("\n_readData--<error><FATFS> f_stat error file name=%s, Error Code: (%i)\r",fname , result);
				 return 0;
		}
		/********************************************************************/
		/*********************check the  "TXIN.txt"**************************/
		fname="SCRIPTADR.txt";
		result = f_stat(fname, &FileInfo);
		memset(buff, '\0',140);
		switch (result) {
			case FR_OK:
					printf("\n_readData--<info>\"%s\"is found. \r", fname);
					result2=f_open(&FileHand, fname, FA_READ);
					if(result2 != FR_OK){ printf("\n_readData--<error><FATFS> open file error file name=%s, Error Code=%d \r", fname, result2); return 0;}
					if(f_gets((TCHAR*)buff, 140, &FileHand)!=0){
						int i = 0;
						while(buff[i]!='\n'){
							keys->scriptAdr[i]=buff[i];
							i++;
						}
						keys->scriptAdr[i]='\0';

					}else{
						printf("\n_readData--<error>\"%s\" reading error. \r", fname);
						return 0;
					}

					f_close(&FileHand);

					break;
		case FR_NO_FILE:
					printf("\n_readData--<info>\"%s\" is not found.\r", fname);
					return 0;

					break;
		default:
				printf("\n_readData--<error><FATFS> f_stat error file name=%s, Error Code: (%i)\r",fname , result);
				return 0;
				}
		/********************************************************************/

		*cnt=counter;
		*verifiedHight=lastVerified;

		result=f_mount(NULL, "", 0);
		if (result != FR_OK) {	printf("\n_readData--<error><FATFS> Un-mounting SD memory error, Error Code: (%i)\r", result); 	return 0; 	}
		printf("\n_readData--<info> Reading is completed");
		return 1;
}


