## Tools

In this folder, you can find several tools, in order to sync, manage, and operate the smart lock. All tools are listed here. Follow the instructions to use each tool.
- exampleKeys.key
- headerDownloader.py
- ownerApp.py
  
###  exampleKeys.key
This is the list of keys used in this example including:
- Owner keys: The owner is responsible for creating the contract and giving the guest access to unlock the lock for the chosen period
- Guest keys: The Guest is a person who wants to unlock the lock by creating a transaction.
- Lock Keys: The lock is responsible for receiving contracts and as each contract will be spent by unlocking transactions of guests so the lock should generate and propagate a new contract after this procidure.

###  headerDownloader.py
This tool is designed to download headers with a data structure that is readable for the smart lock. (As downloading the header by smart lock takes time with this tool you can make your smart lock ready to use as fast as possible).

#### How to use
- First, this tool needs to connect to a bitcoin-core node, so before using this tool install and enable the bitcoin-core client (we use testnet in our implementation, as we mentioned in the uSmartLock readme)
- Open the "headerDownloader.py" file and edit the server and port in the file with the server and port of your Bitcoin core client.
- Have installed Python 3 on your environment.
- Run this command
```sh
 $ python {your_work_space_path}/uSmartLock/tools/headerDownloader.py
```
- When the download is finished you have a main folder named "HEADERS" You need to move this folder to the SD memory of the smartlock.


###  ownerApp.py
This tool generates a contract in order to enable smart lock functionality. The contract is a transaction with three outputs :
Output1-  an output with index 0 and type P2SH based on the address of a script. (The script contains the guest address and owner address with sequence lock for each address. and if the guest manages to spend this output it causes an unlock operation and if the owner manages to do that it means revoking the contract)
Output2- an output with index 1 and type NULLDATA containing the script.
Output3- a Change output which refunds the fund.

#### How to use
This tool gives you a raw transaction that you need to somehow propagate to the Bitcoin network (testnet). (you can use whatever tools to propagate this transaction)
- Having installed python
- Having installed bitcoin-utils  [GIT](https://github.com/karask/python-bitcoin-utils/tree/master)
```sh
$ pip install bitcoin-utils
```
- Open the file "ownerApp.py" with your text editor
- You need to define the Owner's private key with derivation path then a desired UTXO (TxIn (id, index) )
- You should define the address and sequence lock of the guest and the owner (a sequence is a number that shows how many blocks the output needs to get age to be spendable) 
- Save the file
- Run this command
```sh
 $ python {your_work_space_path}/uSmartLock/tools/ownerApp.py
```
- The result should be like this

```sh
Owner  address               : mjBY9nEHqQsSVhy4J821vZQNL1sot71JZv
Redeem script                : [1, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUAL', 'OP_IF', 'OP_CHECKSIG', 'OP_ELSE', 1, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']
Redeem script in HEX         : 51b27576a914508bc12259f07e979e752ffc315981a7c9d326e48763ac6751b27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac68
Redeem script address hex    : 2N5sEbYuoobSJMNwpXBu3823YfZwZgxAboA
Redeem script address hash160: 8a70fdd9eae34ea0acfa4bcdde98a0344b47ccd9

Raw signed transaction:
02000000010be2cdbe8a6a2dede813cc0d2ebe92014a9fd7e7ccba22f1b5186308c72de630020000006a47304402206147678c67b500e74624c616b32d980c908c4b7adb79f0f526889d8a119e0c6e02203aa1ac92ac3768031cc1e4a2ffe58b618a0fad1e0bb8d59c8fdae351417afd20012103c6fa60d51063013677f547120b979b5a2163afcdb448a03551ca1580039b0eb3ffffffff032c0100000000000017a9148a70fdd9eae34ea0acfa4bcdde98a0344b47ccd98700000000000000003c6a51b27576a914508bc12259f07e979e752ffc315981a7c9d326e48763ac6751b27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac684e0a0000000000001976a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac00000000

Transaction ID:
d15a617bf42e195973a26e75af980e20e9d0f04ae2ba08c98bd0eea3aa4a3784
```
 - Then you can copy the "Raw signed transaction" and propagate it with the help of your BitcoinCore node with this command (sendrawtransaction) or an online tool like this
   https://live.blockcypher.com/btc/pushtx/
- If the transaction is accepted by the network, so your contract is assigned to the lock.

###  guestApp.py 
This tool generates a raw transaction in order to spend the P2SH output of the contract. In this way, the lock executes the unlock operation.

#### How to use
- Having installed python
- Having installed bitcoin-utils [GIT](https://github.com/karask/python-bitcoin-utils/tree/master)
```sh
$ pip install bitcoin-utils
```
- Open the file "guestApp.py" with your text editor
- You need to define the TxIn which is an ID and Index of the Contract transaction (P2SH unspend output).
- You should define the private key and derivation path of the guest. 
- Save the file
- Run this command
```sh
 $ python {your_work_space_path}/uSmartLock/tools/guestApp.py
```
- The result should be like this
```sh
Redeem script: [1, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUAL', 'OP_IF', 'OP_CHECKSIG', 'OP_ELSE', 1, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']
Redeem script in HEX: 51b27576a914508bc12259f07e979e752ffc315981a7c9d326e48763ac6751b27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac68
Redeem script address hex:  2N5sEbYuoobSJMNwpXBu3823YfZwZgxAboA
Redeem script address hash160: 8a70fdd9eae34ea0acfa4bcdde98a0344b47ccd9

script_sig script           : ['3044022034e735feb3b9dc014ca2e54efcb0bc28bfbc64cf99348baa10f02677a35eef7502200d8115796a939e3faa637f5ec03ef01eb8abc9721ac0e20c51e0183379cd3e5901', '028dde0415eeb0df62c7f5dd8160ef27549f4646724aafdf03b521cece1ab9cbb7', '51b27576a914508bc12259f07e979e752ffc315981a7c9d326e48763ac6751b27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac68']

Guest address(signer) base58: mnrqmi3gyEavAtv1hFMxqWjRcSzvPzJBnm
Guest address(signer) hash160 : 508bc12259f07e979e752ffc315981a7c9d326e4
Public key                    : 028dde0415eeb0df62c7f5dd8160ef27549f4646724aafdf03b521cece1ab9cbb7

Raw signed transaction:
02000000010be2cdbe8a6a2dede813cc0d2ebe92014a9fd7e7ccba22f1b5186308c72de63000000000a6473044022034e735feb3b9dc014ca2e54efcb0bc28bfbc64cf99348baa10f02677a35eef7502200d8115796a939e3faa637f5ec03ef01eb8abc9721ac0e20c51e0183379cd3e590121028dde0415eeb0df62c7f5dd8160ef27549f4646724aafdf03b521cece1ab9cbb73b51b27576a914508bc12259f07e979e752ffc315981a7c9d326e48763ac6751b27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac68010000000100000000000000001976a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac00000000

TxId: 5a11d1e72604fc6bf4edf25561fc3cc018f89b431eae6fe1e77fbacaaf7f09d7
```
 - Then you can copy the "Raw signed transaction" and propagate it with the help of your BitcoinCore node with this command (sendrawtransaction) or an online tool like this
   https://live.blockcypher.com/btc/pushtx/
- If the transaction is accepted by the network the lock executes the unlock operation. then the lock will send a new contract to the network with the address of the guest.
