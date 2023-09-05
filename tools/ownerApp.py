# requiment 
#$ pip install bitcoin-utils


from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput
from bitcoinutils.keys import P2pkhAddress, PrivateKey
from bitcoinutils.script import Script

#######################TX IN######################
TxIn_id= "c5518a42940571ea2ded5bf6f8206b1b95b97348ce952cfd6d23f6fd3675e991"
TxIn_index=1 
fund=0.00008436
fee=350/100000000

##################Key and addresses################
#Owner MASTER Private Key to sign the transaction
ownerXprv="tprv8ZgxMBicQKsPdJoBnWQ4NXgfYY3a344cVpfxGVVAMex4Ka5UZfdcEVC8E43cpxpj9WfzWJLy8yRQWzD5StoRm6JLqjLNCbfyKsDEmqX3Lhh"
derivePath="m/0"

#Lock address 
lockAddr="n4Epcra9y3WigWeaDJxMxYNvmPeS7eEDwR"

#Guest Address
guestAddr="mnrqmi3gyEavAtv1hFMxqWjRcSzvPzJBnm"



#
def main():
    # Always remember to setup the network
    setup('testnet')

    # 1- Prepare the redeem script
    p2shRedeem=['OP_IF', '9cb3dc6a', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', \
    'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4','OP_EQUALVERIFY', 'OP_CHECKSIG',\
    'OP_ELSE', 'ec76dd6a', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP',\
    'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']
   
   
    # 2.1- Create a TxOut P2SH
    redeemScript=Script(p2shRedeem)
    print('\nRedeem script:\n', redeemScript.get_script())
    P2SH_txout = TxOutput(  to_satoshis(fee), redeemScript.to_p2sh_script_pub_key() )
    
    
    # 2.2- Create a TxOut OP_Return
    opReturn = p2shRedeem
    opReturn.insert( 0, 'OP_RETURN')
    OpreturnScript= Script( opReturn )
    OPRUTURN_txout = TxOutput(  to_satoshis(0), OpreturnScript) 
    print('\nOP_Return script:\n', OpreturnScript.get_script())

    # 2.3- Create a TxOut P2PKH (change output)
    addr = P2pkhAddress(lockAddr)
    P2PKH_txout = TxOutput( to_satoshis(fund - fee - fee) , addr.to_script_pub_key() )
    
    # 3- Create a TxIn
    txin = TxInput(TxIn_id , TxIn_index)

    # 4- Form a Transaction
    tx = Transaction([txin], [ P2SH_txout, OPRUTURN_txout, P2PKH_txout])

    # print raw transaction
    print("\nRaw unsigned transaction:\n" + tx.serialize())

    # 5- Sign the transaction after derive the keys
    hdw = HDWallet(ownerXprv, derivePath)
    privkey = hdw.get_private_key()
    pubkey = privkey.get_public_key()
    addr1 = pubkey.get_address()
    print('\nOwner  address:\n', addr1.to_string() )

   

    # note that we pass the scriptPubkey as one of the inputs of sign_input
    # because it is used to replace the scriptSig of the UTXO we are trying to
    # spend when creating the transaction digest
    from_addr = P2pkhAddress(addr1.to_string())
    sig = privkey.sign_input(
        tx,
        0,
        Script(
            [
                "OP_DUP",
                "OP_HASH160",
                from_addr.to_hash160(),
                "OP_EQUALVERIFY",
                "OP_CHECKSIG",
            ]
        ),
    )
    # print(sig)

    # get public key as hex
    pk = privkey.get_public_key().to_hex()

    # Set the scriptSig (unlocking script)
    txin.script_sig = Script([sig, pk])
    signed_tx = tx.serialize()

    # print raw signed transaction ready to be broadcasted
    print("\nRaw signed transaction:\n" + signed_tx)

    # print the size of the final transaction
    print("\nSigned transaction size (in bytes):\n" + str(tx.get_size()))


if __name__ == "__main__":
    main()
