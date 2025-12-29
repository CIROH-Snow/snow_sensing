// ==========================================================================
//  Defines for TinyGSM
// ==========================================================================
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif
#ifndef TINY_GSM_YIELD_MS
#define TINY_GSM_YIELD_MS 2
#endif


// ==========================================================================
//  Include the libraries required for the data logger
// ==========================================================================
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// EnableInterrupt is used by ModularSensors for external and pin change
// interrupts and must be explicitly included in the main program.
#include <EnableInterrupt.h>

// Include the main header for ModularSensors
#include <ModularSensors.h>

// This helps manage I2C functionality
#include <Wire.h>

#include <AltSoftSerial.h>

AltSoftSerial mySerial(6, -1);


// ==========================================================================
// Set the input and output pins for the logger
// ==========================================================================
// NOTE:  Use -1 for pins that do not apply
// Baud rate for communication with RF data logger
const int serialBaud = 9600;
// Pin for the green LED (don't change)   
const int8_t  greenLED   = 8;
// Pin for the red LED (don't change)
const int8_t  redLED     = 9;
// Pin for debugging mode (ie, button pin, don't change)
const int8_t  buttonPin  = 21;
// MCU interrupt/alarm pin to wake from sleep (don't change)
const int8_t  wakePin    = 31;


// ==========================================================================
//  Wifi/Cellular Modem Options
// ==========================================================================
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

const int rxBufferRadioSize = 8000;
char rxBufferRadio[rxBufferRadioSize];
const char delimeter = ';';

const int txBufferLTEsize = 750;
char txBufferLTE[txBufferLTEsize];

// Prints the transmit buffer for the LTE modem over a Stream
void printLTEBuffer(Stream* stream) {
  Serial.println(txBufferLTE);
  stream->write(txBufferLTE, strlen(txBufferLTE));
  stream->flush();
  for (int i = 0; i < txBufferLTEsize; i++) { txBufferLTE[i] = '\0'; }
}


// ==========================================================================
// Information for connecting to HydroServer
// ==========================================================================
// Set the base URL to your HydroServer instance. Playground set here for testing
const char* serverHost = "playground.hydroserver.org";
const int serverPort = 80;

// Set User Information for authenticating with HydroServer - replace the string below
// Set the string equal to the base64 encoding of your username and password
// Get the encoded information using https://www.base64encode.org/
const char* base64Authorization = "Enter_Encoded_Authentication_Information_Here";  

// POST request headers
const char* postHeader               = "POST /api/sensorthings/v1.1/CreateObservations HTTP/1.1";
const char* hostHeader               = "\r\nHost: lro.hydroserver.org";
const char* acceptHeader             = "\r\nAccept: application/json";
const char* authorizationHeader      = "\r\nAuthorization: Basic ";
const char* contentLengthHeader      = "\r\nContent-Length: ";
const char* contentTypeHeader        = "\r\nContent-Type: application/json\r\n\r\n";

// JSON tags
const char* datastreamTag = "\"Datastream\":";
const char* iotTag        = "\"@iot.id\":";
const char* componentsTag = "\"components\":[\"phenomenonTime\",\"result\"],";
const char* dataArrayTag  = "\"dataArray\":";

String timestamp = "";
String StringVarCount = "";
int varCount;
const int maxVarCount = 40;
String table[maxVarCount][2];
String data = "";

bool timeToPublish;
bool connectSuccess;

void clearAltSoftSerialBuffer() {
  while (mySerial.available()) {
    Serial.print(String(mySerial.available()) + "bytes in the buffer!");
    mySerial.read();
  }
}

String getValue(String data, String variableName) {
  String searchKey = "@" + variableName + "=";
  int startIndex = data.indexOf(searchKey);

  if (startIndex == -1) {
    return "";
  }

  startIndex += searchKey.length();
  int endIndex = data.indexOf(";", startIndex);

  if (endIndex == -1) {
    return "";
  }

  return data.substring(startIndex, endIndex);
}

String targetString = "@endofstation=1;";
char incomingChar;
char potentialChar;

// ==========================================================================
// Arduino setup function
// ==========================================================================
void setup() {
  timeToPublish = false;
  connectSuccess = false;
  
   // Start the primary serial connection
  Serial.begin(serialBaud);
  Serial.println("Setting up AltSoftSerial");
  mySerial.begin(serialBaud);
  Serial.println("Finished setting up AltSoftSerial");
  delay(1000);
  clearAltSoftSerialBuffer();

  // Start the serial connection with the modem
  modemSerial.begin(modemBaud);

  pinMode(20, OUTPUT);  // for proper operation of the onboard flash memory
                        // chip's ChipSelect (Mayfly v1.0 and later)
  modem.setModemLED(modemLEDPin);

  // Start the LTE modem
  modem.setModemWakeLevel(HIGH);   // ModuleFun Bee inverts the signal
  modem.setModemResetLevel(HIGH);  // ModuleFun Bee inverts the signal
  modem.modemWake();               // NOTE:  This will also set up the modem
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
}


// ==========================================================================
// Arduino loop function
// ==========================================================================
void loop() {
  // If something has been sent serially from the RF data logger
  if (mySerial.available() > 0) {
    Serial.println("Serial data came through!");
    data = "";
    while (mySerial.available() > 0 || potentialChar != '*') {
      potentialChar = mySerial.read();
      Serial.print("Potential char: ");
      Serial.println(potentialChar);
      if (potentialChar == '*') break;
      if (potentialChar == '@') {
        Serial.println("potentialChar == '@'!");
        data += potentialChar;
        while (incomingChar != ';') {
          Serial.println("Checking incoming char");
          incomingChar = mySerial.read();
          data += incomingChar;
          Serial.print("Incoming char: ");
          Serial.println(incomingChar);
        }
        incomingChar = '\0';
      }
      //incomingChar = potentialChar;
      //data += incomingChar;
      //Serial.print("data: ");
      //Serial.println(data);
      //Serial.print("getValue(data, \"endofstation\"): ");
      //Serial.println(getValue(data, "endofstation"));
      //if (getValue(data, "endofstation") != "") {
      //  break;
      //}
      delay(10);
      //Serial.println(data);
    }
    
    //Serial.println("Received data");
    //delay(500);
    //Serial.println();
    //Serial.print("mySerial.available(): ");
    //Serial.println(mySerial.available());
    
    Serial.println("New data: ");
    Serial.println(data);

    timestamp = getValue(data, "timestamp");
    String StringVarCount = getValue(data, "varCount");
    varCount = StringVarCount.toInt();
    if (varCount >= 1) timeToPublish = true;

    Serial.print("\r\nTimestamp: ");
    Serial.println(timestamp);
    Serial.print("varCount: ");
    Serial.println(varCount);

    int rowIndex = 0;
    int start = 0;

    while (start < data.length() && rowIndex < varCount + 2) {
      int varStart = data.indexOf('@', start);
      if (varStart == -1) break;

      int eqIndex = data.indexOf('=', varStart);
      if (eqIndex == -1) break;

      int endIndex = data.indexOf(';', eqIndex);
      if (endIndex == -1) break;

      String varName = data.substring(varStart + 1, eqIndex);
      String varValue = data.substring(eqIndex + 1, endIndex);

      //Serial.print("varName: ");
      //Serial.println(varName);
      //Serial.print("varValue: ");
      //Serial.println(varValue);

      if (varName != "timestamp" && varName != "varCount" && varName != "endofstation") {
        Serial.println("\r\nAdding data to table!");
        table[rowIndex][0] = varName;
        table[rowIndex][1] = varValue;
        Serial.print("table[" + String(rowIndex) + "][0]: ");
        Serial.println(table[rowIndex][0]);
        Serial.print("table[" + String(rowIndex) + "][1]: ");
        Serial.println(table[rowIndex][1]);
        rowIndex++;
      }
      
      start = endIndex + 1;
    }
  }

  // Try and connect to the appropriate server if it's time to publish data
  if (timeToPublish) {
    connectSuccess = false;
    Serial.println("Time to publish!");
    if (modem.modemWake()) {  // Wake the LTE modem
      if (modem.connectInternet()) {  // Connect the modem to the internet
        Serial.println("Connected to the internet!");
        if (modem.gsmClient.connect(serverHost, serverPort)) {  // Connect to the desired server
          connectSuccess = true;
          Serial.println("Connected to server!");
          timeToPublish = false;  // Set this back to false so we don't try and connect again if there isn't still something in the serial buffer
        }
      }
    }
  }

  if (connectSuccess) {
    connectSuccess = false;  // set this back to false so we don't do this again unless we flag that it's time to publish again
    Serial.print("rxBufferRadio: ");
    Serial.println(rxBufferRadio);
    char jsonSizeBuffer[5] = "";
    char tempBuffer[37] = "";
    
    /*
    In this section, a POST request is constructed within a buffer piece by piece.
    Before a new section is added to the buffer, we check to see if the buffer has room.
    If there is not enough room, we send what we have so far before adding it and moving on.
    */

    Serial.println("Building the JSON!");
    // Add the post header to the buffer
    snprintf(txBufferLTE, sizeof(txBufferLTE), "%s", postHeader);
    
    // Add the host header
    if (txBufferLTEsize - strlen(txBufferLTE) < 29) printLTEBuffer(&modem.gsmClient);
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", hostHeader);

    // Add the accept header
    if (txBufferLTEsize - strlen(txBufferLTE) < 25) printLTEBuffer(&modem.gsmClient);        
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", acceptHeader);

    // Add the beginning of the authorization header
    if (txBufferLTEsize - strlen(txBufferLTE) < 21) printLTEBuffer(&modem.gsmClient);
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", authorizationHeader);

    // Add the basic64 authorization
    if (txBufferLTEsize - strlen(txBufferLTE) < 16) printLTEBuffer(&modem.gsmClient);
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", base64Authorization);

    // Add the content length header
    if (txBufferLTEsize - strlen(txBufferLTE) < 33) printLTEBuffer(&modem.gsmClient);
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", contentLengthHeader);

    // Calculate content length
    uint16_t jsonLength = 1;   // [
    //Serial.print("JSON Length: ");
    //Serial.println(jsonLength);
    //Serial.print("varCount: ");
    //Serial.println(varCount);
    for (int i = 0; i < varCount; i++) {
      //Serial.print("i: ");
      //Serial.println(i);
      jsonLength += 15;          // {"Datastream":{
      //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      jsonLength += 10;          // "@iot.id":
      //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      jsonLength += 1;           // "
      //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      jsonLength += 36;          // *Datastream ID*
      //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      jsonLength += 3;           // "},
      //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      jsonLength += 41;          // "components":["phenomenonTime","result"],
      //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      jsonLength += 13;          // "dataArray":[
      //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      jsonLength += 29;          // ["*phenomenonTime*",
      //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      jsonLength += table[i][1].length(); // *result*
      //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      if (i == varCount - 1) {
          jsonLength += 3;       // ]]}
          //Serial.print("JSON Length: ");
      //Serial.println(jsonLength);
      }
      else {
        jsonLength += 4;       // ]]},
        //Serial.print("JSON Length: ");
        //Serial.println(jsonLength);
      }
    }
    jsonLength += 1;               // ]

    //Serial.print("JSON length: ");
    //Serial.println(jsonLength);

    // Add the content length
    if (txBufferLTEsize - strlen(txBufferLTE) < 5) printLTEBuffer(&modem.gsmClient);
    itoa(jsonLength, jsonSizeBuffer, 10);
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", jsonSizeBuffer);
    
    // Add the content-type header
    if (txBufferLTEsize - strlen(txBufferLTE) < 33) printLTEBuffer(&modem.gsmClient);
    snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", contentTypeHeader);

    // Construct the JSON body
    if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
    txBufferLTE[strlen(txBufferLTE)] = '[';

    for (int y = 0; y < varCount; y++) {
      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '{';

      if (txBufferLTEsize - strlen(txBufferLTE) < 13) printLTEBuffer(&modem.gsmClient);
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", datastreamTag);

      if (txBufferLTEsize - strlen(txBufferLTE) < 10) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '{';
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", iotTag);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '"';

      if (txBufferLTEsize - strlen(txBufferLTE) < 37) printLTEBuffer(&modem.gsmClient);
      table[y][0].toCharArray(tempBuffer, 37);
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", tempBuffer);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '"';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '}';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = ',';

      if (txBufferLTEsize - strlen(txBufferLTE) < 42) printLTEBuffer(&modem.gsmClient);
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", componentsTag);

      if (txBufferLTEsize - strlen(txBufferLTE) < 12) printLTEBuffer(&modem.gsmClient);
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", dataArrayTag);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '[';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '[';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '"';

      if (txBufferLTEsize - strlen(txBufferLTE) < 37) printLTEBuffer(&modem.gsmClient);
      timestamp.toCharArray(tempBuffer, 37);
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", tempBuffer);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '"';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = ',';

      if (txBufferLTEsize - strlen(txBufferLTE) < 37) printLTEBuffer(&modem.gsmClient);
      table[y][1].toCharArray(tempBuffer, 37);
      snprintf(txBufferLTE + strlen(txBufferLTE), sizeof(txBufferLTE) - strlen(txBufferLTE), "%s", tempBuffer);

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = ']';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = ']';

      if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
      txBufferLTE[strlen(txBufferLTE)] = '}';

      if (y != varCount - 1) {
        if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
        txBufferLTE[strlen(txBufferLTE)] = ',';
      }
    }
    if (txBufferLTEsize - strlen(txBufferLTE) < 1) printLTEBuffer(&modem.gsmClient);
    txBufferLTE[strlen(txBufferLTE)] = ']';

    printLTEBuffer(&modem.gsmClient);
    Serial.println("Sent the JSON");
    // Wait 10 seconds for a response code
    uint32_t start = millis();
    while ((millis() - start) < 10000L) {
      delay(10);
    }

    uint16_t did_respond = 0;
    did_respond = modem.gsmClient.readBytes(rxBufferRadio, 3000);
    modem.gsmClient.stop();
    modem.modemSleep();

    int16_t responseCode = 0;
    if (did_respond > 0) {
      char responseCode_char[4];
      for (uint8_t i = 0; i < 3; i++) {
        responseCode_char[i] = rxBufferRadio[i + 9];
      }
      responseCode = atoi(responseCode_char);
    } else {
      responseCode = 504;
    }

    Serial.print("Response Code: ");
    Serial.println(responseCode);
    Serial.println("Full Response");
    Serial.println(rxBufferRadio);
  } else {
    //Serial.println("Unable to establish connection to data portal");
  }
}
