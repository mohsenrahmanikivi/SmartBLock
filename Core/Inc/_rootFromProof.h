/*
 * exampleBtree.h
 *
 *  Created on: Apr 7, 2022
 *      Author: mrahmanikivi
 */

#ifndef INC__RootFromProof_H_
#define INC__rootFromProof_H_

#include <iostream>
#include <string>


using namespace std;
#define String string


string _rootFromProof(char* bin_flags,int current_depth,int tree_depth, string* hash_list,string tx_in_block);
void Reset (void);



#endif /* INC_MERKLEPROOF_H_ */
