/*
 * mparser.c
 *
 *  Created on: Jan 24, 2022
 *      Author: mrahm
 */




#include <_messageParser.h>

	/*Hint
		 * inPut
		 * const char* txoutproof;
		 *
		 * outPut
		 * vector<string> vector;
		 * vector[0] = "total_TX"
		 * vector[1] = "flag"
		 * vector[2] = "HashNumber"
		 * vector[i+3] Hashes
		 *
		 */

vector<string> _messageParser(const char* txoutproof){


		/*************************Define Variables + '\0'**********************************/
		int split_len=0; 					//show number of the start character of next string
		char ver[8+1];						// bigendian version
		ver[8]='\0';						//	put the null character to show the end of array
		char pre_block[64+1];				// bigendian privious block hash
		pre_block[64]='\0';
		char merkle_root[64+1];				//bigendian merkle root
		merkle_root[64]='\0';
		char timestamp[8+1];				//bigendian timestamp
		timestamp[8]='\0';
		char bits[8+1];						//bigendian dificulty
		bits[8]='\0';
		char nonce[8+1];					//bigendian nonce
		nonce[8]='\0';
		char total_tx[8+1];					//bigendian total tx in the block
		total_tx[8]='\0';
		char hash_count[2+1];				//bigendian total hash proof in this msg
		hash_count[2]='\0';

		char flag_bytes[2+1];				// number of flag bytes
		flag_bytes[2]='\0';

//moved char hashProof[hashNumber][64+1];	//I moved it to hash section

//moved	char flagInHex[(flagByteNum*2)+1];	//I moved it to flag section
//moved	flagInHex[flagByteNum*2]='\0';


		/*************************Spliting by strcpy()**********************************/

			strncpy(ver, txoutproof ,sizeof(ver)-1);
			split_len += sizeof(ver)-1;
			strncpy(pre_block, txoutproof + split_len ,sizeof(pre_block)-1);
			split_len += sizeof(pre_block)-1;
			strncpy(merkle_root, txoutproof + split_len,sizeof(merkle_root)-1);
			split_len += sizeof(merkle_root)-1;
			strncpy(timestamp,txoutproof + split_len,sizeof(timestamp)-1);
			split_len += sizeof(timestamp)-1;;
			strncpy(bits, txoutproof + split_len ,sizeof(bits)-1);
			split_len += sizeof(bits)-1;
			strncpy(nonce, txoutproof + split_len ,sizeof(nonce)-1);
			split_len += sizeof(nonce)-1;
			strncpy(total_tx, txoutproof + split_len ,sizeof(total_tx)-1);
			split_len += sizeof(total_tx)-1;


			// HashProof extractor***by hash_count******************
			strncpy(hash_count, txoutproof + split_len ,sizeof(hash_count)-1);
			split_len += sizeof(hash_count)-1;

			int hashNumber=  atoi(hash_count);
			char hashProof[hashNumber][64+1];

			for(int k=0; k < hashNumber ;k++ )	{
				hashProof[k][64]='\0';
				strncpy(&hashProof[k][0], txoutproof+(split_len) , 64);
				split_len += 64;
			}

			// Flag extractor***by flag_bytes numbers******************
			strncpy(flag_bytes, txoutproof+(split_len) ,sizeof(flag_bytes)-1);
			split_len += sizeof(flag_bytes)-1;

			int flagByteNum=  atoi(flag_bytes);
			char flagInHex[(flagByteNum*2)+1];
			flagInHex[flagByteNum*2]='\0';

			strncpy(flagInHex, txoutproof+(split_len) , flagByteNum*2);
			split_len += flagByteNum*2;


		/*************************Printing the result**********************************/
#ifdef PRINT
				cout<<" _TxOutProof_ENCODER_________________________________________________"<<"\r\n";
				cout<<"ver \r\n"<<ver<<"\r\n";
				cout<<"pre_block \r\n"<<pre_block<<"\r\n";
				cout<<"merkle_root \r\n"<<merkle_root<<"\r\n";
				cout<<"timestamp \r\n"<<timestamp<<"\r\n";
				cout<<"bits \r\n"<<bits<<"\r\n";
				cout<<"nonce \r\n"<<nonce<<"\r\n";
				cout<<"total_tx \r\n"<<total_tx<<"\r\n";
				cout<<"hash_count \r\n"<<hash_count<<"\r\n";
				cout<<"hashNumber \r\n"<<hashNumber<<"\r\n";
				for(int i =0;i<hashNumber;i++) cout<<"hashProof "<<i<<"\r\n"<<hashProof[i]<<"\r\n";
				cout<<"flag_bytes \r\n"<<flag_bytes<<"\r\n";
				cout<<"flagInHex \r\n"<<flagInHex<<"\r\n";
#endif

				/*************************OutPut Preparation**********************************/

				//Hint    vector[0] = "total_TX" vector[0] = "flag" vector[1]="HashNumber"  vrctor[i+3] Hashes

				 vector<string> outPut;
				 outPut.push_back(merkle_root);
				 outPut.push_back(total_tx);
				 outPut.push_back(flagInHex);
				 outPut.push_back(hash_count);

				for(int i=0; i<hashNumber ; i++)
					outPut.push_back(hashProof[i]);

				return outPut;
	}










