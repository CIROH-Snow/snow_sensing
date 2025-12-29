/* ===================================================================
 * Author: Braedon Dority
 * File Name: repeater.ino
 * 
 * Description: Code for a simple repeater station for use within a
 * network of snow sensing stations. A repeater does not make 
 * measurements but participates in the radio network by relaying
 * messages from stations that may not have a clear radio path from 
 * thier location to the base station.
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
 * ACKNOWLEDGEMENTS: Thank you to the Stroud Water Research Center for 
 * developing the foundation to this code. This sketch is based on the 
 * examples made from their modular sensors library. The work of Sarah 
 * Damiano and Shannon Hicks made this possible.
 * =================================================================*/

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

// ==================================================================
// Arduino Setup function
// ==================================================================
void setup() {
  // XBee setup that should be done each time the board powers on
  pinMode(xbeeRegulatorPin, OUTPUT);  // 
  pinMode(xbeeSleepPin, OUTPUT);
  digitalWrite(xbeeRegulatorPin, HIGH);  // Supply power to the XBee regulator
  Serial1.begin(xbeeBaud);  // Begin UART-1 communication

  Wire.begin();

  digitalWrite(xbeeSleepPin, LOW);
}

// ==================================================================
// Arduino loop function
// ==================================================================
void loop() {
  // The Arduino does not do anything while powered on. It simply
  // runs to provide power to the XBee radio so that the radio
  // can participate in the radio network as a repeater.
}
