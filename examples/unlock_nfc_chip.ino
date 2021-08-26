#include <Arduino.h>
#include <nfc_dynamic_tag.h>
#include <Wire.h>

NXP_NTAG_I2C ntag(0x55);

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  ntag.begin();
  ntag.CleanData();
}

void loop()
{
  uint8_t unlock[]={0x55,0x04,0x83,0x84,0xE2,0xE8,0x4B,0x80,0x00,0x44,0x00,0x00,0xE1,0x10,0x6D,0x00}; //for 1k version
  ntag.WriteDataBlock(0,unlock,16);
  Serial.print("Succeed! Please do format!");
  delay(60000);
}
