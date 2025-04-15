// This is the Mayfly sketch for the deployment of a snow station for the CIROH: Advaning Snow Observations project.

/** =========================================================================
 * @file basic.ino
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
const char* sketchName = "basic.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
// This can be found on the bottom of the Mayfly if the ink hasn't rubbed off.
// Sometimes I just use the name I've given the site here instead of the serial number.
// Make sure it is a string (in double quotations).
const char* LoggerID = "roadside";

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
//    Battery voltage
// ==========================================================================
/*
This is for measuring battery voltage using the Adafruit ADS1115 16-bit
Analog-to-digital converters (ADCs). The ADCs can only read voltages within
+/- 4.096 volts, so a voltage divider set up is needed. Please consult the wiring
guide on the CIROH: Advancing Snow Observations GitHub page before including this
measurement; otherwise, it is best to remove it
*/

# include <Adafruit_ADS1015.h>

float calculateBatteryVoltage(void) {
  float res1 = 100;  // resistor value in kOhms that comes directly off the battery
  float res2 = 22;  // resistor value in kOhms that connects directly into ground
  uint8_t i2cAddress = 0x49;  // Hexidecimal address of ADC that is taking the measurement
  uint8_t posBatChannel = 3;  // Channel on ADC the battery is connected to
  float inputVar1 = -9999;
  float calculatedResult = -9999;
  Adafruit_ADS1115 ads(i2cAddress);
  ads.setGain(GAIN_ONE);
  ads.begin();
  inputVar1 = ads.readADC_SingleEnded_V(posBatChannel);
  if (inputVar1 < 4.096 && inputVar1 > 0) {
    calculatedResult = inputVar1 / (res2 / (res2 + res1));
  } else {
    inputVar1 = -9999;
  }
  return calculatedResult;
}

// Properties of the calculated battery voltage

// The number of digits after the decimal place
const uint8_t calculatedBatteryVoltageResolution = 2;
// Variable name (this must be a value from http://vocabulary.odm2.org/variablename/)
const char* calculatedBatteryVoltageName = "BatteryVoltage";
// Variable units (this must be a value from http://vocabulary.odm2.org/units/)
const char* calculatedBatteryVoltageUnit = "V";
// A short code for the variable
const char* calculatedBatteryVoltageCode = "MayflyBattVolt";
// The (optional) universallly unique identifier
const char* calculatedBatteryVoltageUUID = "12345678-abcd-1234-ef00-1234567890ab";

Variable* calculatedBatteryVoltage = new Variable(
    calculateBatteryVoltage, calculatedBatteryVoltageResolution, calculatedBatteryVoltageName,
    calculatedBatteryVoltageUnit, calculatedBatteryVoltageCode, calculatedBatteryVoltageUUID);

// That's the end of the code for the battery voltage

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
const int32_t sonarHeight = 2787.65;
// Set the slope angle in degrees
const double slopeAngleDeg = 0;
double slopeAngleRad = slopeAngleDeg * 3.14159 / 180;

// There is no need to continuously run this sensor when it isn't taking a measurement,
// so set the sensor's power pin to the switched pin which we already defined
const int8_t sonarPower = sensorPowerPin;

// Trigger should be a *unique* negative number if unconnected
const int8_t sonarTrigger = -1;

// How many readings do you want to average? 
const uint8_t sonarNumReadings = 3;

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
      calculatedResult = (inputVar1 - inputVar2) / cos(slopeAngleRad);  // The snow depth is the height of the sensor minus the sonar's range measurement
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
  YELLOW  -> AA2  (0x48 ADDR -> GND)
  BLUE    -> AA3  (0x4B ADDR -> GND)
  RED     -> V12+ (POWER RELAY COM)
  GREEN   -> V12- (BATTERY GROUND)
  
  WIRING FOR *SP-510-SS* SENSOR
  WHITE	  -> AA2  (0x48 ADDR -> GND)
  BLACK   -> AA3  (0x48 ADDR -> GND)
  YELLOW  -> V12+ (POWER RELAY COM)
  BLUE    -> V12- (BATTERY GROUND)
  CLEAR   -> V12- (BATTERY GROUND)
  
*/

float sp510calibFactor = 21.65;

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
  WHITE   -> AA0  (0x4B ADDR -> SCL)
  BLACK   -> AA1  (0x4B ADDR -> SCL)
  CLEAR   -> V12- (BATTERY GROUND)
  
  WIRING FOR *SP-610-SS* SENSOR
  WHITE	  -> AA0  (0x4B ADDR -> SCL)
  BLACK   -> AA1  (0x4B ADDR -> SCL)
  YELLOW  -> V12+ (POWER RELAY COM)
  BLUE    -> V12- (BATTERY GROUND)
  CLEAR   -> V12- (BATTERY GROUND)
*/

float sp610calibFactor = 31.44;

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

float sl510k1 = 9.141;
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

float sl610k1 = 8.997;
float sl610k2 = 1.039;

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
    calculatedSnowDepth,
    //calculatedBatteryVoltage
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
    //"12345678-abcd-1234-ef00-1234567890ab",
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
  if (dataLogger.getNowLocalEpoch() % (loggingInterval * 60) == 2400) {  // Check if we are on the 45th minute (2700 seconds into the interval)
    digitalWrite(22, HIGH);  // Power up the power relay (pin 22 is the Mayfly's switched power output)
    delay(1000);
    digitalWrite(powerRelayPin, HIGH);  // Set the signal pin high (to close the circuit, the relay is waiting for a signal drop)
    delay(1000);
    digitalWrite(powerRelayPin, LOW);  // Let the relay know it is good to close the circuit by dropping the pin to low
    digitalWrite(22, LOW);  // Turn off switched power
  }

  dataLogger.logData();  // During logData, the marked time will update when a new measurement is taken
}
/** End [loop] */
