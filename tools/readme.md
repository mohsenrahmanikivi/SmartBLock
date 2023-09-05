## Tools

In this folder, you can find several tools, in order to sync, manage, and operate the smart lock. All tools are listed here. Follow the instructions to use each tool.

###  Header Downloader
This tool is designed to download headers with a data structure that is readable for the smart lock. (As downloading the header by smart lock takes time with this tool you can make your smart lock ready to use as fast as possible).

#### How to use
- First, this tool needs to connect to a bitcoin-core node, so before using this tool install and enable the bitcoin-core client (we use telnet in our implementation) as we mentioned in the uSmartLock readme.
- Open the .py file and edit the server and port in the file with the server and port of your Bitcoin core client.
- Have installed Python on your environment.
- Run this command
```sh
  python {your_work_space_path}/uSmartLock/tools/headerDownloader.py
```
###  Guest app
This tool generates a raw transaction in order to spend A P2SH output of the contract. In this way, the lock will be unlocked........................UNDONE

#### How to use
- Having installed python
- Having installed bitcoin-utils
  https://github.com/karask/python-bitcoin-utils/tree/master
```sh
$ pip install bitcoin-utils
```
  


###  Owner tool
This tool generates a contract in order to enable smart lock functionality...................................UNDONE.
.
#### How to use
- Having installed python
- Having installed bitcoin-utils
  https://github.com/karask/python-bitcoin-utils/tree/master
```sh
$ pip install bitcoin-utils
```
-Open the file "ownerApp.py" with your text editor
- You need to define the Owner's private key + a Txin
- You should define lock and guest addresses
- Save the file
- Run this command
```sh
  python {your_work_space_path}/uSmartLock/tools/ownerApp.py
```
- The result should be like this

```sh
Redeem script:
 ['OP_IF', '9cb3dc6a', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ELSE', 'ec76dd6a', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']

OP_Return script:
 ['OP_RETURN', 'OP_IF', '9cb3dc6a', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ELSE', 'ec76dd6a', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']

Raw unsigned transaction:
020000000191e97536fdf6236dfd2c95ce4873b9951b6b20f8f65bed2dea710594428a51c50100000000ffffffff035e0100000000000017a914c681204bb2b3ec6a438ce95d0f701bc252ef4fd7870000000000000000446a63049cb3dc6ab17576a914508bc12259f07e979e752ffc315981a7c9d326e488ac6704ec76dd6ab17576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac68381e0000000000001976a914f93dd4863073e9427eb63ff000069973dad4226288ac00000000

Owner  address:
 mjBY9nEHqQsSVhy4J821vZQNL1sot71JZv

Raw signed transaction:
020000000191e97536fdf6236dfd2c95ce4873b9951b6b20f8f65bed2dea710594428a51c5010000006a47304402207b5face5df1334c8fbebb44e93e0efd7648f947f7ef9f50d24189a4837c843e6022061fb53d4da93b6943af5b721959a0d3cd5dce3fc525127096b39008f9e314f14012103c6fa60d51063013677f547120b979b5a2163afcdb448a03551ca1580039b0eb3ffffffff035e0100000000000017a914c681204bb2b3ec6a438ce95d0f701bc252ef4fd7870000000000000000446a63049cb3dc6ab17576a914508bc12259f07e979e752ffc315981a7c9d326e488ac6704ec76dd6ab17576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac68381e0000000000001976a914f93dd4863073e9427eb63ff000069973dad4226288ac00000000

Signed transaction size (in bytes):
300
```
 - Then you can copy the raw transaction and propagate it with the help of your Bitcoin node (sendrawtransaction) or an online tool like this
   https://live.blockcypher.com/btc/pushtx/
