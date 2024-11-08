/******************************************************************************
Tally_I2C.h
Library for interfacing with the Project Project Tally I2C event counter
For use with Tally > v1.0
Bobby Schulz @ Northern Widget LLC
5/23/2019
https://github.com/NorthernWidget-Skunkworks/Project-Tally

This script is used to interface to the Tally event counter module and provide control
capabilities

Library Version = 1.1.0

"On two occasions I have been asked, 'Pray, Mr. Babbage, if you put into the machine
wrong figures, will the right answers come out?' I am not able rightly to apprehend the
kind of confusion of ideas that could provoke such a question."
-Charles Babbage

Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef TALLY__I2C_h
#define TALLY_I2C_h

#include <Arduino.h>
#include "Wire.h"

#define CONFIG 0x00  // Configuration address position
#define DATA 0x01  // Location to read data

#define SAMPLE 0x01  // Config value to request sample
#define PEEK 0x09  // Config value to peek at values
#define RESET 0x04  // Resets hardware registers on device
#define CLEAR 0x02  // Clears I2C data registers on device
#define SLEEP 0x10  // Enables device sleep mode
#define NOCAP 0x40  // Disconnects the capacitor
#define NOCAP_INV 0xBF  // Inverse of NOCAP, used to clear NOCAP bit
#define GET_VOLTAGE 0x20  // Reads float voltage of cap

#define ADR_DEFAULT 0x33

class Tally_I2C
{
  public:
  Tally_I2C(uint8_t ADR_ = 0x33);  // Use default address
  uint8_t begin(uint8_t ADR_ = 0x33, bool Rst = false,
                bool Cap = true);  // Use default address, do not reset by default
  String GetString();
  String GetHeader(bool Debug_ = false);  // No debug output by default
  uint8_t Clear();
  uint8_t Reset();
  uint8_t Sleep();
  uint16_t Peek();
  uint8_t NoCap(bool State = true);  // Disconnect cap by default
  float ReadCap(bool Update = true);  // Update value by default

  private:
  uint8_t ADR = 0x33;  // Default address
  const unsigned long GlobalTimeout = 25;  // Wait at most 25ms for new data
  bool Debug = false;  // Debug flag for printouts
  uint8_t WriteByte(uint8_t Adr, uint8_t Pos, uint8_t Val);
  uint8_t ReadByte(uint8_t Adr, uint8_t Pos);
  unsigned int ReadWord(uint8_t Adr, uint8_t Pos);
};

#endif