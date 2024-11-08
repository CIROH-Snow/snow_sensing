/*
SENSOR ERROR CODES
The TEROS 11/12 has three error codes:
•
-9999 is output in place of the measured value if the sensor detects that the measurement function has been compromised and the subsequent measurement values have no meaning
•
-9992 is output in place of the measured value if the sensor detects corrupt or lost calibrations
•
-9991 is output in place of the measured value if the sensor detects insufficient voltage to perform the measurement
*/

#include <SDI12.h>

SDI12 mySDI12(7);

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  Serial.begin(57600);
  mySDI12.begin();
  digitalWrite(22, HIGH);

}

void loop() {
  mySDI12.sendCommand("aM!");  // Tell the sensor with address 'a' to make a measurement
  delay(30);  // Let things settle
  mySDI12.clearBuffer();  // I think this makes way for a new command to be sent in.
  delay(1000);
  mySDI12.sendCommand("aD0!");  // Get the data from address 'a'
  delay(30);
  String sdiResponse_a = "";  // Create a blank String that will become the reported value
  while (mySDI12.available()) {
    char c = mySDI12.read();  // Read in a character
    if ((c != '\n') && (c != '\r')) {  // If the read-in character isn't a new line
      sdiResponse_a += c;  // Add the character to the String
      delay(10);
    }
  }
  mySDI12.clearBuffer();  // Clear the buffer for the next command
  float vmc_a = getValue(sdiResponse_a, '+', 1).toFloat() * 0.0003879 - 0.6956;  // Pull out the volumetric water content using the getValue function and convert it using equation 5 from the Teros 12 manual

  mySDI12.sendCommand("bM!");
  delay(30);
  mySDI12.clearBuffer();
  delay(1000);
  mySDI12.sendCommand("bD0!");
  delay(30);
  String sdiResponse_b = "";
  while (mySDI12.available()) {
    char c = mySDI12.read();
    if ((c != '\n') && (c != '\r')) {
      sdiResponse_b += c;
      delay(10);
    }
  }
  mySDI12.clearBuffer();
  float vmc_b = getValue(sdiResponse_b, '+', 1).toFloat() * 0.0003879 - 0.6956; 

  mySDI12.sendCommand("cM!");
  delay(30);
  mySDI12.clearBuffer();
  delay(1000);
  mySDI12.sendCommand("cD0!");
  delay(30);
  String sdiResponse_c = "";
  while (mySDI12.available()) {
    char c = mySDI12.read();
    if ((c != '\n') && (c != '\r')) {
      sdiResponse_c += c;
      delay(10);
    }
  }
  mySDI12.clearBuffer();
  float vmc_c = getValue(sdiResponse_c, '+', 1).toFloat() * 0.0003879 - 0.6956;

  Serial.println(sdiResponse_a);
  Serial.println(sdiResponse_b);
  Serial.println(sdiResponse_c);
}
