/******************************************************************************
Tally_I2C.cpp
Library for interfacing with the Project Project Tally I2C event counter
For use with Tally > v1.0
Bobby Schulz @ Northern Widget LLC
5/23/2019
https://github.com/NorthernWidget-Skunkworks/Project-Tally

This script is used to interface to the Tally event counter module and provide
control capabilities

Library Version = 1.1.0

"On two occasions I have been asked, 'Pray, Mr. Babbage, if you put into the machine
wrong figures, will the right answers come out?' I am not able rightly to apprehend the
kind of confusion of ideas that could provoke such a question."
-Charles Babbage

Distributed as-is; no warranty is given.
******************************************************************************/

#include "Tally_I2C.h"

Tally_I2C::Tally_I2C(uint8_t ADR_)
{
  ADR = ADR_;
}

uint8_t Tally_I2C::begin(uint8_t ADR_, bool Rst, bool Cap)
{
  if (ADR == ADR_DEFAULT) ADR = ADR_;  // Only modify address if not already changed
  if (Rst) Reset();  // Call reset if flag is set
  if (!Cap) NoCap();  // Disconnect capacitor
  Wire.begin();
  Wire.beginTransmission(ADR);  // Check status of device
  Wire.write(0x00);
  return Wire.endTransmission();
}

String Tally_I2C::GetString()
{
  String Data = "";

  // uint16_t Val = 0; //Used to store counts
  uint8_t Ctrl = ReadByte(ADR, CONFIG);  // Get cuttent value
  WriteByte(ADR, CONFIG, SAMPLE | Ctrl);  // Request sample
  bool Done = false;
  unsigned long Timeout = millis();
  while (!Done && (millis() - Timeout) < GlobalTimeout) {
    if (ReadByte(ADR, 0x00) == 0) Done = true;  // Proceed once sample is complete
  }

  if (Done && !Debug)
    Data = String(ReadWord(ADR, DATA)) + ",";  // If data is ready, read data in
  else if (Done && Debug)
    Data = String(ReadWord(ADR, DATA)) + "," + String(ReadCap()) +
      ",";  // If data is ready and debugging is enabled, print cap val as
            // well
  else
    Data = "-9999";  // If system times out, return failure

  return Data;
}

String Tally_I2C::GetHeader(bool Debug_)
{
  Debug = Debug_;  // Copy to global
  return "NumTicks [Cnt], ";
}

uint8_t Tally_I2C::Clear()  // Perfrom manual clear
{
  uint8_t Ctrl = ReadByte(ADR, CONFIG);  // Get cuttent value
  return WriteByte(ADR, CONFIG, CLEAR | Ctrl);  // Send clear value
}

uint8_t Tally_I2C::Reset()  // Perform manual reset
{
  uint8_t Ctrl = ReadByte(ADR, CONFIG);  // Get cuttent value
  return WriteByte(ADR, CONFIG, RESET | Ctrl);  // Send reset value
}

uint16_t Tally_I2C::Peek()  // Get values without reset
{
  // uint16_t Val = 0; //Used to store counts
  uint8_t Ctrl = ReadByte(ADR, CONFIG);  // Get cuttent value
  WriteByte(ADR, CONFIG, PEEK | Ctrl);  // Request peek, converts new value
                                        // but does not reset registers
  bool Done = false;
  unsigned long Timeout = millis();
  while (!Done && (millis() - Timeout) < GlobalTimeout) {
    if (ReadByte(ADR, 0x00) == 0) Done = true;  // Proceed once sample is complete
  }
  return ReadWord(ADR, DATA);  // FIX! Add failue?
}

float Tally_I2C::ReadCap(bool Update)  // Get capacitor float voltage, get name value if
                                       // update == true, otherwise read stale value
                                       // from last power cycle of Tally
{
  float Voltage = 0;  // Used to store voltage read in
  if (Update) {  // Only call for new conversion if Update is set
    uint8_t Ctrl = ReadByte(ADR, CONFIG);  // Get cuttent value
    WriteByte(ADR, CONFIG, GET_VOLTAGE | Ctrl);
    delay(1);  // Wait for updated values
  }
  Voltage = ReadWord(ADR, 0x03) * (3.3 / 1024.0);
  return Voltage;
}

uint8_t Tally_I2C::Sleep()  // Set device to sleep mode
{
  uint8_t Ctrl = ReadByte(ADR, CONFIG);  // Get cuttent value
  return WriteByte(ADR, CONFIG, SLEEP | Ctrl);  // Send sleep value
}

uint8_t Tally_I2C::NoCap(bool State)  // Disconnect capacitor from charger to
                                      // reduce pulse/leakage current
{
  uint8_t Ctrl = ReadByte(ADR, CONFIG);  // Get cuttent value
  if (State)
    Ctrl = Ctrl | NOCAP;  // Set no cap bit
  else
    Ctrl = Ctrl & 0xBF;  // Clear No cap bit
  return WriteByte(ADR, CONFIG, Ctrl);  // Send ctrl value
}

uint8_t Tally_I2C::WriteByte(uint8_t Adr, uint8_t Pos, uint8_t Val)
{
  Wire.beginTransmission(Adr);
  Wire.write(Pos);  // Identify register
  Wire.write(Val);  // Write desired value to register
  return (Wire.endTransmission());  // End I2C message, send status
}

uint8_t Tally_I2C::ReadByte(uint8_t Adr, uint8_t Pos)
{
  Wire.beginTransmission(Adr);
  Wire.write(Pos);  // Read from desired position
  uint8_t Status = Wire.endTransmission(true);
  Wire.requestFrom(static_cast<uint8_t>(Adr),
                   static_cast<uint8_t>(1));  // Read a single byte
  // while(Wire.available() < 1); //Wait for byte to be read in
  if (Status == 0)
    return Wire.read();  // Read the desired value back
  else
    return 0x00;  // Return 0 if there is an I2C error
}

unsigned int Tally_I2C::ReadWord(uint8_t Adr, uint8_t Pos)
{
  unsigned int LSB = ReadByte(Adr, Pos);
  unsigned int MSB = ReadByte(Adr, Pos + 1);
  return (MSB << 8) | LSB;  // Read the desired value back
}