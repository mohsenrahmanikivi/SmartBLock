/*
 * ___merkle.cpp
 *
 *  Created on: Apr 19, 2022
 *      Author: mrahmanikivi
 */


#include <__doesItBelong.h>
#include <___merkle.h>



uint8_t merkle(const char * txoutproof,const char * txid){

	/******************preparinginput********************************/
	char temptx[64];
		strncpy(temptx, txid, 64);
		if(strlen(txid)>64){
			cout<<"\nmerkle--> Too long tx id, limited to 64\r";
			return 0;
		}

		string stringTemptx(temptx);
	char temptxoutproof[800];
	strncpy(temptxoutproof, txoutproof, strlen(txoutproof));
	if(strlen(txoutproof)>800){
		printf("\nmerkle-->Too long txoutproof, limited to 800 , but recieved:%d \r", strlen(txoutproof));
		return 0;
	}

	/*****************************************************************/

	 //reverse to big endian
	string tx_in_block(revHexBytesString(stringTemptx));


	return __doesItBelong(temptxoutproof, tx_in_block);

}


string revHexBytesString (string str){
	 string reverseStr;



	 for(int n = str.length()-1; n >= 0; n=n-2)
	 {
		 if(str.length()==1) {
			 reverseStr.push_back('0');
			 reverseStr.push_back(str[n]);
		 }else{
			 reverseStr.push_back(str[n-1]);
			 reverseStr.push_back(str[n]);
		 }


 	 }
	 return reverseStr;
 }
