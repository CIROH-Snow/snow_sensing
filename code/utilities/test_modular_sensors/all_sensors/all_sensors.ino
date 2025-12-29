/** =========================================================================
 * File Name: all_sensors.ino
 * Description: A simple data logging example using all sensors.
 *
 * Author: Braedon Dority <braedon.dority@usu.edu>
 * 
 * Copyright (c) 2025 Utah State University
 * 
 * License: This example is published under the BSD-3 license.
 *
 * Build Environment: Arduino IDE 1.8.19
 * Hardware Platform: EnviroDIY Mayfly Arduino Datalogger v1.1
 *
 * DISCLAIMER: THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
 * ======================================================================= */

// ==========================================================================
//  Include the libraries required for any data logger
// ==========================================================================
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// EnableInterrupt is used by ModularSensors for external and pin change
// interrupts and must be explicitly included in the main program.
#include <EnableInterrupt.h>

// Include the main header for ModularSensors
#include <ModularSensors.h>

#include <Wire.h>

#include <AltSoftSerial.h>  // AltSoftSerial is the best software serial option since
                            // both hardware serials are used when an XBee is used.
AltSoftSerial sonarSerial(6, -1);  // The -1 indicates that no Tx wire is attached.

// ==========================================================================
//  Defines for the Arduino IDE
// ==========================================================================
#define xbeeSleepPin 23
#define xbeeRegulatorPin 18

// ==========================================================================
// XBee Pro S3B Options
// ==========================================================================
const int xbeeBaud = 9600;

// ==========================================================================
// Data Logging Options
// ==========================================================================
// The name of this program file
const char* sketchName = "all_sensors.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "23151_testingSensorsAtGreenhouse";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 1;
// Your logger's timezone.
const int8_t timeZone = -7;  // Mountain Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud = 115200;  // Baud rate for debugging
const int8_t  greenLED   = 8;       // Pin for the green LED
const int8_t  redLED     = 9;       // Pin for the red LED
const int8_t  buttonPin  = 21;      // Pin for debugging mode (ie, button pin)
const int8_t  wakePin    = 31;  // MCU interrupt/alarm pin to wake from sleep
// Mayfly 0.x D31 = A7
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin   = -1;  // MCU SD card power pin
const int8_t sdCardSSPin    = 12;  // SD card chip select/slave select pin
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power


// ==========================================================================
// Using the Processor as a Sensor
// ==========================================================================
#include <sensors/ProcessorStats.h>

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v1.1";
ProcessorStats mcuBoard(mcuBoardVersion);


// ==========================================================================
// Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>  // Includes wrapper functions for Maxim DS3231 RTC

// Create a DS3231 sensor object, using this constructor function:
MaximDS3231 ds3231(1);


// ==========================================================================
// MaxBotix sonar sensor for snow depth
// ==========================================================================
#include <sensors/MaxBotixSonar.h>

const int32_t sonarHeight;
const int8_t sonarPower = sensorPowerPin;
const int8_t sonarTrigger = -1;
const uint8_t sonarNumReadings = 3;
MaxBotixSonar sonar(sonarSerial, sonarPower, sonarTrigger, sonarNumReadings);

Variable* sonarRange =
    new MaxBotixSonar_Range(&sonar, "12345678-abcd-1234-ef00-1234567890ab");

float calculateSnowDepth(void) {
    float calculatedResult = -9999;  // Always safest to start with a bad value
    float inputVar1 = sonarHeight;
    float inputVar2 = sonarRange->getValue();
    // make sure both inputs are good
    if (inputVar1 != -9999 && inputVar2 != -9999) {
      calculatedResult = inputVar1 - inputVar2;
    }
    return calculatedResult;
}

// Properties of the calculated snow depth
// The number of digits after the decimal place
const uint8_t calculatedSnowDepthResolution = 0;
// This must be a value from http://vocabulary.odm2.org/variablename/
const char* calculatedSnowDepthName = "Snow Depth";
// This must be a value from http://vocabulary.odm2.org/units/
const char* calculatedSnowDepthUnit = "mm";
// A short code for the variable
const char* calculatedSnowDepthCode = "snowDepth";
// The (optional) universallly unique identifier
const char* calculatedSnowDepthUUID = "12345678-abcd-1234-ef00-1234567890ab";

Variable* calculatedSnowDepth = new Variable(
    calculateSnowDepth, calculatedSnowDepthResolution, calculatedSnowDepthName,
    calculatedSnowDepthUnit, calculatedSnowDepthCode, calculatedSnowDepthUUID);



// ** IMPORTANT ** 
// ApogeeSP510, ApogeeSP610, ApogeeSL510, and ApogeeSL610 modular sensors source codes
// only function on certain pins of the ADC for now. Follow the wiring.
// ==========================================================================
//    Apogee SP-510-SS for incoming shortwave radiation
// ==========================================================================
#include <sensors/ApogeeSP510.h>

/* WIRING FOR *SP-710* UPWARD SENSOR
  YELLOW  -> AA2
  BLUE    -> AA3
  RED     -> V12+     Heater
  GREEN   -> V12 GND  Heater ground
*/
ApogeeSP510 sp510(sensorPowerPin, 0x48, 50);  // 

Variable* sp510volts =
    new ApogeeSP510_Voltage(&sp510, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sp510rad =
    new ApogeeSP510_ISWR(&sp510, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
// Apogee SP-610-SS for outgoing shortwave radiation
// ==========================================================================
#include <sensors/ApogeeSP610.h>

/* WIRING FOR *SP-710* DOWNWARD SENSOR
  WHITE   -> AA0
  BLACK   -> AA1
  CLEAR   -> GND      Shield
*/
ApogeeSP610 sp610(sensorPowerPin, 0x48, 50);

Variable* sp610volts =
    new ApogeeSP610_Voltage(&sp610, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sp610rad =
    new ApogeeSP610_OSWR(&sp610, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
// Apogee SL-510-SS for incoming longwave radiation
// ==========================================================================
#include <sensors/ApogeeSL510.h>

/* WIRING FOR SL-510
  WHITE   -> AA3
  BLACK   -> AA2
  GREEN   -> AA1
  YELLOW  -> V12+
  BLUE    -> V12 GND
  RED     -> SW3
  CLEAR   -> GND
*/

// Set k1 and k2 factors for the SL-510
float sl510k1 = 9.141;
float sl510k2 = 1.020;

ApogeeSL510 sl510(sensorPowerPin, sl510k1, sl510k2, 1, 0x49, 50);  // Note that this sensor is not attached to the board ADC

Variable* sl510thermistorVolts =
    new ApogeeSL510_Thermistor_Voltage(&sl510, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl510thermopileVolts =
    new ApogeeSL510_Thermopile_Voltage(&sl510, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl510rad =
    new ApogeeSL510_ILWR(&sl510, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
// Apogee SL-610-SS for incoming longwave radiation
// ==========================================================================
#include <sensors/ApogeeSL610.h>

/* WIRING FOR SL-610
  WHITE   -> AA3
  BLACK   -> AA2
  GREEN   -> AA1
  YELLOW  -> SW12
  BLUE    -> GND
  RED     -> SW3
  CLEAR   -> GND
*/

// Set the k1 and k2 factors for the SL-610
float sl610k1 = 8.997;
float sl610k2 = 1.039;

ApogeeSL610 sl610(sensorPowerPin, sl610k1, sl610k2, 1, 0x4A, 50);  // Note that this sensor is not attached to the board ADC

Variable* sl610thermistorVolts =
    new ApogeeSL610_Thermistor_Voltage(&sl610, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl610thermopileVolts =
    new ApogeeSL610_Thermopile_Voltage(&sl610, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl610rad =
    new ApogeeSL610_OLWR(&sl610, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
// METER Teros 12 Soil Moisture Sensors
// ==========================================================================
#include <sensors/MeterTeros12.h>
char add1 = 'a';  // Address 1 at depth XXXX
char add2 = 'b';  // Address 2 at depth XXXX
char add3 = 'c';  // Address 3 at depth XXXX

int8_t sdiPowerPin = sensorPowerPin;
int8_t sdiDataPin = 7;

MeterTeros12 probe1(add1, sdiPowerPin, sdiDataPin);
MeterTeros12 probe2(add2, sdiPowerPin, sdiDataPin);
MeterTeros12 probe3(add3, sdiPowerPin, sdiDataPin);

Variable* rawVWCCounts1 = 
    new MeterTeros12_Count(&probe1, "12345678-abcd-1234-ef00-1234567890ab");
Variable* soilTemp1 =
    new MeterTeros12_Temp(&probe1, "12345678-abcd-1234-ef00-1234567890ab");
Variable* soilEa1 =
    new MeterTeros12_Ea(&probe1, "12345678-abcd-1234-ef00-1234567890ab");
Variable* vwc1 =
    new MeterTeros12_VWC(&probe1, "12345678-abcd-1234-ef00-1234567890ab");
Variable* ec1 =
    new MeterTeros12_EC(&probe1, "12345678-abcd-1234-ef00-1234567890ab");

Variable* rawVWCCounts2 = 
    new MeterTeros12_Count(&probe2, "12345678-abcd-1234-ef00-1234567890ab");
Variable* soilTemp2 =
    new MeterTeros12_Temp(&probe2, "12345678-abcd-1234-ef00-1234567890ab");
Variable* soilEa2 =
    new MeterTeros12_Ea(&probe2, "12345678-abcd-1234-ef00-1234567890ab");
Variable* vwc2 =
    new MeterTeros12_VWC(&probe2, "12345678-abcd-1234-ef00-1234567890ab");
Variable* ec2 =
    new MeterTeros12_EC(&probe2, "12345678-abcd-1234-ef00-1234567890ab");

Variable* rawVWCCounts3 = 
    new MeterTeros12_Count(&probe3, "12345678-abcd-1234-ef00-1234567890ab");
Variable* soilTemp3 =
    new MeterTeros12_Temp(&probe3, "12345678-abcd-1234-ef00-1234567890ab");
Variable* soilEa3 =
    new MeterTeros12_Ea(&probe3, "12345678-abcd-1234-ef00-1234567890ab");
Variable* vwc3 =
    new MeterTeros12_VWC(&probe3, "12345678-abcd-1234-ef00-1234567890ab");
Variable* ec3 =
    new MeterTeros12_EC(&probe3, "12345678-abcd-1234-ef00-1234567890ab");


Variable* variableList[] = {
    new ProcessorStats_SampleNumber(&mcuBoard),
    new ProcessorStats_FreeRam(&mcuBoard),
    new ProcessorStats_Battery(&mcuBoard), 
    new MaximDS3231_Temp(&ds3231),
    sonarRange,
    calculatedSnowDepth,
    sp510volts,
    sp510rad,
    sp610volts,
    sp610rad,
    sl510thermistorVolts,
    sl510thermopileVolts,
    sl510rad,
    sl610thermistorVolts,
    sl610thermopileVolts,
    sl610rad,
    rawVWCCounts1,
    soilTemp1,
    soilEa1,
    vwc1,
    ec1,
    rawVWCCounts2,
    soilTemp2,
    soilEa2,
    vwc2,
    ec2,
    rawVWCCounts3,
    soilTemp3,
    soilEa3,
    vwc3,
    ec3
};

// The number of UUID's must match the number of variables!
const char* UUIDs[] = {
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
};

// Count up the number of pointers in the array
uint8_t variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);


// ==========================================================================
// The Logger Object[s]
// ==========================================================================
// Create a logger instance
Logger dataLogger;


// ==========================================================================
// Working Functions
// ==========================================================================
// Flashes the LED's on the primary board
void greenredflash(uint8_t numFlash = 4, uint8_t rate = 75) {
    for (uint8_t i = 0; i < numFlash; i++) {
        digitalWrite(greenLED, HIGH);
        digitalWrite(redLED, LOW);
        delay(rate);
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, HIGH);
        delay(rate);
    }
    digitalWrite(redLED, LOW);
}


// ==========================================================================
// Arduino Setup Function
// ==========================================================================
void setup() {
    // Start the primary serial connection
    Serial.begin(serialBaud);

    // Setup XBee
    pinMode(xbeeRegulatorPin, OUTPUT);
    pinMode(xbeeSleepPin, OUTPUT);
    digitalWrite(xbeeRegulatorPin, HIGH);  // Supply power to the XBee regulator
    Serial1.begin(xbeeBaud);  // Begin UART-1 communication
    Wire.begin();
    digitalWrite(xbeeSleepPin, HIGH);  // Put XBee in sleep mode

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(sketchName);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);
    Serial.println();

    Serial.print(F("Using ModularSensors Library version "));
    Serial.println(MODULAR_SENSORS_VERSION);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set the timezones for the logger/data and the RTC
    // Logging in the given time zone
    Logger::setLoggerTimeZone(timeZone);
    // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
    Logger::setRTCTimeZone(0);

    // Set information pins
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                             greenLED);

    // Begin the variable array[s], logger[s], and publisher[s]
    varArray.begin(variableCount, variableList);
    dataLogger.begin(LoggerID, loggingInterval, &varArray);

    // Set up the sensors
    Serial.println(F("Setting up sensors..."));
    varArray.setupSensors();

    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct and
    // all sensor names correct
    dataLogger.createLogFile(true);  // true = write a new header

    // Call the processor sleep
    dataLogger.systemSleep();

    sonarSerial.begin(9600);
}


// ==========================================================================
// Arduino Loop Function
// ==========================================================================
void loop() {
  dataLogger.logData();  
}
