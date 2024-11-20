// This is the Mayfly sketch for the deployment of a snow station for the CIROH: Advaning Snow Observations project.

/** =========================================================================
 * @file satellite_station.ino
 * @brief Arduino code for the Advancing Snow Observations CIROH project
 *
 * @author Braedon Dority <braedon.dority@usu.edu>
 * @author Jeff Horsburgh <jeff.horsburgh@usu.edu>
 * @copyright (c) 2017-2022 Utah Water Research Laboratory (UWRL)
 *            This example is published under the BSD-3 license.
 *
 * Build Environment: Visual Studios Code with PlatformIO
 * Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
 *
 * DISCLAIMER:
 * THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
 *
 * ACKNOWLEDGEMENTS:
 * Thank you to the Stroud Water Research Center for developing the foundation
 * to this code. This sketch is based on the examples made from their modular
 * sensors library. The work of Sarah Damiano and Shannon Hicks made this
 * possible.
 * ======================================================================= */

// ==========================================================================
//  Include the libraries required for the data logger
// ==========================================================================
/** Start [includes] */
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// EnableInterrupt is used by ModularSensors for external and pin change
// interrupts and must be explicitly included in the main program.
#include <EnableInterrupt.h>

// Include the main header for ModularSensors
#include <ModularSensors.h>

// This helps manage I2C functionality
#include <Wire.h>

// Hardware serial ports are limited or already designated for specific uses
// The MaxBotix sonar sensor requires serial communication to send data
// We can create a serial port on one of the digital pins using this software
#include <AltSoftSerial.h> 

/** End [includes] */

// ==========================================================================
//  Defines for the Arduino IDE
//  NOTE:  These are ONLY needed to compile with the Arduino IDE.
//         If you use PlatformIO, you should set these build flags in your
//         platformio.ini
// ==========================================================================
/** Start [defines] */
// We need to tell the Mayfly where the XBee's asyncronous sleep pin is connected to it
#define xbeeSleepPin 23

// The pin that controls power supply to the XBee modules
#define xbeeRegulatorPin 18

// This pin will interface the Adafruit power relay for the Apogee sensors' heaters with the Mayfly
// Make sure your power relay's grove port is connected to the Mayfly's grove port that includes the
// digital pin defined here
#define powerRelayPin 10
/** End [defines] */

// ==========================================================================
//  Data Logging Options
// ==========================================================================
/** Start [logging_options] */
// The name of this program file
const char* sketchName = "satellite_station.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
// This can be found on the bottom of the Mayfly if the ink hasn't rubbed off.
// Sometimes I just use the name I've given the site here instead of the serial number.
// Make sure it is a string (in double quotations).
const char* LoggerID = "marshes";

// How frequently (in minutes) to log data
const uint8_t loggingInterval = 60;

// Your logger's timezone.
const int8_t timeZone = -7;  // Mountain Standard Time is -7
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud = 9600;  // Baud rate for debugging (this is what you'll set your serial monitor's baud rate to)
const int8_t  greenLED   = 8;       // Pin for the green LED (don't change)
const int8_t  redLED     = 9;       // Pin for the red LED (don't change)
const int8_t  buttonPin  = 21;      // Pin for debugging mode (ie, button pin, don't change)
const int8_t  wakePin    = 31;  // MCU interrupt/alarm pin to wake from sleep (don't change)

// The following is just some information if you are using an older Mayfly version (0.x)
// Mayfly 0.x D31 = A7
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1


const int8_t sdCardPwrPin   = -1;  // MCU SD card power pin (don't change)
const int8_t sdCardSSPin    = 12;  // SD card chip select/slave select pin (don't change)
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power
								   // Pin 22 is switched power, which can be 3.3V, 5V, or 12V
								   // depending on where you connect your sensors on the board
								   // and if you adjust the jumper headers (see wiring instructions
								   // in the GitHub repository in snow_sensing > hardware)
/** End [logging_options] */


// ==========================================================================
//  Using the Processor as a Sensor
// ==========================================================================
// Generally do not adjust this section unless you are not using version 1.1 for your Mayfly
/** Start [processor_sensor] */
#include <sensors/ProcessorStats.h>

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v1.1";  // If your board is a different version, change this value
ProcessorStats mcuBoard(mcuBoardVersion);
/** End [processor_sensor] */


// ==========================================================================
//  Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
// Do not change this section
/** Start [ds3231] */
#include <sensors/MaximDS3231.h>  // Includes wrapper functions for Maxim DS3231 RTC

// Create a DS3231 sensor object, using this constructor function:
MaximDS3231 ds3231(1);
/** End [ds3231] */





// The next few sections contain the declarations for each sensor. Pay attention
// to where edits need to be made. If there is a sensor you are not using, you can simply
// delete the corresponding section, but make sure to also delete its associated variables
// in the variable list found after these.


// ==========================================================================
//    MaxBotix sonar sensor for snow depth
// ==========================================================================
#include <sensors/MaxBotixSonar.h>

// Tell the Mayfly what pin will have our homemade serial port on it
AltSoftSerial sonarSerial(6, -1);  // The -1 indicates that no Tx wire is attached.
								   // (We don't need to talk to the sensor, it just
								   // needs to talk to us through a receiving pin [Rx]
								   // on the board, pin 6).

// Set the height of the sensor (in millimeters)
const int32_t sonarHeight = 2247.9;

// There is no need to continuously run this sensor when it isn't taking a measurement,
// so set the sensor's power pin to the switched pin which we already defined
const int8_t sonarPower = sensorPowerPin;

// Trigger should be a *unique* negative number if unconnected
const int8_t sonarTrigger = -1;

// How many readings do you want to average? 
const uint8_t sonarNumReadings = 50;

// Construct the sensor
MaxBotixSonar sonar(sonarSerial, sonarHeight, sonarPower, sonarTrigger, sonarNumReadings);

// Each variable that we want to report needs to be "constructed" as well.
// For this sensor it will include the range the sonar detected (in millimeters)
// and the snow depth (calculated based on the sonar height and the range measured).

// Sonar range variable construction
Variable* sonarRange =
    new MaxBotixSonar_Range(&sonar, "12345678-abcd-1234-ef00-1234567890ab");

// This function calculates the depth of snow
float calculateSnowDepth(void) {
    float calculatedResult = -9999;  // This should be what you want your error values to equal
    float inputVar1 = sonarHeight;  // The first variable input is the sonar height
    float inputVar2 = sonarRange->getValue();  // The second variable input is the sonar range measured
    // make sure both inputs are good
	// A reading of 5000 is an error for the sensor as well
	// It may seem odd to check if the first input variable isn't -9999 because it is just the sonar height we entered,
	// but for the sake of consistency it is included
    if (inputVar1 != -9999 && inputVar2 != -9999 && inputVar2 != 5000) {
      calculatedResult = inputVar1 - inputVar2;  // The snow depth is the height of the sensor minus the sonar's range measurement
    }
    return calculatedResult;
}

// Properties of the calculated snow depth

// The number of digits after the decimal place
const uint8_t calculatedSnowDepthResolution = 0;
// Variable name (this must be a value from http://vocabulary.odm2.org/variablename/)
const char* calculatedSnowDepthName = "Snow Depth";
// Variable units (this must be a value from http://vocabulary.odm2.org/units/)
const char* calculatedSnowDepthUnit = "mm";
// A short code for the variable
const char* calculatedSnowDepthCode = "snowDepth";
// The (optional) universallly unique identifier
const char* calculatedSnowDepthUUID = "12345678-abcd-1234-ef00-1234567890ab";

Variable* calculatedSnowDepth = new Variable(
    calculateSnowDepth, calculatedSnowDepthResolution, calculatedSnowDepthName,
    calculatedSnowDepthUnit, calculatedSnowDepthCode, calculatedSnowDepthUUID);

// That's the end of the code for the MaxBotix sonar







/* ** IMPORTANT ** 
ApogeeSP510, ApogeeSP610, ApogeeSL510, and ApogeeSL610 modular sensors source codes
only function on certain pins and certain hardware. Make sure you follow the wiring,
or you may otherwise damage your sensors or hardware.

Very brief wiring instructions are included in the sketch for reference, but make sure
you wire according to the directions in the hardware folder of the GitHub repository.
In this sketch, the hexidecimal (0x...) listed in parenthesis after a wiring instruction is
the address of the ADC you need to connect to:
0x48 on the Mayfly: pins noted with two As (AA0, AA1, etc.)
0x49 ADDR jumpered to VIN
0x4A ADDR jumpered to SDA
*/

// ==========================================================================
//    Apogee SP-510-SS for incoming shortwave radiation
// ==========================================================================
// There is wiring listed in this section for both an SP-710 implementation, 
// which has both the upward and downward pyranometers together, and the stand-
// alone SP-510-SS and SP-610-SS implementation. Just make sure you follow whichever
// one you are doing for your implementation. The coding itself will not change.
#include <sensors/ApogeeSP510.h>

/* WIRING FOR *SP-710-SS* SENSOR
  YELLOW  -> AA2  (0x48)
  BLUE    -> AA3  (0x48)
  RED     -> V12+ (POWER RELAY COM)
  GREEN   -> V12- (BATTERY GROUND)
  
  WIRING FOR *SP-510-SS* SENSOR
  WHITE	  -> AA2  (0x48)
  BLACK   -> AA3  (0x48)
  YELLOW  -> V12+ (POWER RELAY COM)
  BLUE    -> V12- (BATTERY GROUND)
  CLEAR   -> V12- (BATTERY GROUND)
  
*/

float sp510calibFactor = 26.03;

// Construct the Apogee SP-510-SS sensor object
ApogeeSP510 sp510(-1, sp510calibFactor, 0x48, 3);  // The -1 indicates that there is no powering up necessary for measurement 

// Construct the variable for the differential voltage measurement
Variable* sp510volts =
    new ApogeeSP510_Voltage(&sp510, "12345678-abcd-1234-ef00-1234567890ab");

// Construct the variable for the incoming shortwave radiation
Variable* sp510rad =
    new ApogeeSP510_ISWR(&sp510, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Apogee SP-610-SS for outgoing shortwave radiation
// ==========================================================================
#include <sensors/ApogeeSP610.h>

/* WIRING FOR *SP-710-SS* SENSOR
  WHITE   -> AA0  (0x48)
  BLACK   -> AA1  (0x48)
  CLEAR   -> V12- (BATTERY GROUND)
  
  WIRING FOR *SP-610-SS* SENSOR
  WHITE	  -> AA0  (0x48)
  BLACK   -> AA1  (0x48)
  YELLOW  -> V12+ (POWER RELAY COM)
  BLUE    -> V12- (BATTERY GROUND)
  CLEAR   -> V12- (BATTERY GROUND)
*/

float sp610calibFactor = 30.82;

// Construct the Apogee SP-610-SS sensor object
ApogeeSP610 sp610(-1, sp610calibFactor, 0x48, 3);

// Construct the variable for the differential voltage measurement
Variable* sp610volts =
    new ApogeeSP610_Voltage(&sp610, "12345678-abcd-1234-ef00-1234567890ab");

// Construct the variable for the outgoing shortwave radiation
Variable* sp610rad =
    new ApogeeSP610_OSWR(&sp610, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Apogee SL-510-SS for incoming longwave radiation
// ==========================================================================
#include <sensors/ApogeeSL510.h>

/* WIRING FOR *SL-510* UPWARD SENSOR
  WHITE   -> A0   (0x4A)
  BLACK   -> A1   (0x4A)
  GREEN   -> A1   (0x49)
  YELLOW  -> V12+ (POWER RELAY COM)
  BLUE    -> V12- (BATTERY GROUND)
  RED     -> SW3  (3.3V SWITCHED POWER PIN)
  CLEAR   -> V12- (BATTERY GROUND)
*/

float sl510k1 = 9.509;
float sl510k2 = 1.020;

// Construct the Apogee SL-510-SS sensor object
// The only parameter you should adjust is how many measurements you want to take (last parameter)
ApogeeSL510 sl510(sensorPowerPin, sl510k1, sl510k2, 1, 0x49, 0x4A, 3);  // Note that this sensor is not attached to the Mayfly's ADC pins (0x48)

// Construct the variable for the thermistor voltage
Variable* sl510thermistorVolts =
    new ApogeeSL510_Thermistor_Voltage(&sl510, "12345678-abcd-1234-ef00-1234567890ab");

// Construct the variable for the thermopile voltage
Variable* sl510thermopileVolts =
    new ApogeeSL510_Thermopile_Voltage(&sl510, "12345678-abcd-1234-ef00-1234567890ab");

// Construct the variable for incoming longwave radiation
Variable* sl510rad =
    new ApogeeSL510_ILWR(&sl510, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Apogee SL-610-SS for incoming longwave radiation
// ==========================================================================
#include <sensors/ApogeeSL610.h>

/* WIRING FOR SL-610
  WHITE   -> A2   (0x4A)
  BLACK   -> A3   (0x4A)
  GREEN   -> A2   (0x49)
  YELLOW  -> V12+ (POWER RELAY COM)
  BLUE    -> V12- (BATTERY GROUND)
  RED     -> SW3  (3.3V SWITCHED POWER PIN)
  CLEAR   -> V12- (BATTERY GROUND)
*/

float sl610k1 = 9.323;
float sl610k2 = 1.033;

// Construct the Apogee SL-610-SS sensor object
// The only parameter you should adjust is how many measurements you want to take (last parameter)
ApogeeSL610 sl610(sensorPowerPin, sl610k1, sl610k2, 2, 0x49, 0x4A, 3);  // Note that this sensor is not attached to the Mayfly's ADC pins (0x48)

// Construct the variable for the thermistor voltage
Variable* sl610thermistorVolts =
    new ApogeeSL610_Thermistor_Voltage(&sl610, "12345678-abcd-1234-ef00-1234567890ab");

// Construct the variable for the thermopile voltage
Variable* sl610thermopileVolts =
    new ApogeeSL610_Thermopile_Voltage(&sl610, "12345678-abcd-1234-ef00-1234567890ab");

// Construct the variable for outgoing longwave radiation
Variable* sl610rad =
    new ApogeeSL610_OLWR(&sl610, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    METER Teros 12 Soil Moisture Sensors
// ==========================================================================
#include <sensors/MeterTeros12.h>

// Set the addresses of each of the sensors
// Make sure this matches what you programmed them to be
// Change the addresses given in the single quotes ('') if you did not follow this same naming schema
char add1 = 'a';  // Address 1 at 2" depth
char add2 = 'b';  // Address 2 at 8" depth
char add3 = 'c';  // Address 3 at 20" depth

int8_t sdiPowerPin = sensorPowerPin;
int8_t sdiDataPin = 7;

// Construct each of the sensor objects
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
  WHITE   -> A0  (0x49)
  BLACK   -> GND (Mayfly ground)
  RED     -> SW3 (3.3V SWITCHED POWER PIN)
  CLEAR   -> GND (Mayfly ground)
*/

// Create the air temperature sensor object
ApogeeST110 st110(sensorPowerPin, 0, 0x49, 50);

// Create the thermistor voltage variable
Variable* st110thermistorVolts =
    new ApogeeST110_Thermistor_Voltage(&st110, "12345678-abcd-1234-ef00-1234567890ab");

// Create the air temperature variable
Variable* st110airTemp =
    new ApogeeST110_AIRT(&st110, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Variable List
// ==========================================================================

/*
This list of variables is what the program will cycle through when determining what to collect.
Note that I commented out two variables that are measurements from the Mayfly itself.
I personally didn't see the utility in collecting these variables, but if you would like them
you may remove the comment signage. Just note that the battery measurement is meaningless for
the CIROH project implementation because we are not connnecting the battery through the Mayfly's
LiPo ports. If you do decide to include these variables, make sure to uncomment the UUIDs in the 
next list as well.
*/

Variable* variableList[] = {
    new ProcessorStats_SampleNumber(&mcuBoard),
    //new ProcessorStats_FreeRam(&mcuBoard),
    //new ProcessorStats_Battery(&mcuBoard), 
    new MaximDS3231_Temp(&ds3231),
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
    st110airTemp,
    sonarRange,
    calculatedSnowDepth
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
// The EnviroDIY community created this function. It can be useful for debugging,
// such as when you want to monitor the board while it runs, you can stick this function
// in where you want to check if the datalogger ever made it to that section of the program
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
//  Radio Communications
// ==========================================================================
/** Start [radio_communications] */

/*
The following section goes through the setup of using XBee S3B radio modules for
networking multiple satellite stations together. If you do not wish to set up
a radio network, simply change the "networking" variable to false and ignore the
rest of the Radio Communications section.
*/

// Setting the XBee baud rate. Keep at 9600
const int xbeeBaud = 9600;

// This is where you decide if you will use radio networking or not in your implementation
// If you will, set the boolean variable to true; if not, set it to false;
const bool networking = true;

/*This is the serial number of the XBee S3B module that will act as the central station
that is interfacing with your Campbell Scientific datalogger. To properly program it into
this sketch, it needs to be broken up into its component bytes. This just means taking two
characters at a time from the serial address and putting those together as a hexidecimal.
Hexidecimals are designated in Arduino with an "0x" to start followed by the two-character number.
*/
byte highAddress[] = {0x00, 0x13, 0xA2, 0x00, 0x42, 0x2F, 0xE2, 0x12};  // This
                            // is the 64-bit destination address of the central XBee module
							
// The low address is not something we need to adjust, as we don't need that level of subcategorization
// Nevertheless, a low address is needed for the modules to operate, so leave it as the default: FFFE
byte lowAddress[] = {0xFF, 0xFE};  // 16-bit destination address using FF FE if the address is unknown

// This is one of the two responses the satellite station can send back to the central station after the 
// central station sends its initial contact message each logging interval
char ready[] = "R";
char error[] = "E";

// Variable declarations that will help later
uint32_t previousEpoch;  // A variable for tracking what the last time was when data was logged
String dataToSend;  // A String object that will contain the final CSV message to be sent
byte rx[64];  // Buffer (or a place to store received serial data) on the Mayfly for incoming messages from its attached XBee

// This is a function that prints out over the main serial port (Serial0, not Serial1) whatever is in a buffer
// It is mainly a debugging function for visually noting what bytes are in a buffer at a time when the function is called
void serialPrintBuffer(byte buffer[], int bufferSize) {
  for (int i = 0; i < bufferSize; i++) {  // for each byte in the buffer
    Serial.print(buffer[i], HEX);  // print out that byte as a hexidecimal over the serial line
    if (i == bufferSize - 1) {  // if it is the last byte
      Serial.println();  // print a new line
    } else {  // if it isn't the last byte
      Serial.print(" ");  // then just add a space to help distinguish between bytes
    }
  }
}

/* 
This function prints out a string message over the Serial0 line in the framing of what the XBee modules
require. This is mainly a debugging function. This is only for transmit frame types (See XBee documentation).
Each of the byte parameters are essential XBee framing parameters that the XBee requires for a transmit request.
*/
void serialPrintTransmit(char mess[], int messSize, byte frameID, byte destAddress64[], byte destAddress16[], byte broadcastRadius, byte options) {
  Serial.print(0x7E, HEX);  // Print the starting delimeter
  Serial.print(" ");
  Serial.print(lengthMSB(messSize), HEX);  // Print the MSB
  Serial.print(" ");
  Serial.print(lengthLSB(messSize), HEX);  // Print the LSB
  Serial.print(" ");
  Serial.print(0x10, HEX);  // Print the frame type (0x10 frame type is a transmit request)
  Serial.print(" ");
  Serial.print(frameID, HEX);  // Print the frame ID
  Serial.print(" ");
  for (int i = 0; i < 8; i++) {  // For each byte in the address of the XBee you want to send the message to
    Serial.print(destAddress64[i], HEX);  // Print the byte
    Serial.print(" ");
  }
  for (int i = 0; i < 2; i++) {  // Print out the two low address bytes
    Serial.print(destAddress16[i], HEX);
    Serial.print(" ");
  }
  Serial.print(broadcastRadius, HEX);  // Print the broadcast radius
  Serial.print(" ");
  Serial.print(options, HEX);  // Print the options
  Serial.print(" ");
  for (int i = 0; i < messSize; i++) {  // For each byte in the message
    Serial.print(mess[i], HEX);  // Print the byte
    Serial.print(" ");
  }
  Serial.println(checksumString(mess, messSize, frameID, broadcastRadius, options), HEX);  // Print the checksum
}

// This function calculates the most signifcant byte of a message's size
// It assumes that the message includes a null terminator, so makes sure to 
// use sizeof(*string message*) function as the entry for size
byte lengthMSB(int size) {
  return byte((13 + size) / 255);
}

// Calculates the least significant byte. Follow directions of lengthMSB for sizing input
byte lengthLSB(int size) {
  return byte((13 + size) % 255);
}

/*
Checksum of a transmit request frame whose payload is a string
A checksum is a method of quality-control in XBee radio communication. The Mayfly will send a
bunch of bytes that say its a transmit request along with a checksum byte at the end. The XBee
module will do its own checksum on the frame and check it with what the Mayfly gave. If they match
the XBee sends the message. A checksum calculation in this scenario works by adding up all the 
bytes in the frame (not including the message and the checksum byte itself), calculating the remainder
of the summed value and the largest hexidecimal number and finding the difference between that value and 
the largest hexidecimal number. It's confusing, but it works. It makes more sense in the code.
*/
byte checksumString(char rfData[], int size, byte frameID, byte broadcastRadius, byte options) {
  int sum;  // Initialize the variable storing the sum of the bytes
  sum = 0;
  sum += 0x10;  // Add the frame type byte (0x10 for transmit requests)
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
  return byte(255 - (sum % 256));  // The 256 is because there are 256 total numbers (inclduing zero), but 255 is the highest possible number
}

// Checksum of a transmit request frame whose payload is a singular byte rather than a string message
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

/*
Sends a transmit request frame to the XBee with a string payload over the Serial1 communication line
frameID can be whatever hexidecimal byte you wish
broadcastRadius should be 0x00 for the largest radius
options should be left as 0x00 unless you want to do very specific, nuanced transmissions, such as encrypted payloads,
trace routes, disabling route discoveries. If, for some reason, you wish to use these, consult the documentation on 
transmit request frames for how to construct a byte (literally bit by bit) for the options field.
*/  
void transmitString(char message[], int messageSize, byte frameID, byte broadcastRadius, byte options) {
  // Writing to Serial1 means sending a byte over Serial1 to whatever is connected on that line. If the XBee
  // is attached to the Bee header on the Mayfly, then it will be the XBee
  Serial1.write(0x7E);  // Starting delimeter never changes
  Serial1.write(lengthMSB(messageSize));
  Serial1.write(lengthLSB(messageSize));
  Serial1.write(0x10);  // Frame type is always 0x10 for a transmit request
  Serial1.write(frameID);
  Serial1.write(highAddress, 8);
  Serial1.write(lowAddress, 2);
  Serial1.write(broadcastRadius);
  Serial1.write(options);
  Serial1.write(message);
  Serial1.write(checksumString(message, messageSize, frameID, broadcastRadius, options));
}

// Sends a transmit request frame with a singular byte payload
// This is nice for when numbers need to be sent rather than strings
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

/** End [radio_communications] */

// ==========================================================================
//  Arduino Setup Function
// ==========================================================================
/** Start [setup] */
// This function always runs first after the Mayfly/Arduino is powered up,
// and only runs once
void setup() {
  // Start the primary serial connection
  Serial.begin(serialBaud);
  Wire.begin();

  // Power relay setup
  pinMode(powerRelayPin, OUTPUT);
  digitalWrite(powerRelayPin, LOW);

  // Setup XBee
  if (networking) {
	pinMode(xbeeRegulatorPin, OUTPUT);
	pinMode(xbeeSleepPin, OUTPUT);
	digitalWrite(xbeeRegulatorPin, HIGH);  // Supply power to the XBee regulator
	Serial1.begin(xbeeBaud);  // Begin UART-1 communication
	digitalWrite(xbeeSleepPin, LOW);  // Waking up the XBee momentarily to clear out anything that could potentially be in the buffer
	memset(rx, 0x00, sizeof(rx));  // Clear out the buffer where we'll store incoming data from the XBee on the Mayfly
	delay(100);
	int count = Serial1.available();  // Count up the bytes waiting to come in
	Serial1.readBytes(rx, count);  // Read in that many bytes
	memset(rx, 0x00, sizeof(rx));  // Erase them
	digitalWrite(xbeeSleepPin, HIGH);  // Put the XBee back to sleep
  }

  // Print a start-up note to the main serial port
  // This is just for if you have your laptop connected while running this program
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

  // Begin the MaxBotix's serial communication
  sonarSerial.begin(9600);
}
/** End [setup] */


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [loop] */

// This function continuously runs until the Arduino/Mayfly is no longer powered
void loop() {

  /*
  Check if it is time to power up the Apogee radiometer heaters
  You can change how long before the logging interval you turn them on by changing
  what the expression needs to be equal to. If you want it to turn on at the 45-minute
  mark (15 minutes before logging if you are logging on the hour), it needs equal 2700 
  (45 min * 60 sec/min). If you want them to turn on at the 15-minute mark, for example,
  set it equal to 15 min * 60 sec/min = 900
  */
  if (dataLogger.getNowLocalEpoch() % (loggingInterval * 60) == 900) {  // Check if we are on the 45th minute (2700 seconds into the interval)
    digitalWrite(22, HIGH);  // Power up the power relay (pin 22 is the Mayfly's switched power output)
    delay(1000);
    digitalWrite(powerRelayPin, HIGH);  // Set the signal pin high (to close the circuit, the relay is waiting for a signal drop)
    delay(1000);
    digitalWrite(powerRelayPin, LOW);  // Let the relay know it is good to close the circuit by dropping the pin to low
    digitalWrite(22, LOW);  // Turn off switched power
  }
  
  // Check if the newest marked time from logging data is different from what we previously had
  if (previousEpoch != dataLogger.markedLocalEpochTime) {  // If they are different
    previousEpoch = dataLogger.markedLocalEpochTime;  // then update to the latest logging time
  }

  dataLogger.logData();  // During logData, the marked time will update when a new measurement is taken
  
  /*
  This is where radio communications will come into play.
  We will first check if new data has been collected by verifying there is
  a new "marked" from the Mayfly that does not match what we previously had on record.
  We will then verify if you even want to do networking in the first place. This should
  have been declared up in the Radio Communications section.
  */
  if (previousEpoch != dataLogger.markedLocalEpochTime && networking) {  // If conditions are right for radio communication
    digitalWrite(redLED, HIGH);  // Turn on the red LED. This is just a nice visual aid when monitoring these loggers to let you know they have started radio communications
    digitalWrite(xbeeSleepPin, LOW);  // Wake the XBee up
    delay(100);
    
	// These variables will help housekeep
    uint8_t varCount;
    uint8_t varNum;

    // Clear out the Mayfly's buffer upon wake up in case the XBee has sent any rogue or unanticipated messages upon power up
    uint8_t wakeUpCount = Serial1.available();  // Count how many bytes are waiting to be read into the buffer
    Serial1.readBytes(rx, wakeUpCount);  // Read the bytes into the buffer
    memset(rx, 0x00, sizeof(rx));  // Clear out where we just stored those bytes by setting everything to zero

    bool hostReady = false;  // Assume the host station (central station where all data is aggregated) is not ready to get data
    bool heardNothing = false;  // Assume we have heard something from the XBee. This will change shortly if we really don't hear anything
	
    uint32_t starttime = dataLogger.getNowLocalEpoch();  // Start a timer

    while (Serial1.available() == 0) {  // Stay in this loop until more bytes enter the buffer from the XBee
      if (dataLogger.getNowLocalEpoch() - starttime > 600) {  // If we've waited for at least ten minutes (600 seconds)
        heardNothing = true;  // then we haven't heard anything
        break;  // Leave the loop
      } 
    }
    delay(100);  // Give a chance for the full message to come through
	
    if (heardNothing) {  // If we didn't hear anything from the XBee
      digitalWrite(redLED, LOW);  // Turn off the red LED
      digitalWrite(xbeeSleepPin, HIGH);  // Put the XBee back to sleep
    } else {  // If we did hear something from the XBee
      int newCount = Serial1.available();  // Count how many bytes are in the buffer
      Serial1.readBytes(rx, newCount);  // Read in all those bytes
      if (rx[15] == 0x52) {  // If the message we received was 'R' (ASCII character for 0x52)
        hostReady = true;  // Then the host station is ready to collect this station's data
        transmitString(ready, sizeof(ready), 0x00, 0x00, 0x00);  // Let the host know we are ready
      } else {  // If it wasn't an 'R' that came through, send an error message 'E'
        transmitString(error, sizeof(error), 0x00, 0x00, 0x00);  // Let the host know there was an error
      }
	  
      memset(rx, 0x00, sizeof(rx));  // Clear out the buffer for incoming messages
	  
      if (hostReady) {  // If the host station is ready for the data
        greenredflash(10);  // Give a visual cue
		
        bool timeRequested = false;  // Assume a timestamp has not been requested by the host station
        
        uint32_t starttime = dataLogger.getNowLocalEpoch();  // Set a timer
        while (Serial1.available() == 0) {  // Wait for a message from the XBee
          if (dataLogger.getNowLocalEpoch() - starttime > 60) {  // If we wait more than a minute for a message
            break;  // Leave the while loop
          }
        }
		
        delay(100);  // Give a chance for all the bytes to come in
		
        if (Serial1.available() != 0) {  // If something came through
          Serial1.readBytes(rx, Serial1.available());  // Read the bytes in
          if (rx[3] == 0x90 && rx[15] == 0x54) {  // Check if it was a receive packet (3rd byte is 0x90) and that the message is 'T' (0x54)
            timeRequested = true;  // If so, a timestamp has been requested
          }
        }
		
        if (timeRequested) {  // If a timestamp was requested
          String datetime = "";  // Create an empty String object for the datetime
		  
		  // Retrieve the datetime from the datalogger and store it in the String we just made
          dataLogger.dtFromEpoch(dataLogger.markedLocalEpochTime).addToString(datetime);
		  
          char timestamp[datetime.length() + 1];  // Create a string variable that is compatible with transmit requests
          datetime.toCharArray(timestamp, sizeof(timestamp));  // Turn the timestamp String object into a string of characters
          transmitString(timestamp, sizeof(timestamp), 0x00, 0x00, 0x00);  // Send the timestamp to the host   
        }
		
        memset(rx, 0x00, sizeof(rx));  // Clear out the buffer

        bool varCountRequested = false;  // Assume the host station has not requested a variable count
		
        starttime = dataLogger.getNowLocalEpoch();  // Start a timer
        while (Serial1.available() == 0) {  // Wait for a message
          if (dataLogger.getNowLocalEpoch() - starttime > 60) {  // If we waited more than a minute
            break;  // Then break the while loop
          }
        }
		
        delay(100);  // Give a chance for all bytes to come in if something was received
		
        if (Serial1.available() != 0 && timeRequested) {  // If something came through and we didn't miss the timestamp request
          Serial1.readBytes(rx, Serial1.available());  // Read the bytes in
          if (rx[3] == 0x90 && rx[15] == 0x56) {  // Check if it was a receive packet (3rd byte is 0x90) and that the message is 'V' (0x56)
            varCountRequested = true;  // If so, the variable count has been requested
          }
        }
		
        if (varCountRequested) {  // If the variable count has been requested
		  // We will send that number
		  // Here I have elected to create the frame myself, as I had trouble implementing
		  // the transmitByte function. Further debugging could clean this up to just use
		  // that function rather than construct a frame ourselves

		  // Start of checksum
          int sum = 0;  // Sum for checksum at end of transmit request frame
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
          
          varCount = dataLogger.getArrayVarCount();  // Get the variable count while doing the checksum

          sum += varCount;
          sum = 255 - (sum % 256);
		  // End of checksum

          Serial1.write(0x7E);  // Send the XBee the starting delimeter
          Serial1.write(0x00);  // Send the XBee the message's MSB
          Serial1.write(0x0F);  // Send the XBee the message's LSB
          Serial1.write(0x10);  // Send the XBee the frame type (0x10 is transmit request)
          Serial1.write(0x00);  // Send the XBee the frame ID
          Serial1.write(highAddress, 8);  // Send the XBee the address of the message's final destination
          Serial1.write(lowAddress, 2);  // Send the XBee the default low address settings
          Serial1.write(0x00);  // Send the XBee the broadcast radius
          Serial1.write(0x00);  // Send the XBee the options
          Serial1.write(byte(varCount));  // Send the variable count
          Serial1.write(byte(sum));  // Send the checksum
        }
		
        memset(rx, 0x00, sizeof(rx));  // Clear out the buffer


        bool allDataSent = false;  // Assume that not all the data has been sent
		
		// While loop for sending all the data to the host station
        while (!allDataSent) {  // While all the data has not been sent
          memset(rx, 0x00, sizeof(rx));  // Clear the buffer
          bool breakWhile = false;  // Assume that we are going to break out of this while loop, unless something changes
		  
          starttime = dataLogger.getNowLocalEpoch();  // Start a timer
          while (Serial1.available() == 0) {  // Wait for a message
            if (dataLogger.getNowLocalEpoch() - starttime > 60) {  // If we waited more than a minute
              breakWhile = true;  // Flag that we want to break the larger while loop where we try to send all our data
              break;  // Break the current while loop where we are just waiting for a message
            }
          }
		  
          delay(100);  // Give a chance for every byte to come in from the message
		  
          if (breakWhile) {  // If we want to break this while loop where we send the data
            break;  // Break the overarching while loop where we send all the data, effectively ending all communication until the next logging interval
          }
		  
		  // In this part, we will check which variable number the host station is interested in and supply the name of that variable
          Serial1.readBytes(rx, Serial1.available());  // Move what was received into the buffer
          if (rx[3] == 0x90 && rx[15] < varCount) {  // If the message was a receive packet and a number less than the varCount
            delay(20);
            varNum = rx[15];  // Record which variable number the host is interested in
            memset(rx, 0x00, sizeof(rx));  // Clear the buffer
            String varName = "";  // Create a String object to store the name of this variable number
            varName += dataLogger.getVarCodeAtI(varNum);  // Retrieve the variable name
            char name[varName.length() + 1];  // Create a string of characters to store this in
            varName.toCharArray(name, sizeof(name));  // Store the String in the string
            transmitString(name, sizeof(name), 0x00, 0x00, 0x00);  // Transmit the variable name to the host
            memset(rx, 0x00, sizeof(rx));  // Clear the buffer
          } else {  // If what was received is not a valid number request
            break;  // Break the overarching while loop where we send all the data, effectively ending all communication until the next logging interval
          }       

          // In this section, we will supply the measurement of the current variable of interest upon request
          starttime = dataLogger.getNowLocalEpoch();  // Start a timer
          while (Serial1.available() == 0) {  // Wait for a message
            if (dataLogger.getNowLocalEpoch() - starttime > 60) {  // If we waited for more than a minute
              breakWhile = true;  // Mark that we want to stop communication by breaking the overarching while loop
              break;  // Break this current while loop
            }
          }
		  
          if (breakWhile) {  // If we noted that we want to stop trying to send data
            break;  // Break the overarching while loop where we send all the data, effectively ending all communication until the next logging interval
          }
		  
          delay(100);  // Give a chance for all bytes to come in
		  
          Serial1.readBytes(rx, Serial1.available());  // Read in the bytes
          if (rx[15] == 0x4E) {  // If the payload was an 'N'
            delay(20);
            String dataValue = "";  // Create a String object where the measurement can be stored
            dataValue += dataLogger.getValueStringAtI(varNum);  // Grab the measurement and put it in the String
            char val[dataValue.length() + 1];  // Create a string of characters that will actually send
            dataValue.toCharArray(val, sizeof(val));  // Turn the String into a string
            transmitString(val, sizeof(val), 0x00, 0x00, 0x00);  // Send the data value to the host
            memset(rx, 0x00, sizeof(rx));  // Clear the buffer
          }
		  
          if (varNum == varCount - 1) {  // If that was our last variable
            allDataSent = true;  // Then all the data has been sent

            // for some reason, it will not send the last variable measured until the XBee is powered off then powered on again, so this catches that
			// some debugging is likely needed to fix this
            digitalWrite(xbeeSleepPin, HIGH);  // Put the XBee to sleep
            delay(100);  // Let its stomach settle
            digitalWrite(xbeeSleepPin, LOW);  // Wake the XBee
            delay(100);  // Let it make its last transmission
          }
        }
      }
    }
	
	// We are all done with radio communications
    digitalWrite(xbeeSleepPin, HIGH);  // Put the XBee to sleep
	digitalWrite(redLED, LOW);  // Turn off the red LED
  }
}
/** End [loop] */
