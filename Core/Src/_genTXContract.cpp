/*
 * _TXgenerator.cpp
 *
 *  Created on: Aug 25, 2023
 *      Author: mrahm
 */

#include <___merkle.h>
#include <_genTXContract.h>
#include <math.h>



	/**********0- Redeem Script***********************************************************************

	   	   	   	   <guest_timelock> , 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', guestAddr.to_hash160(), 'OP_EQUAL'      ,
		'OP_NOTIF'
					<owner_timeLock> , 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', ownerAddr.to_hash160(), 'OP_EQUALVERIFY',
		'OP_ENDIF'
					'OP_CHECKSIG'

	***************************************************************************************************
			<len><GuestSeq> b1 75 76 a9 <len><g_addr0> 87
		64
			<len><OwnerSeq> b1 75 76 a9 <len><g_addr0> 88
		68
			ac
	*************************************************************************************************/



uint8_t _genTXContractFromScript (Tx* tx, int fee, txinDataStruct* TxIn_contract, lockDataStruct* keys ){

// Extraction data from SCRIPT
    char Redeem[250];
    memset(Redeem, '\0', 250);
    int beforeSize=32;
	char before[beforeSize];
    memset(before, '\0', beforeSize);
    int afterSize=84;
	char after[128];
    memset(after, '\0', afterSize);

	int before_cnt=0;
    int after_cnt=0;

    strcpy(Redeem, keys->script);

    if (strlen(Redeem)< 120) {
         printf("\n_genTXContractFromScript--<error> Script is wrong (redeem)\r");
    	return 0;
    }
   	while(  Redeem[before_cnt]    != '7' ||
			Redeem[before_cnt+ 1] != '6' ||
			Redeem[before_cnt+ 2] != 'a' ||
			Redeem[before_cnt+ 3] != '9' ) {
	           before_cnt++;
	        if( before_cnt > beforeSize) {
	     	   printf("\n_genTXContractFromScript--<error> Script is wrong (before)\r");
	     	   return 0;
	        }
	         }
	         before_cnt= before_cnt +4 ;

	     	for (int i=0 ; i< before_cnt ; i++) before[i]= Redeem[i];

	         while(  Redeem[after_cnt]    != '8' ||
	                 Redeem[after_cnt+ 1] != '7' ||
	                 Redeem[after_cnt+ 2] != '6' ||
	                 Redeem[after_cnt+ 3] != '4' ) {

	           after_cnt++;
	           if( after_cnt > afterSize) {
	         	  printf("\n_genTXContractFromScript--<error> Script is wrong (after)\r");
	         	  return 0;
	           }
	         }
	     	for (int j=0 ; j+ after_cnt < (int)strlen(Redeem) ; j++) {after[j]= Redeem[after_cnt+j];}



// data extracted as a "befor" and "after"

	// Path preparation
	sprintf( keys->inPath, "%s%d", keys->derivativePath, atoi(keys->index));
	sprintf( keys->outPath, "%s%d", keys->derivativePath, atoi(keys->index)+1);

	char GuestAdr[64];
	char LockAdr[64];
	//for P2SH output
	sprintf(GuestAdr ,"%s", keys->guestXpub.derive(keys->outPath).address().c_str());

	//for change output
	sprintf(LockAdr ,"%s", keys->lockXprv.derive(keys->outPath).address().c_str());
	//for sign the input
	HDPrivateKey InputSignerKey= keys->lockXprv.derive(keys->inPath);



	if (TxIn_contract->fund < 2*fee) {
		printf("\n_genTXContract--<error> Fund is not sufficient. Minimum fund should be >700 satoshi \r");
		printf("\n_genTXContract--<error> Fund is= %d , fee = %d\r",TxIn_contract->fund, fee );
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

	char redeemScripInHex[150]; 		memset(redeemScripInHex, '\0', 150);

	//GuestAdr
	memset(output, '\0', outputSize);
	output_lengh = fromBase58(GuestAdr, strlen((const char *)GuestAdr), output , outputSize);
	hexlengh=toHex(output, output_lengh, GuestAdr_Inhex, 60);

	char GuestAdr_Inhex_Cleaned [50]; 	memset( GuestAdr_Inhex_Cleaned,'\0', 50);
	hexlengh = hexlengh - 8 - 2 ; 							// exclude the 4 checksum byte at the end 1 byte identifier at the beginning
	for(uint8_t i=0 ; i < hexlengh ; i++) 	GuestAdr_Inhex_Cleaned[i] = GuestAdr_Inhex[i+2]; // shift 1 byte to avoid identify byte at the beginning

	hexlengh=strlen(GuestAdr_Inhex_Cleaned);
	bytelengh=hexlengh/2;     //2hex = 1byte
	if(bytelengh==0) bytelengh = 1;
	toHex(&bytelengh, 1, byteLen_GuestAdr_Inhex , 3);

		sprintf(redeemScripInHex, "%s" "%s%s" "%s", before, byteLen_GuestAdr_Inhex, GuestAdr_Inhex_Cleaned, after );


		/**********1- txOut_P2SH (P2SH_Script)************/
		Script redeemScript;
		size_t redeemInbytesSize=80;
		uint8_t redeemInbytes[redeemInbytesSize];
		//HEX to Bytes array
		size_t len = fromHex((const char *)redeemScripInHex, strlen(redeemScripInHex), redeemInbytes, redeemInbytesSize);
		redeemScript.push(redeemInbytes, len);
		Script P2SH_Script(redeemScript, P2SH );


		TxIn_contract->fund= TxIn_contract->fund - fee;
		TxOut txOut_P2SH( fee , P2SH_Script);

		if (!txOut_P2SH.isValid()){
			printf("\n_genTXContract--<error> txOut_P2SH is NOT VALID. \r");
			printf("\n P2SH Redeem  = %s\r", redeemScripInHex);
			printf("\n P2SH Address = %s\r", P2SH_Script.address().c_str());
			printf("\n P2SH PubKey Script = %s\r", P2SH_Script.toString().c_str());
		}


		/**********2- txOut_change **************************/

		TxIn_contract->fund= TxIn_contract->fund - fee;
		TxOut txOut_Change( LockAdr , TxIn_contract->fund);
		if (!txOut_Change.isValid()){
			printf("\n_genTXContract--<error> txOut_Change is NOT VALID. \r");
		}

		/**********4- txIns **************************/

		TxIn txInFromContract((const char *)TxIn_contract->id, TxIn_contract->index);
		TxIn txInFromUnlock((const char *)keys->lastUnlockTxId, 0);

		/**********5- PrepareTX **************************/
		tx->addInput(txInFromContract);
	 	tx->addInput(txInFromUnlock);

		tx->addOutput(txOut_P2SH);
		//tx->addOutput(txOut_OpReturn);
		tx->addOutput(txOut_Change);


		tx->signInput( 0, InputSignerKey);
		tx->signInput( 1, InputSignerKey);






//		printf("\n\nTX is       = %s\r", tx->toString().c_str());


	return 1;
}

/* this function use sequesnce whitch we will not use any more..
 *
 *
 **********0- Redeem Script***********************************************************************
			<GuestSeq>	OP_CHECKSEQUENCEVERIFY	OP_DROP	OP_DUP	OP_HASH160	<GuestAddr>	OP_EQUAL
		OP_NOTIF
			<OwnerSeq>	OP_CHECKSEQUENCEVERIFY	OP_DROP	OP_DUP	OP_HASH160	<OwnerAddr>	OP_EQUALVERIFY
		OP_ENDIF
			OP_CHECKSIG

 **************************************************************************************************
			<len><GuestSeq> b2 75 76 a9 <len><g_addr0> 87
		64
			<len><OwnerSeq> b2 75 76 a9 <len><g_addr0> 88
		68
			ac
 *************************************************************************************************
 */
uint8_t _genTXContract (Tx* tx,
						int fee,
						txinDataStruct* TxIn_contract,
						lockDataStruct* keys,
						char* GuestSeq,
						char* OwnerSeq){

	// Path preparation
	sprintf( keys->inPath, "%s%d", keys->derivativePath, atoi(keys->index));
	sprintf( keys->outPath, "%s%d", keys->derivativePath, atoi(keys->index)+1);

	char GuestAdr[64];
	char OwnerAdr[64];
	char LockAdr[64];
	//for P2SH output
	sprintf(GuestAdr ,"%s", keys->guestXpub.derive(keys->outPath).address().c_str());
	sprintf(OwnerAdr ,"%s", keys->ownerXpub.derive(keys->outPath).address().c_str());
	//for change output
	sprintf(LockAdr ,"%s", keys->lockXprv.derive(keys->outPath).address().c_str());
	//for sign the input
	HDPrivateKey InputSignerKey= keys->lockXprv.derive(keys->inPath);



	if (TxIn_contract->fund < 2*fee) {
		printf("\n_genTXContract--<error> Fund is not sufficient. Minimum fund should be >700 satoshi \r");
		printf("\n_genTXContract--<error> Fund is= %d , fee = %d\r",TxIn_contract->fund, fee );
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

	char redeemScripInHex[150]; 		memset(redeemScripInHex, '\0', 150);

	//GuestAdr
	memset(output, '\0', outputSize);
	output_lengh = fromBase58(GuestAdr, strlen((const char *)GuestAdr), output , outputSize);
	hexlengh=toHex(output, output_lengh, GuestAdr_Inhex, 60);

	char GuestAdr_Inhex_Cleaned [50]; 	memset( GuestAdr_Inhex_Cleaned,'\0', 50);
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
	int G_S = atoi(GuestSeq);
	sprintf(GuestSeq_Inhex, "%x", G_S);
	hexlengh = strlen((const char *)GuestSeq_Inhex);
	bytelengh = hexlengh/2;
	if(bytelengh == 0) bytelengh = 1;
	toHex(&bytelengh, 1, ByteLen_GuestSeq_Inhex , 3);
	strcpy(GuestSeq_Inhex_big , revHexBytesString (string(GuestSeq_Inhex)).c_str());

	//nlock_Owner
	int O_S = atoi(OwnerSeq);
	sprintf(OwnerSeq_Inhex, "%x", O_S);
	hexlengh = strlen((const char *)OwnerSeq_Inhex);
	bytelengh = hexlengh/2;
	if(bytelengh == 0) bytelengh = 1;
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


	char OP_Number[2];
	if(G_S > 16 && O_S > 16) {
		sprintf(redeemScripInHex,
					"%s" "%s" "b2" "75" "76" "a9" "%s" "%s" "87"
				"64"
					"%s" "%s" "b2" "75" "76" "a9" "%s" "%s" "88"
				"68"
					"ac"
				,
				ByteLen_GuestSeq_Inhex, GuestSeq_Inhex_big, byteLen_GuestAdr_Inhex, GuestAdr_Inhex_Cleaned,
				ByteLen_OwnerSeq_Inhex, OwnerSeq_Inhex_big, byteLen_OwnerAdr_Inhex, OwnerAdr_Inhex_Cleaned );

	}else if (G_S < 16 && O_S > 16){

		sprintf(OP_Number,"%x", 80+G_S );
		sprintf(redeemScripInHex,"%sb27576a9%s%s8764%s%sb27576a9%s%s8868ac",
		OP_Number, byteLen_GuestAdr_Inhex, GuestAdr_Inhex_Cleaned, ByteLen_OwnerSeq_Inhex, OwnerSeq_Inhex_big, byteLen_OwnerAdr_Inhex, OwnerAdr_Inhex_Cleaned );

	}else if (G_S > 16 && O_S < 16){
		sprintf(OP_Number,"%x", 80+O_S );
		sprintf(redeemScripInHex,"%s%sb27576a9%s%s8764%sb27576a9%s%s8868ac",
		ByteLen_GuestSeq_Inhex, GuestSeq_Inhex_big, byteLen_GuestAdr_Inhex, GuestAdr_Inhex_Cleaned, OP_Number, byteLen_OwnerAdr_Inhex, OwnerAdr_Inhex_Cleaned );

	}else if (G_S < 16 && O_S < 16){
		sprintf(OP_Number,"%x", 80+G_S );
		char OP_Number2[2];
		sprintf(OP_Number2,"%x", 80+O_S );
		sprintf(redeemScripInHex,"%sb27576a9%s%s8764%sb27576a9%s%s8868ac",
		OP_Number, byteLen_GuestAdr_Inhex, GuestAdr_Inhex_Cleaned, OP_Number2, byteLen_OwnerAdr_Inhex, OwnerAdr_Inhex_Cleaned );

	}


		/**********1- txOut_P2SH (P2SH_Script)************/
		Script redeemScript;
		size_t redeemInbytesSize=80;
		uint8_t redeemInbytes[redeemInbytesSize];
		//HEX to Bytes array
		size_t len = fromHex((const char *)redeemScripInHex, strlen(redeemScripInHex), redeemInbytes, redeemInbytesSize);
		redeemScript.push(redeemInbytes, len);
		Script P2SH_Script(redeemScript, P2SH );


		TxIn_contract->fund= TxIn_contract->fund - fee;
		TxOut txOut_P2SH( fee , P2SH_Script);

		if (!txOut_P2SH.isValid()){
			printf("\n_genTXContract--<error> txOut_P2SH is NOT VALID. \r");
			printf("\n P2SH Redeem  = %s\r", redeemScripInHex);
			printf("\n P2SH Address = %s\r", P2SH_Script.address().c_str());
			printf("\n P2SH PubKey Script = %s\r", P2SH_Script.toString().c_str());


		}



		/**********2- txOut_OPReturn (OpRetuen_Script)************/
		Script opReturn_Script;
		char opReturnInHex[161];
		uint8_t opReturnInBytes[80];
		if(strlen(redeemScripInHex)>160) {
			printf("\n_genTXContract--<error> OpReturn unacceptable Data lenght, Len= %d\r", strlen(redeemScripInHex));
			while(1);
		}
		//prepare outPath
		uint8_t outPath_inhex[7];
		uint8_t outPath_inhex2[7];
		uint8_t l=0;
		if (atoi(keys->index)< 17){
		int path = 80 + atoi(keys->index);
		sprintf((char *)outPath_inhex, "%x",path);
		}else{
			sprintf((char *)outPath_inhex,"%x", atoi(keys->index)+1);
			l = strlen((char *)outPath_inhex);
			if ( l  % 2 != 0 ) {

				char TEMP[l+2];
				TEMP[0]='0';
				for(int i=0 ; i< l ; i++){
					TEMP[i+1] = outPath_inhex[i];
				}
				TEMP[l+1]= '\0';

				strcpy((char *)outPath_inhex, (char *)TEMP);
				l= l+1;
			}
			int i=0;
			for (; i < l; i=i+2){
				outPath_inhex2[i] = outPath_inhex[l-2-i];
				outPath_inhex2[i+1] = outPath_inhex[l-1-i];
			}
			outPath_inhex2[i]='\0';
		}


		sprintf( opReturnInHex,"6a%s%02x%s", redeemScripInHex, l/2, outPath_inhex2);

		int len_op=fromHex((const char *)opReturnInHex, strlen(opReturnInHex), opReturnInBytes, 80);
		opReturn_Script.push(opReturnInBytes, len_op);

		TxOut txOut_OpReturn( 0 , opReturn_Script);

		if (!txOut_OpReturn.isValid()){
			printf("\n_genTXContract--<error> txOut_OpReturn is NOT VALID. \r");
			printf("\n redeem Script 	= %s\r", redeemScripInHex);
			printf("\n OpReturn Script  = %s\r", txOut_OpReturn.scriptPubkey.toString().c_str());
		}
		/**********3- txOut_change **************************/

		TxIn_contract->fund= TxIn_contract->fund - fee;
		TxOut txOut_Change( LockAdr , TxIn_contract->fund);
		if (!txOut_Change.isValid()){
			printf("\n_genTXContract--<error> txOut_Change is NOT VALID. \r");
		}

		/**********4- txIns **************************/

		TxIn txInFromContract((const char *)TxIn_contract->id, TxIn_contract->index);
		TxIn txInFromUnlock((const char *)keys->lastUnlockTxId, 0);

		/**********5- PrepareTX **************************/
		tx->addInput(txInFromContract);
	 	tx->addInput(txInFromUnlock);

		tx->addOutput(txOut_P2SH);
		//tx->addOutput(txOut_OpReturn);
		tx->addOutput(txOut_Change);


		tx->signInput( 0, InputSignerKey);
		tx->signInput( 1, InputSignerKey);






//		printf("\n\nTX is       = %s\r", tx->toString().c_str());


	return 1;
}

