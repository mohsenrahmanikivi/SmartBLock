/*
 * __headerVerifier.c
 *
 *  Created on: May 23, 2022
 *      Author: Administrator
 */

#include "stdlib.h"  //atoi
#include "string.h" //memset
#include "stdio.h"  //sprintf
#include "math.h"   //floor
#include "fatfs.h"
#include "_doublesha256.h"
#include "_headerVerifier.h"

using namespace std;

//string GlobalpreHASH="0000000000000000000000000000000000000000000000000000000000000000";







uint8_t _headerVerifier(int localHight,string GlobalpreHASH,int verifiedHight){
			printf("\nVERF-->  Verifying START ");
			/*********************************Variables**************************************/
			FATFS fatHandler; 	//Fatfs handle
			FIL fileHandler;
			FRESULT fResult; //Result after operations

			/*********************************FATFS Mounting*********************************/
			//0- Unmount
			f_mount(NULL, "", 0);
			//1=mount now
			fResult = f_mount(&fatHandler, "", 1);
			//3=Check mounting
			if (fResult != FR_OK) {	printf("\nVERF--> f_mount error (%i)\r\n", fResult); 	while(1);	}

			/*********************************Main operation*********************************/

			for(int first= verifiedHight/100 ; first< floor(localHight/100) ; first++)
			{
				if (fileVerifier (first,&fileHandler, &GlobalpreHASH)!=1) {
					printf("\nVERF--> %d.txt Verifying Failed ",first);
					while(1);
				}


			}



			/*********************************FATFS Mounting*********************************/
			f_mount(NULL, "", 0);
			printf("\nVERF--> All Headers Verified ");
			printf("\nVERF--> Last Verified Hight %d ",(int)floor(localHight/100)*100);

			return 1;
}



uint8_t fileVerifier(int fileNumber, FIL* fileHandler,string* GlobalpreHASH){

				FRESULT fResult; //Result after operations
				UINT bytesNUM;
				int pointerToFile=0;   //pointr to file from top of file to this address
				char fileName[64];

				//generate the directory path
				char folderName[32];
				long int f1000= floor(fileNumber/1000);
				sprintf(folderName,"HEADERS/%ld_%ld" , f1000*10000 , ((f1000+1)*10000)-1);
				sprintf (fileName,"%s/%d.txt",folderName, fileNumber*100);

				//1- read fileNumber.txt
				char TMP[240];
				memset(TMP,'\0',240);

				//0- open fileNumber.txt
				fResult = f_open(fileHandler, fileName, FA_READ);
				while(fResult != FR_OK) { printf("\nVERF--> f_open with error (%i) in fileVerifier() and %s\r\n", fResult, fileName); return 0;}


				//000- Reading Loop
				for(int hight=fileNumber*100; hight<(fileNumber+1)*100 ; hight++ )
				{

					//00- number of characters per line
					uint8_t digit=0;
					int t=hight;
					do { t /= 10;   ++digit;  } while (t != 0);
					int chPerLine=227+digit;



					fResult = f_read(fileHandler, TMP, chPerLine, &bytesNUM);
					if(fResult != FR_OK){	printf("\nVERF--> reading %s failed error:(%d) in fileVerifier()\r\n",fileName, fResult); return 0;}

					pointerToFile+= chPerLine;

					fResult=f_lseek (fileHandler, pointerToFile);
					if(fResult != FR_OK) { printf("\nVERF--> f_lseek error (%i)in fileVerifier()\r\n", fResult); return 0; }

					//	 GlobalpreHASH for hight=0
					if (hight==0) GlobalpreHASH->assign("0000000000000000000000000000000000000000000000000000000000000000");

					if( checkHash(TMP, hight, GlobalpreHASH)==1 ){
						printf("\nVERF--> Block %d in %s verified",hight,fileName);
					}
					else{
						printf("\nVERF--> Block %d in %s verification FAILED",hight,fileName);
						return 0;
					}

				}
				//4-close
				f_close(fileHandler);

				if(f_open(fileHandler, "verified.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ) != FR_OK) printf("\nVERF--> f_open verified.txt with error \r\n");

				memset(TMP,'\0',240);
				sprintf(TMP,"%d,%s\r",(fileNumber+1)*100, (*GlobalpreHASH).c_str());
				fResult = f_write(fileHandler, TMP, strlen(TMP), &bytesNUM);
				if(fResult!= FR_OK){
					printf("\nVERF--> f_Write in verified.txt with error %d \r\n",fResult);
					return 0;
				}

				f_close(fileHandler);
				return 1;
}





uint8_t checkHash(char LineOfData[240],int hight,string *GlobalpreHASH){

	int digit=0;
	int t=hight;
	do { t /= 10;   ++digit;  } while (t != 0);
	int len=1+digit;

	char HASH[64+1];
	HASH[64]='\0';
	for(int i=0; i<64; i++) HASH[i]=LineOfData[i+len];

	char HEADER[160+1];
	HEADER[160]='\0';
	for(int i=0; i<160; i++) HEADER[i]=LineOfData[i+len+64+1];

	char preHASH[64+1];
	preHASH[64]='\0';
	for(int i=0; i<64; i++) preHASH[i]=HEADER[i+8];


	string result=ReversBytes(_doublesha256((string)HEADER));

	string hashHASH=ReversBytes((string)preHASH);

	//	 compare GlobalpreHASH with preHASH comes from LineOfData
	if (GlobalpreHASH->compare(hashHASH)!=0)
	{	printf("\nVERF--> Global PreHASH is not the same as preHASH\r Hight=%d\r\n",hight);
	return 0;}

	else if( ((string)HASH).compare(result)!=0 )
	{	printf("\nVERF--> MISMATCH  Hight=%d blockID  =%s \nblockHash=%s \n", hight, HASH, result.c_str());
	return 0;}


	GlobalpreHASH->assign(result);
	return 1;
}

string ReversBytes (string str){
	 string reverseStr;
	 for(int n = str.length()-1; n >= 0; n=n-2)
	 {
		 reverseStr.push_back(str[n-1]);
		 reverseStr.push_back(str[n]);
 	 }
	 return reverseStr;
 }




