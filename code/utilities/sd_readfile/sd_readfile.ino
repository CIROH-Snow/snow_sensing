/*
This sketch is meant to be used with your laptop and Arduino IDE's serial monitor.

This sketch allows a user to know all the files listed on a microSD card through a Mayfly,
tell the Mayfly which file it wants to open, and read the contents of that file in the serial
monitor. This data can then be copied out of the serial monitor into another file of your choosing
on your computer, effectively downloading the data from the microSD card without altering the file.

This should hopefully help you download data from your microSD cards if you do not have a laptop
that has built-in SD ports.

There is a lot going on in this sketch. All you really need to know in order to successfully use it
is the baud rate that your serial port needs to be set to.

The baud rate is found in the setup() function, and it is whatever the Serial.begin() parameter is set to,
which is likely 9600.

This sketch has not been tested with other boards besides the Mayfly, and you will likely need to make adjustments
if you are not using the Mayfly.
*/

#include "SdFat.h"

// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 1
/*
  Change the value of SD_CS_PIN if you are using SPI and
  your hardware does not use the default value, SS.
  Common values are:
  Arduino Ethernet shield: pin 4
  Sparkfun SD shield: pin 8
  Adafruit SD shields and modules: pin 10
*/

// SDCARD_SS_PIN is defined for the built-in SD on some boards.
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else   // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif  // SDCARD_SS_PIN

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(50)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS

#if SD_FAT_TYPE == 0
SdFat sd;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
#else  // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE

char line[300];

//------------------------------------------------------------------------------
// Store error strings in flash to save RAM.
#define error(s) sd.errorHalt(&Serial, F(s))
//------------------------------------------------------------------------------
// Check for extra characters in field or find minus sign.
char* skipSpace(char* str) {
  while (isspace(*str)) str++;
  return str;
}
//------------------------------------------------------------------------------
bool parseLine(char* str) {
  char* ptr;

  // Set strtok start of line.
  str = strtok(str, ",");
  if (!str) {
    Serial.println("set strtok start of line fail");
    return false;
  }

  // Print text field.
  Serial.println(str);

  // Subsequent calls to strtok expects a null pointer.
  str = strtok(nullptr, ",");
  if (!str) {
    Serial.println("error 2");
    return false;
  }

  // Convert string to long integer.
  int32_t i32 = strtol(str, &ptr, 0);
  if (str == ptr || *skipSpace(ptr)) return false;
  Serial.println(i32);

  str = strtok(nullptr, ",");
  if (!str) return false;

  // strtoul accepts a leading minus with unexpected results.
  if (*skipSpace(str) == '-') return false;

  // Convert string to unsigned long integer.
  uint32_t u32 = strtoul(str, &ptr, 0);
  if (str == ptr || *skipSpace(ptr)) return false;
  Serial.println(u32);

  str = strtok(nullptr, ",");
  if (!str) return false;

  // Convert string to double.
  double d = strtod(str, &ptr);
  if (str == ptr || *skipSpace(ptr)) return false;
  Serial.println(d);

  // Check for extra fields.
  return strtok(nullptr, ",") == nullptr;
}
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  // Wait for USB Serial
  while (!Serial) {
    yield();
  }
  Serial.println("Type any character to start");
  while (!Serial.available()) {
    yield();
  }
  Serial.read();
  // Initialize the SD.
  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
    return;
  }

  // Print out the root directory
  sd.ls(LS_R);

  // Get the filename
  Serial.println("Enter the desired filename exactly as it is listed, including the file extension:");
  while (Serial.available() == 0) {
    ;
  }

  delay(100);
  String FileName = Serial.readString();
  char filename[30];
  FileName.toCharArray(filename, 30);
  Serial.print("Retrieving data from: ");
  Serial.println(filename);

  if (!file.open(filename, FILE_WRITE)) {
    error("open failed");
  }

  
  // Rewind file for read.
  file.rewind();
  

  while (file.available()) {
    Serial.write(file.read());
  }

  file.close();
  Serial.println(F("Done"));
  

}

void loop() {}
