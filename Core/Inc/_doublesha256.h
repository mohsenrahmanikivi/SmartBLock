/*
 * merklehash.h
 *
 *  Created on: Apr 13, 2022
 *      Author: mrahmanikivi
 */

#ifndef INC__doublesha256_H_
#define INC__doublesha256_H_


#include <iostream>
#include <string>
#include "Hash.h"
#include <vector> //for hex to char


using namespace std;
#define String string

vector<uint8_t>  HexToBytes(const string& hex);
string _doublesha256(string strIn);


#endif /* INC_MERKLEHASH_H_ */
