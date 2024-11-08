#include <Arduino.h>
#include <AltSoftSerial.h>
AltSoftSerial sonarSerial(6, -1);

void setup() 
{
  Serial.begin(57600);
  sonarSerial.begin(9600);

  digitalWrite(22, HIGH);
  pinMode(6, INPUT);

  Serial.println("Sonar test");

  delay(5000);
}

void loop() 
{
  sonarSerial.read();
  int snow_depth = sonarSerial.parseInt();
  
  
  Serial.println(snow_depth);
  delay(100);
}
