# NFC_dynamic_tag
C++ library and sketches for the NFC dynamic tag for Arduino UNO based on NT3H1101 chip from NXP.

## Library

## Examples

Sketch examples for Arduino IDE. Copy/paste the library files (.cpp, .h and keywords.txt) in your Arduino library folder. The examples can be used directly in your Arduino IDE.

### WifiConfigurationRecordSketch

This sketch pushes into the EEPROM of the NTAG a wifi credential content based on the Wifi Simple Configuration standard (for further informations read the paper ). Using the serial port a series of questions are send to the user and the tag is programmed based on the data provided. Data required are : network SSID, network key, authentication type and encryption type. The rest is built from these data.

The following table gives the usual attribute in the order they are pushed inside the EEPROM.


|   Wifi Credential Attribute   |    Size (byte nb)    |      Default Value (hex)     |
|:-----------------------------:|:--------------------:|:----------------------------:|
|      Credential Attribute     |          2B          |            0x100E            |
|       Credential Length       |          2B          |        variable value        |
|    Network Index Attribute    |          2B          |            0x1026            |
|      Network Index Length     |          2B          |            0x0001            |
|      Network Index Value      |          1B          |             0x01             |
|         SSID Attribute        |          2B          |            0x1045            |
|          SSID Length          |          2B          |        variable value        |
|           SSID Value          | variable byte number |        variable value        |
| Authentication Type Attribute |          2B          |            0x1003            |
|     Authentication Length     |          2B          |            0x0002            |
|      Authentication Value     |          2B          | 0x0020 (example for WPA2PSK) |
|   Encryption Type Attribute   |          2B          |            0x100F            |
|     Encryption Type Length    |          2B          |            0x0002            |
|     Encryption Type Value     |          2B          |   0x0008 (example for AES)   |
|     Network Key Attribute     |          2B          |            0x1027            |
|       Network Key Length      |          2B          |        variable value        |
|       Network Key Value       | variable byte number |        variable value        |
|     MAC Address Attribute     |          2B          |            0x1020            |
|       MAC Address Length      |          2B          |            0x0006            |
|       MAC Address Value       |          6B          |  0x000000000000 (if unknown) |
|    Vendor Address Attribute   |          2B          |            0x1049            |
|     Vendor Address Length     |          2B          |            0x0006            |
|      Vendor Address Value     |          6B          |        0x00372A------        |

The entire credential consitutes the payload. The credential is wrapped in a NDEF message as a external type record application/vnd.wfa.wsc.

A typical NDEF message wrapper is :
* Start Byte: B11010010 (meaning MB = 1 ME = 1 CF = 0 SR = 1 IL = 0 and TNF = 0x04),
* Type Length Byte
* Payload Length Byte (under 255 bytes as the short record bit is 1)
* Type Bytes (ASCII code for application/vnd.wfa.wsc)
* then the payload (credential)
* Final Byte of NDEF message is 0xFE

### WPandAndroidApplicationRecordSketch

This sketch implements two records in a NDEF message. The first is dedicated to Windows Phone terminals, the second is dedicated to Android terminals (AAR). The records need to be placed in that order if you want to have a dual use for Windows phones and Android phones.

### NTAGMemoryDump



## Projects
