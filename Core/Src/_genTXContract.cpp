/*
 * _TXgenerator.cpp
 *
 *  Created on: Aug 25, 2023
 *      Author: mrahm
 */

#include <___merkle.h>
#include <_genTXContract.h>



/**********0- Redeem Script************
		<GuestSeq>		OP_CHECKSEQUENCEVERIFY	OP_DROP	OP_DUP	OP_HASH160	<GuestAddr>		OP_EQUAL
		OP_IF 	        OP_CHECKSIG
		OP_ELSE     	<OwnerSeq>	OP_CHECKSEQUENCEVERIFY	OP_DROP	OP_DUP	OP_HASH160	<OwnerAddr>	OP_EQUALVERIFY	OP_CHECKSIG
		OP_ENDIF

	**************************************
		<len><GuestSeq> b2 75 76 a9 <len> <g_addr0> 87
		63	ac
		67	<len> <OwnerSeq> b2 75 76 a9 <len> <g_addr0> 88 ac
		68
	*/

#define PRINT

uint8_t _genTXContract (Tx* tx, int fee,
						char* TxInid,int TxInIndex, int TxInfund,
						HDPrivateKey TxInPrivateKey,
						const char* GuestAdr,char* GuestSeq, const char* OwnerAdr, char* OwnerSeq){

	if (TxInfund < 2*fee) {
		printf("\n_genTXContract--<error> Fund is not sufficient. Minimum fund should be >700 satoshi \r");
		printf("\n_genTXContract--<error> Fund is= %d , fee = %d\r",TxInfund, fee );
		return 0;

	}

	uint8_t outputSize=100;
	uint8_t output[outputSize];
	int output_lengh=0;
	uint8_t hexlengh=0;
	uint8_t bytelengh=0;

	char GuestAdr_Inhex[60];			memset( GuestAdr_Inhex,'\0', 60);
	char byteLen_GuestAdr_Inhex[3];		memset( byteLen_GuestAdr_Inhex,'\0', 3);
	char OwnerAdr_Inhex[60]; 			memset( OwnerAdr_Inhex,'\0', 60);
	char byteLen_OwnerAdr_Inhex[3]; 	memset( byteLen_OwnerAdr_Inhex,'\0', 3);

	char GuestSeq_Inhex_big[9];		 	memset( GuestSeq_Inhex_big,'\0', 9);
	char GuestSeq_Inhex[9];				memset( GuestSeq_Inhex,'\0', 9);
	char ByteLen_GuestSeq_Inhex[3];		memset( ByteLen_GuestSeq_Inhex,'\0', 3);
	char OwnerSeq_Inhex_big[9];			memset( OwnerSeq_Inhex_big,'\0', 9);
	char OwnerSeq_Inhex[9];				memset( OwnerSeq_Inhex,'\0', 9);
	char ByteLen_OwnerSeq_Inhex[3];		memset( ByteLen_OwnerSeq_Inhex,'\0', 3);

	//GuestAdr
	memset(output, '\0', outputSize);
	output_lengh = fromBase58(GuestAdr, strlen((const char *)GuestAdr), output , outputSize);
	hexlengh=toHex(output, output_lengh, GuestAdr_Inhex, 60);

	char GuestAdr_Inhex_Cleaned [50];
	memset( GuestAdr_Inhex_Cleaned,'\0', 50);
	hexlengh = hexlengh - 8 - 2 ; 							// exclude the 4 checksum byte at the end 1 byte identifier at the beginning
	for(uint8_t i=0 ; i < hexlengh ; i++) 	GuestAdr_Inhex_Cleaned[i] = GuestAdr_Inhex[i+2]; // shift 1 byte to avoid identify byte at the beginning

	hexlengh=strlen(GuestAdr_Inhex_Cleaned);
	bytelengh=hexlengh/2;     //2hex = 1byte
	if(bytelengh==0) bytelengh = 1;
	toHex(&bytelengh, 1, byteLen_GuestAdr_Inhex , 3);

	//OwnerAdr
	memset(output, '\0', outputSize);
	output_lengh =0;
	output_lengh= fromBase58(OwnerAdr, strlen((const char *)OwnerAdr), output , outputSize);
	hexlengh=toHex(output, output_lengh, OwnerAdr_Inhex, 60);

	char OwnerAdr_Inhex_Cleaned [50];
	memset( OwnerAdr_Inhex_Cleaned,'\0', 50);
	hexlengh = hexlengh - 8 -2 ; 							// exclude the 4 checksum byte at the end 1 byte identifier at the beginning
	for(uint8_t i=0 ; i < hexlengh ; i++) OwnerAdr_Inhex_Cleaned[i] = OwnerAdr_Inhex[i+2]; // shift 1 byte to avoid identify byte at the beginning

	hexlengh=strlen(OwnerAdr_Inhex_Cleaned);
	bytelengh=hexlengh/2;     //2hex = 1byte
	if(bytelengh==0) bytelengh = 1;
	toHex(&bytelengh, 1, byteLen_OwnerAdr_Inhex , 3);

	//Guest_Sequence
	int tmp=atoi(GuestSeq);
	sprintf(GuestSeq_Inhex, "%x",tmp);
	hexlengh=strlen((const char *)GuestSeq_Inhex);
	bytelengh=hexlengh/2;
	if(bytelengh==0) bytelengh = 1;
	toHex(&bytelengh, 1, ByteLen_GuestSeq_Inhex , 3);
	strcpy(GuestSeq_Inhex_big , revHexBytesString (string(GuestSeq_Inhex)).c_str());

	//nlock_Owner
	tmp=atoi(OwnerSeq);
	sprintf(OwnerSeq_Inhex, "%x",tmp);
	hexlengh=strlen((const char *)OwnerSeq_Inhex);
	bytelengh = hexlengh/2;
	if(bytelengh==0) bytelengh = 1;
	toHex(&bytelengh, 1, ByteLen_OwnerSeq_Inhex , 3);
	strcpy(OwnerSeq_Inhex_big , revHexBytesString (string(OwnerSeq_Inhex)).c_str());


	if ((strlen(GuestAdr) && strlen(GuestAdr_Inhex+2) && strlen(byteLen_GuestAdr_Inhex) && strlen(GuestSeq)
		&& strlen(GuestSeq_Inhex_big) && strlen(ByteLen_GuestSeq_Inhex) &&
		strlen(OwnerAdr) && strlen(OwnerAdr_Inhex+2) && strlen(byteLen_OwnerAdr_Inhex)
		&& strlen(OwnerSeq) && strlen(OwnerSeq_Inhex_big) && strlen(ByteLen_OwnerSeq_Inhex)) == 0){
	printf("\n_genTXContract--<error> There is an ERROR. Prepared data:\r");
	printf(
			"\n# Guest Address             = %s\r"
			"\n# ----- Address InHex       = %s\r"
			"\n# ----- length InHex        = %s\r"
			"\n# ----- Time Lock           = %s\r"
			"\n# ----- Time Lock Big InHex = %s\r"
			"\n# ----- length InHex        = %s\r"
			"\n# Owner Address             = %s\r"
			"\n# ----- InHex               = %s\r"
			"\n# ----- length InHex        = %s\r"
			"\n# ----- Time Lock           = %s\r"
			"\n# ----- Big InHex           = %s\r"
			"\n# ----- hex length InHex    = %s\r"
			,GuestAdr, GuestAdr_Inhex_Cleaned, byteLen_GuestAdr_Inhex, GuestSeq , GuestSeq_Inhex_big, ByteLen_GuestSeq_Inhex,
			 OwnerAdr, OwnerAdr_Inhex_Cleaned, byteLen_OwnerAdr_Inhex, OwnerSeq , OwnerSeq_Inhex_big, ByteLen_OwnerSeq_Inhex
			);
	return 0;
	}


		char redeemScripInHex[160];
		memset(redeemScripInHex, '\0', 160);

		sprintf(
		redeemScripInHex,
		"%s%sb27576a9%s%s87"
		"63ac"
		"67%s%sb27576a9%s%s88ac"
		"68",
		ByteLen_GuestSeq_Inhex, GuestSeq_Inhex_big, byteLen_GuestAdr_Inhex, GuestAdr_Inhex_Cleaned,
		ByteLen_OwnerSeq_Inhex, OwnerSeq_Inhex_big, byteLen_OwnerAdr_Inhex, OwnerAdr_Inhex_Cleaned );

		/**********1- txOut_P2SH (P2SH_Script)************/
		Script redeemScript;
		size_t redeemInbytesSize=80;
		uint8_t redeemInbytes[redeemInbytesSize];
		//HEX to Bytes array
		size_t len = fromHex((const char *)redeemScripInHex, strlen(redeemScripInHex), redeemInbytes, redeemInbytesSize);
		redeemScript.push(redeemInbytes, len);
		Script P2SH_Script(redeemScript, P2SH );


		TxInfund= TxInfund - fee;
		TxOut txOut_P2SH( fee , P2SH_Script);

		if (!txOut_P2SH.isValid()){
			printf("\n_genTXContract--<error> txOut_P2SH is NOT VALID. \r");
			printf("\n_genTXContract--<error> redeem (prepared)     = \n%s\r", redeemScripInHex);
			printf("\n_genTXContract--<error> redeem in P2SH_Script = \n%s\r",redeemScript.toString().c_str());
			printf("\n_genTXContract--<error> P2SH_Script (prepared)      = \n%s\r", P2SH_Script.toString().c_str());
			printf("\n_genTXContract--<error> P2SH_Script (in txOut_P2SH) = \n%s\r", txOut_P2SH.scriptPubkey.toString().c_str());

		}



		/**********2- txOut_OPReturn (OpRetuen_Script)************/
		Script opReturn_Script;
		char opReturnInHex[160];
		uint8_t opReturnInBytes[80];
		if(strlen(redeemScript.toString().c_str())>160) {
			printf("\n_genTXContract--<error> OpReturn unacceptable Data lenght, Len= %d\r", strlen(redeemScript.toString().c_str()));
			while(1);
		}
		sprintf( opReturnInHex,"6a%s",redeemScript.toString().c_str());

		int len_op=fromHex((const char *)opReturnInHex, strlen(opReturnInHex), opReturnInBytes, 80);
		opReturn_Script.push(opReturnInBytes, len_op);

		TxOut txOut_OpReturn( 0 , opReturn_Script);

		if (!txOut_OpReturn.isValid()){
			printf("\n_genTXContract--<error> txOut_OpReturn is NOT VALID. \r");
			printf("\n_genTXContract--<error> OpReturn_Script (prepared)          = \n%s\r", opReturn_Script.toString().c_str());
			printf("\n_genTXContract--<error> OpReturn_Script (in txOut_OpReturn) = \n%s\r", txOut_OpReturn.scriptPubkey.toString().c_str());
		}
		/**********3- txOut_change **************************/

		TxInfund= TxInfund - fee;

		TxOut txOut_Change( TxInPrivateKey.address() , TxInfund);
		if (!txOut_OpReturn.isValid()){
			printf("\n_genTXContract--<error> txOut_Change is NOT VALID. \r");

		}

		/**********4- txIn **************************/

		TxIn txIn((const char *)TxInid, TxInIndex);
		/**********5- PrepareTX **************************/
		tx->addInput(txIn);
		tx->addOutput(txOut_P2SH);
		tx->addOutput(txOut_OpReturn);
		tx->addOutput(txOut_Change);

		Signature sig = tx->signInput(0, TxInPrivateKey);

//		printf("\n\nSignatur= %s\r", sig.toString().c_str());
//		printf("\n\nPublicKey= %s\r", TxInPrivateKey.publicKey().toString().c_str());
//
//		printf("\n\nLock Address is= %s\r", TxInPrivateKey.address().c_str());
//
//		printf("\n\nTX is= %s\r", tx->toString().c_str());


	return 1;
}
