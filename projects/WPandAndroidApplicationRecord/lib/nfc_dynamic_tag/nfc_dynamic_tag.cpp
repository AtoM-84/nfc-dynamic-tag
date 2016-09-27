
/**************************************************************************/
/*!
    @file     NTAG_I2C.cpp
    @author   AtoM
	@license  MIT

This is a library for the NXP NTAG_I2C board

	@section  HISTORY

	v0.1 Functions:
			ReadDataBlock (read 16bytes block)
			SessionReadDataBlock (read 1byte subblock of session block)
			PrintHex (Hexa printing of one block)
			PrintHexLight (same as PrintHex without 0x prefix for hexa bytes)
			GetSerialNumber
			GetStaticLockStatus
			GetCapabilityContainer
			GetConfigurationStatus
			GetSessionStatus
			GetNTAGFullReport
			UserMemoryDump

		Added


	v0.0  - Defining command codes and functions


*/
/**************************************************************************/

#include "Arduino.h"
#include <Wire.h>
#include <nfc_dynamic_tag.h>

#define NTAG_I2C_SERIAL_NB_BLOCK 0x00
#define NTAG_I2C_USER_MEMORY_BLOCK 0x01  //first user memory block, last one is 0x38
#define NTAG_I2C_DYNAMIC_LOCK_BLOCK 0x38 //Dynamic Lock Bytes are bytes 8, 9 and 10; previous bytes are last user memory bytes
#define NTAG_I2C_CONF_REG_BLOCK 0x3A
#define NTAG_I2C_SRAM_BLOCK 0xF8
#define NTAG_I2C_SESSION_REG_BLOCK 0xFE

/**************************************************************************/
/*! NXP_NTAG_I2C(const byte device_address)
    @brief  Instantiates new NXP_NTAG_I2C
    @param  device_address			I2C device_address (7 bits SA)
*/
/**************************************************************************/

NXP_NTAG_I2C::NXP_NTAG_I2C(const byte device_address) : _device_address(device_address)
{
}

/**************************************************************************/
/*! NXP_NTAG_I2C::begin()
    @brief  Instantiates Wire.h and create new Serial connection
*/
/**************************************************************************/

void NXP_NTAG_I2C::begin()
{
    delay(100);
}

/**************************************************************************/
/*! ReadDataBlock(const byte block_address, uint8_t *out_buffer, int out_buffer_length)
    @brief  Read a block data from NTAG (i.e. max 16 consecutive bytes for a block) and store it in a max 16 bytes uint8_t output buffer table given by user
		Return last written byte index that must be equals to out_buffer_length
	see pp. 34-35 of the Rev3.2 NT3H1101 datasheet
    @param  block_address			Block address to read (MEMA)
    @param  out_buffer				Output buffer pointer
    @param  out_buffer_length		Output buffer Length
*/
/**************************************************************************/

int NXP_NTAG_I2C::ReadDataBlock(const byte block_address, uint8_t *out_buffer, int out_buffer_length)
{
    if (out_buffer_length > 16)
	out_buffer_length = 16;

    Wire.beginTransmission((uint8_t)_device_address);
    Wire.write((uint8_t)block_address);
    Wire.endTransmission();
    Wire.beginTransmission((uint8_t)_device_address);
    Wire.requestFrom((uint32_t)_device_address, out_buffer_length, true);
    int i = 0;
    for (; i < out_buffer_length; i++)
    {
	out_buffer[i] = Wire.read();
    }
    return i + 1;
}

/**************************************************************************/
/*! WriteDataBlock(const byte block_address, uint8_t * input_buffer, int input_buffer_length)
    @brief write a complete Data block, i.e. a block of 16 bytes following a block address
    @param  block_address
    @param  input_buffer
    @param  input_buffer_length
*/
/**************************************************************************/

void NXP_NTAG_I2C::WriteDataBlock(const byte block_address, uint8_t *input_buffer, int input_buffer_length)
{
    Wire.beginTransmission((uint8_t)_device_address);
    Wire.write((uint8_t)block_address);
    int i = 0;
    for (; i < input_buffer_length; i++)
    {
	Wire.write(input_buffer[i]);
    }
    for (; i < 16; i++)
    {
	Wire.write(0x00);
    }
    Wire.endTransmission();
    delay(5);
}

/**************************************************************************/
/*! CleanDataBlock(const byte block_address, uint8_t * input_buffer, int input_buffer_length)
    @brief same as WriteDataBlock but filling with 0x00
    @param  block_address
*/
/**************************************************************************/

void NXP_NTAG_I2C::CleanDataBlock(const byte block_address)
{
    Wire.beginTransmission((uint8_t)_device_address);
    Wire.write((uint8_t)block_address);
    for (int j = 0; j < 16; j++)
    {
	Wire.write(0x00);
    }
    Wire.endTransmission();

    delay(5);
}

/**************************************************************************/
/*! CleanData()
    @brief Clean data block applied on all EEPROM blocks
*/
/**************************************************************************/

void NXP_NTAG_I2C::CleanData()
{
    for (int i = 1; i < 56; i++)
    {
	CleanDataBlock(i);
    }
    Wire.beginTransmission((uint8_t)_device_address);
    Wire.write(0x38);
    for (int j = 0; j < 8; j++)
    {
	Wire.write(0x00);
    }
    for (int j = 8; j < 16; j++)
    {
	Wire.write(0x00);
    }
    Wire.endTransmission();
    delay(5);
}
/**************************************************************************/
/*! StartSRAMMirror()
    @brief activate the SRAM Mirror on address 0x01
*/
/**************************************************************************/

void NXP_NTAG_I2C::StartSRAMMirror()
{
    byte newConf[] = {0x01, 0x00, 0x01, 0x48, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //Modify the configuration register with the new configuration
    WriteDataBlock(0x3A, newConf, 16);
    // Note the special sequence for the writing in session register
    Wire.beginTransmission(0x55);
    //write on the block address 0xFE
    Wire.write(0xFE);
    //select the byte to be modified i.e. 0x00
    Wire.write(0x00);
    //select the bit(s) to be modified i.e. the second bit for SRAM Mirror enabling
    Wire.write(0x02);
    //give the desired result i.e. 0x03 (b00000011) as the previous value was 0x01 (b00000001)
    Wire.write(0x03);
    //stop
    Wire.endTransmission(true);
}

/**************************************************************************/
/*! WriteDataEEPROM(uint8_t * input_buffer, int input_buffer_length)
    @brief write an array of byte values in the EEPROM memory, filling the block from the address 0x01 (I2C addressing) up until the last full or incomplete block
    @param  input_buffer
    @param  input_buffer_length
*/
/**************************************************************************/

void NXP_NTAG_I2C::WriteDataEEPROM(uint8_t *input_buffer, int input_buffer_length)
{
    int32_t full_block;
    uint32_t last_block_remainder;

    full_block = (uint32_t)(input_buffer_length / 16);
    last_block_remainder = input_buffer_length % 16;
    for (int i = 1; i < full_block + 1; i++)
    {
	WriteDataBlock(i, &input_buffer[0 + (i - 1) * 16], 16);
    }
    WriteDataBlock(full_block + 1, &input_buffer[full_block * 16], last_block_remainder);
}

/**************************************************************************/
/*! WriteDataSRAM(uint8_t * input_buffer, int input_buffer_length)
    @brief write an array of byte values in the SRAM memory, filling the block from the address 0xF8 (I2C addressing) up until the last full or incomplete block that is at the most the boclk a
    @param  input_buffer
    @param  input_buffer_length
*/
/**************************************************************************/

void NXP_NTAG_I2C::WriteDataSRAM(uint8_t *input_buffer, int input_buffer_length)
{
    int32_t full_block;
    uint32_t last_block_remainder;

    full_block = (uint32_t)(input_buffer_length / 16);
    last_block_remainder = input_buffer_length % 16;
    for (int i = 248; i < 248 + full_block + 1; i++)
    {
	WriteDataBlock(i, &input_buffer[0 + (i - 248) * 16], 16);
    }
    WriteDataBlock(248 + full_block + 1, &input_buffer[full_block * 16], last_block_remainder);
}

/**************************************************************************/
/*! PrintHex(const byte * data, const uint32_t nbBytes, bool prefix)
    @brief  Prints a hexadecimal value with or without Ox prefix
    @param  data		Pointer to the byte data
    @param  nbBytes		Data length in bytes
    @param  prefix		Leading 0x prefix enabler
*/
/**************************************************************************/

void NXP_NTAG_I2C::PrintHex(const byte *data, const uint32_t nbBytes, bool prefix)
{
    uint32_t index;
    for (index = 0; index < nbBytes; index++)
    {
	if (prefix == true)
	{
	    Serial.print(F("0x"));
	}
	// Append leading 0 for small values
	if (data[index] > 0x0F)
	{
	    Serial.print(data[index], HEX);
	}
	else
	{
	    Serial.print("0");
	    Serial.print(data[index], HEX);
	}
	Serial.print(F(" "));
    }
    Serial.println();
}

/**************************************************************************/
/*! PrintHexASCII(const byte * data, const uint32_t nbBytes)
    @brief  Prints a hexadecimal value  without the 0x prefix and the
			corresponding ASCII code in brackets
    @param  data      Pointer to the byte data
    @param  nbBytes  Data length in bytes
*/
/**************************************************************************/

void NXP_NTAG_I2C::PrintHexASCII(const byte *data, const uint32_t nbBytes)
{
    uint32_t index;
    for (index = 0; index < nbBytes; index++)
    {
	// Append leading 0 for small values
	if (data[index] > 0x0F)
	{
	    Serial.print(data[index], HEX);
	}
	else
	{
	    Serial.print("0");
	    Serial.print(data[index], HEX);
	}
	Serial.print(F(" "));
    }
    if (nbBytes != 16)
    {
	Serial.print("                        ");
    }
    Serial.print("  [");
    for (index = 0; index < nbBytes; index++)
    {
	if (data[index] < 128 && data[index] > 19)
	{
	    Serial.write(data[index]);
	}
	else
	{
	    Serial.print(".");
	}
    }
    Serial.print("]");
    Serial.print('\r');
    Serial.println();
}

/**************************************************************************/
/*! GetSerialNumber()
    @brief  Get and display the NTAG I2C serial number
			see pp. 15-16 of the datasheet Rev3.2 for more details on S/N
*/
/**************************************************************************/

void NXP_NTAG_I2C::GetSerialNumber()
{
    uint8_t NTAG_serial[7];

    ReadDataBlock(NTAG_I2C_SERIAL_NB_BLOCK, NTAG_serial, 7);

    Serial.println();
    Serial.print("------------------------------------------------------------------");
    Serial.println();
    Serial.print("   NTAG I2C Serial Number : ");
    PrintHex(NTAG_serial, 7, true);
    Serial.println();
    Serial.print('\r');
    Serial.print("First byte is manufacturer code (NXP = 0x04)");
    Serial.println();
    Serial.print("------------------------------------------------------------------");
    Serial.println();
}

/**************************************************************************/
/*! GetStaticLockStatus()
    @brief  Get and display the NTAG Static Lock Bytes
			see p. 16 of the datasheet Rev3.2 for more details on static lock bytes
*/
/**************************************************************************/

void NXP_NTAG_I2C::GetStaticLockStatus()
{
    uint8_t buffer_static_lock_bytes[16];
    uint8_t static_lock_bytes[2];

    ReadDataBlock(NTAG_I2C_SERIAL_NB_BLOCK, buffer_static_lock_bytes, 16);

    for (int i = 0; i < 2; i++)
    {
	static_lock_bytes[i] = buffer_static_lock_bytes[i + 10];
    }

    Serial.println();
    Serial.print("------------------------------------------------------------------");
    Serial.println();
    Serial.print("                 Static Lock Bytes :");
    PrintHex(static_lock_bytes, 2, true);
    Serial.println();
    Serial.print("+------------+---+---+---+---+---+---+---+---+---+---+---+---+---+");
    Serial.println();
    Serial.print("|Locked pages|C-C| 4 | 5 | 6 | 7 | 8 | 9 | 10| 11| 12| 13| 14| 15|");
    Serial.println();
    Serial.print("+------------+---+---+---+---+---+---+---+---+---+---+---+---+---+");
    Serial.println();
    Serial.print("|   Status   | ");
    Serial.print(bitRead(static_lock_bytes[0], 3));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[0], 4));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[0], 5));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[0], 6));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[0], 7));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[1], 0));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[1], 1));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[1], 2));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[1], 3));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[1], 4));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[1], 5));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[1], 6));
    Serial.print(" | ");
    Serial.print(bitRead(static_lock_bytes[1], 7));
    Serial.print(" |");
    Serial.println();
    Serial.print("+------------+---+---+---+---+---+---+---+---+---+---+---+---+---+");
    Serial.println();
    Serial.println();
    Serial.print("                  Static Block Lock Bytes");
    Serial.println();
    Serial.println();
    Serial.print("+------------+-----+-----+-----+");
    Serial.println();
    Serial.print("|Block Locker| C-C | 4-9 |10-15|");
    Serial.println();
    Serial.print("+------------+-----+-----+-----+");
    Serial.println();
    Serial.print("|   Status   |  ");
    Serial.print(bitRead(static_lock_bytes[0], 0));
    Serial.print("  |  ");
    Serial.print(bitRead(static_lock_bytes[0], 1));
    Serial.print("  |  ");
    Serial.print(bitRead(static_lock_bytes[0], 2));
    Serial.print("  |  ");
    Serial.println();
    Serial.print("+------------+-----+-----+-----+");
    Serial.println();
}

/**************************************************************************/
/*! GetCapabilityContainer()
    @brief  Get and display the NTAG I2C capability container
			see p. 19 of the datasheet Rev3.2 for more details on capability
			container
*/
/**************************************************************************/

void NXP_NTAG_I2C::GetCapabilityContainer()
{
    uint8_t buffer_cc[16];
    uint8_t capability_container[4];

    ReadDataBlock(NTAG_I2C_SERIAL_NB_BLOCK, buffer_cc, 16);

    for (int i = 0; i < 4; i++)
    {
	capability_container[i] = buffer_cc[i + 12];
    }

    Serial.print("------------------------------------------------------------------");
    Serial.println();
    Serial.print("Capability Container : ");
    PrintHex(capability_container, 4, true);
    Serial.println();
}

/**************************************************************************/
/*! GetConfigurationStatus()
    @brief  Get and display the NTAG I2C session register
			see pp. 20-26  of the datasheet Rev3.2 for more details on conf and
			session registers
*/
/**************************************************************************/

void NXP_NTAG_I2C::GetConfigurationStatus()
{
    uint8_t configuration_register[8];

    ReadDataBlock(NTAG_I2C_CONF_REG_BLOCK, configuration_register, 8);

    Serial.print("------------------------------------------------------------------");
    Serial.println();
    Serial.print("Configuration Register : ");
    PrintHex(configuration_register, 8, true);
    Serial.println();
}

/**************************************************************************/
/*! GetSessionStatus()
    @brief  Get and display the NTAG I2C session register
			see pp. 20-26  of the datasheet Rev3.2 for more details on conf and
			session registers
*/
/**************************************************************************/

void NXP_NTAG_I2C::GetSessionStatus()
{

    Serial.print("------------------------------------------------------------------");
    Serial.println();
    Serial.print("Session Register : ");

    uint8_t session_register[8];

    for (int i = 0; i < 8; i++)
    {
	Wire.beginTransmission((uint8_t)_device_address);
	Wire.write(254);
	Wire.write(i);
	Wire.endTransmission();
	Wire.beginTransmission((uint8_t)_device_address);
	Wire.requestFrom((uint32_t)_device_address, 1, true);
	session_register[i] = Wire.read();
	Wire.endTransmission(true);
	delay(10);
    }
    PrintHex(session_register, 8, true);

    Serial.println();
}

/**************************************************************************/
/*! GetNTAGFullReport()
    @brief  Get and display Serial Number, CC, StaticLockStatus Conf Status
			and Session Status.
*/
/**************************************************************************/

void NXP_NTAG_I2C::GetNTAGFullReport()
{
    GetSerialNumber();
    GetCapabilityContainer();
    GetStaticLockStatus();
    GetConfigurationStatus();
    GetSessionStatus();
}

/**************************************************************************/
/*! UserMemoryDump()
    @brief  Get and display User Memory
*/
/**************************************************************************/

void NXP_NTAG_I2C::UserMemoryDump()
{
    uint8_t block_mem[16];
    uint8_t last_block_mem[8];

    for (int i = 1; i < 55; i++)
    {
	ReadDataBlock(i, block_mem, 16);
	PrintHexASCII(block_mem, 16);
    }

    ReadDataBlock(56, last_block_mem, 8);
    PrintHexASCII(last_block_mem, 8);
    Serial.println();
}
