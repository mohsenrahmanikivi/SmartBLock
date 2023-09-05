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
 $ python {your_work_space_path}/uSmartLock/tools/headerDownloader.py
```
###  Guest app
This tool generates a raw transaction in order to spend A P2SH output of the contract. In this way, the lock will be unlocked........................UNDONE...................

#### How to use
- Having installed python
- Having installed bitcoin-utils
  https://github.com/karask/python-bitcoin-utils/tree/master
```sh
$ pip install bitcoin-utils
```
  


###  Owner tool
This tool generates a contract in order to enable smart lock functionality
.
#### How to use
- Having installed python
- Having installed bitcoin-utils
  https://github.com/karask/python-bitcoin-utils/tree/master
```sh
$ pip install bitcoin-utils
```
- Open the file "ownerApp.py" with your text editor
- You need to define the Owner's private key and a desired UTXO (TxIn (id, index) )
- You should define lock and guest addresses and lock time based on the Unix time
- Save the file
- Run this command
```sh
 $ python {your_work_space_path}/uSmartLock/tools/ownerApp.py
```
- The result should be like this

```sh
Owner  address:
 mjBY9nEHqQsSVhy4J821vZQNL1sot71JZv

Redeem script:
 ['OP_IF', 'ed12f264', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ELSE', '1c3c4e64', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']

OP_Return script:
 ['OP_RETURN', 'OP_IF', 'ed12f264', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '508bc12259f07e979e752ffc315981a7c9d326e4', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ELSE', '1c3c4e64', 'OP_CHECKLOCKTIMEVERIFY', 'OP_DROP', 'OP_DUP', 'OP_HASH160', '2834725421aa6f0dd62ef3f71ca88cb72c4ef3c9', 'OP_EQUALVERIFY', 'OP_CHECKSIG', 'OP_ENDIF']

Raw unsigned transaction:
02000000019fb689ff79eaa4ff004cc8c33cbfcb01417001e18ec0d08394f952e5bab657b30100000000ffffffff035e0100000000000017a914aaae4c926afd3d78226dc1f1ba369103aac66b1e870000000000000000446a6304ed12f264b17576a914508bc12259f07e979e752ffc315981a7c9d326e488ac67041c3c4e64b17576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac6812160000000000001976a914f93dd4863073e9427eb63ff000069973dad4226288ac00000000

Raw signed transaction:
02000000019fb689ff79eaa4ff004cc8c33cbfcb01417001e18ec0d08394f952e5bab657b3010000006a47304402202ba38a4ef08b9957b8f176a99a83744fdd2861690d7382adc24e6d9909fb1bfb0220563344fc7fc898b9fb23ded850db8387973ebe7dfd17ab380d3c0904f3291291012103c6fa60d51063013677f547120b979b5a2163afcdb448a03551ca1580039b0eb3ffffffff035e0100000000000017a914aaae4c926afd3d78226dc1f1ba369103aac66b1e870000000000000000446a6304ed12f264b17576a914508bc12259f07e979e752ffc315981a7c9d326e488ac67041c3c4e64b17576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c988ac6812160000000000001976a914f93dd4863073e9427eb63ff000069973dad4226288ac00000000

Signed transaction size (in bytes):
300

Transaction ID:
53d288da157b038c113b89aff95783219fc6334164015fa3df70fc7dfa78534c
```
 - Then you can copy the raw transaction and propagate it with the help of your Bitcoin node (sendrawtransaction) or an online tool like this
   https://live.blockcypher.com/btc/pushtx/
