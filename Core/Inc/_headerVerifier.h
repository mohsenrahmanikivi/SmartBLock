

#ifndef INC__HEADERVERIFIER_H_
#define INC__HEADERVERIFIER_H_



#include "stdlib.h"  //atoi
#include "string.h" //memset
#include "stdio.h"  //sprintf
#include "math.h"   //floor
#include "fatfs.h"


using namespace std;

//some variables for FatFs



uint8_t _headerVerifier(int localHight,string GlobalpreHASH,int verifiedHight);

uint8_t checkHash(char LineOfData[240],int hight,string *GlobalpreHASH);
uint8_t fileVerifier(int fileNumber, FIL* fileHandler,string* GlobalpreHASH);
string ReversBytes (string str);


#endif /* INC__HEADERVERIFIER_H_ */


