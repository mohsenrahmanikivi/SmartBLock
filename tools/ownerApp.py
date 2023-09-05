# requiment 
#$ pip install bitcoin-utils

from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput
from bitcoinutils.keys import P2pkhAddress
from bitcoinutils.script import Script


#Owner Private Key to sign the transaction
guestXprv="tprv8ZgxMBicQKsPdJoBnWQ4NXgfYY3a344cVpfxGVVAMex4Ka5UZfdcEVC8E43cpxpj9WfzWJLy8yRQWzD5StoRm6JLqjLNCbfyKsDEmqX3Lhh"
derivePath="m/0"

#Owner address to receive the change
ownerAddr="mjBY9nEHqQsSVhy4J821vZQNL1sot71JZv"

#Contract details including RedeemScript and TxIN ID + TxIn Index
P2shRedeem="['OP_IF', '9cb3dc6a', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ELSE', 'ec76dd6a', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']"
TxIn_id= "c5518a42940571ea2ded5bf6f8206b1b95b97348ce952cfd6d23f6fd3675e991"
TxIn_index=1 

#
def main():
    # Always remember to setup the network
    setup('testnet')

    # 1- P2SH_Redeem  Script + add OP_1 
    redeemScript = Script.from_raw(P2shRedeemInHex)
    redeemScript=Script(['OP_1', redeemScript.to_hex()])

    print('Redeem script:', redeemScript.get_script())
   
   
    # 2- Create TxOut
    addr = P2pkhAddress(ownerAddr)
    txout = TxOutput(to_satoshis(0), addr.to_script_pub_key() )

    
    # 3- Create TxIn
    txin = TxInput(TxIn_id , TxIn_index)
   

    # 4- Prepare the key 
    hdw = HDWallet(guestXprv, derivePath)
    privkey = hdw.get_private_key()
    pubkey = privkey.get_public_key()
    addr1 = pubkey.get_address()
    print('Guest  address:', addr1.to_string() )

    # 5- Form the TX
    tx = Transaction([txin], [txout])
    
    # 6- Sign the TX
    sig = privkey.sign_input(tx, 0, redeemScript )

    
    txin.script_sig = Script([sig, redeemScript.to_hex()])
    signed_tx = tx.serialize()

    # print raw signed transaction ready to be broadcasted
    print("\nRaw signed transaction:\n" + signed_tx)
    print("\nTxId:", tx.get_txid())

 

if __name__ == "__main__":
    main()
