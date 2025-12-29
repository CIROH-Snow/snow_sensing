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

// ==========================================================================
//  Data Logging Options
// ==========================================================================
// The name of this program file
const char* sketchName = "sp710_only.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "23151_testingSP710atTGRS";
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
//  Using the Processor as a Sensor
// ==========================================================================
#include <sensors/ProcessorStats.h>

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v1.1";
ProcessorStats mcuBoard(mcuBoardVersion);


// ==========================================================================
//  Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>  // Includes wrapper functions for Maxim DS3231 RTC

// Create a DS3231 sensor object, using this constructor function:
MaximDS3231 ds3231(1);


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
  RED     -> SW12     Heater
  GREEN   -> GND      Heater ground
*/
ApogeeSP510 sp510(sensorPowerPin, 0x48, 50);  // 

Variable* sp510volts =
    new ApogeeSP510_Voltage(&sp510, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sp510rad =
    new ApogeeSP510_ISWR(&sp510, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Apogee SP-610-SS for outgoing shortwave radiation
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

Variable* variableList[] = {
    new ProcessorStats_SampleNumber(&mcuBoard),
    new ProcessorStats_FreeRam(&mcuBoard),
    new ProcessorStats_Battery(&mcuBoard), 
    new MaximDS3231_Temp(&ds3231),
    sp510volts,
    sp510rad,
    sp610volts,
    sp610rad
};

//  The number of UUID's must match the number of variables!
const char* UUIDs[] = {
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab",
    "12345678-abcd-1234-ef00-1234567890ab"
};

// Count up the number of pointers in the array
uint8_t variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);


// ==========================================================================
//  The Logger Object[s]
// ==========================================================================
// Create a logger instance
Logger dataLogger;


// ==========================================================================
//  Working Functions
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


void setup() {
    // Start the primary serial connection
    Serial.begin(serialBaud);

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


void loop() {
  // put your main code here, to run repeatedly:
  dataLogger.logData();
}
