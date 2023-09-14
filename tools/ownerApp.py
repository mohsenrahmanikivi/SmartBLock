

from hdwallet import HDWallet as HDW
from hdwallet.symbols import BTCTEST


from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput, Sequence
from bitcoinutils.keys import P2pkhAddress, P2shAddress
from bitcoinutils.script import Script
from bitcoinutils.constants import TYPE_RELATIVE_TIMELOCK


#[CHANGE with your data]Contract details including RedeemScript and TxIN ID + TxIn Index
TxIn_id= "8998f4551191282b5e76ccfed729e9ea67ebbdc99baf1557e7e71bbc767c000e"
TxIn_index= 2 
fund=  0.00004211  
fee= 300/100000000

#[CHANGE with your data]Guest Address
guestAddrBase58 = "mosZv9VentzfxbQKFopkgFJ2gMJfpLMrQ1"
guestXpub = "tpubD6NzVbkrYhZ4YbGzfZ2uchThpawBzS7FgaUs3k5AZyapwWB9EkvFV6CyVNT2JWCqACyhWDiHfdityAZajDoQmh9uEPakg1LPV5TXvbpaZFs"
guest_sequence = 1 #meaning sequence as n spend_after_n_block


#[CHANGE with your data]Owner MASTER Private Key to sign the transaction
ownerXprv="tprv8ZgxMBicQKsPdJoBnWQ4NXgfYY3a344cVpfxGVVAMex4Ka5UZfdcEVC8E43cpxpj9WfzWJLy8yRQWzD5StoRm6JLqjLNCbfyKsDEmqX3Lhh"
ownerXpub="tpubD6NzVbkrYhZ4WmpygA4emwLn7ZZWCPFX58GjZ1XTmvkTA4LFC4TCQyozQCc12TcKUqrS83sp5KyNBkyxmrPM68SCx7dgfsBwD7QSt1U9LAT"
owner_sequence = 1 #meaning sequence as n spend_after_n_block

#[CHANGE with your data]Lock address 
lockXpub="tpubD6NzVbkrYhZ4YDpCdJW8s3ihthS5xpojJLVr5Mx82teeeo8oAcs62YLqPYzzGtXQYA7CTNgsV58mxD9R2zkHePtKVTj1cMaBXR1UbHXAdXt"


#[DEFINE THE INDEX] to start a contract 
Index= 2




def main():
    setup('testnet')
    # HINT in a TX we use these derivativr path(In_PATH --> (Out_PATH, Out_PATH), (Out_PATH)
    # Owner --> (Guest,  Owner) (Lock)
    # Owner --> (Guest,  Owner) (Owner)
    inPath ="m/8/"+ str(Index)
    outPath="m/8/"+ str(Index+1)

    # 1 Lock - Out_PATH
    lockHDW = HDW(symbol=BTCTEST)
    lockHDW.from_xpublic_key(lockXpub)
    lockHDW.from_path(outPath)
    lockAddrBase58 = lockHDW.p2pkh_address()
    lockaddr = P2pkhAddress(lockAddrBase58)
    # 2 Owner - In_PATH
    ownerPrv_inPath = HDWallet(ownerXprv,inPath).get_private_key()
    ownerAddr_inPath = ownerPrv_inPath.get_public_key().get_address()
    # 2 Owner - Out_PATH
    ownerPrv_outPath = HDWallet(ownerXprv,outPath).get_private_key()
    ownerAddr_outPath = ownerPrv_outPath.get_public_key().get_address()

    # 3 Guest - Out_PATH
    guestHDW = HDW(symbol=BTCTEST)
    guestHDW.from_xpublic_key(lockXpub)
    guestHDW.from_path(outPath)
    guestAddrBase58 = guestHDW.p2pkh_address()
    guestAddr = P2pkhAddress(guestAddrBase58)

    # guest op_csv
    guestSeq = Sequence(TYPE_RELATIVE_TIMELOCK, guest_sequence)
    # owner op_csv
    ownerSeq = Sequence(TYPE_RELATIVE_TIMELOCK, owner_sequence)
    
    

        
    p2shRedeem=[
                guestSeq.for_script(), 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', guestAddr.to_hash160(), 'OP_EQUAL'      , 
                'OP_NOTIF'           , 
                ownerSeq.for_script(), 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', ownerAddr_outPath.to_hash160(), 'OP_EQUALVERIFY', 
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

    print('\nOwner(inPath)   address    :', ownerAddr_inPath.to_string() )
    print('Owner(outPath)  address    :', ownerAddr_outPath.to_string() )
    print('Redeem script              :', redeemScript.get_script())
    print('Redeem script in HEX       :', redeemScript.to_hex())
    scriptAddress =  P2shAddress.from_script(redeemScript)
    print('P2SH script address in HEX :', scriptAddress.to_string() )
    P2SH_txout = TxOutput(  to_satoshis(fee), redeemScript.to_p2sh_script_pub_key() )
    
    # 2.2- Create a TxOut with OP_Return
    opReturn = p2shRedeem
    opReturn.insert( 0, 'OP_RETURN')
    opReturn.append(Index)
    opReturnScript= Script( opReturn )
    OPRUTURN_txout = TxOutput(  to_satoshis(0), opReturnScript) 
    print('OP_Return (redeamscript + NewIndex):', opReturnScript.to_hex())

    
    # 2.3- Create a TxOut with P2PKH (change output)
    changeAddr = P2pkhAddress(lockAddrBase58)
    # P2PKH_txout = TxOutput( to_satoshis(fund - fee - fee) , changeAddr.to_script_pub_key() )
    P2PKH_txout = TxOutput( to_satoshis(fund - fee - fee) , ownerAddr_outPath.to_script_pub_key() )################return fund to owner to test##############################

    # 3- Create a TxIn
    txin = TxInput(TxIn_id , TxIn_index)
 
    # 4- Form a Transaction
    tx = Transaction([txin], [ P2SH_txout, OPRUTURN_txout, P2PKH_txout])
 
    fundTX_addr = P2pkhAddress(ownerAddr_inPath.to_string())
    sig = ownerPrv_inPath.sign_input(
        tx,
        0,
        Script(["OP_DUP", "OP_HASH160", fundTX_addr.to_hash160(), "OP_EQUALVERIFY", "OP_CHECKSIG", ] ),
    )

    pubKey = ownerPrv_inPath.get_public_key().to_hex()
    txin.script_sig = Script([sig, pubKey])
    signed_tx = tx.serialize()

    print("\nRaw signed transaction:\n" + signed_tx)
    print("\nTransaction ID:\n" + tx.get_txid())


if __name__ == "__main__":
    main()

