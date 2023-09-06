# requiment 
#$ pip install bitcoin-utils

from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput
from bitcoinutils.keys import P2pkhAddress , P2shAddress
from bitcoinutils.script import Script


#[CHANGE with your data]Contract detailes including RedeemScript and TxIN ID + TxIn Index
TxIn_id= "c6af4bb99d252efe8d84d33cb0abc12756ea07391f54ac8bc513f9e583896bb0"
TxIn_index= 0
P2shRedeem= ['OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUAL', 
             'OP_IF', 'OP_CHECKSIG', 
             'OP_ELSE', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 
             'OP_ENDIF'
            ]
            

#[CHANGE with your data]Guest Private Key to signe the transaction
guestXprv="tprv8ZgxMBicQKsPf8FCmuNKDHobFZRFq6vM7Gt5mE2s9hnS71vNcN6fJbb7KDXdahL7sZNqoyPktfkRdjVUw8v7aJRxw6Yp96tmjWzQKTCpTRo"
derivePath="m/0"

#[CHANGE with your data]Owner address to recive the change
ownerAddr="mjBY9nEHqQsSVhy4J821vZQNL1sot71JZv"




#
def main():
    # always remember to setup the network
    setup('testnet')

    # 1- P2SH_Redeem  Script + add OP_1 
   # redeemScript = Script.from_raw(P2shRedeemInHex)
    redeemScript = Script(P2shRedeem)
    print('Redeem script:', redeemScript.get_script())
    print('Redeem script in HEX:', redeemScript.to_hex())
    scriptAddress =  P2shAddress.from_script(redeemScript)
    print('Redeem script address hex: ', scriptAddress.to_string() )
    print('Redeem script address hash160:', scriptAddress.to_hash160() )
   
    # 2- Create Change TxOut
    changeAddr = P2pkhAddress(ownerAddr)
    change_txout = TxOutput(to_satoshis(0), changeAddr.to_script_pub_key() )

    
    # 3- Create TxIn
    txin = TxInput(TxIn_id , TxIn_index)
   

    # 4- Prepare the key 
    hdw  = HDWallet(guestXprv, derivePath)
    prv  = hdw.get_private_key()
    pub  = prv.get_public_key()
    addr = prv.get_public_key().get_address()

    # 5- Form the TX
    tx = Transaction([txin], [change_txout])
    
    # 6- Sign the TX
    sig = prv.sign_input(tx, 0, redeemScript)
    txin.script_sig = Script([sig, pub.to_hex() , redeemScript.to_hex()])
    signed_tx = tx.serialize()

    print('\nscript_sig script           :', txin.script_sig.get_script())
    print('\nGuest address(signer) base58:', addr.to_string() ) 
    print('Guest address(signer) hash160 :', addr.to_hash160() )
    print('Public key                    :', pub.to_hex()) 
    print("\nRaw signed transaction:\n" + signed_tx)
    print("\nTxId:", tx.get_txid())

 

if __name__ == "__main__":
    main()
