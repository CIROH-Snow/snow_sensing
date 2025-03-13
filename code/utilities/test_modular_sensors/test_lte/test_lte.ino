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
//  Defines for TinyGSM
// ==========================================================================
/** Start [defines] */
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif
#ifndef TINY_GSM_YIELD_MS
#define TINY_GSM_YIELD_MS 2
#endif
/** End [defines] */

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
const char* sketchName = "test_lte.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
// This can be found on the bottom of the Mayfly if the ink hasn't rubbed off.
// Sometimes I just use the name I've given the site here instead of the serial number.
// Make sure it is a string (in double quotations).
const char* LoggerID = "UWRL";

// How frequently (in minutes) to log data
const uint8_t loggingInterval = 3;

// Your logger's timezone.
const int8_t timeZone = -7;  // Mountain Standard Time is -7
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud = 57600;  // Baud rate for debugging (this is what you'll set your serial monitor's baud rate to)
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
//  Wifi/Cellular Modem Options
// ==========================================================================
/** Start [sim_com_sim7080] */
// For almost anything based on the SIMCom SIM7080G
#include <modems/SIMComSIM7080.h>

// Create a reference to the serial port for the modem
HardwareSerial& modemSerial = Serial1;  // Use hardware serial if possible
const int32_t   modemBaud = 9600;  //  SIM7080 does auto-bauding by default, but
                                   //  for simplicity we set to 9600

// Modem Pins - Describe the physical pin connection of your modem to your board
// NOTE:  Use -1 for pins that do not apply

const int8_t modemVccPin = 18;
// MCU pin controlling modem power --- Pin 18 is the power enable pin for the
// bee socket on Mayfly v1.0, use -1 if using Mayfly 0.5b or if the bee socket
// is constantly powered (ie you changed SJ18 on Mayfly 1.x to 3.3v)
const int8_t modemStatusPin  = 19;  // MCU pin used to read modem status
const int8_t modemSleepRqPin = 23;  // MCU pin for modem sleep/wake request
const int8_t modemLEDPin = redLED;  // MCU pin connected an LED to show modem
                                    // status

// Network connection information
const char* apn =
    "hologram";  // APN connection name, typically Hologram unless you have a
                 // different provider's SIM card. Change as needed

// Create the modem object
SIMComSIM7080 modem7080(&modemSerial, modemVccPin, modemStatusPin,
                        modemSleepRqPin, apn);
// Create an extra reference to the modem by a generic name
SIMComSIM7080 modem = modem7080;
/** End [sim_com_sim7080] */


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
    //new ProcessorStats_SampleNumber(&mcuBoard, "2a4186be-5691-414b-aa9a-cfb80a42649a"),
    //new ProcessorStats_FreeRam(&mcuBoard),
    //new ProcessorStats_Battery(&mcuBoard), 
    new MaximDS3231_Temp(&ds3231, "df6f3045-de6c-4a30-add0-bf6b08de818f")    
    // Additional sensor variables can be added here, by copying the syntax
    //   for creating the variable pointer (FORM1) from the
    //   `menu_a_la_carte.ino` example
    // The example code snippets in the wiki are primarily FORM2.
};

const char* UUIDs[] = {
    //"2a4186be-5691-414b-aa9a-cfb80a42649a",  // sampNum
    //"12345678-abcd-1234-ef00-1234567890ab", not used
    //"12345678-abcd-1234-ef00-1234567890ab", not used
    "df6f3045-de6c-4a30-add0-bf6b08de818f"  // boardTemp
    
    //  ... The number of UUID's must match the number of variables!
    //"12345678-abcd-1234-ef00-1234567890ab"  // calculatedSnowDepth
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
//  Creating Data Publisher[s]
// ==========================================================================
/** Start [publishers] */
const char* base64Authorization = "YnJhZWRvbi5kb3JpdHlAZ21haWwuY29tOkJhbGVhZGFzMTgh";  // https://www.base64encode.org/

// Create a data publisher for the Monitor My Watershed/EnviroDIY POST endpoint
#include <publishers/HydroServer.h>
HydroServerPublisher HydroServerPOST(dataLogger, &modem.gsmClient,
                                 base64Authorization);
/** End [publishers] */


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
  // Power relay setup
  pinMode(powerRelayPin, OUTPUT);
  digitalWrite(powerRelayPin, LOW);

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
  Serial.print(F("TinyGSM Library version "));
  Serial.println(TINYGSM_VERSION);
  Serial.println();

  // Start the serial connection with the modem
  modemSerial.begin(modemBaud);

  // Set up pins for the LED's
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, LOW);
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);
  // Blink the LEDs to show the board is on and starting up
  greenredflash();

  pinMode(20, OUTPUT);  // for proper operation of the onboard flash memory
                        // chip's ChipSelect (Mayfly v1.0 and later)

  // Set the timezones for the logger/data and the RTC
  // Logging in the given time zone
  Logger::setLoggerTimeZone(timeZone);
  // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
  Logger::setRTCTimeZone(0);

  // Attach the modem and information pins to the logger
  dataLogger.attachModem(modem);
  modem.setModemLED(modemLEDPin);
  dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                            greenLED);

  // Begin the logger
  dataLogger.begin(LoggerID, loggingInterval, &varArray);

  // Set up the sensors, except at lowest battery level
  Serial.println(F("Setting up sensors..."));
  varArray.setupSensors();


  /** Start [setup_sim7080] */
  modem.setModemWakeLevel(HIGH);   // ModuleFun Bee inverts the signal
  modem.setModemResetLevel(HIGH);  // ModuleFun Bee inverts the signal
  Serial.println(F("Waking modem and setting Cellular Carrier Options..."));
  modem.modemWake();  // NOTE:  This will also set up the modem
  modem.gsmModem.setBaud(modemBaud);   // Make sure we're *NOT* auto-bauding!
  modem.gsmModem.setNetworkMode(38);   // set to LTE only
                                        // 2 Automatic
                                        // 13 GSM only
                                        // 38 LTE only
                                        // 51 GSM and LTE only
  modem.gsmModem.setPreferredMode(1);  // set to CAT-M
                                        // 1 CAT-M
                                        // 2 NB-IoT
                                        // 3 CAT-M and NB-IoT
  /** End [setup_sim7080] */


  // Synchronize the RTC with NIST
  // This will also set up the modem
  dataLogger.syncRTC();


  // Create the log file, adding the default header to it
  // Do this last so we have the best chance of getting the time correct and
  // all sensor names correct
  // Writing to the SD card can be power intensive, so if we're skipping
  // the sensor setup we'll skip this too.
  Serial.println(F("Setting up file on SD card"));
  dataLogger.turnOnSDcard(
      true);  // true = wait for card to settle after power up
  dataLogger.createLogFile(true);  // true = write a new header
  dataLogger.turnOffSDcard(
      true);  // true = wait for internal housekeeping after write

  // Call the processor sleep
  Serial.println(F("Putting processor to sleep\n"));
  dataLogger.systemSleep();
}
/** End [setup] */


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [loop] */

// This function continuously runs until the Arduino/Mayfly is no longer powered
void loop() {
  dataLogger.logDataAndPublish();  // During logData, the marked time will update when a new measurement is taken
}
/** End [loop] */
