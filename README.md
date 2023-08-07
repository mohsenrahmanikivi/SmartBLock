# uSmartlock

This is a micro smart lock based on the C++ Bitcoin library (uBitcoin) for 32-bit microcontrollers. 

## Hardware
1. STM32F4re466re microcontroller [Refrence Link](https://www.st.com/en/evaluation-tools/nucleo-f446re.html)  ![image](images/stm32F466re.jpg) </br>
2. WiFi module ESP8266  ![image](images/ESP-01(ESP8266).jpg)</br>
3. SD Card Memory Shield Adapter 6 Pin SPI Interface  ![image](images/SD_Memory_Modul.jpg) </br>
4. Step motor ( to show lock /Unlock operation)

## Pinout
This is the pinout that we used in our configuration 

| Micro SD Adapter pins  | STM32F466re pins |
| ------------- | ------------- |
| CS            | PB6(D10)  |
| SCK           | PA5(D13)  |
| MOSI          | PA7(D11)  |
| MISO          | PA6(D12)  |
| VCC           | VCC 5V    |
| GND           | GND       |

| WiFi Module   | STM32F466re pins |
| ------------- | -------------    |
| Rx            | PA10(D2)         |
| Tx            | PA9(D8)          |
| VCC           | VCC 3,3V         |
| GND           | GND              |

## Required Software
1. Have installed the latest STM32CubeIDE [Refrence Link](https://www.st.com/en/evaluation-tools/nucleo-f446re.html) to compile and download the app on the mainboard (stm32f466re).
2. Have installed the latest BitcoinCore [Refrence Link](https://bitcoin.org/en/bitcoin-core/) to prepare server environment for the system.


## Installation
Follow these steps to install and prepare the environment:
1. Acquire the hardware and connect them based on the provided pinout.
2. have installed all required software mentioned above.
3. 

The library is  in the Arduino Library manager, or you can download and install it manually.

[Download](https://github.com/micro-bitcoin/uBitcoin/archive/master.zip) the zip file from our [repository](https://github.com/micro-bitcoin/uBitcoin/) and select in Arduino IDE `Sketch` → `Include library` → `Add .ZIP library...`.

Or clone it into your `Documents/Arduino/libraries` folder:

```sh
git clone https://github.com/micro-bitcoin/uBitcoin.git
```

When installed you will also see a few examples in `File` → `Examples` → `Bitcoin` menu.

## Basic usage example

First, don't forget to include the necessary headers:

```cpp
// we use these two in our sketch:
#include "Bitcoin.h"
#include "PSBT.h"       // if using PSBT functionality
// other headers of the library
#include "Conversion.h" // to get access to functions like toHex() or fromBase64()
#include "Hash.h"       // if using hashes in your code
```

Now we can write a simple example that does the following:

1. Creates a master private key from a recovery phrase and empty password
2. Derives account and prints master public key for a watch-only wallet (`zpub` in this case)
3. Derives and print first segwit address
4. Parses, signs and prints signed PSBT transaction

```cpp
// derive master private key
HDPrivateKey hd("add good charge eagle walk culture book inherit fan nature seek repair", "");
// derive native segwit account (bip-84) for tesnet
HDPrivateKey account = hd.derive("m/84'/1'/0'/");
// print xpub: vpub5YkPqVJTA7gjK...AH2rXvcAe3p781G
Serial.println(account.xpub());
// or change the account type to UNKNOWN_TYPE to get tpub
HDPublicKey xpub = account.xpub();
xpub.type = UNKNOWN_TYPE;
// this time prints tpubDCnYy4Ty...dL4fLKsBFjFQwz
Serial.println(xpub);
// set back correct type to get segwit addresses by default
xpub.type = P2WPKH;
Serial.println(hd.fingerprint());

// print first address: tb1q6c8m3whsag5zadgl32nmhuf9q0qmtklws25n6g
Serial.println(xpub.derive("m/0/0").address());

PSBT tx;
// parse unsigned transaction
tx.parseBase64("cHNidP8BAHECAAAAAUQS8FqBzYocPDpeQmXBRBH7NwZHVJF39dYJDCXxq"
"zf6AAAAAAD+////AqCGAQAAAAAAFgAUuP0WcSBmiAZYi91nX90hg/cZJ1U8AgMAAAAAABYAF"
"C1RhUR+m/nFyQkPSlP0xmZVxlOqAAAAAAABAR/gkwQAAAAAABYAFNYPuLrw6igutR+Kp7vxJ"
"QPBtdvuIgYDzkBZaAkSIz0P0BexiPYfzInxu9mMeuaOQa1fGEUXcWIYoyAeuFQAAIABAACAA"
"AAAgAAAAAAAAAAAAAAiAgMxjOiFQofq7l9q42nsLA3Ta4zKpEs5eCnAvMnQaVeqsBijIB64V"
"AAAgAEAAIAAAACAAQAAAAAAAAAA");
// sign with the root key
tx.sign(hd);
// print signed transaction
Serial.println(tx.toBase64());
```

Ready for more? Check out the [tutorial](https://micro-bitcoin.github.io/#/tutorial/README) and start writing your very own hardware wallet!