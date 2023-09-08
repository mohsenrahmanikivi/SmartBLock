## Tools

In this folder, you can find several tools, in order to sync, manage, and operate the smart lock. All tools are listed here. Follow the instructions to use each tool.
###  Example Keys
This is the list of keys used in this example including:
- Owner keys: The owner is responsible for creating the contract and giving the guest access to unlock the lock for the chosen period
- Guest keys: The Guest is a person who wants to unlock the lock by creating a transaction.
- Lock Keys: The lock is responsible for receiving contracts and as each contract will be spent by unlocking transactions of guests so the lock should generate and propagate a new contract after this procidure.

###  Header Downloader
This tool is designed to download headers with a data structure that is readable for the smart lock. (As downloading the header by smart lock takes time with this tool you can make your smart lock ready to use as fast as possible).

#### How to use
- First, this tool needs to connect to a bitcoin-core node, so before using this tool install and enable the bitcoin-core client (we use testnet in our implementation, as we mentioned in the uSmartLock readme)
- Open the "headerDownloader.py" file and edit the server and port in the file with the server and port of your Bitcoin core client.
- Have installed Python 3 on your environment.
- Run this command
```sh
 $ python {your_work_space_path}/uSmartLock/tools/headerDownloader.py
```
- When the download is finished you have a main folder named "HEADERS" you need to move this folder to the SD memory of the smartlock.


###  Owner App
This tool generates a contract in order to enable smart lock functionality. The contract is a transaction with three outputs :
1- First, an output with index 0 and type P2SH based on the address of a script. (The script contains the guest address and owner address with sequence lock for each address. and if the guest manages to spend this output it causes an unlock operation and if the owner manages to do that it means revoking the contract)
2- Second, an output with index 1 and type NULLDATA containing the script.
3- Third, a Change output which refunds the fund.

.
#### How to use
This tool gives you a raw transaction that you need to somehow propagate to the network (testnet).
- Having installed python
- Having installed bitcoin-utils
  https://github.com/karask/python-bitcoin-utils/tree/master
```sh
$ pip install bitcoin-utils
```
- Open the file "ownerApp.py" with your text editor
- You need to define the Owner's private key and a desired UTXO (TxIn (id, index) )
- You should define the sequence of guest and owner (a sequence is a number which shows how many blocks the output needs to get age to be spendable) 
- Save the file
- Run this command
```sh
 $ python {your_work_space_path}/uSmartLock/tools/ownerApp.py
```
- The result should be like this

```sh
Owner  address               : mjBY9nEHqQsSVhy4J821vZQNL1sot71JZv
Redeem script                : [1, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUAL', 'OP_IF', 'OP_CHECKSIG', 'OP_ELSE', 1, 'OP_CHECKSEQUENCEVERIFY', 'OP_Redeem script in HEX         : 51b27576a914508bc12259f07e979e752ffc315981a7c9d326e48763ac6751b27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac68
Redeem script address hex    : 2N5sEbYuoobSJMNwpXBu3823YfZwZgxAboA
Redeem script address hash160: 8a70fdd9eae34ea0acfa4bcdde98a0344b47ccd9

Raw signed transaction:
0200000001d930d68b52689bd9ce98b6edc641d522864c253233c52df89754bf3523294ffd020000006a47304402202eda05558a8e23617c147049d85085cb469bf4ee06195181388c3588589f6a75022032cb36a1970de6b68828af903e53f076d6b07b7b9a653005d7e7c4f4337633dc012103c6fa60d51063013677f547120b979b5a2163afcdb448a03551ca1580039b0eb3ffffffff032c0100000000000017a9148a70fdd9eae34ea0acfa4bcdde98a0344b47ccd98700000000000000003c6a51b27576a914508bc12259f07e979e752ffc315981a7c9d326e48763ac6751b27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac68a60c0000000000001976a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac00000000

Transaction ID:
30e62dc7086318b5f122bacce7d79f4a0192be2e0dcc13e8ed2d6a8abecde20b
```
 - Then you can copy the raw transaction and propagate it with the help of your Bitcoin node command (sendrawtransaction) or an online tool like this
   https://live.blockcypher.com/btc/pushtx/

###  Guest app ..............................................UNDONE..................................
This tool generates a raw transaction in order to spend the P2SH output of the contract. In this way, the lock will be unlocked.

#### How to use
- Having installed python
- Having installed bitcoin-utils
  https://github.com/karask/python-bitcoin-utils/tree/master
```sh
$ pip install bitcoin-utils
```
