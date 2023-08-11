/*
 * _transmitTX.h
 *
 *  Created on: May 24, 2022
 *      Author: Administrator
 */

#ifndef INC__TRANSMITTX_H_
#define INC__TRANSMITTX_H_

void _getUTXO(char* adr,char* server, char* port);



void _sendTX (char* srv,char * prt,char* rawtx, int size);

void _getTXfromAdd(char* server,char * port, char* addr);
void _getTXfromAdd2(char* server,char * port);
void _getTX(char* srv,char * prt,char * txid, char * Blockid);
void _getTX2(char* srv,char * prt,char * txId, char * blockId);



#endif /* INC__TRANSMITTX_H_ */
