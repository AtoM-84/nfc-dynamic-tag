

# NFC_dynamic_tag

C++ library and sketches for the NFC dynamic tag for Arduino UNO based on [NT3H1101](http://www.nxp.com/documents/data_sheet/NT3H1101_1201.pdf) chip from NXP.

## Library


## Sketch Examples

Sketch examples for Arduino IDE. Copy/paste the library files (.cpp, .h and keywords.txt) in your Arduino library folder. The examples can be used directly in your Arduino IDE.

### Wifi Credential Logger (WifiConfigurationRecordSketch)

This sketch pushes into the EEPROM of the NTAG a wifi credential content based on the [Wifi Simple Configuration standard](https://www.wi-fi.org/downloads-registered-guest/Wi-Fi_Simple_Configuration_Technical_Specification_v2.0.5.pdf). Using the serial port a series of questions are send to the user and the tag is programmed based on the data provided. Data required are : network SSID, network key, authentication type and encryption type. The rest is built from these data. The generic structure is a 2bytes attributes, followed by the length on 2 bytes and the value of the attributes on the specified number of bytes.  

The following table gives the usual attribute in the order they are pushed inside the EEPROM.

<center>

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

</center>

The entire credential consitutes the payload. The credential is wrapped in a NDEF message as a external type record __application/vnd.wfa.wsc__.

A typical NDEF message wrapper is :
* Start Byte: B11010010 (meaning MB = 1 ME = 1 CF = 0 SR = 1 IL = 0 and TNF = 0x04),
* Type Length Byte
* Payload Length Byte (under 255 bytes as the short record bit is 1)
* Type Bytes (ASCII code for application/vnd.wfa.wsc)
* then the payload (credential)
* Final Byte of NDEF message is 0xFE

### Windows Phone and Android Application Launcher (WPandAndroidApplicationRecordSketch)

This sketch implements two records in a NDEF message. The example is taken from the Orange Cash application launcher.The first is dedicated to Windows Phone terminals, the second is dedicated to Android terminals (AAR). Note that the records need to be placed in tis very order if you want to have a dual use for Windows phones and Android phones.

### Full Memory Dump (NTAGMemoryDumpSketch)

This sketch dumps the whole content of the memory and give a report of the different registers (session, configuration, EEPROM etc...).

## Projects


