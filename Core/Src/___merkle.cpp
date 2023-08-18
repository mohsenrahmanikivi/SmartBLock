/*
 * ___merkle.cpp
 *
 *  Created on: Apr 19, 2022
 *      Author: mrahmanikivi
 */
#include <iostream>
#include <__doesItBelong.h>
#include <___merkle.h>
#include <string.h>



uint8_t merkle(const char * txoutproof,const char * txid){

	/******************preparinginput********************************/
	char temptx[64];
		strncpy(temptx, txid, 64);
		if(strlen(txid)>64){
			cout<<"\nmerkle--> Too long tx id, limited to 64\r";
			return 0;
		}

		string stringTemptx(temptx);
	char temptxoutproof[690];
	strncpy(temptxoutproof, txoutproof, strlen(txoutproof));
	if(strlen(txoutproof)>690){
		cout<<"\nmerkle-->Too long txoutproof, limited to 690\r";
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
		 reverseStr.push_back(str[n-1]);
		 reverseStr.push_back(str[n]);
 	 }
	 return reverseStr;
 }
