from hdwallet import HDWallet as HDW
from hdwallet.symbols import BTCTEST

from bitcoinutils.setup import setup
from bitcoinutils.hdwallet import HDWallet
from bitcoinutils.utils import to_satoshis
from bitcoinutils.transactions import Transaction, TxInput, TxOutput, Sequence, Locktime
from bitcoinutils.keys import P2pkhAddress , P2shAddress
from bitcoinutils.script import Script, OP_CODES
from bitcoinutils.constants import TYPE_ABSOLUTE_TIMELOCK


########[CHANGE by your data] 
# the latest contract TX ID and Index
TxIn_P2sh_id= "892d79d9890fd0b2d02e85f2a14f1fe8e67fef8e8d5af9fa9d4bab1f06d72113"
TxIn_P2sh_index= 0

# the latest DerivativePath and Path_index 
DerivativePath="m/24/"
Path_Index= 1 

# Guest extended private key
guestXpub = "tpubD6NzVbkrYhZ4YbGzfZ2uchThpawBzS7FgaUs3k5AZyapwWB9EkvFV6CyVNT2JWCqACyhWDiHfdityAZajDoQmh9uEPakg1LPV5TXvbpaZFs"
# Owner extended public key
ownerXpub = "tpubD6NzVbkrYhZ4WmpygA4emwLn7ZZWCPFX58GjZ1XTmvkTA4LFC4TCQyozQCc12TcKUqrS83sp5KyNBkyxmrPM68SCx7dgfsBwD7QSt1U9LAT"
ownerXprv="tprv8ZgxMBicQKsPdJoBnWQ4NXgfYY3a344cVpfxGVVAMex4Ka5UZfdcEVC8E43cpxpj9WfzWJLy8yRQWzD5StoRm6JLqjLNCbfyKsDEmqX3Lhh"

# Lock extended public key
lockXpub = "tpubD6NzVbkrYhZ4YDpCdJW8s3ihthS5xpojJLVr5Mx82teeeo8oAcs62YLqPYzzGtXQYA7CTNgsV58mxD9R2zkHePtKVTj1cMaBXR1UbHXAdXt"


# value >= op_cltv , value <= current hight
guest_timelock = 2505655 #should be the same as contract 
owner_timeLock = 2505655 #should be the same as contract 

####### Finish modifiable part ########################################################################################

# Data cleaning

# Derivative Path
guestPath = DerivativePath + str(Path_Index)
revokePath= DerivativePath + str(Path_Index+1)

ownerPath= DerivativePath + '0'

def main():
    # always remember to setup the network
    setup('testnet')

    # Guest 
    guestHDW  = HDW(symbol=BTCTEST)
    guestHDW.from_xpublic_key(guestXpub)
    guestHDW.from_path(guestPath)
    guestAddrBase58 = guestHDW.p2pkh_address()
    guestAddr = P2pkhAddress(guestAddrBase58)

    #  Owner 
    ownerHDW  = HDWallet( ownerXprv, ownerPath)
    ownerPrv  =  ownerHDW.get_private_key()
    ownerPub  =  ownerPrv.get_public_key()
    ownerAddr =  ownerPrv.get_public_key().get_address()

    #  Lock 
    lockHDW = HDW(symbol=BTCTEST)
    lockHDW.from_xpublic_key(lockXpub)
    lockHDW.from_path(revokePath)
    lockAddrBase58 = lockHDW.p2pkh_address()
    lockAddr = P2pkhAddress(lockAddrBase58)

         
    p2shRedeem=[
                guest_timelock , 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', guestAddr.to_hash160(), 'OP_EQUAL'      , 
                'OP_NOTIF'           , 
                owner_timeLock , 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', ownerAddr.to_hash160(), 'OP_EQUALVERIFY', 
                'OP_ENDIF' , 'OP_CHECKSIG'
                ]
    

    
    # 1- P2SH_Redeem  Script 
    redeemScript = Script(p2shRedeem)
    scriptAddress =  P2shAddress.from_script(redeemScript)
   
    # 2- Create Change TxOut
    changeAddr = P2pkhAddress(lockAddr.to_string())
    change_txout = TxOutput(to_satoshis(0), changeAddr.to_script_pub_key() )

    

    # 3- Create TxIn
    seq = Sequence(TYPE_ABSOLUTE_TIMELOCK, ) # yes there is a "," and it is empthu after that
    txin = TxInput(TxIn_P2sh_id , TxIn_P2sh_index, sequence = seq.for_input_sequence())


    # 4- Form the TX
    lock_time= Locktime(guest_timelock)
    tx = Transaction([txin], [change_txout], locktime = lock_time.for_transaction())

    # 5- Sign the TX
    sig = ownerPrv.sign_input(tx, 0, redeemScript)
    txin.script_sig = Script([sig, ownerPub.to_hex() , redeemScript.to_hex()])
    signed_tx = tx.serialize()

    
    

    print('Owner address :', ownerAddr.to_string() )
    print('Guest address :', guestAddr.to_string() )
    print('Lock address  :', lockAddr.to_string() ) 
    print('Redeem script :', redeemScript.get_script())
    print('Redeem script in hex :', redeemScript.to_hex())
    scriptAddress =  P2shAddress.from_script(redeemScript)
    print('P2SH script address in HEX :', scriptAddress.to_string() )

    
    print("\nRaw signed transaction:\n" + signed_tx)
    print("\nTxId:", tx.get_txid())

 
if __name__ == "__main__":
    main()