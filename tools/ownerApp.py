
from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput, Sequence
from bitcoinutils.keys import P2pkhAddress, P2shAddress
from bitcoinutils.script import Script
from bitcoinutils.constants import TYPE_RELATIVE_TIMELOCK


#[CHANGE with your data]Contract details including RedeemScript and TxIN ID + TxIn Index
TxIn_id= "dbb703c107aa6a706ae1f5dc40b5b5d7117fb73e4e58ef097ef8b9916c7c3513"
TxIn_index= 2 
fund=  0.00000238 
fee= 300/100000000

#[CHANGE with your data]Guest Address
guestAddrBase58="mnrqmi3gyEavAtv1hFMxqWjRcSzvPzJBnm"
guest_sequence = 1 #meaning sequence as n spend_after_n_block

#[CHANGE with your data]Owner MASTER Private Key to sign the transaction
ownerXprv="tprv8ZgxMBicQKsPdJoBnWQ4NXgfYY3a344cVpfxGVVAMex4Ka5UZfdcEVC8E43cpxpj9WfzWJLy8yRQWzD5StoRm6JLqjLNCbfyKsDEmqX3Lhh"
derivePath="m/0"
owner_sequence = 1 #meaning sequence as n spend_after_n_block

#[CHANGE with your data]Lock address 
lockAddrBase58="n4Epcra9y3WigWeaDJxMxYNvmPeS7eEDwR"






#
def main():
    setup('testnet')

    # guest op_csv
    guestSeq = Sequence(TYPE_RELATIVE_TIMELOCK, guest_sequence)
    # owner op_csv
    ownerSeq = Sequence(TYPE_RELATIVE_TIMELOCK, owner_sequence)
    
    #Derive Owner key
    hdw = HDWallet(ownerXprv, derivePath)
    privkey = hdw.get_private_key()
    pubkey = privkey.get_public_key()
    ownerAddr = pubkey.get_address()
    guestAddr = P2pkhAddress(guestAddrBase58)
        
    p2shRedeem=[
                guestSeq.for_script(), 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', guestAddr.to_hash160(), 'OP_EQUAL'      , 
                'OP_NOTIF'           , 
                ownerSeq.for_script(), 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', ownerAddr.to_hash160(), 'OP_EQUALVERIFY', 
                'OP_ENDIF' , 'OP_CHECKSIG'
                ]
    
    # check Script
    if  p2shRedeem[0] > p2shRedeem[8] :
        print('Guest secuence should be lower or equal than Owner Sequence !!!')
        print('Guest secuence =', p2shRedeem[0], ', Owner Sequence =', p2shRedeem[8])
        while 1 :
            pass 
   
    
    # 2.1- Create a TxOut with P2SH
    redeemScript=Script(p2shRedeem)

    print('\nOwner  address               :', ownerAddr.to_string() )
    print('Redeem script                :', redeemScript.get_script())
    print('Redeem script in HEX         :', redeemScript.to_hex())
    scriptAddress =  P2shAddress.from_script(redeemScript)
    print('Redeem script address hex    :', scriptAddress.to_string() )
    print('Redeem script address hash160:', scriptAddress.to_hash160() )
    P2SH_txout = TxOutput(  to_satoshis(fee), redeemScript.to_p2sh_script_pub_key() )
    
    # 2.2- Create a TxOut with OP_Return
    opReturn = p2shRedeem
    opReturn.insert( 0, 'OP_RETURN')
    OpreturnScript= Script( opReturn )
    OPRUTURN_txout = TxOutput(  to_satoshis(0), OpreturnScript) 
    
    # 2.3- Create a TxOut with P2PKH (change output)
    changeAddr = P2pkhAddress(lockAddrBase58)
    #P2PKH_txout = TxOutput( to_satoshis(fund - fee - fee) , changeAddr.to_script_pub_key() )
    P2PKH_txout = TxOutput( to_satoshis(fund - fee - fee) , ownerAddr.to_script_pub_key() )################return fund to owner to test##############################

    # 3- Create a TxIn
    txin = TxInput(TxIn_id , TxIn_index)
 
    # 4- Form a Transaction
    tx = Transaction([txin], [ P2SH_txout, OPRUTURN_txout, P2PKH_txout])
 
    fundTX_addr = P2pkhAddress(ownerAddr.to_string())
    sig = privkey.sign_input(
        tx,
        0,
        Script(["OP_DUP", "OP_HASH160", fundTX_addr.to_hash160(), "OP_EQUALVERIFY", "OP_CHECKSIG", ] ),
    )

    pk = privkey.get_public_key().to_hex()
    txin.script_sig = Script([sig, pk])
    signed_tx = tx.serialize()

    print("\nRaw signed transaction:\n" + signed_tx)
    print("\nTransaction ID:\n" + tx.get_txid())


if __name__ == "__main__":
    main()

