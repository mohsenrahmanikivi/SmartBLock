/*
 * _update.h
 *
 *  Created on: Oct 7, 2022
 *      Author: Administrator
 */

#ifndef INC__SEARCH_H_
#define INC__SEARCH_H_


uint8_t _search( char* server, char* port, char* adr, txDataStruct* tx);

uint8_t _getUTXO(char* server, char* port, char* adr,txDataStruct* tx);


#endif /* INC__SEARCH_H_ */
