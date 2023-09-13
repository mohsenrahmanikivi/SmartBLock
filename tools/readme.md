## Tools

In this folder, you can find several tools, in order to sync, manage, and operate the smart lock. All tools are listed here. Follow the instructions to use each tool.
1. exampleKeys.key
2. headerDownloader.py
3. ownerApp.py
4. scriptDecoder.py
  
###  1. exampleKeys.key
This is the list of keys used in this example including:
- Owner keys: The owner is responsible for creating the contract and giving the guest access to unlock the lock for the chosen period
- Guest keys: The Guest is a person who wants to unlock the lock by creating a transaction.
- Lock Keys: The lock is responsible for receiving contracts and as each contract will be spent by unlocking transactions of guests so the lock should generate and propagate a new contract after this procidure.

###  2. headerDownloader.py
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


###  3. ownerApp.py
This tool generates a contract in order to enable smart lock functionality. The contract is a transaction with three outputs :
Output1.  an output with index 0 and type P2SH based on the address of a script. (The script contains the guest address and owner address with sequence lock for each address. and if the guest manages to spend this output it causes an unlock operation and if the owner manages to do that it means revoking the contract)
Output2. an output with index 1 and type NULLDATA containing the script.
Output3. a Change output which refunds the fund.

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
Redeem script                : [1, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUAL', 'OP_NOTIF', 15, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_ENDIF', 'OP_CHECKSIG']
Redeem script in HEX         : 51b27576a914508bc12259f07e979e752ffc315981a7c9d326e487645fb27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c98868ac
Redeem script address hex    : 2N9hFAMTwNx7ioYMqrSERtDqHaeLqHCBYRb
Redeem script address hash160: b46dcea6bf2b2d62823461a3acd22ca35661d805

Raw signed transaction:
0200000001431174f1cc73e9d251a4571477e8181ee3bc7c7287c61a1b7cde4c03501a831f020000006a47304402201d01185ac55347b5c9937912581f09125eaafdfbe6815b3c05a2cd82abefead20220433249ed9462f8f72fa689bae29edb3c260066421bb04ae2eee7333e9ea1132a012103c6fa60d51063013677f547120b979b5a2163afcdb448a03551ca1580039b0eb3ffffffff032c0100000000000017a914b46dcea6bf2b2d62823461a3acd22ca35661d8058700000000000000003a6a51b27576a914508bc12259f07e979e752ffc315981a7c9d326e487645fb27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c98868ac9e050000000000001976a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac00000000

Transaction ID:
7017e4e56da713b3c3309e29724dfa06b8b5be9ad4f9b68369ff5dcfbc511209
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
Redeem script: [1, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUAL', 'OP_NOTIF', 15, 'OP_CHECKSEQUENCEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_ENDIF', 'OP_CHECKSIG']
Redeem script in HEX: 51b27576a914508bc12259f07e979e752ffc315981a7c9d326e487645fb27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c98868ac
Redeem script address hex:  2N9hFAMTwNx7ioYMqrSERtDqHaeLqHCBYRb
Redeem script address hash160: b46dcea6bf2b2d62823461a3acd22ca35661d805

script_sig script           : ['304402200e20aa975b68f0505699c848252b6e53f21c24d490af9803d263c750a4e114a002205725d9a930a5186ee5dbed97e55c26ed279e4bdb76605ddb243a6278a22d0e3201', '028dde0415eeb0df62c7f5dd8160ef27549f4646724aafdf03b521cece1ab9cbb7', '51b27576a914508bc12259f07e979e752ffc315981a7c9d326e487645fb27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c98868ac']

Guest address(signer) base58: mnrqmi3gyEavAtv1hFMxqWjRcSzvPzJBnm
Guest address(signer) hash160 : 508bc12259f07e979e752ffc315981a7c9d326e4
Public key                    : 028dde0415eeb0df62c7f5dd8160ef27549f4646724aafdf03b521cece1ab9cbb7

Raw signed transaction:
0200000001091251bccf5dff6983b6f9d49abeb5b806fa4d72299e30c3b313a76de5e4177000000000a447304402200e20aa975b68f0505699c848252b6e53f21c24d490af9803d263c750a4e114a002205725d9a930a5186ee5dbed97e55c26ed279e4bdb76605ddb243a6278a22d0e320121028dde0415eeb0df62c7f5dd8160ef27549f4646724aafdf03b521cece1ab9cbb73951b27576a914508bc12259f07e979e752ffc315981a7c9d326e487645fb27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c98868ac010000000100000000000000001976a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac00000000

TxId: 4b689bb96335eddf0129b6da8446be56cab0600f510da2fe48483440494a0b7a
```
 - Then you can copy the "Raw signed transaction" and propagate it with the help of your BitcoinCore node with this command (sendrawtransaction) or an online tool like this
   https://live.blockcypher.com/btc/pushtx/
- If the transaction is accepted by the network the lock executes the unlock operation. then the lock will send a new contract to the network with the address of the guest.

##  4. scriptDecoder.py
This application decodes your script to the human-readable OP Codes. Just open the file with your text editor replace the example script with your script and run by this command to see the result.
```sh
$ python {your_work_space_path}/uSmartLock/tools/scriptDecoder.py
```
  
