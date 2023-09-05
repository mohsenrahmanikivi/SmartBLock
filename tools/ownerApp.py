# requiment 
#$ pip install bitcoin-utils


from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput
from bitcoinutils.keys import P2pkhAddress, PrivateKey, Address
from bitcoinutils.script import Script

#Contract details including RedeemScript and TxIN ID + TxIn Index
TxIn_id= "b357b6bae552f99483d0c08ee101704101cbbf3cc3c84c00ffa4ea79ff89b69f"
TxIn_index=1 
fund=0.0000635
fee=350/100000000

#Owner MASTER Private Key to sign the transaction
ownerXprv="tprv8ZgxMBicQKsPdJoBnWQ4NXgfYY3a344cVpfxGVVAMex4Ka5UZfdcEVC8E43cpxpj9WfzWJLy8yRQWzD5StoRm6JLqjLNCbfyKsDEmqX3Lhh"
derivePath="m/0"
ownerTime=1682848796

#Lock address 
lockAddr="n4Epcra9y3WigWeaDJxMxYNvmPeS7eEDwR"


#Guest Address
guestAddr="mnrqmi3gyEavAtv1hFMxqWjRcSzvPzJBnm"
guestTime=1693586157




#
def main():
    # Always remember to setup the network
    setup('testnet')

    
    # Time convert to hex and big endian
    guestTimebytes = guestTime.to_bytes( 10 , 'little')
    guestTimeHexBig=guestTimebytes.hex()
    ownerTimebytes = ownerTime.to_bytes( 10 , 'little')
    ownerTimeHexBig=ownerTimebytes.hex()

    #Derive Owner key
    hdw = HDWallet(ownerXprv, derivePath)
    privkey = hdw.get_private_key()
    pubkey = privkey.get_public_key()
    ownerAddr = pubkey.get_address()
    print('\nOwner  address:\n', ownerAddr.to_string() )

    guestAddrString = P2pkhAddress(guestAddr)
    lockAddrString = P2pkhAddress(lockAddr)

    # 1- Prepare the redeem script
    p2shRedeem=['OP_IF'     , guestTimeHexBig[:8] , 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', \
                'OP_DUP'    , 'OP_HASH160', guestAddrString.to_hash160(),'OP_EQUALVERIFY', 'OP_CHECKSIG',\
                'OP_ELSE'   , ownerTimeHexBig[:8], 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP',\
                'OP_DUP'    , 'OP_HASH160', ownerAddr.to_hash160(), 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']
   
   
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
      

    # note that we pass the scriptPubkey as one of the inputs of sign_input
    # because it is used to replace the scriptSig of the UTXO we are trying to
    # spend when creating the transaction digest
    from_addr = P2pkhAddress(ownerAddr.to_string())
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

    # set the scriptSig (unlocking script)
    txin.script_sig = Script([sig, pk])
    signed_tx = tx.serialize()

    # print raw signed transaction ready to be broadcasted
    print("\nRaw signed transaction:\n" + signed_tx)

    # print the size of the final transaction
    print("\nSigned transaction size (in bytes):\n" + str(tx.get_size()))

    print("\nTransaction ID:\n" + tx.get_txid())


if __name__ == "__main__":
    main()
