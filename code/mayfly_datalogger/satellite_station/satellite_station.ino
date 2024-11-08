/** =========================================================================
 * @file simple_logging.ino
 * @brief A simple data logging example.
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

#include <AltSoftSerial.h>  // AltSoftSerial is the best software serial option since
                            // both hardware serials are used when an XBee is used.
AltSoftSerial sonarSerial(6, -1);  // The -1 indicates that no Tx wire is attached.

// ==========================================================================
//  Defines for the Arduino IDE
//  NOTE:  These are ONLY needed to compile with the Arduino IDE.
//         If you use PlatformIO, you should set these build flags in your
//         platformio.ini
// ==========================================================================
/** Start [defines] */
#define xbeeSleepPin 23
#define xbeeRegulatorPin 18
#define powerRelayPin 10
/** End [defines] */

// ==========================================================================
//  XBee Pro S3B Options
// ==========================================================================
const int xbeeBaud = 9600;

// ==========================================================================
//  Data Logging Options
// ==========================================================================
/** Start [logging_options] */
// The name of this program file
const char* sketchName = "satellite_station.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "Marshes";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 60;
// Your logger's timezone.
const int8_t timeZone = -7;  // Mountain Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud = 9600;  // Baud rate for debugging
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
//    MaxBotix sonar sensor for snow depth
// ==========================================================================
#include <sensors/MaxBotixSonar.h>

const int32_t sonarHeight = 2260;
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
  YELLOW  -> AA2 (0x48)
  BLUE    -> AA3 (0x48)
  RED     -> V12+     Heater
  GREEN   -> V12 GND  Heater ground
*/
ApogeeSP510 sp510(sensorPowerPin, 0x48, 3);  // 

Variable* sp510volts =
    new ApogeeSP510_Voltage(&sp510, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sp510rad =
    new ApogeeSP510_ISWR(&sp510, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Apogee SP-610-SS for outgoing shortwave radiation
// ==========================================================================
#include <sensors/ApogeeSP610.h>

/* WIRING FOR *SP-710* DOWNWARD SENSOR
  WHITE   -> AA0 (0x48)
  BLACK   -> AA1 (0x48)
  CLEAR   -> GND      Shield
*/
ApogeeSP610 sp610(sensorPowerPin, 0x48, 3);

Variable* sp610volts =
    new ApogeeSP610_Voltage(&sp610, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sp610rad =
    new ApogeeSP610_OSWR(&sp610, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Apogee SL-510-SS for incoming longwave radiation
// ==========================================================================
#include <sensors/ApogeeSL510.h>

/* WIRING FOR SL-510
  WHITE   -> AA0 (0x4A)
  BLACK   -> AA1 (0x4A)
  GREEN   -> AA1 (0x49)
  YELLOW  -> V12+
  BLUE    -> V12 GND
  RED     -> SW3
  CLEAR   -> GND
*/

ApogeeSL510 sl510(sensorPowerPin, 1, 0x49, 0x4A, 3);  // Note that this sensor is not attached to the board ADC

Variable* sl510thermistorVolts =
    new ApogeeSL510_Thermistor_Voltage(&sl510, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl510thermopileVolts =
    new ApogeeSL510_Thermopile_Voltage(&sl510, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl510rad =
    new ApogeeSL510_ILWR(&sl510, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Apogee SL-610-SS for incoming longwave radiation
// ==========================================================================
#include <sensors/ApogeeSL610.h>

/* WIRING FOR SL-610
  WHITE   -> AA2 (0x4A)
  BLACK   -> AA3 (0x4A)
  GREEN   -> AA2 (0x49)
  YELLOW  -> SW12
  BLUE    -> GND
  RED     -> SW3
  CLEAR   -> GND
*/
ApogeeSL610 sl610(sensorPowerPin, 2, 0x49, 0x4A, 3);  // Note that this sensor is not attached to the board ADC

Variable* sl610thermistorVolts =
    new ApogeeSL610_Thermistor_Voltage(&sl610, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl610thermopileVolts =
    new ApogeeSL610_Thermopile_Voltage(&sl610, "12345678-abcd-1234-ef00-1234567890ab");

Variable* sl610rad =
    new ApogeeSL610_OLWR(&sl610, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    METER Teros 12 Soil Moisture Sensors
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

// If you are using more than one soil sensor, make sure to differentiate between 
// the different depths using the third parameter of the variable declaration
Variable* rawVWCCounts1 = 
    new MeterTeros12_Count(&probe1, "12345678-abcd-1234-ef00-1234567890ab", "rawCounts_2in");
Variable* soilTemp1 =
    new MeterTeros12_Temp(&probe1, "12345678-abcd-1234-ef00-1234567890ab", "soilTemp_2in");
Variable* soilEa1 =
    new MeterTeros12_Ea(&probe1, "12345678-abcd-1234-ef00-1234567890ab", "soilEa_2in");
Variable* vwc1 =
    new MeterTeros12_VWC(&probe1, "12345678-abcd-1234-ef00-1234567890ab", "soilVWC_2in");
Variable* ec1 =
    new MeterTeros12_EC(&probe1, "12345678-abcd-1234-ef00-1234567890ab", "soilEC_2in");

Variable* rawVWCCounts2 = 
    new MeterTeros12_Count(&probe2, "12345678-abcd-1234-ef00-1234567890ab", "rawCounts_8in");
Variable* soilTemp2 =
    new MeterTeros12_Temp(&probe2, "12345678-abcd-1234-ef00-1234567890ab", "soilTemp_8in");
Variable* soilEa2 =
    new MeterTeros12_Ea(&probe2, "12345678-abcd-1234-ef00-1234567890ab", "soilEa_8in");
Variable* vwc2 =
    new MeterTeros12_VWC(&probe2, "12345678-abcd-1234-ef00-1234567890ab", "soilVWC_8in");
Variable* ec2 =
    new MeterTeros12_EC(&probe2, "12345678-abcd-1234-ef00-1234567890ab", "soilEC_8in");

Variable* rawVWCCounts3 = 
    new MeterTeros12_Count(&probe3, "12345678-abcd-1234-ef00-1234567890ab", "rawCounts_20in");
Variable* soilTemp3 =
    new MeterTeros12_Temp(&probe3, "12345678-abcd-1234-ef00-1234567890ab", "soilTemp_20in");
Variable* soilEa3 =
    new MeterTeros12_Ea(&probe3, "12345678-abcd-1234-ef00-1234567890ab", "soilEa_20in");
Variable* vwc3 =
    new MeterTeros12_VWC(&probe3, "12345678-abcd-1234-ef00-1234567890ab", "soilVWC_20in");
Variable* ec3 =
    new MeterTeros12_EC(&probe3, "12345678-abcd-1234-ef00-1234567890ab", "soilEC_20in");

// ==========================================================================
//    Apogee ST-110-SS for Air Temperature
// ==========================================================================
#include <sensors/ApogeeST110.h>

/* WIRING FOR ST-110
  WHITE   -> AA0 (0x49)
  BLACK   -> GND
  RED     -> SW3
  CLEAR   -> GND
*/

ApogeeST110 st110(sensorPowerPin, 0, 0x49, 50);

Variable* st110thermistorVolts =
    new ApogeeST110_Thermistor_Voltage(&st110, "12345678-abcd-1234-ef00-1234567890ab");

Variable* st110airTemp =
    new ApogeeST110_AIRT(&st110, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Variable List
// ==========================================================================

Variable* variableList[] = {
    new ProcessorStats_SampleNumber(&mcuBoard),
    //new ProcessorStats_FreeRam(&mcuBoard),
    //new ProcessorStats_Battery(&mcuBoard), 
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
    ec3,
    st110thermistorVolts,
    st110airTemp
    // Additional sensor variables can be added here, by copying the syntax
    //   for creating the variable pointer (FORM1) from the
    //   `menu_a_la_carte.ino` example
    // The example code snippets in the wiki are primarily FORM2.
};

const char* UUIDs[] = {
    "12345678-abcd-1234-ef00-1234567890ab",
    //"12345678-abcd-1234-ef00-1234567890ab",
    //"12345678-abcd-1234-ef00-1234567890ab",
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

void serialPrintBuffer(byte buffer[], int bufferSize) {
  for (int i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i], HEX);
    if (i == bufferSize - 1) {
      Serial.println();
    } else {
      Serial.print(" ");
    }
  }
}

void serialPrintTransmit(char mess[], int messSize, byte frameID, byte destAddress64[], byte destAddress16[], byte broadcastRadius, byte options) {
  Serial.print(0x7E, HEX);
  Serial.print(" ");
  Serial.print(lengthMSB(messSize), HEX);
  Serial.print(" ");
  Serial.print(lengthLSB(messSize), HEX);
  Serial.print(" ");
  Serial.print(0x10, HEX);
  Serial.print(" ");
  Serial.print(frameID, HEX);
  Serial.print(" ");
  for (int i = 0; i < 8; i++) {
    Serial.print(destAddress64[i], HEX);
    Serial.print(" ");
  }
  for (int i = 0; i < 2; i++) {
    Serial.print(destAddress16[i], HEX);
    Serial.print(" ");
  }
  Serial.print(broadcastRadius, HEX);
  Serial.print(" ");
  Serial.print(options, HEX);
  Serial.print(" ");
  for (int i = 0; i < messSize; i++) {
    Serial.print(mess[i], HEX);
    Serial.print(" ");
  }
  Serial.println(checksumString(mess, messSize, frameID, broadcastRadius, options), HEX);
}
/** End [working_functions] */


// ==========================================================================
//  Radio Communications
// ==========================================================================
// This is the setup for an XBee Tx (Transmit) Request using 64-bit addresses
// The following are some of the basic parameters
// Make sure bytes are written in hexadecimal format (0x__)
// I tried to keep the parameters in the order they are sent

byte highAddress[] = {0x00, 0x13, 0xA2, 0x00, 0x42, 0x2F, 0xE2, 0x12};  // This
                            // is the 64-bit destination address of the receiving XBee module
byte lowAddress[] = {0xFF, 0xFE};  // 16-bit destination address using FF FE if the address is unknown

//int sum;

char ready[] = "R";
char error[] = "E";

uint32_t previousEpoch;
String dataToSend;
byte rx[64];  // Buffer for incoming data

// Calculate the most signifcant byte (assumes that message includes a null terminator, i.e. use sizeof(message) for input)
byte lengthMSB(int size) {
  return byte((13 + size) / 255);
}

// Calculate the least significant byte (assumes the message includes a null terminator, i.e. use sizeof(message) for input)
byte lengthLSB(int size) {
  return byte((13 + size) % 255);
}

// Checksum of a transmit request frame whose payload is a string
byte checksumString(char rfData[], int size, byte frameID, byte broadcastRadius, byte options) {
  int sum;
  sum = 0;
  sum += 0x10;
  sum += frameID;
  for (int i = 0; i <= sizeof(highAddress) - 1; i++) {
    sum += highAddress[i];
  }
  for (int i = 0; i <= sizeof(lowAddress) - 1; i++) {
    sum += lowAddress[i];
  }
  sum += options;
  sum += broadcastRadius;
  for (int i = 0; i < size - 1; i++) {
    sum += byte(rfData[i]);
  }
  return byte(255 - (sum % 256));
}

// Checksum of a transmit request frame whose payload is a singular byte
byte checksumByte(uint8_t byteMessage, byte frameID, byte broadcastRadius, byte options) {
  uint8_t sum;
  sum = 0;
  sum += 0x10;
  sum += frameID;
  for (int i = 0; i <= sizeof(highAddress) - 1; i++) {
    sum += highAddress[i];
  }
  for (int i = 0; i <= sizeof(lowAddress) - 1; i++) {
    sum += lowAddress[i];
  }
  sum += options;
  sum += broadcastRadius;
  sum += byteMessage;
  return 255 - (sum % 256);
}

// Sends a transmit request frame to the XBee with a string payload
void transmitString(char message[], int messageSize, byte frameID, byte broadcastRadius, byte options) {
  Serial1.write(0x7E);  // Starting delimeter
  Serial1.write(lengthMSB(messageSize));
  Serial1.write(lengthLSB(messageSize));
  Serial1.write(0x10);  // Frame type
  Serial1.write(frameID);
  Serial1.write(highAddress, 8);
  Serial1.write(lowAddress, 2);
  Serial1.write(broadcastRadius);
  Serial1.write(options);
  Serial1.write(message);
  Serial1.write(checksumString(message, messageSize, frameID, broadcastRadius, options));
}

// Sends a transmit request frame with a singular byte payload
// This is nice for when numbers need to be sent
void transmitByte(uint8_t byteMessage, byte frameID, byte broadcastRadius, byte options) {
  Serial1.write(0x7E);  // Starting delimeter
  Serial.write(0x00);  // Most significant byte length (this is a fixed value when doing a transmit request with only one byte for the payload)
  Serial.write(0x0F);  // Least significant byte length (this is a fixed value when doing a transmit request with only one byte for the payload)
  Serial1.write(0x10);  // Frame type (0x10 is used to identify a transmit request)
  Serial1.write(frameID);  // Frame ID set to 0x00 so we don't get a transmit status frame in the buffer
  Serial1.write(highAddress, 8);  // 64-bit address
  Serial1.write(lowAddress, 2);  // 16-bit address
  Serial1.write(broadcastRadius);  // Broadcast radius
  Serial1.write(options);  // Options
  Serial1.write(byteMessage);
  Serial1.write(byte(checksumByte(byteMessage, frameID, broadcastRadius, options)));
}


// ==========================================================================
//  Arduino Setup Function
// ==========================================================================
/** Start [setup] */
void setup() {
    // Start the primary serial connection
    Serial.begin(serialBaud);

    // Power relay setup
    pinMode(10, OUTPUT);
    digitalWrite(10, LOW);

    // Setup XBee
    pinMode(xbeeRegulatorPin, OUTPUT);
    pinMode(xbeeSleepPin, OUTPUT);
    digitalWrite(xbeeRegulatorPin, HIGH);  // Supply power to the XBee regulator
    Serial1.begin(xbeeBaud);  // Begin UART-1 communication
    Wire.begin();
    digitalWrite(xbeeSleepPin, LOW);  // Waking up the XBee momentarily to clear out anything that could potentially be in the buffer
    memset(rx, 0x00, sizeof(rx));
    delay(100);
    int count = Serial1.available();
    Serial1.readBytes(rx, count);
    memset(rx, 0x00, sizeof(rx));
    digitalWrite(xbeeSleepPin, HIGH);  // Put the XBee back to sleep

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

    pinMode(10, OUTPUT);
}
/** End [setup] */


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [loop] */

void loop() {

  // Closing the power relay (NC) to supply power to the radiometer heaters
  if (dataLogger.getNowLocalEpoch() % (loggingInterval * 60) == 2700) {  // Check if the scanning interval is on the 45th minute (15 minutes before logging)
    digitalWrite(22, HIGH);  // Power up the power relay
    delay(1000);
    digitalWrite(powerRelayPin, HIGH);  // Set the signal pin high (NC requires a pin drop)
    delay(1000);
    digitalWrite(powerRelayPin, LOW);  // Close the circuit by dropping relay signal pin
    digitalWrite(22, LOW);  // Turn off switched power
  }
  
  // If there is a new marked time, update what we have on record
  if (previousEpoch != dataLogger.markedLocalEpochTime) {
    previousEpoch = dataLogger.markedLocalEpochTime;
  }

  Serial.println("Begin logData");
  dataLogger.logData();  // During logData, the marked time will update when a new measurement is taken
  Serial.println("End logData");
  
  // Checking if new data has been collected by checking if there is a new marked time
  //bool not_time = true;
  if (previousEpoch != dataLogger.markedLocalEpochTime) {  // If it's time to wake the XBee
    //Serial.println("It is time!");
    digitalWrite(redLED, HIGH);
    //delay(30000);
    //digitalWrite(greenLED, LOW);
    //memset(rx, 0x00, sizeof(rx));
    digitalWrite(xbeeSleepPin, LOW);  // Wake the XBee up
    delay(100);
    

    uint8_t varCount;
    uint8_t varNum;

    // Clear out the buffer upon wake up
    uint8_t wakeUpCount = Serial1.available();  // The amount of bytes in the Rx buffer on wakeup
    Serial1.readBytes(rx, wakeUpCount);  // Read out the bytes from the buffer
    memset(rx, 0x00, sizeof(rx));  // Clear where we just stored those bytes

    bool hostReady = false;  // Is the host station ready to get data?
    bool heardNothing = false;
    uint32_t starttime = dataLogger.getNowLocalEpoch();
    //digitalWrite(redLED, HIGH);
    while (Serial1.available() == 0) {  // Stay in this loop until more bytes enter the buffer
      if (dataLogger.getNowLocalEpoch() - starttime > 600) {  // Wait seven minutes for a new message
        heardNothing = true;  // After five minutes, we did not hear anything new coming in
        break;  // Leave the loop
      } 
    }
    delay(100);  // Give a chance for the full message to come through
    if (heardNothing) {
      //greenredflash(20);
      digitalWrite(redLED, LOW);  // If we said that nothing came through
      digitalWrite(xbeeSleepPin, HIGH);  // Send the XBee back to sleep
    } else {  // If we said that something did come through that was new
      int newCount = Serial1.available();
      Serial1.readBytes(rx, newCount);
      if (rx[15] == 0x52) {  // If the message we received was 'R' (ASCII character for 0x52)
        hostReady = true;
        //greenredflash(20);
        transmitString(ready, sizeof(ready), 0x00, 0x00, 0x00);  // Let the host know we are ready
      } else {  // If it wasn't an 'R' that came through, send an error message 'E'
        transmitString(error, sizeof(error), 0x00, 0x00, 0x00);  // Let the host know we aren't ready
      }
      //digitalWrite(redLED, LOW);
      memset(rx, 0x00, sizeof(rx));
      if (hostReady) {
        greenredflash(10);
        // Wait for the host station to request the previous timestamp and send it when it does
        bool timeRequested = false;
        
        uint32_t starttime = dataLogger.getNowLocalEpoch();
        while (Serial1.available() == 0) {  // Wait for host request
          //greenredflash(20);
          if (dataLogger.getNowLocalEpoch() - starttime > 60) {  // Wait a minute for a timestamp request
            //greenredflash(20);
            break;
          }
        }
        delay(100);
        if (Serial1.available() != 0) {  // If something came through
          //greenredflash(20);
          Serial1.readBytes(rx, Serial1.available());  // Read the bytes in
          if (rx[3] == 0x90 && rx[15] == 0x54) {  // Check if it was a receive packet (3rd byte is 0x90) and that the message is 'T' (0x54)
            timeRequested = true;
          }
        }
        if (timeRequested) {
          
          String datetime = "";
          dataLogger.dtFromEpoch(dataLogger.markedLocalEpochTime).addToString(datetime);
          char timestamp[datetime.length() + 1]; 
          datetime.toCharArray(timestamp, sizeof(timestamp));
          transmitString(timestamp, sizeof(timestamp), 0x00, 0x00, 0x00);    
        }
        memset(rx, 0x00, sizeof(rx));

        // Wait for the host station to request the variable count and send it when it does
        bool varCountRequested = false;
        starttime = dataLogger.getNowLocalEpoch();
        while (Serial1.available() == 0) {  // Wait for host request
          if (dataLogger.getNowLocalEpoch() - starttime > 60) {  // Wait a minute for a varCount request
            break;
          }
        }
        delay(100);
        if (Serial1.available() != 0) {  // If something came through
          Serial1.readBytes(rx, Serial1.available());  // Read the bytes in
          if (rx[3] == 0x90 && rx[15] == 0x56) {
            //greenredflash(20);  // Check if it was a receive packet (3rd byte is 0x90) and that the message is 'V' (0x56)
            varCountRequested = true;
          }
        }
        if (varCountRequested) {
          int sum = 0;
          sum += 0x10;
          sum += 0x00;
          for (int i = 0; i < 8; i++) {
            sum += highAddress[i];
          }
          for (int i = 0; i < 2; i++) {
            sum += lowAddress[i];
          }
          sum += 0x00;
          sum += 0x00;
          
          varCount = dataLogger.getArrayVarCount();

          sum += varCount;
          sum = 255 - (sum % 256);

          Serial1.write(0x7E);
          Serial1.write(0x00);
          Serial1.write(0x0F);
          Serial1.write(0x10);
          Serial1.write(0x00);
          Serial1.write(highAddress, 8);
          Serial1.write(lowAddress, 2);
          Serial1.write(0x00);
          Serial1.write(0x00);
          Serial1.write(byte(varCount));
          Serial1.write(byte(sum));
          //greenredflash(20);
        }
        memset(rx, 0x00, sizeof(rx));

        // Send all the data points requested
        bool allDataSent = false;
        while (!allDataSent) {
          //greenredflash(20);
          memset(rx, 0x00, sizeof(rx));
          bool breakWhile = false;
          starttime = dataLogger.getNowLocalEpoch();
          while (Serial1.available() == 0) {  // Wait for host request
            if (dataLogger.getNowLocalEpoch() - starttime > 60) {  // Wait a minute for a variable request
              breakWhile = true;
              break;
            }
          }
          delay(100);
          if (breakWhile) {  // If no data request came through, break the loop
            break;
          }
          Serial1.readBytes(rx, Serial1.available());  // Move what was received in
          if (rx[3] == 0x90 && rx[15] < varCount) {  // If it was a received packet and a number less than the varCount
            //digitalWrite(greenLED, HIGH);
            delay(20);
            varNum = rx[15];
            memset(rx, 0x00, sizeof(rx));
            Serial.print("varNum: ");
            Serial.println(varNum);
            String varName = "";
            varName += dataLogger.getVarCodeAtI(varNum);
            char name[varName.length() + 1];
            varName.toCharArray(name, sizeof(name));
            Serial.print("Variable name: ");
            Serial.println(name);
            transmitString(name, sizeof(name), 0x00, 0x00, 0x00);
            memset(rx, 0x00, sizeof(rx));
            //digitalWrite(greenLED, LOW);
          } else {  // If what was received is not a valid number request
            break;
          }       

          // Wait for the host to send a packet with payload 'N' to denote that it is ready to receive the data value for the variable just sent
          starttime = dataLogger.getNowLocalEpoch();
          while (Serial1.available() == 0) {  // Wait for host request
            if (dataLogger.getNowLocalEpoch() - starttime > 60) {  // Wait a minute for a variable request
              breakWhile = true;
              break;
            }
          }
          if (breakWhile) {
            break;
          }
          delay(100);
          Serial1.readBytes(rx, Serial1.available());  // Read in the bytes
          if (rx[15] == 0x4E) {  // If the payload was an 'N'
            //if (varNum == varCount - 1) {
            //  greenredflash(20);
            //}
            //greenredflash(20);
            //digitalWrite(redLED, HIGH);
            delay(20);
            String dataValue = "";  // Create a String where the value can be stored
            dataValue += dataLogger.getValueStringAtI(varNum);  // Grab the value and put it in the String
            char val[dataValue.length() + 1];  // Create a string that will actually send
            dataValue.toCharArray(val, sizeof(val));  // Turn the String into a string
            Serial.print("varNum: ");
            Serial.println(varNum);
            Serial.print("val: ");
            Serial.println(val);
            transmitString(val, sizeof(val), 0x00, 0x00, 0x00);  // Send the data value in a packet
            memset(rx, 0x00, sizeof(rx));
            digitalWrite(redLED, LOW);
            //greenredflash(20);
          }
          if (varNum == varCount - 1) {
            allDataSent = true;

            // for some reason, it will not send the last variable measured until the XBee is powered off then powered on again, so this catches that
            digitalWrite(xbeeSleepPin, HIGH);
            delay(100);
            digitalWrite(xbeeSleepPin, LOW);
            delay(100);
          }
        }
      }
    }
    digitalWrite(xbeeSleepPin, HIGH);
  }
}
/** End [loop] */
