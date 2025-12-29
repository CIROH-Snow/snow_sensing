/** ==============================================================================
 * File name: push_to_cr800.ino
 * Description: This file contains the code needed for operating a Mayfly at a 
 * "base" station that is connected to a Campbell Scientific CR800 datalogger 
 * that is using the code found in the "internet-connected-datalogger" directory 
 * of this repository. You will need a programmed XBee S3B in order to properly 
 * use this as well.
 *  
 * Author: Braedon Dority <braedon.dority@usu.edu>
 *  
 * Copyright (c) 2025 Utah State University
 * 
 * License: This example is published under the BSD-3 open source license.
 *
 * Build Environment: Arduino IDE Version 1.8.19
 * Hardware Platform: EnviroDIY Mayfly Arduino Datalogger V1.1
 *
 * DISCLAIMER: THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
 * 
 * The data communicated to the CR800 is framed as follows:
 * 
 * @variableName=variableMeasurement;
 * 
 * The CR800 looks for an @ to know the measurement being taken, such as snow depth, 
 * timestamp, shortwave radiation, etc. The = tells the CR800 what the actual value 
 * of the measurement was, such as 500 for the snow depth in mm.
 * The ; notes the end of the measurement.
 * Consecutive variable names and measurements can be tacked onto the String. The 
 * CR800 will parse them upon reception. Once the String for a satellite station's 
 * data is ready to be sent to the CR800, an ending variable is added called 
 * "endofstation" and it is set to 1. "@endofstation=1;" The CR800 uses this to 
 * know that there is no more data for the string that has just come through, and 
 * that it can go ahead and call a table to log everything to.
 * 
 * **WIRING**
 * Mayfly TX0 -> CR800 ____
 * Mayfly GND -> CR800 GND
 * Buck converter RED -> CR800 SW12
 * Buck converter BLACK -> CR800 GND
 * ==============================================================================*/ 

// Needed for all Arduino sketches
#include <Arduino.h>

// Helps with I2C
#include <Wire.h>

// Real-time clock (RTC)
#include "Sodaq_DS3231.h"

// Pin numbers for useful LEDs on the Mayfly that sometimes help to troubleshoot
const int8_t redLED = 9;
const int8_t greenLED = 8;


// ==========================================================================
//  Radio Communications
// ==========================================================================
/*
This is the setup for a the base station radio with the XBee Pro S3B
using the Digimesh protocol. In this context, satellite stations refer to
the snow data collection stations surrounding a central base station 
where data is communicated from those sites via radio frequency.
*/

// Pins for power supply and sleep control of the XBee
#define xbeeRegulatorPin 18
#define xbeeSleepPin 23 

// This is a function created by the EnviroDIY team that flashes the spare green 
// and red LEDs. It can be useful in troubleshooting as well as other things
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

// Setting the baud rate of the XBee
// This needs to match what is set using the XCTU software
// 9600 is a normal baud rate for these XBees
const int xbeeBaud = 9600;

// The number of satellite stations you will have set up
const int numStations = 5;

/*
An array of all the station names as Strings (not to be confused with strings or character arrays)
Make sure to put these in an order that will not disable communication in your network.
For example, if you have a satellite station that cannot directly talk to your central station,
but it can talk to another satellite station that does have contact with the central station,
then you do not want to get data from the in-between station first because it will power down
after it's finished sending data, which will then no longer help communicate data from your far
station to your central station. Collect the data from the farthest station first (i.e., most
remote to least remote). 
*/
String stationNames[numStations] = {"Roadside", "Marshes", "Conifers", "Aspens", "Sunny"};

// The length of each of the XBee addresses
// It consists of the serial number found on each XBee and an optional 2-byte
// address that is just [FF FE] if you don't wish to have one
// I suggest just leaving the optional 2-byte address alone
const int sizeAddress = 10;

// An array of all the satellite station addresses
// The first 8 bytes are the serial number
// Make sure the order of these addresses matches the order of the stationNames array 
byte satellites[numStations][sizeAddress] = {{0x00, 0x13, 0xA2, 0x00, 0x42, 0x2B, 0x1E, 0xA2, 0xFF, 0xFE}, {0x00, 0x13, 0xA2, 0x00, 0x42, 0x33, 0x67, 0xDD, 0xFF, 0xFE}, {0x00, 0x13, 0xA2, 0x00, 0x42, 0x2F, 0xDB, 0xA7, 0xFF, 0xFE}, {0x00, 0x13, 0xA2, 0x00, 0x42, 0x33, 0x67, 0xB0}, {0x00, 0x13, 0xA2, 0x00, 0x42, 0x33, 0x67, 0xCC, 0xFF, 0xFE}};

// A boolean array used in the loop function to determine if all the data has been sent from each station
// Each slot in the array corresponds to the stations listed in the stationNames array
bool dataSent[numStations];

// The final String that will be sent to the CR800 data logger over serial communication
String stringToSend;

// The time to delay between transmiting a message and waiting for a response (in seconds).
// Having this delay helps make sure a message comes through in its entirety.
uint32_t wait = 10;

// This variable helps track when measurements were last requested so the central station's
// radio is not perpetually requesting data
int prevLogHour;

// This boolean variable helps signal that not only are we in a timing interval that's appropriate to log
// but also that we've met other conditions needed
bool timeToLog;

// Variable used in the checksum function
int sum;

// The number of times you want to try and make contact with a station before giving up
int totalTries = 7;

// Possible character messages to send to a satellite station
// DO NOT CHANGE THESE
// The satellite stations are listening for these specific messages
char ready[] = "R";   // "Are you ready to start communicating?"
char time[] = "T";    // "Could I get the timestamp?"
char var[] = "V";     // "How many variables did you measure?"
char number[] = "N";  // "Can I get the measurement made for the variable number I just sent you?"

// This is a place to store any information the XBee reads into the Mayfly's
// serial port. This is usually referred to as a buffer in serial communication.
byte rx[64];

/* 
This function calculates the most signifcant byte (MSB).
It assumes that the message includes a null terminator at the end which should make it easy
to just use the sizeof() function as the size parameter. The 13 is there because
this function is solely used for making a Transmit Request frame type on the XBee, and the amount
of other bytes that it cares about when checking the frame is a fixed value aside from the
actual message being sent.
*/
byte lengthMSB(int size) {
  return byte((13 + size) / 255);
}

/*
This function calculates the least signifcant byte (LSB).
It assumes that message includes a null terminator at the end which should make it easy
to just use the sizeof() function as the size parameter. The 13 is there because
this function is solely used for making a Transmit Request frame type on the XBee, and the amount
of other bytes that it cares about when checking the frame is a fixed value aside from the
actual message being sent.
*/
byte lengthLSB(int size) {
  return byte((13 + size) % 255);
}

/*
This function calculates the checksum of a transmit request frame that is about to be sent to the XBee. This
function is only useful for sending strings of characters (char[]). The size parameter is just
the output using the sizeof(rfData[]) function. There is no need to try and account for it using sizeof(). 
The station index refers to the placement order of each station in the stationNames array.
The last three variables are described in the XBee manual section about transmit request frames.
I leave them as 0x00 for each. I would suggest not changing those last three, especially frameID and options. 
*/
byte checksum(char rfData[], int size, int stationIndex, byte frameID, byte broadcastRadius, byte options) {
  sum = 0;
  sum += 0x10;
  sum += frameID;
  
  for (int p = 0; p <= sizeof(satellites[stationIndex]) - 1; p++) {
    sum += satellites[stationIndex][p];
  }

  sum += options;
  sum += broadcastRadius;
  for (int i = 0; i < size - 1; i++) {
    sum += byte(rfData[i]);
  }
  
  return byte(255 - (sum % 256));
}

/*
This function pushes a transmit request to the XBee through the Mayfly's serial port.
The XBee then attempts to send the message to the station specified with the stationIndex parameter.
The parameters are all the same for this as the checksum function (message[] and messageSize are rfData[] 
and size respectively). I chose to have checksum be its own separate function so I could debug more easily 
and to illustrate clearly how the transmit request works. I leave the frameID parameter as 0x00 because 
then the XBee won't try and tell me if it made contact with the other XBee. I'll know if I have contact 
if I receive data back. Changing this to any other value you want will kick messages back to the Mayfly 
which can be helpful for debugging but will derail the code as it is currently setup in an actual deployment.
*/
void transmitRequest(char message[], int messageSize, byte frameID, int stationIndex, byte broadcastRadius, byte options) {
  Serial1.write(0x7E);  // Starting delimeter
  Serial1.write(lengthMSB(messageSize));  // Most significant byte
  Serial1.write(lengthLSB(messageSize));  // Least significant byte
  Serial1.write(0x10);  // Frame type (10 for transmit requests)
  Serial1.write(frameID);  // Set to 0x00, in which case it will stop the XBee from telling the Mayfly if it made contact (which is what we want)
  Serial1.write(satellites[stationIndex], 10);  // The destination address of the station the message is intended for
  Serial1.write(broadcastRadius);  // Set to the default 0x00 because that will give the maximum broadcast hops (no need to change really)
  Serial1.write(options);  // Just set to 0x00, otherwise you may change the programming of your XBee
  Serial1.write(message);  // The message being sent. The Serial1.write() method will take care of sending the string as hexidecimals
  Serial1.write(checksum(message, messageSize, stationIndex, frameID, broadcastRadius, options));  // Final checksum for quality control
}

/*
This is a handy DEBUGGING function for printing out what is currently stored in a buffer.
Simply use the buffer's variable name for the buffer[] parameter and 
sizeof(*buffer's variable name*) for the bufferSize parameter.
This function won't appear in the code by default, but you may consider using it when trying
to troubleshoot.
*/
void serialPrintBuffer(byte buffer[], int bufferSize) {
  for (int i = 0; i < bufferSize; i++) {  // For each byte in the buffer
    Serial.print(buffer[i], HEX);  // Print the byte over Serial0
    if (i == bufferSize - 1) {  // and check if the byte is the last one in the buffer.
      Serial.println();  // If so, then end the printed line,
    } else {  // otherwise
      Serial.print(" ");  // add a space to make it easy to differentiate bytes
    }
  }
}

// This function will wait for a specified amount of time or until bytes appear in the Serial1 buffer
void waitForReceive(uint32_t secondsWait) {
  uint32_t start_ts = rtc.now().getEpoch();  // Start a timer
  delay(1000);
  while (Serial1.available() == 0) {  // While we don't have anything in the buffer
    if (rtc.now().getEpoch() - start_ts > secondsWait) {  // If we have waited longer than wanted
      break;  // Leave the while loop
    }
  }
  delay(100);
}

// ==========================================================================
// Arduino setup function that runs each time the Mayfly is powered on
// This is not the same as waking up from a sleeping mode.
// The setup function runs when there is a complete shut off and turn on.
// ==========================================================================
void setup() {
  // XBee setup that should be done each time the board powers on
  pinMode(xbeeRegulatorPin, OUTPUT);  // Make sure we control the power to the XBee 
  pinMode(xbeeSleepPin, OUTPUT);  // Make sure we control if the XBee sleeps or not
  digitalWrite(xbeeRegulatorPin, HIGH);  // Supply power to the XBee regulator
  Serial1.begin(xbeeBaud);  // Begin UART-1 communication

  Wire.begin();

  // Waking up the XBee momentarily to clear out anything that could potentially be in the buffer
  digitalWrite(xbeeSleepPin, LOW);  // Driving the sleep pin low wakes the XBee
  memset(rx, 0x00, sizeof(rx));  // Make sure where we are going to store these potenetial bytes is empty by setting everything to zero
  delay(100);
  int count = Serial1.available();  // Count how many bytes there are in the UART-1
  Serial1.readBytes(rx, count);  // Read that many into the buffer (read everything that's there)
  memset(rx, 0x00, sizeof(rx));  // Clear it out
  digitalWrite(xbeeSleepPin, HIGH);  // Driving the sleep pin high puts the XBee back to sleep

  // Start up the real-time clock (RTC)
  rtc.begin();

  // Set the baud (communication) rate between the Mayfly and the CR800 datalogger connected over UART-0
  // Make sure this is compatible with what the CR800 datalogger is expecting (i.e. not too fast for it)
  Serial.begin(9600);

  // Assume that it is not time to log in setup
  timeToLog = false;
  
}

// ==========================================================================
// Arduino loop function that continuously runs unless the board shuts off
// ==========================================================================
void loop() {
  if (rtc.now().minute() == 1) {  // If the current minute is on the one
    timeToLog = true;  // then it's time to log and collect new data
  }

  if (timeToLog) {  // If it's time to log
    digitalWrite(xbeeSleepPin, LOW);  // Wake the XBee up
    digitalWrite(redLED, HIGH);  // Turn on the red LED as a visually cue that radio communication has started
    delay(1000);  // Let the XBee's stomach settle

    // Clear out the UART-1 received storage so there is no confusion
    memset(rx, 0x00, sizeof(rx));  // Make sure where we are going to store things is empty
    Serial1.readBytes(rx, Serial1.available());  // Read in everything in UART-1
    memset(rx, 0x00, sizeof(rx));  // Wipe it out

    /*
    // Set the dataSent boolean array to all falses to start out because no data has been sent
    for (int i = 0; i < sizeof(dataSent); i++) {
      dataSent[i] = false;
    }
    */ 

    // This boolean array helps keep track of which stations got to have their maximum attempts at communication
    // much like the dataSent array.
    bool didTotalTries[numStations];

    // Set the boolean array to all falses since no tries have been attempted yet
    for (int u = 0; u < numStations; u++) {
      didTotalTries[u] = false;
    }
    
    // We will now attempt to collect data from each station
    for (int i = 0; i < numStations; i++) {  // For each station
      delay(1000);
      int tries = 0;  // start off with a clean slate of no tries attempted yet
      while (!didTotalTries[i]) {  // While we haven't used up all the tries we specified
        greenredflash(i);  // Visual cue that we are attempting contact with station "i"
        digitalWrite(redLED, HIGH);  // Make sure the red LED stays on
        tries++;  // Add a try to our count
        stringToSend = "";  // Prep an empty String that will contain all the data
        transmitRequest(ready, sizeof(ready), 0x00, i, 0x00, 0x00);  // Ask if station "i" is ready
        waitForReceive(wait);  // Give a chance for the satellite to respond (the XBees take care of retries)

        if (Serial1.available() > 0) {  // If there is something in the UART-1, that means we've received a message, because we cleared it out a few lines ago
          // We have established contact, and even if something happens and we can't get everything or communications goes down, we don't want to try and reach out again from the beginning. We'll just abandon mission
          // More robust communication protocols could be established. That would require changing the coding on the satellite station end of things as well
          // From this point on, we will not try to reach out to the station again if something happens in the middle of collecting its data, hence setting didTotalTries[i] to true
          didTotalTries[i] = true;
          delay(100);  // Give a chance for everything to come in
		      int count = Serial1.available();  // Count up how many bytes are there
          Serial1.readBytes(rx, count);  // Read in everything that was there
		  
          if (rx[15] == 0x52) {  // Check if byte 15 was an 'R' (the satellites station's response that it is ready)
            digitalWrite(greenLED, HIGH);  // Visual cue that contact has been made
            memset(rx, 0x00, sizeof(rx));  // If so, clear out the buffer
            stringToSend += "@station=";  // Add the variable name "station" to the String, properly framed
            stringToSend += stationNames[i];  // Add the station's name to the String
            stringToSend += ";";  // Add the semicolon seperator to finish the framing
            delay(500);

            transmitRequest(time, sizeof(time), 0x00, i, 0x00, 0x00);  // Ask for a timestamp

            delay(500);
            digitalWrite(greenLED, LOW);  // Turn the green LED off
            waitForReceive(wait);  // Give a chance for the satellite to respond
			
            if (Serial1.available() > 0) {  // If we received something
			        delay(100);  // Give a chance for everything to come in
              count = Serial1.available();  // Count up the bytes received
              Serial1.readBytes(rx, count);  // then read in the new message to our buffer
			  
			        // Adding the timestamp data
              stringToSend += "@timestamp=";  // Add the variable name "timestamp" to the String, properly framed
              for (int z = 15; z < count - 1; z++) {  // For every byte in the message portion of the received XBee frame (bytes 15 to the second-to-last byte)
                stringToSend += (char)rx[z];  // Convert each byte to a char and tack it onto the String we'll be sending
                if (z == count - 2) {  // If we've come to the last byte of interest
                  stringToSend += ";";  // then add on the semicolon seperator
                }
              }
			  
              memset(rx, 0x00, sizeof(rx));  // Clear out the buffer
			  
              transmitRequest(var, sizeof(var), 0x00, i, 0x00, 0x00);  // Ask for the amount of variables measured
              waitForReceive(wait);  // Wait for a response
			  
              if (Serial1.available() > 0) {  // If we got a response
			          delay(100);  // Give a chance for every byte to come in
                int count = Serial1.available();  // Count up how many bytes came in
                Serial1.readBytes(rx, count);  // Read in the bytes to our buffer
                int varCount = rx[15];  // Store number given in the payload (15th byte) in the varCount variable

                // I'm not certain the while (gettingDataVaules) loop is necessary
                //bool gettingDataValues = true;
                //while (gettingDataValues) {
                for (int b = 0; b < varCount; b++) {  // For each variable measured by the satellite station
                  memset(rx, 0x00, sizeof(rx));  // Make space for the variable name we anticipate receiving first in the buffer

                  /*
                  The transmitRequest function takes in a char array as the message, not a number value,
                  and we want to send over an actual number that the satellite station will interepret as: 
                  "the central station wants the name of variable number __",
                  so here we will just customize a transmit request frame to request that
                  */
                  int sum = 0;  // Checksum value
                  sum += 0x10;  // Add the byte value of the frame type
                  sum += 0x00;  // Add the byte value of the frame ID
                  
                  // Add the byte values of the address
                  for (int x = 0; x <= sizeof(satellites[i]) - 1; x++) {
                    sum += satellites[i][x];
                  }
                  
                  sum += 0x00;  // Add the broadcast radius
                  sum += 0x00;  // Add the options
                  sum += b;  // Add the payload (the variable number being requested)

                  sum = (255 - (sum % 256));  // Calculate the checksum

                  // Send the transmit request frame to the XBee asking for the variable name of this number "b"
                  Serial1.write(0x7E);  // Starting delimeter
                  Serial1.write(0x00);  // Length MSB
                  Serial1.write(0x0F);  // Length LSB
                  Serial1.write(0x10);  // Frame type (transmit request)
                  Serial1.write(0x00);  // Frame ID (0x00 means we don't want a response frame that tells us if a connection was made)
                  Serial1.write(satellites[i], 10);  // Destination address of the satellite station
                  Serial1.write(0x00);  // Broadcast radius
                  Serial1.write(0x00);  // Options
                  Serial1.write(byte(b));  // Message (here it is just the variable number we are requesting)
                  Serial1.write(byte(sum));  // Checksum

                  waitForReceive(wait);  // Wait for a response

                  if (Serial1.available() == 0) {  // If we didn't hear anything
                    //gettingDataValues = false;
                    stringToSend += "@endofstation=1;";  // Properly end the String
                    Serial.println(stringToSend);  // Send what we have to the CR800
                    break;  // Stop what we're doing in this for loop
                  } else {  // If we did hear something
                    int count = Serial1.available();  // Count up what's waiting
                    Serial1.readBytes(rx, count);  // Read in the XBee frame
                    
                    // Record the payload to our String
                    stringToSend += "@";
                    for (int c = 15; c < count - 1; c++) {  // For each byte in the payload section of the received message
                      stringToSend += (char)rx[c];  // convert the byte into a character and add it to the String
                      if (c == count - 2) {  // If this is the last byte
                        stringToSend += "=";  // then add the semicolon separator
                      }
                    }

                    // We will now ask for the actual measurement sampled for variable "b"
                    memset(rx, 0x00, sizeof(rx));  // Make some space for a new message

                    transmitRequest(number, sizeof(number), 0x00, i, 0x00, 0x00);  // Ask for the recorded measurement

                    waitForReceive(wait);  // Wait for a response

                    if (Serial1.available() == 0) {  // If we didn't receive anything
                      //gettingDataValues = false;
                      stringToSend += ";@endofstation=1;";  // Properly end the String
                      Serial.println(stringToSend);  // Send what we have to the CR800
                      break;  // Stop asking for the next variable "b" by breakin the current for loop
                    } else {  // If we did hear something
                      count = Serial1.available();  // Count up what's waiting
                      Serial1.readBytes(rx, count);  // Read in the XBee frame

                      // Record the payload
                      for (int d = 15; d < count - 1; d++) {  // For each byte in the payload portion of the frame
                        stringToSend += (char)rx[d];  // convert the byte to a character and add it to the String
                      }

                      if (b != varCount - 1) {  // If it's not the last variable sampled
                        stringToSend += ";";  // add on the semicolon seperator
                      } else {  // If it is the last measurement
                        stringToSend += ";@endofstation=1;";  // Properly end the String
                        Serial.println(stringToSend);  // Send out the string over the Serial UART-0 port to the CR800
                        //gettingDataValues = false;  // We no longer need to collect data values
                        //dataSent[i] = true;  // and we can mark that this station has sent all its data.
                        greenredflash(10);  // Visual cue that a successful transmission was made
                        digitalWrite(redLED, HIGH);  // Make sure the red LED stays on
                      }
                    }
                  }
                }
                //}
              } else {  // If we didn't receive a variable count
                stringToSend += "@endofstation=1;";  // Properly end the String
                Serial.println(stringToSend);  // Send what we have to the CR800
              }
            } else {  // If we didn't receive a timestamp
              stringToSend += "@endofstation=1;";  // Properly end the String
              Serial.println(stringToSend);  // Send what we have to the CR800
            }
          } else {  // If we received something other than an 'R'
            memset(rx, 0x00, sizeof(rx));  // then get rid of it
            stringToSend += "@station=";  // Add the variable name "station" to the String, properly framed
            stringToSend += stationNames[i];  // Add the station's name to the String
            stringToSend += ";@endofstation=1;";  // Properly end the String
            Serial.println(stringToSend);  // Send over to CR800
          }
        }
        if (tries == totalTries && didTotalTries[i] != true) {  // If we've tried as much as we'd like and we haven't successfully made contact
          didTotalTries[i] = true;  // then take note of it for this station so it doesn't try again
          stringToSend += "@station=";  // Add the variable name "station" to the String, properly framed
          stringToSend += stationNames[i];  // Add the station's name to the String
          stringToSend += ";@endofstation=1;";  // Properly end the String
          Serial.println(stringToSend);  // Send over to CR800
        }
      }
    }
    // At this point, we have made contact or made every attempt to contact each station
    digitalWrite(xbeeSleepPin, HIGH);  // Put the XBee back to sleep
    timeToLog = false;  // We don't want to log until the right conditions again
    digitalWrite(redLED, LOW);  // Turn off the red LED
  }
}
