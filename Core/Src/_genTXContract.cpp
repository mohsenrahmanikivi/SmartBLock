/*
 * _TXgenerator.cpp
 *
 *  Created on: Aug 25, 2023
 *      Author: mrahm
 */

#include <___merkle.h>
#include <_genTXContract.h>



/**********0- Redeem Script************
		OP_IF
		<BigEndian_Hex_Unix_timestamp_1> OP_CHECKLOCKTIMEVERIFY OP_DROP OP_DUP OP_HASH160 <g_addr0> OP_EQUALVERIFY OP_CHECKSIG
		OP_ELSE
		<BigEndian_Hex_Unix_timestamp_2> OP_CHECKLOCKTIMEVERIFY OP_DROP OP_DUP OP_HASH160 <o_addr0> OP_EQUALVERIFY OP_CHECKSIG
		OP_ENDIF
	**************************************
		63
		<bythelengh=04> 809DE664 b1 75 76 a9 <bythelengh=11> <g_addr0> 88 ac
		67
		<bythelengh=04> 809DE664 b1 75 76 a9 <hex_strlen/2> <g_addr0> 88 ac
		68
	*/

#define PRINT

uint8_t _genTXContract (Tx* tx, int fee,
						char* TxInid,int TxInIndex, int TxInfund, HDPrivateKey TxInPrivateKey,
						const char* changeAddr,
						char* GuestAdr, char* nlock_Guest, char* OwnerAdr,char* nlock_Owner){



	uint8_t outputSize=100;
	uint8_t output[outputSize];
	int output_lengh=0;
	uint8_t hexlengh=0;
	uint8_t bytelengh=0;

	char GuestAdr_Inhex[100];
	memset( GuestAdr_Inhex,'\0', 100);
	char byteLen_GuestAdr_Inhex[3];
	memset( byteLen_GuestAdr_Inhex,'\0', 3);
	char OwnerAdr_Inhex[100];
	memset( OwnerAdr_Inhex,'\0', 100);
	char byteLen_OwnerAdr_Inhex[3];
	memset( byteLen_OwnerAdr_Inhex,'\0', 3);

	char nlock_Guest_big_Inhex[9];
	memset( nlock_Guest_big_Inhex,'\0', 9);
	char nlock_Guest_Inhex[9];
	memset( nlock_Guest_Inhex,'\0', 9);
	char byteLen_nlock_G_Inhex[3];
	memset( byteLen_nlock_G_Inhex,'\0', 3);
	char nlock_Owner_big_Inhex[9];
	memset( nlock_Owner_big_Inhex,'\0', 9);
	char nlock_Owner_Inhex[9];
	memset( nlock_Owner_Inhex,'\0', 9);
	char byteLen_nlock_O_Inhex[3];
	memset( byteLen_nlock_O_Inhex,'\0', 3);

	//GuestAdr
	memset(output, '\0', outputSize);
	output_lengh=fromBase58Check(GuestAdr, strlen((const char *)GuestAdr), output , outputSize);
	hexlengh=toHex(output, output_lengh, GuestAdr_Inhex, 100);
	hexlengh=hexlengh-2;//removing the two
	bytelengh=hexlengh/2;
	if(bytelengh==0) bytelengh = 1;
	toHex(&bytelengh, 1, byteLen_GuestAdr_Inhex , 3);
	//OwnerAdr
	memset(output, '\0', outputSize);
	output_lengh=fromBase58Check(OwnerAdr, strlen((const char *)OwnerAdr), output , outputSize);
	hexlengh=toHex(output, output_lengh, OwnerAdr_Inhex, 100);
	hexlengh=hexlengh-2;//removing the two
	bytelengh=hexlengh/2;
	if(bytelengh==0) bytelengh = 1;
	toHex(&bytelengh, 1, byteLen_OwnerAdr_Inhex , 3);
	//nlock_Guest
	int tmp=atoi(nlock_Guest);
	sprintf(nlock_Guest_Inhex, "%x",tmp);
	hexlengh=strlen((const char *)nlock_Guest_Inhex);
	bytelengh=hexlengh/2;
	if(bytelengh==0) bytelengh = 1;
	toHex(&bytelengh, 1, byteLen_nlock_G_Inhex , 3);
	strcpy(nlock_Guest_big_Inhex , revHexBytesString (string(nlock_Guest_Inhex)).c_str());
	//nlock_Owner
	tmp=atoi(nlock_Owner);
	sprintf(nlock_Owner_Inhex, "%x",tmp);
	hexlengh=strlen((const char *)nlock_Owner_Inhex);
	bytelengh = hexlengh/2;
	if(bytelengh==0) bytelengh = 1;
	toHex(&bytelengh, 1, byteLen_nlock_O_Inhex , 3);
	strcpy(nlock_Owner_big_Inhex , revHexBytesString (string(nlock_Owner_Inhex)).c_str());


	if ((GuestAdr && GuestAdr_Inhex+2 && byteLen_GuestAdr_Inhex && nlock_Guest && nlock_Guest_big_Inhex && byteLen_nlock_G_Inhex &&
				 OwnerAdr && OwnerAdr_Inhex+2 && byteLen_OwnerAdr_Inhex && nlock_Owner && nlock_Owner_big_Inhex && byteLen_nlock_O_Inhex) == 0){
	printf("\nThere is ERROR. CHECK this list. \r");
	printf(
			"\nGuest Address                   = %s\r"
			"\nGuest Address InHex             = %s\r"
			"\nGuest Address hex length InHex  = %s\r"
			"\nGuest Time Lock                 = %s\r"
			"\nGuest Time Lock Big InHex       = %s\r"
			"\nGuest Time Lock hex length InHex= %s\r"
			"\nOwner Address                   = %s\r"
			"\nOwner Address InHex             = %s\r"
			"\nOwner Address hex length InHex  = %s\r"
			"\nGuest Time Lock                 = %s\r"
			"\nGuest Time Lock Big InHex       = %s\r"
			"\nGuest Time Lock hex length InHex= %s\r"
			,GuestAdr, GuestAdr_Inhex+2, byteLen_GuestAdr_Inhex, nlock_Guest, nlock_Guest_big_Inhex, byteLen_nlock_G_Inhex,
			 OwnerAdr, OwnerAdr_Inhex+2, byteLen_OwnerAdr_Inhex, nlock_Owner, nlock_Owner_big_Inhex, byteLen_nlock_O_Inhex
			);
	}


		char redeemScripInHex[160];
		memset(redeemScripInHex, '\0', 160);

		sprintf(
		redeemScripInHex,
		"63"
		"%s%sb17576a9%s%s88ac"
		"67"
		"%s%sb17576a9%s%s88ac"
		"68",
		byteLen_nlock_G_Inhex, nlock_Guest_big_Inhex, byteLen_GuestAdr_Inhex, GuestAdr_Inhex+2,
		byteLen_nlock_O_Inhex, nlock_Owner_big_Inhex, byteLen_OwnerAdr_Inhex, OwnerAdr_Inhex+2 );

		/**********1- txOut_P2SH (P2SH_Script)************/
		Script redeemScript;
		size_t redeemInbytesSize=80;
		uint8_t redeemInbytes[redeemInbytesSize];
		//HEX to Bytes array
		size_t len = fromHex((const char *)redeemScripInHex, strlen(redeemScripInHex), redeemInbytes, redeemInbytesSize);
		redeemScript.push(redeemInbytes, len);
		Script P2SH_Script(redeemScript, P2SH );

		TxOut txOut_P2SH( 0 , P2SH_Script);

		if (!txOut_P2SH.isValid()){
			printf("\ntxOut_P2SH is NOT VALID. \r");
			printf("\nredeem (prepared)     = \n%s\r", redeemScripInHex);
			printf("\nredeem in P2SH_Script = \n%s\r",redeemScript.toString().c_str());
			printf("\nP2SH_Script (prepared)      = \n%s\r", P2SH_Script.toString().c_str());
			printf("\nP2SH_Script (in txOut_P2SH) = \n%s\r", txOut_P2SH.scriptPubkey.toString().c_str());

		}



		/**********2- txOut_OPReturn (OpRetuen_Script)************/
		Script opReturn_Script;
		char opReturnInHex[160];
		uint8_t opReturnInBytes[80];
		if(strlen(redeemScript.toString().c_str())>160) {
			printf("\nOpReturn unacceptable Data lenght, Len= %d\r", strlen(redeemScript.toString().c_str()));
			while(1);
		}
		sprintf( opReturnInHex,"6a%s",redeemScript.toString().c_str());

		int len_op=fromHex((const char *)opReturnInHex, strlen(opReturnInHex), opReturnInBytes, 80);
		opReturn_Script.push(opReturnInBytes, len_op);

		TxOut txOut_OpReturn( 0 , opReturn_Script);

		if (!txOut_OpReturn.isValid()){
			printf("\ntxOut_OpReturn is NOT VALID. \r");
			printf("\nOpReturn_Script (prepared)          = \n%s\r", opReturn_Script.toString().c_str());
			printf("\nOpReturn_Script (in txOut_OpReturn) = \n%s\r", txOut_OpReturn.scriptPubkey.toString().c_str());
		}
		/**********3- txOut_change **************************/

		int amount= 0;
		amount= TxInfund - fee;
		TxOut txOut_Change( changeAddr , amount);
		if (!txOut_OpReturn.isValid()){
			printf("\ntxOut_Change is NOT VALID. \r");

		}

		/**********4- txIn **************************/

		TxIn txIn((const char *)TxInid, TxInIndex);
		/**********5- PrepareTX **************************/
		tx->addInput(txIn);
		tx->addOutput(txOut_P2SH);
		tx->addOutput(txOut_OpReturn);
		tx->addOutput(txOut_Change);

		PrivateKey pk=TxInPrivateKey;
		Signature sig = tx->signInput(0, pk);






	return 1;
}
