/** =========================================================================
 * @file sl510_only.ino
 * @brief A simple data logging example for the SL-510-SS.
 *
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @copyright (c) 2017-2022 Stroud Water Research Center (SWRC)
 *                          and the EnviroDIY Development Team
 *            This example is published under the BSD-3 license.
 *
 * Build Environment: Visual Studios Code with PlatformIO
 * Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
 *
 * DISCLAIMER:
 * THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
 * ======================================================================= */

// ==========================================================================
//  Include the libraries required for any data logger
// ==========================================================================
/** Start [includes] */
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// EnableInterrupt is used by ModularSensors for external and pin change
// interrupts and must be explicitly included in the main program.
#include <EnableInterrupt.h>

// Include the main header for ModularSensors
#include <ModularSensors.h>
/** End [includes] */

#include <Wire.h>

// ==========================================================================
//  Defines for the Arduino IDE
//  NOTE:  These are ONLY needed to compile with the Arduino IDE.
//         If you use PlatformIO, you should set these build flags in your
//         platformio.ini
// ==========================================================================
/** Start [defines] */

/** End [defines] */

// ==========================================================================
//  Data Logging Options
// ==========================================================================
/** Start [logging_options] */
// The name of this program file
const char* sketchName = "sl510_only.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "23151_testingSL510atTGRS";
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
/** End [logging_options] */


// ==========================================================================
//  Using the Processor as a Sensor
// ==========================================================================
/** Start [processor_sensor] */
#include <sensors/ProcessorStats.h>

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v1.1";
ProcessorStats mcuBoard(mcuBoardVersion);
/** End [processor_sensor] */


// ==========================================================================
//  Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
/** Start [ds3231] */
#include <sensors/MaximDS3231.h>  // Includes wrapper functions for Maxim DS3231 RTC

// Create a DS3231 sensor object, using this constructor function:
MaximDS3231 ds3231(1);
/** End [ds3231] */


// ==========================================================================
//    Apogee SL-510-SS for incoming longwave radiation
// ==========================================================================
#include <sensors/ApogeeSL610.h>

/* WIRING FOR SL-510
  WHITE   -> AA3
  BLACK   -> AA2
  GREEN   -> AA1
  YELLOW  -> SW12
  BLUE    -> GND
  RED     -> SW3
  CLEAR   -> GND
*/
ApogeeSL610 sl610(sensorPowerPin, 1, 0x4A, 50);  // Note that this sensor is not attached to the board ADC

Variable* sl610thermistorVolts =
    new ApogeeSL610_Thermistor_Voltage(&sl610, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl610thermopileVolts =
    new ApogeeSL610_Thermopile_Voltage(&sl610, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl610rad =
    new ApogeeSL610_OLWR(&sl610, "12345678-abcd-1234-ef00-1234567890ab");


Variable* variableList[] = {
    new ProcessorStats_SampleNumber(&mcuBoard),
    new ProcessorStats_FreeRam(&mcuBoard),
    new ProcessorStats_Battery(&mcuBoard), 
    new MaximDS3231_Temp(&ds3231),
    sl610thermistorVolts,
    sl610thermopileVolts,
    sl610rad
    // Additional sensor variables can be added here, by copying the syntax
    //   for creating the variable pointer (FORM1) from the
    //   `menu_a_la_carte.ino` example
    // The example code snippets in the wiki are primarily FORM2.
};

const char* UUIDs[] = {
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",    
    //  ... The number of UUID's must match the number of variables!
    "12345678-abcd-1234-ef00-1234567890ab",
};

// Count up the number of pointers in the array
uint8_t variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);
/** End [variable_arrays] */


// ==========================================================================
//  The Logger Object[s]
// ==========================================================================
/** Start [loggers] */
// Create a logger instance
Logger dataLogger;
/** End [loggers] */


// ==========================================================================
//  Working Functions
// ==========================================================================
/** Start [working_functions] */
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
/** End [working_functions] */


// ==========================================================================
//  Arduino Setup Function
// ==========================================================================
/** Start [setup] */
void setup() {
    // Start the primary serial connection
    Serial.begin(serialBaud);

    Wire.begin();
 
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

}
/** End [setup] */


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [loop] */

void loop() {
  dataLogger.logData();
}
/** End [loop] */
