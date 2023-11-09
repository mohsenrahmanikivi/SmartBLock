# SmartBLock (Smart Blockchain based Lock)

This is a micro smart blockchain based lock that is based on the uBitcoin library written in C++ for The 32-bit microcontroller. This provides a decentralized smart lock that manages by sending and receiving the Bitcoin transaction. By setting up the entire system, students can learn about embedded systems and gain a deeper understanding of how the Bitcoin blockchain functions and works. This is an efficient system that provides an opportunity for learning and experimentation.

Please note that this system is designed for learning purposes. Thus, a lot of crucial information is revealed in the terminal for debugging purposes (which causes security leaks). Therefore, this app is not intended for real-world use and it is your responsibility to ensure that you use it appropriately.

## System 
![image](images/SmartBLock.jpeg)

## Architecture 
In the context of SmartBlock (SBL), there are three parties involved: SBL itself, the owner of the house, and a guest. When the owner agrees to let a guest enter the house, they configure the SBL by sending a transaction with some funds to the SBL. Once the SBL has been configured, the guest can enter the house by providing the necessary transaction to the SBL. At the end of the agreement period, the owner can revoke the grant by sending another transaction. The SBL will return any remaining funds. 
1. The owner initializes the SBL by sharing the owner's extended public key, derivative path, and index to the SBL. (it happens by putting the owner extended public key in the file "OPUB.txt", derivative path in the file "PATH.txt", index in the file "PATHINDEX.txt")
2. The owner and SBL make an agreement in an outside channel and agree based on the rent period, and expenses. ( it is talking about absolute time lock "op_cltv" or "OP_CHECKLOCKTIMEVERIFY" that we are using in the script check the ownerAPP.py in tools folder) 
3. The owner establishes a contract based on Bitcoin Script following the agreement to show the start and end of the contract in a script. (a transaction with two outputs, one a P2SH and another P2PKH check the ownerAPP.py in the tools folder) 
4. SBL waits to receive the contract (from the bitcoin network) and then waits for the unlock transaction from the guest.
  SBL Iteration:
1. SBL receives the unlock transaction from the guest (guest needs to paste TX in the terminal) then SBL establishes a new contract (with a new guest and new SBL addresses) and executes the unlock operation. 
7. SBL waits to receive the new contract(from the Bitcoin network) or the revoking transaction from the owner. 
8. When SBL receives a revoke transaction then sends a refund transaction to the owner. 

To have a deep understanding please read the paper published here: 
PAPER


## Installation
To install the SmartBLock, please follow the instructions  provided in [the installation](docs/Installation.md).


 ## Manual
To set up the system, follow these steps:
0. To receive reports and debugging, connect your STM32f446re board to your computer through a USB port (make sure the STlink driver is installed, if you installed STm32IDE, it is already fulfilled), and open the device manager to find out the number of "COM" , then with the help of a serial terminal application such as "tera term," you can observe the reports and logs.

1. Ensure that the Bitcoin core node is running on your laptop and accessible via the IP address 191.168.137.1 on the Wi-Fi network named "SSID:Password". ( make sure your hot spot network using this IP range 192.168.137.0/24 )
2. You need to save the public extended keys of the owner and guest as well as the private key of SBL onto the SD memory. All keys must be extended keys. To accomplish this, create three files: "OXPUB.TXT" for the owner's extended public key, "GXPUB.TXT" for the guest's extended public key, and "XPRV.txt" for the SBL extended private key (the file names are important, please follow the recommended names).
3. It is crucial to include the "OXPUB.TXT" and "GXPUB.TXT" files for the system to function properly. However, if the "XPRV.txt" file is missing, it will be generated with default data. Additionally, there is a function that generates proposal keys and stores them in the "KEYGEN.TXT" file, which you can manually copy and use.
4. To speed up synchronization when SBL finds keys, you can use a tool named "headerDownloader.py" located in the "tools" folder. This tool allows you to download block headers in advance, which are required by the SBL to ensure it has all the headers of blocks in the chain. However, downloading these headers can be time-consuming, so it's better to download them in advance and store them on the SD memory. To download headers in a readable format for the SBL, you need to have Python installed and then run a specific command. 
```sh
$python headerDownloader/downlowder.py
```
4. When you put all headers in the SD memory, restart SBL, and then it starts to recognize and verify them. you can accelerate the verification by modifying the "VERIFIED.TXT" file. You can find the block height and the previous block hash of the current verified block. (you can find these data in the header files, the data stored in those files with this structure hight, hash, header). for example, you need to put hight, hash (hight-1).
5. The SBL reports the status in the serial terminal (USB to COM serial port). The SBL needs these files to operate as a SBL, so you need to create these files and put them in the SD memory.
   - "GXPUB.TXT" guest extended public key
   - "OXPUB.TXT" owner extended public key
   - "MNEMONIC.TXT" (Optional) mnemonic words that generate SBL private key.
   - "PATH.txt" Derivative PATH "m/10/" (default is m/10/)
   - "WIFI.txt" SSID and password of wifi network (hotspot)
```sh
################ STAT #################
# Derivative PATH : m/10/
# Index           : 521
# Lock address  In: n2RywdZjB5g9jwVviQWYN3qMxQWXRh38yN
# Revoke address Out: mrKNAFYnamjNsETk5PixyxHywAci6handt
# Script address  : 2N4yQ2DcMaUmAjjvwsZPwrqG5Y89YooxPHR
# Fund (satoshi)  : 42840
# Local Hight     : 2503867
# Verified Hight  : 2503800
# WIFI SSID:PSW   : mohsen
# Owner Xpub----->
tpubD6NzVbkrYhZ4WmpygA4emwLn7ZZWCPFX58GjZ1XTmvkTA4LFC4TCQyozQCc12TcKUqrS83sp5KyNBkyxmrPM68SCx7dgfsBwD7QSt1U9LAT
# Guest Xpub----->
tpubD6NzVbkrYhZ4YbGzfZ2uchThpawBzS7FgaUs3k5AZyapwWB9EkvFV6CyVNT2JWCqACyhWDiHfdityAZajDoQmh9uEPakg1LPV5TXvbpaZFs
# Lock  Xpub----->
tpubD6NzVbkrYhZ4YDpCdJW8s3ihthS5xpojJLVr5Mx82teeeo8oAcs62YLqPYzzGtXQYA7CTNgsV58mxD9R2zkHePtKVTj1cMaBXR1UbHXAdXt
#######################################
```
6. If the SBL finds all the needed files and synchronizes by downloading and verifying headers. Then it looks for a contract transaction that desires to "Lock address" which is the SBL address with the derived path " Derivative PATH/Index ".
7. To create the contract, the owner should go to the folder "tools" and find "ownerApp.py", generate the contract, and propagate it.
8. If the SBL receives the contract, it will wait for a guest who spends the contract pasting and an unlocking transaction in the terminal.
9. The guest needs to use "GuestAPP.py" in the tools folder to generate an unlocking transaction spend the contract and paste it into the SBL terminal when the SBL is in the step of receiving the unlocking transaction. (when the SBl is synchronized it will notify you by a message in the terminal that it is waiting to receive the unlocking transaction)
10. when the contract is spent and the unlocking transaction is confirmed by the network, the SBL executes the unlock operation generates and propagates a new contract( with the new address of the guest and SBL which is generated based on the new derivate path).
11. To create the revoking transaction, the owner can go to the folder "tools" and find "ownerRevokation.py" and generate and propagate a revoke transaction.
12. when the SBL receives a revoked transaction it will refund the remaining fund in the contract transaction to the owner address, change the derivative path to a new one and wait to receive a new contract to its new address (which is generated from the new derivative path) 
   



