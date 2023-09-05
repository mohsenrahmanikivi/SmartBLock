# requiment 
#$ pip install bitcoin-utils

from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput
from bitcoinutils.keys import P2pkhAddress
from bitcoinutils.script import Script


#Guest Private Key to sign the transaction
guestXprv="tprv8ZgxMBicQKsPf8FCmuNKDHobFZRFq6vM7Gt5mE2s9hnS71vNcN6fJbb7KDXdahL7sZNqoyPktfkRdjVUw8v7aJRxw6Yp96tmjWzQKTCpTRo"
derivePath="m/0"

#Owner address to receive the change
ownerAddr="mjBY9nEHqQsSVhy4J821vZQNL1sot71JZv"

#Contract details including RedeemScript and TxIN ID + TxIn Index
P2shRedeemInHex="63049cb3dc6ab17576a914508bc12259f07e979e752ffc315981a7c9d326e488ac6704ec76dd6ab17576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac68"
TxIn_id= "7f74f5426774fc3270b9864cf1cf7050470ed01dbe307789f62b9a12b596b64d"
TxIn_index= 0

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
