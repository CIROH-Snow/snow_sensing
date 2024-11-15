/*
This sketch logs the amperage measured across the Adafruit INA260 sensor and logs it to a microSD card
on the EnvrioDIY's Mayfly datalogger.

Sensor Wiring (Adafruit INA260 Sensor):
VCC    --> 3.3V (Arduino)
GND    --> GND (Arduino)
SCL    --> SCL (Arduino)
SDA    --> SDA (Arduino)

The wiring is fairly intuitive and follows I2C. You'll need to solder the header pins to the sensor and then likely connect
these with a breadboard and jumper wires. The Mayfly has a Grove connector that is meant for I2C integration.
You'll just need to use a screw terminal to Grove adapter in order to use a Grove cable. This may be more
secure than just finding and jumpering the wires into the Mayfly headers. 

Station wiring:
The current of electricity needs to flow through the screw terminal on the INA260 sensor, going in the
V+ gate and out out the V- gate. If logging current draw for the entire set up, you will want this sensor
on a wire coming right off the positive terminal on your battery powering the station. You'll need a wire going from the positive node
of your battery to the V+ gate of the INA260 and then a wire going from the V- gate of the INA260 to the
station setup like you would normally have.

This SD library only allows short file names, so sticking with "amp.csv" should be fine. 

This will record everything in CSV format.

There is some functionality where you can use the Arduino IDE serial monitor to see what is being read, however, it is not necessary to connect your laptop for this program to run after uploading the sketch.

The only thing you should worry about changing are the scanInterval and recordInterval variable declarations.
scanInterval is how often you would like to see the amperage measurement if you are watching your serial monitor.
recordInterval is how often you would like for the Mayfly to make an amperage measurement and record that the microSD card.
*/

// Include the SD library
#include <SD.h>

// This should be included with all Arduino sketches
#include <Arduino.h>
// This library helps with I2C communication
#include <Wire.h>
// Include the RTC library
#include "Sodaq_DS3231.h"

// Include the sensor library
#include <Adafruit_INA260.h>
// Create a new instance of the INA260 class
Adafruit_INA260 ina260 = Adafruit_INA260(); 

// Declare some needed variables making measurements
unsigned long scanInterval = 1;     // Time between scans within the main loop in seconds
unsigned long recordInterval = 1;   // Time between recorded values in seconds
unsigned long currMillis = 0;       // Timing variable
unsigned long prevMillis = 0;       // Timing variable
unsigned long prevRecordMillis = 0; // Timing variable
unsigned long scanCounter = 0;      // A variable to hold a count of scans
unsigned long recordCounter = 0;    // A variable to hold a count of output recordsint recordNum = 1; 

void setup() {
  Serial.begin(115200);
  // Wait until serial port is opened
  while (!Serial) { delay(10); }

  Serial.println("Adafruit INA260 Test");

  // Check to see if the board could find the sensor connected to it
  if (!ina260.begin()) {
    Serial.println("Couldn't find INA260 chip");
    // If it didn't find anything stay stuck in this loop
    while (1);
  }
  Serial.println("Found INA260 chip");
  
  // Pin 12 is where the CS connection is located on the Mayfly
  if (! SD.begin(12)){
    Serial.println("SD card initialization failed!");
    return;
  }
  
  // Initialization of the SD card was successful
  Serial.println("SD card initialization done.");

  // Create a new file on the SD card to which you want to write
  // Only one file can be open at a time
  // If you want to use a different file, close this one first
  // NOTE: The file name needs to be 8 characters or less!
  File myFile = SD.open("amp.csv", FILE_WRITE);

  // Print a line of text to the open file
  // Also send the text to the serial port so we can see what's going on
  myFile.println("Snow Station Amperage Draw");
  //Serial.println("Hydroinformatics - Assignment 2");
  
  // Print a header line to the file with column names
  myFile.println("Time (sec), DateTime, Amps (mA)");
  Serial.println("Time (sec), DateTime, Amps (mA)");
  
  // Close the file 
  myFile.close();
  }

void loop(){
  // Get the current time 
  // Number of milliseconds since the program started
  currMillis = millis();

  // Check timing to see if the scan interval has been reached
  if ((currMillis - prevMillis) >= (scanInterval * 1000)){
    // If YES, do a scan
    // Perform measurements and calculations here
    // Get new measurements of temperature and humidity from the sensor
    // aht.getEvent() gets the humidty and temperature values as sensor event objects
    // The "&" in front of the variable passes the memory address of the
    // variable to the function so the function can modify it.
    float current = ina260.readCurrent();
    
    // Create a string with output for this scan to print to the serial port
    // Use the "+" operator to concatenate strings together
    // The "String()" function converts numbers to strings
    String dateTime = String(rtc.now().year()) + "/" + String(rtc.now().month()) + "/" + String(rtc.now().date()) + " " + String(rtc.now().hour()) + ":" + String(rtc.now().minute()) + ":" + String(rtc.now().second());
    String stringToPrint = String(currMillis/1000) + ", " + dateTime + ", " + String(current);
    Serial.println(stringToPrint);

    // Manage the timing variables to reflect that I just finished a scan
    prevMillis = currMillis;

    // Check to see if it's time to record data
    if ((currMillis - prevRecordMillis) >= (recordInterval * 1000)){
      // Record a data record - for now print to the serial port 
      // A later example will show how to write to a file
      // ----------------------------------------------------------------
      // Calculate the average value of the random number

      File myFile = SD.open("amp.csv", FILE_WRITE);      

      // Create a string to record the output data to the serial port
      String recordToPrint = String(currMillis / 1000) + ", " + dateTime + ", " + String(current);
      Serial.println(recordToPrint);
      myFile.println(recordToPrint);      
     
      // Manage the timing variables to reflect that I just recorded data
      // and reset the scanCounter and randomSum variables for the next scan
      prevRecordMillis = currMillis;

      myFile.close();
    }
  } 
}
