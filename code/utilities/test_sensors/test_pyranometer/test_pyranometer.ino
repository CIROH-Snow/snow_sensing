#include <Wire.h>             // I'm not sure what this does, but my code breaks without it
#include "Sodaq_DS3231.h"     // I think this has something to do with the clock
#include <Adafruit_ADS1015.h> // Library for 16-bit Adafruit ADC

Adafruit_ADS1115 ads(0x48);

// Calibration coefficients from apogee calibration certificate
float c_in = 21.89;  // SP-510 calibration (W/m-2 per mV)
float c_out = 31.90;  // SP-610 calibration (W/m-2 per mV)

/* Wiring
Green   -> GND
Red     -> 12V+
Black   -> AA1
White   -> AA0
Blue    -> AA3
Yellow  -> AA2
Clear   -> GND
*/

void setup() 
{
  Serial.begin(57600);
  ads.begin();
  Wire.begin();
  rtc.begin();

  digitalWrite(22, HIGH);

  Serial.println("Pyranometer test");

  delay(5000);
}

uint32_t old_ts;

void loop() 
{
  DateTime now = rtc.now();
  uint32_t ts = now.getEpoch();

  if (old_ts == 0 || old_ts != ts) {
    old_ts = ts;
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.date(), DEC);
    Serial.print(' ');
    Serial.print(now.hour() - 6, DEC);  // Daylight savings (-6)
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(", ");
  }
  
  // Incoming shortwave radiation calculation
  float inVoltage = ads.readADC_Differential_2_3_V();
  float inShortRad = c_in * inVoltage * 1000;

  // Outgoing shortwave radiation calculation
  float outVoltage = ads.readADC_Differential_0_1_V();
  float outShortRad = c_out * outVoltage * 1000;

  // Print to serial monitor
  Serial.print(inVoltage * 1000);
  Serial.print(", ");
  Serial.print(inShortRad);
  Serial.print(", ");
  Serial.print(outVoltage * 1000);
  Serial.print(", ");
  Serial.println(outShortRad);
  
  delay(1000);
}
