#include <Arduino.h>
#include <AltSoftSerial.h>
AltSoftSerial sonarSerial(6, -1);

byte buff[500];

void setup() 
{
  Serial.begin(57600);
  sonarSerial.begin(9600);

  digitalWrite(22, HIGH);
  pinMode(6, INPUT);

  Serial.println("Sonar test");

  delay(50);
}

void loop() 
{
  sonarSerial.readBytes(buff, sonarSerial.available());

  for (int i = 0; i < sizeof(buff); i++) {
    if (buff[i] != 0x00) {
      Serial.print((char) buff[i]);
      //Serial.print(" ");
      if (buff[i] == 0x0D) {
        Serial.println();
      }
    }
  }
  memset(buff, 0x00, sizeof(buff));
}
