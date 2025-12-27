// This adds extra code that will help the program run such as the code
// needed for the real time clock found on the Mayfly.
#include <Arduino.h>
#include <Wire.h>
#include "Sodaq_DS3231.h"

// Pin numbers for useful LEDs on the Mayfly that sometimes help to troubleshoot
const int8_t redLED = 9;
const int8_t greenLED = 8;

// Pins for power supply and sleep control of the XBee
#define xbeeRegulatorPin 18
#define xbeeSleepPin 23 

const int xbeeBaud = 9600;


void setup() {
  // XBee setup that should be done each time the board powers on
  pinMode(xbeeRegulatorPin, OUTPUT);  // 
  pinMode(xbeeSleepPin, OUTPUT);
  digitalWrite(xbeeRegulatorPin, HIGH);  // Supply power to the XBee regulator
  Serial1.begin(xbeeBaud);  // Begin UART-1 communication

  Wire.begin();

  digitalWrite(xbeeSleepPin, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

}
