/*****************************************************************************
KellerTest.ino

Modified by Anthony Aufdenkampe, from YosemitechModbus/GetValues.ino
2018-April

For testing Keller functionality
Does not use functions in the KellerModbus library

*****************************************************************************/

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <AltSoftSerial.h>
#include <SensorModbusMaster.h>


// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------

// Define the sensor's modbus address
byte modbusAddress = 0x01;  // The sensor's modbus address, or SlaveID
// Keller defines the following:
//   Address 0 is reserved for broadcasting.
//   Addresses 1 (default) ...249 can be used for bus mode.
//   Address 250 is transparent and reserved for non-bus mode. Every device can be contacted with this address.
//   Addresses 251...255 are reserved for subsequent developments.

// Define pin number variables
const int PwrPin = 22;  // The pin sending power to the sensor *AND* RS485 adapter
const int DEREPin = -1;   // The pin controlling Recieve Enable and Driver Enable
                          // on the RS485 adapter, if applicable (else, -1)
                          // Setting HIGH enables the driver (arduino) to send text
                          // Setting LOW enables the receiver (sensor) to send text

// Construct software serial object for Modbus
AltSoftSerial modbusSerial;  // On Mayfly, requires connection D5 & D6

// Construct the modbus instance
modbusMaster modbus;


// ---------------------------------------------------------------------------
// Working Functions
// ---------------------------------------------------------------------------

// Give values to variables;
byte modbusSlaveID = modbusAddress;
byte _slaveID = modbusSlaveID;


int getSlaveID(void)
{
    return modbus.byteFromRegister(0x03, 0x020D, 2); // byte byteFromRegister(byte regType, int regNum, int byteNum)

}

long getSerialNumber(void)
{
    return modbus.uint32FromRegister(0x03, 0x0202); // uint32_t uint32FromRegister(byte regType, int regNum, endianness endian=bigEndian);
}

float calcWaterDepthM(float waterPressureBar, float waterTempertureC)
{
    /// Initialize variables
    float waterPressurePa;
    float waterDensity;
    float waterDepthM;
    const float gravitationalConstant = 9.80665; // m/s2, meters per second squared

    waterPressurePa = 1e5 * waterPressureBar;
    // Water density (kg/m3) from equation 6 from JonesHarris1992-NIST-DensityWater.pdf
    waterDensity =  + 999.84847
                    + 6.337563e-2 * waterTempertureC
                    - 8.523829e-3 * pow(waterTempertureC,2)
                    + 6.943248e-5 * pow(waterTempertureC,3)
                    - 3.821216e-7 * pow(waterTempertureC,4)
                    ;
    waterDepthM = waterPressurePa/(waterDensity * gravitationalConstant);  // from P = rho * g * h

    return waterDepthM;
}


// ---------------------------------------------------------------------------
// Main setup function
// ---------------------------------------------------------------------------
void setup()
{

    pinMode(PwrPin, OUTPUT);
    digitalWrite(PwrPin, HIGH);

    if (DEREPin > 0) pinMode(DEREPin, OUTPUT);

    Serial.begin(57600);  // Main serial port for debugging via USB Serial Monitor
    modbusSerial.begin(9600);  // The modbus serial stream - Baud rate MUST be 9600.

    // Start up the modbus sensor
    modbus.begin(modbusAddress, &modbusSerial, DEREPin);

    // Turn on debugging
    //modbus.setDebugStream(&Serial);

    // Start up note
    Serial.println("Keller Acculevel (or other Series 30, Class 5, Group 20 sensor)");
    Serial.println("Waiting for sensor and adapter to be ready.");
    delay(500);

    Serial.print("Device Address, as integer: ");
    Serial.println(getSlaveID());

    Serial.print("Serial Number: ");
    Serial.println(getSerialNumber());

    Serial.print("Firmware Version: ");
    Serial.print(modbus.byteFromRegister(0x03, 0x020E, 1));
    Serial.print(".");
    Serial.print(modbus.byteFromRegister(0x03, 0x020E, 2));
    Serial.print("-");
    Serial.print(modbus.byteFromRegister(0x03, 0x020F, 1));
    Serial.print(".");
    Serial.print(modbus.byteFromRegister(0x03, 0x020F, 2));
    Serial.println();

    Serial.println("Started!");
}

// Initialize variables
float waterPressureBar = -9999.0;
float waterPressurePa  = -9999.0;
float waterTempertureC = -9999.0;
float waterDensity = -9999.0;
float waterDepthM = -9999.0;
const float gravitationalConstant = 9.80665; // m/s2, meters per second squared

// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    Serial.println("Reading!");

    waterPressureBar = modbus.float32FromRegister(0x03,  0x0100);
    Serial.print("Presure (mbar): ");
    Serial.println(waterPressureBar, 6);

    waterTempertureC = modbus.float32FromRegister(0x03,  0x0102);
    Serial.print("Temperature (C): ");
    Serial.println(waterTempertureC, 4);

    // Calculate Water Depth in Meters
    waterDepthM = calcWaterDepthM(waterPressureBar, waterTempertureC);  // float calcWaterDepthM(float waterPressureBar, float waterTempertureC)
    Serial.print("Depth (mWC) by func: ");
    Serial.println(waterDepthM, 8);


    delay(3000);

}
