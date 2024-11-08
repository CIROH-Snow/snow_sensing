// This adds extra code that will help the program run such as the code
// needed for the real time clock found on the Mayfly.
#include <Arduino.h>
#include <Wire.h>
#include "Sodaq_DS3231.h"

// Pin numbers for useful LEDs on the Mayfly that sometimes help to troubleshoot
const int8_t redLED = 9;
const int8_t greenLED = 8;



// ==========================================================================
//  Radio Communications
// ==========================================================================
// This is the setup for a the central radio station with the XBee Pro S3B
// using the Digimesh protocol. In this context, satellite stations refer to
// the snow data collection stations surrounding a central data hub station 
// where data is communicated from that site via radio frequency.

// Pins for power supply and sleep control of the XBee
#define xbeeRegulatorPin 18
#define xbeeSleepPin 23 


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
// This needs to match what is set using the XCT-U software
// 9600 is a normal baud rate for these XBees
const int xbeeBaud = 9600;

// The number of satellite stations
const int numStations = 5;

// An array of all the station names as Strings (not to be confused with strings)
// Make sure to put these in an order that will not disable communication in your network.
// For example, if you have a satellite station that cannot directly talk to your central station,
// but it can talk to another satellite station that does have contact with the central station,
// then you do not want to get data from the inbetween station first because it will power down
// after it's finished sending data, which will then no longer help communicate data from your far
// station to your central station. Collect the data from the farthest station first (i.e. most
// remote to least remote). 
String stationNames[numStations] = {"Roadside", "Marshes", "Conifers", "Aspens", "Sunny"};

// The length of each of the XBee addresses
// It consists of the serial number found on each XBee and an optional 2-byte
// address that is just [FF FE] if you don't wish to have one
const int sizeAddress = 10;

// An array of all the satellite station addresses
// The first 8 bytes are the serial number
// Make sure the order of these addresses matches the order of the stationNames array 
byte satellites[numStations][sizeAddress] = {{0x00, 0x13, 0xA2, 0x00, 0x42, 0x2B, 0x1E, 0xA2, 0xFF, 0xFE}, {0x00, 0x13, 0xA2, 0x00, 0x42, 0x33, 0x67, 0xDD, 0xFF, 0xFE}, {0x00, 0x13, 0xA2, 0x00, 0x42, 0x2F, 0xDB, 0xA7, 0xFF, 0xFE}, {0x00, 0x13, 0xA2, 0x00, 0x42, 0x33, 0x67, 0xB0}, {0x00, 0x13, 0xA2, 0x00, 0x42, 0x33, 0x67, 0xCC, 0xFF, 0xFE}};

// An array used in the loop function to determine if all the data has been sent from each station
// Each slot corresponds to the stations listed in the stationNames array
bool dataSent[numStations];

// The final String that will be sent to the CS data logger
// over serial communication
String stringToSend;

// The time to delay between transmiting a message and waiting for a response (in seconds)
// Having this delay helps make sure a message comes through in its entirety.
uint32_t wait = 10;

// This variable helps track if when measurements were last requested so the central station's
// radio is not perpetually requesting data
int prevLogHour;

// This variable helps signal that not only are we in a timing interval that's appropriate to log
// but also that we've met other conditions needed
bool timeToLog;

// Variable used in the checksum function
int sum;

// The amount of times you want to try and make contact with a station before giving up
int totalTries = 7;

// Possible character messages to send to a satellite station
char ready[] = "R";  // "Are you ready to start communicating?"
char time[] = "T";  // "Could I get the timestamp?"
char var[] = "V";  // "How many variables did you measure?"
char number[] = "N";  // "Can I get the measurement made number I just sent you?"

// This is a place to store any information the XBee reads into the Mayfly's
// serial port. This is referred to as a buffer usually when referring to serial communication.
byte rx[64];

// This function calculates the most signifcant byte.
// It assumes that message includes a null terminator at the end which should make it easy
// to just use the sizeof() function as the size parameter. The 13 is there because
// this function is solely used for making a Transmit Request on the XBee, and the amount
// of other bytes that it cares about when checking the frame is a fixed value aside from the
// actual message being sent.
byte lengthMSB(int size) {
  return byte((13 + size) / 255);
}

// This function calculates the least signifcant byte.
// It assumes that message includes a null terminator at the end which should make it easy
// to just use the sizeof() function as the size parameter. The 13 is there because
// this function is solely used for making a Transmit Request on the XBee, and the amount
// of other bytes that it cares about when checking the frame is a fixed value aside from the
// actual message being sent.
byte lengthLSB(int size) {
  return byte((13 + size) % 255);
}

// This function calculates the checksum of a frame that is about to be sent on the XBee. This
// function is only useful for sending strings of characters (char[]). The size parameter is just
// the output using the sizeof(rfData[]). No need to try and account for it using sizeof(). 
// The station index refers to the placement order of each station in the stationNames array.
// The last three variables are described in the XBee manual section about transmit request frames.
// I leave them as 0x00 for each. 
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

// This functions pushes a transmit request to the XBee through the Mayfly's serial port.
// The XBee then attempts to send the message to the station specified with the stationIndex parameter.
// The parameters are all the same for this as the checksum function.
// message[] and messageSize are rfData[] and size respectively. I chose to have checksum be its
// own separate function so I could debug more easily and to illustrate clearly how the transmit
// request works. I leave the frameID parameter as 0x00 because then the XBee won't try and tell
// me if it made contact with the other XBee. I'll know if I have contact if I receive data back.
// Changing this to any other value you want will kick messages back to the Mayfly which can be
// helpful for debugging but can make things hard to manage when you have limited buffer space
// for incoming messages.
void transmitRequest(char message[], int messageSize, byte frameID, int stationIndex, byte broadcastRadius, byte options) {
  Serial1.write(0x7E);  // Starting delimeter
  Serial1.write(lengthMSB(messageSize));  // Most significant byte
  Serial1.write(lengthLSB(messageSize));  // Least significant byte
  Serial1.write(0x10);  // Frame type (10 for transmit requests)
  Serial1.write(frameID);  // An arbitrary value unless set to 0x00, which will stop the XBee from telling the Mayfly if it made contact
  Serial1.write(satellites[stationIndex], 10);  // The destination address of the station the message is intended for
  Serial1.write(broadcastRadius);  // Set to the default 0x00 because that will give the maximum broadcast hops (no need to change really)
  Serial1.write(options);  // Just set to 0x00
  Serial1.write(message);  // The message being sent. The Serial1.write() method will take care of sending the string as hexidecimals
  Serial1.write(checksum(message, messageSize, stationIndex, frameID, broadcastRadius, options));  // Final checksum for quality control
}

// This is a handy debugging function for printing out what is currently stored in a buffer
// Simply use the buffer variable name and sizeof(buffer variable name)
void serialPrintBuffer(byte buffer[], int bufferSize) {
  for (int i = 0; i < bufferSize; i++) {  // For each byte in the buffer
    Serial.print(buffer[i], HEX);  // Print the byte
    if (i == bufferSize - 1) {  // and check if the byte is the last one.
      Serial.println();  // If so, then end the printed line,
    } else {  // otherwise
      Serial.print(" ");  // add a space to make it easy to differentiate bytes
    }
  }
}

// This function will wait for a specified amount of time or until bytes appear in the Serial1 buffer
void waitForReceive(uint32_t secondsWait) {
  //Serial.println("Waiting for response");
  //digitalWrite(redLED, HIGH);
  //DateTime start = rtc.now();  // RTC object declaration
  uint32_t start_ts = rtc.now().getEpoch();  // Start of function timestamp
  //Serial.print("Waiting start: ");
  //Serial.println(start_ts);
  delay(1000);
  //DateTime end = rtc.now();
  //uint32_t end_ts = rtc.now().getEpoch();  // Most recent timestamp
  while (Serial1.available() == 0) {  // Stay in this loop if we have not waited the full wait time and there is nothing in the buffer
    if (rtc.now().getEpoch() - start_ts > secondsWait) {
      //Serial.println(rtc.now().getEpoch() - start_ts);
      break;
    }
    //DateTime now = rtc.now();
    //uint32_t now_ts = start.getEpoch();
    //Serial.print("Waiting now: ");
    //Serial.println(now_ts);
    //Serial.println(now_ts - start_ts);
    //end_ts = now_ts;
  }
  //Serial.println("End wait");
  delay(100);
  //digitalWrite(redLED, LOW);
}

// Arduino setup function that runs each time the Mayfly is powered on
// This is not the same as waking up from a sleeping mode.
// The setup function runs when there is a complete shut off and turn on.
void setup() {
  // XBee setup that should be done each time the board powers on
  pinMode(xbeeRegulatorPin, OUTPUT);  // 
  pinMode(xbeeSleepPin, OUTPUT);
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

  // Set the baud (communication) rate between the Mayfly and the CS datalogger connected over UART-0
  // Make sure this is compatible with what the datalogger is expecting (i.e. not too fast for it)
  Serial.begin(9600);

  // Set this to false to start
  timeToLog = false;

  // The default prevLogHour value will 0 after turning back on from a shutoff, which could be a potential hour (midnight)
  // that we haven't logged data. So in the off chance that the data logger shuts off and wakes back up at midnight during the logging interval
  // I'm setting the prevLogHour to a number that can't exist for the time
  prevLogHour = 30;
}

// Arduino loop function that continuously runs unless the board shuts off (which is not the same as sleeping)
void loop() {
  // Check to see if it is time to collect data by checking whether the current minute falls in an appropriate
  // interval and that we aren't requested data that was already sent or we already tried to get
  //DateTime now = rtc.now();  // Make a timestamp
  //int currLogHour = now.hour();  // Grab the hour from that timestamp
  
  /*
  if (prevLogHour == 0) {  // If the previously logged hour is zero, that means this will be the first logging since the Mayfly powered off,
    timeToLog = true;  // so we don't need to worry about waiting a certain amount of time
  }
  
  // This if statement is set up based on the five-minute window set up in the following if statement
  if (currLogHour != prevLogHour) {  // If the current hour is different than the previously noted hour
    timeToLog = true;  // then we are good to ask for data when the next interval comes around
  }
  */
  if (rtc.now().minute() == 1) {  // If the current minute is on the one
    timeToLog = true;  // then it's time to log and collect new data
  }

  if (rtc.now().hour() != prevLogHour && rtc.now().minute() > 1 && rtc.now().minute() < 8) {  // If the current hour is different than the previously logged hour and we fall in the window of when the satellite stations are listening for signals
    timeToLog = true;  // then it's time to log and collect new data
  }

  // If we are in the window where the satellite stations will be waiting to transmit data, timeToLog variable is true
  // Even though it will start logging when it first hits that window, I'm keeping it like this in case the Mayfly powers off and powers back
  // on inside a potential window when the satellite station is still listening for a data request
  if (timeToLog) {
    prevLogHour = rtc.now().hour(); // Set the previous logging hour to the current one since we are now logging
    digitalWrite(xbeeSleepPin, LOW);  // Wake the XBee up
    digitalWrite(redLED, HIGH);
    delay(1000);  // Let the XBee's stomach settle

    // Clear out the buffer so there is no confusion about what's in the UART-1
    memset(rx, 0x00, sizeof(rx));
    Serial1.readBytes(rx, Serial1.available());
    memset(rx, 0x00, sizeof(rx));

    // Set the boolean array to all falses to start out because no data has been sent
    for (int i = 0; i < sizeof(dataSent); i++) {
      dataSent[i] = false;
    } 

    // This boolean array helps keep track of which stations got to have their maximum attempts at communication
    // much like the dataSent array.
    bool didTotalTries[numStations];

    // Set the boolean array to all falses since no tries have been attempted yet
    for (int u = 0; u < numStations; u++) {
      didTotalTries[u] = false;
    }
    
    // We will now attempt to collect data from each station
    for (int i = 0; i < numStations; i++) {  // For each station
      //greenredflash(20);
      delay(1000);
      int tries = 0;  // start off with a clean slate of no tries attempted yet
      while (!dataSent[i] && !didTotalTries[i]) {  // While we don't have the data and we haven't used up all the tries we specified
        greenredflash(i);
        digitalWrite(redLED, HIGH);
        //delay(1000);
        tries++;  // then we'll try again
        //Serial.print(stationNames[i]);
        //Serial.print(" try: ");
        //Serial.println(tries);
        stringToSend = "";  // Prep an empty String we'd like to send over radio if we do get something this go-around
        transmitRequest(ready, sizeof(ready), 0x00, i, 0x00, 0x00);  // Ask if station is ready
        //waitForReceive(wait);  // Give a chance for the satellite to respond (the XBees take care of retries)
        delay(wait * 1000);
        if (Serial1.available() > 0) {  // If there is something in the UART-1, that means we've received a message, because we cleared it out a few lines ago
          int count = Serial1.available();
          Serial1.readBytes(rx, count);  // Read in everything that was there
          if (rx[15] == 0x52) {  // Check if byte 15 was an 'R' (the satellites station's response that it is ready)
            //Serial.print("Received an R from ");
            //Serial.println(stationNames[i]);
            digitalWrite(greenLED, HIGH);
            memset(rx, 0x00, sizeof(rx));  // If so, clear out the buffer and start adding the data to the String we'll be sending
            stringToSend += "@station=";
            stringToSend += stationNames[i];  // The first data point is the name of the station this data is collected from
            stringToSend += ";";  // Add the comma seperator
            //delay(1000);
            delay(500);
            //digitalWrite(greenLED, HIGH);
            transmitRequest(time, sizeof(time), 0x00, i, 0x00, 0x00);  // Ask for a timestamp of the recorded data
            delay(500);
            digitalWrite(greenLED, LOW);
            waitForReceive(wait);  // Give a chance for the satellite to respond
            //delay(wait * 1000);
            if (Serial1.available() > 0) {  // If we received something (hopefully a timestamp)
              count = Serial1.available();
              Serial1.readBytes(rx, count);  // then read in the new message
              // Add the timestamp to the string
              //Serial.print("Received timestamp from ");
              //Serial.println(stationNames[i]);
              stringToSend += "@timestamp=";
              for (int z = 15; z < count - 1; z++) {  // by starting with the 15th byte and going up to the second-to-last byte
                stringToSend += (char)rx[z];  // Convert each byte to a char and tack it onto the String we'll be sending
                if (z == count - 2) {  // If we've come to the last byte of interest
                  stringToSend += ";";  // then add on the comma seperator
                }
              }
              memset(rx, 0x00, sizeof(rx));  // Clear out the buffer
              transmitRequest(var, sizeof(var), 0x00, i, 0x00, 0x00);  // Ask for the amount of variables measured
              waitForReceive(wait);  // Wait for a response
              //delay(wait * 1000);
              if (Serial1.available() > 0) {  // If we got a response (hopefully with the amount of variables measured)
                int count = Serial1.available();
                Serial1.readBytes(rx, count);  // Read in that response
                int varCount = rx[15];  // Store number given in the payload (15th byte) in the varCount variable
                //Serial.print(stationNames[i]);
                //Serial.print(" variable count: ");
                //Serial.println(varCount);
                // I'm not certain the while (gettingDataVaules) loop is necessary
                bool gettingDataValues = true;
                while (gettingDataValues) {
                  for (int b = 0; b < varCount; b++) {  // For each variable measured by the satellite station
                    //Serial.print("Getting variable ");
                    //Serial.println(b);
                    memset(rx, 0x00, sizeof(rx));  // Make space for the variable name we anticipate receiving first in the buffer

                    // The transmitRequest function takes in a char array as the message, not a number value,
                    // and we want to send over an actual number that the satellite station will interepret as: 
                    // "the central station wants the name of variable number X",
                    // so here we will just customize a transmit request frame to request that
                    int sum = 0;  // Starting for the frame's checksum
                    sum += 0x10;  // Add the value of the frame type's byte
                    sum += 0x00;  // Add the value of the frame ID's byte
                    
                    // Add the value of the address's bytes
                    for (int x = 0; x <= sizeof(satellites[i]) - 1; x++) {
                      sum += satellites[i][x];
                    }
                    
                    sum += 0x00;  // Add the broadcast radius
                    sum += 0x00;  // Add the options
                    sum += b;  // Add the payload (the number being requested)

                    sum = (255 - (sum % 256));  // Calculate the checksum

                    // Send the transmit request to the XBee
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
                    //delay(wait * 1000);
                    if (Serial1.available() == 0) {  // If we didn't hear anything
                      gettingDataValues = false;
                      break;  // Break the for loop it's feast or famine
                    } else {  // If we did hear something (hopefully the variable name)
                      int count = Serial1.available();
                      Serial1.readBytes(rx, count);  // Read in the message
                      
                      // Record the payload
                      stringToSend += "@";
                      for (int c = 15; c < count - 1; c++) {  // For each byte in the payload section of the received message
                        stringToSend += (char)rx[c];  // convert the byte into a character and add it to the String
                        if (c == count - 2) {  // If this is the last byte
                          stringToSend += "=";  // then add the comma separator
                        }
                      }

                      // Get ready to ask for that variables actual measurement taken
                      memset(rx, 0x00, sizeof(rx));  // Make some space for a new message
                      transmitRequest(number, sizeof(number), 0x00, i, 0x00, 0x00);  // Ask for the recorded measurement
                      waitForReceive(wait);  // Wait for a response
                      //delay(wait * 1000);
                      if (Serial1.available() == 0) {  // If we didn't receive anything
                        gettingDataValues = false;
                        break;  // Break the for loop. Feast or famine
                      } else {  // If we did hear something (hopefully the measurement)
                        count = Serial1.available();
                        Serial1.readBytes(rx, count);  // Read in the message

                        // Record the payload
                        for (int d = 15; d < count - 1; d++) {  // For each byte in the payload
                          stringToSend += (char)rx[d];  // convert the byte to a character and add it to the String
                        }
                        if (b != varCount - 1) {  // If it's not the last variable measured
                          stringToSend += ";";  // add on the semicolon seperator
                        } else {  // If it is the last measurement
                          stringToSend += ";@endofstation=1;";  // This helps with parsing data on the CR800 side
                          Serial.println(stringToSend);  // send out the string over the Serial UART-0 port
                          gettingDataValues = false;  // We no longer need to collect data values
                          dataSent[i] = true;  // and we can mark that this station has sent all its data.
                          didTotalTries[i] = true;  // Mark that we did the total tries just so the program doesn't think we missed a station
                          greenredflash(10);
                          digitalWrite(redLED, HIGH);
                        }
                      }
                    }
                  }
                }
              }
            }
          } else {  // If we received something other than an 'R'
            memset(rx, 0x00, sizeof(rx));  // then get rid of it and we'll take another crack at it
          }
        }
        if (tries == totalTries) {  // If we've tried as much as we'd like
          didTotalTries[i] = true;  // then take note of it for this station so it doesn't try again
        }
      }
    }
    // At this point, either all the data was sent, or we ran out of tries
    digitalWrite(xbeeSleepPin, HIGH);  // Put the XBee back to sleep
    timeToLog = false;  // We don't want to log until the right conditions again
    digitalWrite(redLED, LOW);
  }
}
