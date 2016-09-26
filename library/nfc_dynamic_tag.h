/**************************************************************************/
/*!
    @file     NTAG_I2C.h
    @author   AtoM
	@license  BSD (see license.txt)
    
This is a library for the NXP NTAG_I2C board

	@section  HISTORY

	v0.1 Functions:
		BlockReadData (read 16bytes block)
		SessionBlockReadData (read 1byte subblock of session block)
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

#ifndef NTAG_I2C_H
#define NTAG_I2C_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// NTAG_I2C standard I2C address

// NTAG_I2C I2C Register addresses

#define NTAG_I2C_SERIAL_NB_BLOCK					0x00
#define NTAG_I2C_USER_MEMORY_BLOCK				0x01 //first user memory block, last one being 0x38
#define NTAG_I2C_DYNAMIC_LOCK_BLOCK				0x38 //Dynamic Lock Bytes are bytes 8, 9 and 10; previous bytes are last user memory bytes
#define NTAG_I2C_CONF_REG_BLOCK						0x3A
#define NTAG_I2C_SRAM_BLOCK								0xF8
#define NTAG_I2C_SESSION_REG_BLOCK				0xFE


class NXP_NTAG_I2C{
 public:
  NXP_NTAG_I2C (const byte device_address);
  void begin(void);

//general purpose functions

void PrintHex(const byte * data, const uint32_t nbBytes, bool prefix);
void PrintHexASCII(const byte * data, const uint32_t nbBytes);

int ReadDataBlock(const byte block_address, uint8_t * out_buffer, int out_buffer_length);
void WriteDataBlock(const byte block_address, uint8_t * input_buffer, int input_buffer_length);
void WriteDataEEPROM(uint8_t * input_buffer, int input_buffer_length);
void WriteDataSRAM(uint8_t * input_buffer, int input_buffer_length);
void StartSRAMMirror();
void CleanDataBlock(const byte block_address);
void CleanData();

//special register read and print functions
void GetCapabilityContainer();
void GetStaticLockStatus();
void GetConfigurationStatus();
void GetSessionStatus();
void GetSerialNumber();
void GetNTAGFullReport();

//Memory dump
void UserMemoryDump();

private:
const byte _device_address;

};

#endif