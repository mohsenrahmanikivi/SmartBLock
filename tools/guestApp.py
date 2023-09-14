from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput, Sequence
from bitcoinutils.keys import P2pkhAddress , P2shAddress
from bitcoinutils.script import Script, OP_CODES
from bitcoinutils.constants import TYPE_RELATIVE_TIMELOCK


########[CHANGE-1 with your data] Contract detailes including RedeemScript and TxIN ID + TxIn Index + Op_Return ScripPub
TxIn_id= "8998f4551191282b5e76ccfed729e9ea67ebbdc99baf1557e7e71bbc767c000e"
TxIn_P2sh_index= 0
TXin_OpReturnScript_inHex="6a51b27576a91455d73f1283b1fb24323341fdaad21d19be4197f9876451b27576a914f6ba845b23fe71a5bf7e1fd483263ac799fe3d2c8868ac52"

########[CHANGE-2 with your data] Guest/Owner Private Key to sign the transaction
## Guest key
# Xprv="tprv8ZgxMBicQKsPf8FCmuNKDHobFZRFq6vM7Gt5mE2s9hnS71vNcN6fJbb7KDXdahL7sZNqoyPktfkRdjVUw8v7aJRxw6Yp96tmjWzQKTCpTRo"
# IsGuest= 1 #dont touch this
 
## Owner key
Xprv="tprv8ZgxMBicQKsPdJoBnWQ4NXgfYY3a344cVpfxGVVAMex4Ka5UZfdcEVC8E43cpxpj9WfzWJLy8yRQWzD5StoRm6JLqjLNCbfyKsDEmqX3Lhh"
IsGuest= 0  #dont touch this

####### Finish modifiable part ########################################################################################

# Data cleaning
TXin_OpReturnScript = Script.from_raw(TXin_OpReturnScript_inHex).get_script()
P2shRedeem = TXin_OpReturnScript [1:-1]
key = TXin_OpReturnScript[18]

if TXin_OpReturnScript[18] in OP_CODES:
    index = int.from_bytes(OP_CODES[TXin_OpReturnScript[18]], "little")- 80
else: 
    index= int(TXin_OpReturnScript[18], 16)
 
# Derivative Path
inPath="m/8/"+ str(index)
outPath="m/8/"+ str(index+1)

if IsGuest:
    seqInScript = TXin_OpReturnScript[1]
else:
    seqInScript = TXin_OpReturnScript[9]

if seqInScript in OP_CODES:
    op_csv = int.from_bytes(OP_CODES[seqInScript], "little")- 80
else: 
    op_csv= int(seqInScript, 16)



def main():
    # always remember to setup the network
    setup('testnet')
    
    # 0- Prepare the key 
    hdwInPath  = HDWallet(Xprv, inPath)
    prvInPath  = hdwInPath.get_private_key()
    pubInPath  = prvInPath.get_public_key()
    addrInPath = prvInPath.get_public_key().get_address()

    hdwOutPath  = HDWallet(Xprv, outPath)
    prvOutPath  = hdwOutPath.get_private_key()
    pubOutPath  = prvOutPath.get_public_key()
    addrOutPath = prvOutPath.get_public_key().get_address()
    
    # 1- P2SH_Redeem  Script 
    redeemScript = Script(P2shRedeem)
    scriptAddress =  P2shAddress.from_script(redeemScript)

    print('Redeem script:', redeemScript.get_script())
    print('Redeem script in HEX:', redeemScript.to_hex())
    print('Redeem script address hex: ', scriptAddress.to_string() )
    print('Redeem script address hash160:', scriptAddress.to_hash160() )
   
    # 2- Create Change TxOut
    changeAddr = P2pkhAddress(addrOutPath.to_string())
    change_txout = TxOutput(to_satoshis(0), changeAddr.to_script_pub_key() )

  
    opReturnScript= Script( ['OP_RETURN', index] )
    OPRUTURN_txout = TxOutput(  to_satoshis(0), opReturnScript) 
    print('OP_Return (NewIndex):', opReturnScript.get_script())

    

    # 3- Create TxIn
    seq = Sequence(TYPE_RELATIVE_TIMELOCK, int(op_csv))
    seq_for_n_seq = seq.for_input_sequence()
    assert seq_for_n_seq is not None

    txin = TxInput(TxIn_id , TxIn_P2sh_index, sequence=seq_for_n_seq)

  

    # 4- Form the TX
    tx = Transaction([txin], [OPRUTURN_txout, change_txout])

    # 5- Sign the TX
    sig = prvInPath.sign_input(tx, 0, redeemScript)
    txin.script_sig = Script([sig, pubInPath.to_hex() , redeemScript.to_hex()])
    signed_tx = tx.serialize()

    print('\nscript_sig script           :', txin.script_sig.get_script())
    print('\nGuest address InPath :', addrInPath.to_string() )
    print('Guest address OutPath:', addrOutPath.to_string() ) 
    
    print('Public key InPath    :', pubInPath.to_hex()) 
    print("\nRaw signed transaction:\n" + signed_tx)
    print("\nTxId:", tx.get_txid())

 
if __name__ == "__main__":
    main()
