/*
 * ___merkle.h
 *
 *  Created on: Apr 19, 2022
 *      Author: mrahmanikivi
 */

#ifndef SRC____MERKLE_H_
#define SRC____MERKLE_H_
#include <iostream>
#include <string.h>

using namespace std;

#define String string

uint8_t merkle(const char * txoutproof,const char * tx);
string revHexBytesString (string str);


#endif /* SRC____MERKLE_H_ */
