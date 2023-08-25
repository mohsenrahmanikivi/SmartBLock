/*
 * doesItBlongs.cpp
 *
 *  Created on: Apr 19, 2022
 *      Author: mrahmanikivi
 */
#include "_doublesha256.h"
#include <_messageParser.h>
#include <_rootFromProof.h>
#include <__doesItBelong.h>



//#define PRINT

uint8_t  __doesItBelong(const char* txoutproof, string tx_in_block){






/*2*/	int current_depth = 0;
		/*Hint
		 * outPut
		 * vector<string> vector;
		 * vector[0] = "total_TX"
		 * vector[1] = "flag"
		 * vector[2] = "HashNumber"
		 * vector[i+3] Hashes
		 *
		 */

		vector<string> s = _messageParser(txoutproof);

		/************vector[0] = Merkle Root****************************************/
		string merkleRoot((char *)s[0].data());

		/************vector[1] = "total_TX"****************************************/
														//HexArray of total TX number
		string totalTxStr((char *)s[1].data());								//HexArray to HexString
		vector<uint8_t> totalTxByt = HexToBytes(totalTxStr);			//convert HexString to ByteVector
		uint8_t *totalTxByteArray = &totalTxByt[0];						//convert ByteVector to ByteArray
		totalTxByteArray[4]= '\0';



		int totalTxInt = littleEndianToInt(totalTxByteArray, 4);			//convert to int
		if (totalTxInt % 2 != 0) totalTxInt++;


/*3*/	int tree_depth = ceil(log2(totalTxInt));						//calculate treeDepth

		/************vector[2] = "flag""****----> flagbytes[i] *******flags in bytes array***********************/
		string flagStr((char *)s[2].data());
		int flagLen = flagStr.length();

		vector<uint8_t> flagByte = HexToBytes(flagStr);			//convert HexString to ByteVector
		uint8_t *flagbyte = &flagByte[0];						//convert ByteVector to ByteArray

		/***** Flags in byte array  flagbytes[0] --> flagbytes[flagLen/2] ******/
		flagbyte[flagLen/2]= '\0';								//bytelengh = hexlengh/2

/****************************************Definition of finalFlags[] witch is empty ******************************************************/

		char finalFlags[(flagLen/2*8)+1];


	memset( finalFlags, '\0', sizeof( finalFlags )-1 );

/********************************************filling the finalFlags[]*************************************************************************/
		/************************************Manually*****************************************************/
//		char finalFlags[(flagLen/2*8)+1]={'1','1','0','0','0','0','0','0','1','1','1','1','0','1','1','0','\0'};

		/************************************Automatic***************************************************/

		for(int flagCount=0 ; flagCount < flagLen/2; flagCount++)
		{

//			 0- convert byte to int (flagInt)
			int flagInt = (int)flagbyte[flagCount];

//			1-  int to binary (flagBinary)
			string flagBinary;
			while(flagInt!=0) {	flagBinary=(flagInt%2==0 ?"0":"1")+flagBinary;
				flagInt/=2;	  }

//			2- padding to 8 bit with zero   BEFOR reverse
//			while(Bin.length() % 8 != 0) Bin += '0';

//			3- convert to char array (flagChar)
			char flagChar[8 + 1]={'0','0','0','0','0','0','0','0','\0'};
//			binChar[8]='\0';
			strcpy(flagChar, flagBinary.c_str());

//			4- reverse flagChar
			char rev_flagChar[8+1]={'0','0','0','0','0','0','0','0','\0'};
//			rev_binChar[8]='\0';
			for(int i=0 ; i < (int)strlen(flagChar) ; i++) rev_flagChar[i] = flagChar[(int)strlen(flagChar)-1-i];

// 			5- padding to 8 bit with zero   AFTER reverse
//			while(strlen(rev_binChar) % 8 != 0) rev_binChar[strlen(rev_binChar)]= '0';

//			6- concatinate(append) with finalFlags
/*1*/   	strcat(finalFlags, rev_flagChar);

		}

		/************vector[3] = "HashNumber"****************************************/
		string hashNumStr((char *)s[3].data());
		uint8_t hashNum_t[1] ;
		hashNum_t[1]='\0';
		fromHex(hashNumStr, hashNum_t, 1);
		int hashNum= (int)hashNum_t[0];
		string hash_list[hashNum];
/*4*/	for(int i=0;i<hashNum ; i++) hash_list[i]= s[i+4].data();


		/************Print output***************************************************/
#ifdef PRINT
	    cout<<"Total Tx Number = "<<totalTxInt<<"\r\n";
		cout<<"tree_depth = "<<tree_depth<<"\r\n";
	    cout<<"flaginHex = "<<flagStr<<"\r\n";
		cout <<"finalFlags = "<< finalFlags<<"\r\n";
		cout<<"hashNumber="<<hashNum<<"\r\n";
	  	for(int i=0;i<hashNum ; i++) cout<<"HashProof["<<i<<"]="<<hash_list[i]<<"\r\n";
#endif

		/************Compute merkle root********************************************/
//  	string _rootFromProof(char* bin_flags,int current_depth,int tree_depth, string* hash_list,int hashCount,string tx_in_block)

	    string result = _rootFromProof(finalFlags, current_depth, tree_depth, hash_list, tx_in_block);

	    								/*1*/   	/*2*/			/*3*/		/*4*/		/*5*/
	    Reset ();

#ifdef PRINT
	    cout<<"Compute merkle root :\r\n"<<result<<"\r\n";
	    cout<<"Desired merkle root :\r\n"<<merkleRoot<<"\r\n";
#endif
	    /************Print The result***********************************************/


	    if (merkleRoot == result)
	    	return 1;
	    	else
	    	return 0;


}
