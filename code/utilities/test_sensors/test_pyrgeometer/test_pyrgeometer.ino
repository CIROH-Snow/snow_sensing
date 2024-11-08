#include <Wire.h>             // I'm not sure what this does, but my code breaks without it
#include "Sodaq_DS3231.h"     // I think this has something to do with the clock
#include <Adafruit_ADS1015.h> // Library for 16-bit Adafruit ADC

Adafruit_ADS1115 ads(0x48);

// Steinhart-Hart equation constants for thermistor with 30 kOhm resistance at 25 deg C
// Found on page 14 of SL-510-610 manual
const float A_less = 0.000932960;     // Temperatures less than 0 deg C
const float B_less = 0.000221424;     // Temperatures less than 0 deg C
const float C_less = 0.000000126329;  // Temperatures less than 0 deg C
const float A_more = 0.000932794;     // Temperatures more than 0 deg C
const float B_more = 0.000221451;     // Temperatures more than 0 deg C
const float C_more = 0.000000126233;  // Temperatures more than 0 deg C

// Stefan-Boltzmann constant
float sb_const = 0.000000056704;

// Calibration coefficients from apogee calibration certificate
float k1 = 9.007;  // SL-510 = 9.007  SL-610 = 9.021
float k2 = 1.026;  // SL-510 = 1.026  SL-610 = 1.031

/* Wiring
Green -> A1
Red   -> SW3
Black -> A3
White -> A2
Blue  -> GND
Clear -> GND
*/

void setup() 
{
  Serial.begin(57600);
  ads.begin();
  Wire.begin();
  rtc.begin();

  digitalWrite(22, HIGH);

  Serial.println("Pyrgeometer test");

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
    Serial.print(now.hour() - 6, DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(", ");
  }
  
  // Incoming longwave radiation calculation
  float thermistorVoltage = ads.readADC_SingleEnded_V(1);
  float Rt = 24900 * (thermistorVoltage / (3.3 - thermistorVoltage));
  float pyrg_temp;
  if (Rt > 94980)
  {
    pyrg_temp = 1 / (A_less + B_less * log(Rt) + C_less * pow(log(Rt), 3));
  }
  else 
  {
    pyrg_temp = 1 / (A_more + B_more * log(Rt) + C_more * pow(log(Rt), 3));
  }
  float thermopileVoltage = ads.readADC_Differential_2_3_V();
  float longRad = k1 * thermopileVoltage * 1000 + k2 * sb_const * pow(pyrg_temp, 4);

  // Print to serial monitor
  Serial.print(thermopileVoltage * 1000);
  Serial.print(", ");
  Serial.print(pyrg_temp - 273.15);
  Serial.print(", ");
  Serial.println(longRad);
  
  delay(1000);
}
