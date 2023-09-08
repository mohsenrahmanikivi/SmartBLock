from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput, Sequence
from bitcoinutils.keys import P2pkhAddress , P2shAddress
from bitcoinutils.script import Script
from bitcoinutils.constants import TYPE_RELATIVE_TIMELOCK


#[CHANGE1 with your data]### Contract detailes including RedeemScript and TxIN ID + TxIn Index
TxIn_id= "30e62dc7086318b5f122bacce7d79f4a0192be2e0dcc13e8ed2d6a8abecde20b"
TxIn_index= 0
P2shRedeem=  [1, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUAL', 'OP_IF', 'OP_CHECKSIG', 'OP_ELSE', 1, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']

#[CHANGE2 with your data]### Guest Private Key to signe the transaction
#guest to send unlock tx 
Xprv="tprv8ZgxMBicQKsPf8FCmuNKDHobFZRFq6vM7Gt5mE2s9hnS71vNcN6fJbb7KDXdahL7sZNqoyPktfkRdjVUw8v7aJRxw6Yp96tmjWzQKTCpTRo"
op_csv=P2shRedeem[0]
derivePath="m/0"
#owner to revoke the contract
# Xprv="tprv8ZgxMBicQKsPdJoBnWQ4NXgfYY3a344cVpfxGVVAMex4Ka5UZfdcEVC8E43cpxpj9WfzWJLy8yRQWzD5StoRm6JLqjLNCbfyKsDEmqX3Lhh"
# op_csv=P2shRedeem[10]
# derivePath="m/0"

#[CHANGE3 with your data]### Owner address to receive the change
ownerAddr="mjBY9nEHqQsSVhy4J821vZQNL1sot71JZv"




def main():
    # always remember to setup the network
    setup('testnet')
   
    # spend_after_n_block set when this tx is will be accepted by the network
    spend_after_n_block = op_csv
    if  spend_after_n_block < op_csv :
        print('spend_after_n_block should be bigger or equal than op_csv of UTXO that you are going to spend')
        print('spend_after_n_block =', spend_after_n_block, 'op_csv =', op_csv)
        while 1 :
            pass 

    # 1- P2SH_Redeem  Script 
    redeemScript = Script(P2shRedeem)
    scriptAddress =  P2shAddress.from_script(redeemScript)

    print('Redeem script:', redeemScript.get_script())
    print('Redeem script in HEX:', redeemScript.to_hex())
    print('Redeem script address hex: ', scriptAddress.to_string() )
    print('Redeem script address hash160:', scriptAddress.to_hash160() )
   
    # 2- Create Change TxOut
    changeAddr = P2pkhAddress(ownerAddr)
    change_txout = TxOutput(to_satoshis(0), changeAddr.to_script_pub_key() )

    # 3- Create TxIn
    seq = Sequence(TYPE_RELATIVE_TIMELOCK, spend_after_n_block)
    seq_for_n_seq = seq.for_input_sequence()
    assert seq_for_n_seq is not None

    txin = TxInput(TxIn_id , TxIn_index, sequence=seq_for_n_seq)

    # 4- Prepare the key 
    hdw  = HDWallet(Xprv, derivePath)
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
