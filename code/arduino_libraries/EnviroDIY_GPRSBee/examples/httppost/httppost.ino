// Fill in your own APN here
#define APN "apn.konekt.io"

#include <Arduino.h>
#include <Wire.h>
#include <GPRSbee.h>

int BEE_DTR_PIN = 23;  // Bee DTR Pin (Data Terminal Ready - used for sleep)
int BEE_CTS_PIN = 19;   // Bee CTS Pin (Clear to Send)

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  gprsbee.init(Serial1, BEE_CTS_PIN, BEE_DTR_PIN);
  //Comment out the next line when used with GPRSbee Rev.4
  gprsbee.setPowerSwitchedOnOff(true);
  gprsbee.setDiag(Serial); // To see for debugging

  // Make sure the GPRSbee is switched off to begin with
  gprsbee.off();

  const char testData[] = "testdata3 = hello world with newline\n";
  Serial.println(testData);
  char buffer[50];
  memset(buffer, 0, sizeof(buffer));
  bool retval = gprsbee.doHTTPPOSTWithReply(APN, "http://httpbin.org/post",
      testData, strlen(testData), buffer, sizeof(buffer));
  if (retval) {
    Serial.print(("Post result: "));
    Serial.print('"');
    Serial.print(buffer);
    Serial.println('"');
  }
}
void loop()
{
}
