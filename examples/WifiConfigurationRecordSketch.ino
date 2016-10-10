#include <Arduino.h>
#include <nfc_dynamic_tag.h>
#include <Wire.h>

//Types definition according to Wifi Simple Configuration TS v 2.0.5

#define CREDENTIAL_TYPE 0x100E
#define CREDENTIAL_LENGTH 0x0000
#define NETWORK_INDEX_TYPE 0x1026
#define NETWORK_INDEX_LENGTH 0x0001
#define NETWORK_INDEX_VALUE 0x01
#define SSID_TYPE 0x1045
#define SSID_LENGTH 0x0000
#define AUTHENTICATION_TYPE 0x1003
#define AUTHENTICATION_LENGTH 0x0002
#define ENCRYPTION_TYPE 0x100F
#define ENCRYPTION_LENGTH 0x0002
#define NETWORK_KEY_TYPE 0x1027
#define NETWORK_KEY_LENGTH 0x0000
#define MAC_ADDRESS_TYPE 0x1020
#define MAC_ADDRESS_LENGTH 0x0006
#define VENDOR_ADDRESS_TYPE 0x1049
#define VENDOR_ADDRESS_LENGTH 0x0006

byte WPS_TYPE[] = {0x61, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2f, 0x76, 0x6e, 0x64, 0x2e, 0x77, 0x66, 0x61, 0x2e, 0x77, 0x73, 0x63}; //ASCII code for application/vnd.wfa.wsc

byte MAC_ADDRESS[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

byte VENDOR_ADDRESS[] = {0x00, 0x37, 0x2A, 0x00, 0x01, 0x20};

//Ntag instanciate

NXP_NTAG_I2C ntag(0x55);

// Input buffer

char input_buffer[128];

// Wifi credential tag content and cursor

uint8_t ntagcontent[256];
int ntagcontent_cursor = 0;


int read_data()
{
    uint8_t index = 0;
    char c;
    while (index < 127)
    {
        c = Serial.read();
        if (((char)c == '\r') || ((char)c == '\n'))
            break;
        if (((char)c == '\x7F') || ((char)c == '\x08'))
        {
            if (index > 0)
                index--;
        }
        else if (c >= 0)
        {
            input_buffer[index++] = c;
        }
    }
    input_buffer[index] = '\0'; // terminate string with NULL

    if ((char)c == '\r')
    {
        delay(10);
        if (Serial.peek() == '\n')
            Serial.read();
    }

    return index;
}

int push2BytesToArray(uint8_t array[], uint16_t value, int cursor)
{
  array[cursor+1] = (uint8_t)(value & 0xFF);
  array[cursor] = (uint8_t)(value >> 8);
  return cursor + 2;
}

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    ntag.begin();

    // ntagcontent[0] = 0xD2; //NDEF message start byte corresponding to B11010010
    // ntagcontent[1] = 0x17; //NDEF type length for application/vnd.wfa.wsc
    // ntagcontent[2] = 0x00; //payload length to be defined at the end of the data pushed from user
    // ntagcontent_cursor = 3;


    ntagcontent[0] = 0x03; //NDEF message start byte corresponding to B11010010
    ntagcontent[1] = 0x00; //NDEF type length for application/vnd.wfa.wsc
    ntagcontent[2] = 0xD2; //payload length to be defined at the end of the data pushed from user
    ntagcontent[3] = 0x17;
    ntagcontent[4] = 0x00;
    ntagcontent_cursor = 5;

    // Inserting the WPS TYPE of the NDEF record

    for(int i = 0; i < 23; i++)
    {
      ntagcontent[ntagcontent_cursor + i] = WPS_TYPE[i];
    }
    ntagcontent_cursor = ntagcontent_cursor + 23;

    ntagcontent_cursor = push2BytesToArray(ntagcontent, CREDENTIAL_TYPE, ntagcontent_cursor);
    ntagcontent_cursor = push2BytesToArray(ntagcontent, CREDENTIAL_LENGTH, ntagcontent_cursor);
    ntagcontent_cursor = push2BytesToArray(ntagcontent, NETWORK_INDEX_TYPE, ntagcontent_cursor);
    ntagcontent_cursor = push2BytesToArray(ntagcontent, NETWORK_INDEX_LENGTH, ntagcontent_cursor);
    ntagcontent[ntagcontent_cursor] = 0x01;
    ntagcontent_cursor = ntagcontent_cursor + 1;
    ntagcontent_cursor = push2BytesToArray(ntagcontent, SSID_TYPE, ntagcontent_cursor);
    ntagcontent_cursor = push2BytesToArray(ntagcontent, SSID_LENGTH, ntagcontent_cursor);

    Serial.print(F("\n***********************Wifi Credential Tag***********************\n"));
    Serial.print(F("\nEnter the WiFi SSID\n"));
    while (!Serial.available())
    {
    }

    int input_buffer_length = read_data();
    Serial.println(input_buffer);
    Serial.flush();
    ntagcontent[ntagcontent_cursor - 1] = input_buffer_length;
    for(int m = 0; m < input_buffer_length; m++)
    {
      ntagcontent[ntagcontent_cursor + m] = input_buffer[m];
    }

    ntagcontent_cursor = ntagcontent_cursor + input_buffer_length;

    Serial.print(F("\nSelect the authentication type (1 - Open, 2 - WPA2-Entreprise, 3 - WPA2-Personal, 4 - WPA-Entreprise, 5 - WPA-Personal, 6 - Shared)\n"));
    while (!Serial.available())
    {
    }

    int value = read_data();
    ntagcontent_cursor = push2BytesToArray(ntagcontent, AUTHENTICATION_TYPE, ntagcontent_cursor);
    ntagcontent_cursor = push2BytesToArray(ntagcontent, AUTHENTICATION_LENGTH, ntagcontent_cursor);
    switch(input_buffer[value-1]) {
      case '1':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0001, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
      case '2':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0010, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
      case '3':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0020, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
      case '4':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0008, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
      case '5':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0002, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
      case '6':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0004, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
    }

    Serial.print(F("\nSelect the encryption type (1 - None, 2 - WEP, 3 - TKIP, 4 - AES, 5 - AES/TKIP)\n"));
    while (!Serial.available())
    {
    }
    int value2 = read_data();
    ntagcontent_cursor = push2BytesToArray(ntagcontent, ENCRYPTION_TYPE, ntagcontent_cursor);
    ntagcontent_cursor = push2BytesToArray(ntagcontent, ENCRYPTION_LENGTH, ntagcontent_cursor);
    switch(input_buffer[value2-1]) {
      case '1':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0001, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
      case '2':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0002, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
      case '3':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0004, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
      case '4':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x0008, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
      case '5':
      ntagcontent_cursor = push2BytesToArray(ntagcontent, 0x000c, ntagcontent_cursor);
      Serial.println(ntagcontent_cursor);
      break;
    }

    Serial.print(F("\nEnter the WiFi key\n"));
    while (!Serial.available())
    {
    }
    ntagcontent_cursor = push2BytesToArray(ntagcontent, NETWORK_KEY_TYPE, ntagcontent_cursor);
    ntagcontent_cursor = push2BytesToArray(ntagcontent, NETWORK_KEY_LENGTH, ntagcontent_cursor);
    int input_buffer_length2 = read_data();
    Serial.println(input_buffer);
    Serial.flush();
    ntagcontent[ntagcontent_cursor - 1] = input_buffer_length2;
    for(int l = 0; l < input_buffer_length2; l++)
    {
      ntagcontent[ntagcontent_cursor + l] = input_buffer[l];
    }

    ntagcontent_cursor = ntagcontent_cursor + input_buffer_length2;

    ntagcontent_cursor = push2BytesToArray(ntagcontent, MAC_ADDRESS_TYPE, ntagcontent_cursor);
    ntagcontent_cursor = push2BytesToArray(ntagcontent, MAC_ADDRESS_LENGTH, ntagcontent_cursor);

    for(int u = 0; u < 6; u++)
    {
      ntagcontent[ntagcontent_cursor + u] = MAC_ADDRESS[u];
    }
    ntagcontent_cursor = ntagcontent_cursor + 6;

    ntagcontent[31] = ntagcontent_cursor - 32;

    ntagcontent_cursor = push2BytesToArray(ntagcontent, VENDOR_ADDRESS_TYPE, ntagcontent_cursor);
    ntagcontent_cursor = push2BytesToArray(ntagcontent, VENDOR_ADDRESS_LENGTH, ntagcontent_cursor);

    for(int v = 0; v < 6; v++)
    {
      ntagcontent[ntagcontent_cursor + v] = VENDOR_ADDRESS[v];
    }

    ntagcontent_cursor = ntagcontent_cursor + 6;

    ntagcontent[4] = ntagcontent_cursor - 5 - 23;
    ntagcontent[1] = ntagcontent_cursor - 2;

    ntagcontent[ntagcontent_cursor] = 0xFE;

    Serial.println();
    Serial.println();

    for(int k = 0; k < ntagcontent_cursor + 1; k++)
    {
      Serial.println(ntagcontent[k], HEX);
    }
    ntagcontent_cursor = ntagcontent_cursor + 1;
    ntag.CleanData();
    ntag.WriteDataEEPROM(ntagcontent, ntagcontent_cursor);
    ntag.UserMemoryDump();
}

void loop()
{
}
