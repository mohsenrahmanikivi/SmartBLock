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
