/*
 * merklehash.cpp
 *
 *  Created on: Apr 13, 2022
 *      Author: mrahmanikivi
 */

#include "_doublesha256.h"



string _doublesha256(string strIn)	{

	int strInlen = (int)strIn.length();
/*********************** strIn -----HexToBytes()----> bytesIn[ strInlen/2 ] ***********************/
	vector<uint8_t> bytes = HexToBytes(strIn);
	uint8_t *bytesIn = &bytes[0];
	bytesIn[strInlen/2]= '\0';
/**********************  bytesIn[ strInlen/2 ] ---hash()---> bytesOut[ 32 ]  *******************/
	uint8_t bytesOut[32];
	doubleSha(bytesIn, strInlen/2, bytesOut);
/**********************  bytesOut[ 32 ] ---toHex()---> hexOut [ 64 ] *****************************/
	char hexOut[64];
	toHex(bytesOut, 32, hexOut, 64);
	hexOut[64] = '\0';
/**********************   hexOut [ 64 ]  ---to str---> strOut   *****************************/
	string strOut(hexOut);

return strOut;
}

vector<uint8_t> HexToBytes(const std::string& hex) {

	vector<uint8_t> bytes;
  for (unsigned int i = 0; i < hex.length(); i += 2) {
    string byteString = hex.substr(i, 2);
    uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }

return bytes;
}
