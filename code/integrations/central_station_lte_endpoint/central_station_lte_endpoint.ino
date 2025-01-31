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

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud = 9600;    // Baud rate for communication with RF data logger
const int8_t  greenLED   = 8;       // Pin for the green LED (don't change)
const int8_t  redLED     = 9;       // Pin for the red LED (don't change)
const int8_t  buttonPin  = 21;      // Pin for debugging mode (ie, button pin, don't change)
const int8_t  wakePin    = 31;  // MCU interrupt/alarm pin to wake from sleep (don't change)
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

const int rxBufferRadioSize = 8000;
char rxBufferRadio[rxBufferRadioSize];
const char delimeter = ';';

const int txBufferLTEsize = 750;
char txBufferLTE[txBufferLTEsize];

// Prints the transmit buffer for the LTE modem over a Stream
void printLTEBuffer(Stream* stream) {
  stream->write(txBufferLTE, strlen(txBufferLTE));
  stream->flush();
  for (int i = 0; i < txBufferLTEsize; i++) { txBufferLTE[i] = '\0'; }
}
// Server Information
const char* serverHost = "lro.hydroserver.org";
const int serverPort = 80;

// User Information
const char* base64Authorization = "";  // https://www.base64encode.org/

// POST request headers
const char* postHeader               = "POST api/sensorthings/v1.1/CreateObservations HTTP/1.1\r\n";
const char* hostHeader               = "Host: lro.hydroserver.org\r\n";
const char* acceptHeader             = "Accept: appliation/json\r\n";
const char* beginAuthorizationHeader = "Authorization: Basic ";
const char* endAuthorizationHeader   = "\r\n";
const char* contentTypeHeader        = "Conent-Type: application/json\r\n\r\n";

// JSON tags
const char* datastreamTag = "\"Datastream\":";
const char* iotTag        = "\"@iot.id\":";
const char* componentsTag = "\"components\":[\"phenomenonTime\",\"result\"],";
const char* dataArrayTag  = "\"dataArray\":";

void setup() {
   // Start the primary serial connection
  Serial.begin(serialBaud);

  // Start the serial connection with the modem
  modemSerial.begin(modemBaud);

  pinMode(20, OUTPUT);  // for proper operation of the onboard flash memory
                        // chip's ChipSelect (Mayfly v1.0 and later)
  modem.setModemLED(modemLEDPin);

  /** Start [setup_sim7080] */
  modem.setModemWakeLevel(HIGH);   // ModuleFun Bee inverts the signal
  modem.setModemResetLevel(HIGH);  // ModuleFun Bee inverts the signal
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
  
  modem.modemSleep();
  /** End [setup_sim7080] */
}

void loop() {
  bool timeToPublish = false;
  bool connectSuccess = false;

  // If something has been sent serially from the RF data logger
  if (Serial.available() > 0) {
    delay(500);
    timeToPublish = true;
    String data = Serial.readStringUntil('*');
    data.toCharArray(rxBufferRadio, sizeof(data));
  }

  // Try and connect to the appropriate server if it's time to publish data
  if (timeToPublish) {
    connectSuccess = false;
    if (modem.modemWake()) {  // Wake the LTE modem
      if (modem.connectInternet()) {  // Connect the modem to the internet
        if (modem.gsmClient.connect(serverHost, serverPort)) {  // Connect to the desired server
          connectSuccess = true;
          timeToPublish = false;  // Set this back to false so we don't try and connect again if there isn't still something in the serial buffer
        }
      }
    }
  }

  if (connectSuccess) {
    connectSuccess = false;  // set this back to false so we don't do this again unless we flag that it's time to publish again
    /*
    In this section, a POST request is constructed within a buffer piece by piece.
    Before a new section is added to the buffer, we check to see if the buffer has room.
    If there is not enough room, we send what we have so far before adding it and moving on.
    */
    // Add the post header to the buffer
    snprintf(txBufferLTE, sizeof(txBufferLTE), "%s", postHeader);
    
    // Add the host header
    if (txBufferLTEsize - strlen(txBufferLTE) < 29) {
      printLTEBuffer(&modem.gsmClient);        
    }
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", hostHeader);

    // Add the accept header
    if (txBufferLTEsize - strlen(txBufferLTE) < 25) {
      printLTEBuffer(&modem.gsmClient);        
    }
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", acceptHeader);

    // Add the beginning of the authorization header
    if (txBufferLTEsize - strlen(txBufferLTE) < 21) {
      printLTEBuffer(&modem.gsmClient);        
    }
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", beginAuthorizationHeader);

    // Add the basic64 authorization
    if (txBufferLTEsize - strlen(txBufferLTE) < 16) {
      printLTEBuffer(&modem.gsmClient);        
    }
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", base64Authorization);

    // Add the end of the authorization header
    if (txBufferLTEsize - strlen(txBufferLTE) < 2) {
      printLTEBuffer(&modem.gsmClient);        
    }
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", endAuthorizationHeader);

    // Add the content-type header
    if (txBufferLTEsize - strlen(txBufferLTE) < 33) {
      printLTEBuffer(&modem.gsmClient);        
    }
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", endAuthorizationHeader);

    // Construct the JSON
    if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
      printLTEBuffer(&modem.gsmClient);
    }
    txBufferLTE[strlen(txBufferLTE)] = '[';

    char* timeToken = strtok(rxBufferRadio, &delimeter);
    char* token = strtok(nullptr, &delimeter);
    while (token != nullptr) {
      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = '{';

      if (txBufferLTEsize - strlen(txBufferLTE) < 13) {
        printLTEBuffer(&modem.gsmClient);        
      }
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", datastreamTag);

      if (txBufferLTEsize - strlen(txBufferLTE) < 33) {
        printLTEBuffer(&modem.gsmClient);        
      }
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", endAuthorizationHeader);

      if (txBufferLTEsize - strlen(txBufferLTE) < 10) {
        printLTEBuffer(&modem.gsmClient);        
      }
      txBufferLTE[strlen(txBufferLTE)] = '{';
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", iotTag);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = '"';

      if (txBufferLTEsize - strlen(txBufferLTE) < 36) {
        printLTEBuffer(&modem.gsmClient);        
      }
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", token);

      token = strtok(nullptr, &delimeter);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = '"';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = '}';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = ',';

      if (txBufferLTEsize - strlen(txBufferLTE) < 42) {
        printLTEBuffer(&modem.gsmClient);        
      }
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", componentsTag);

      if (txBufferLTEsize - strlen(txBufferLTE) < 12) {
        printLTEBuffer(&modem.gsmClient);        
      }
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", dataArrayTag);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = '[';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = '[';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = '"';

      if (txBufferLTEsize - strlen(txBufferLTE) < 37) {
        printLTEBuffer(&modem.gsmClient);        
      }
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", timeToken);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = '"';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = ',';

      if (txBufferLTEsize - strlen(txBufferLTE) < 37) {
        printLTEBuffer(&modem.gsmClient);        
      }
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", token);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = ']';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = ']';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
        printLTEBuffer(&modem.gsmClient);
      }
      txBufferLTE[strlen(txBufferLTE)] = '}';

      token = strtok(nullptr, &delimeter);
      if (token != nullptr) {
        if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
          printLTEBuffer(&modem.gsmClient);
        }
        txBufferLTE[strlen(txBufferLTE)] = ',';
      }
    }
    if (txBufferLTEsize - strlen(txBufferLTE) < 1) {
      printLTEBuffer(&modem.gsmClient);
    }
    
    txBufferLTE[strlen(txBufferLTE)] = ']';
    printLTEBuffer(&modem.gsmClient);
    modem.gsmClient.stop();
    modem.modemSleep();
  }
}
