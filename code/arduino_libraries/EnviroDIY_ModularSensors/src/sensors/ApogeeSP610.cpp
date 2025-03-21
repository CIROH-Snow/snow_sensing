/**
 * @file ApogeeSP610.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.cpp by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Implements the ApogeeSP610 class.
 */


#include "ApogeeSP610.h"
#include <Adafruit_ADS1015.h>


// The constructor - need the power pin
ApogeeSP610::ApogeeSP610(int8_t powerPin, float calibrationFactor,
                         uint8_t i2cAddress, uint8_t measurementsToAverage)
    : Sensor("ApogeeSP610", SP610_NUM_VARIABLES, SP610_WARM_UP_TIME_MS,
             SP610_STABILIZATION_TIME_MS, SP610_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage, SP610_INC_CALC_VARIABLES),
      _calibrationFactor(calibrationFactor),
      _i2cAddress(i2cAddress) {}

// Destructor
ApogeeSP610::~ApogeeSP610() {}


String ApogeeSP610::getSensorLocation(void) {
#ifndef MS_USE_ADS1015
    String sensorLocation = F("ADS1115_0x");
#else
    String sensorLocation = F("ADS1015_0x");
#endif
    sensorLocation += String(_i2cAddress, HEX);
    sensorLocation += F("_Channels_0_1");
    return sensorLocation;
}


bool ApogeeSP610::addSingleMeasurementResult(void) {
    // Variables to store the results in
    float adcVoltage  = -9999;
    float calibResult = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

// Create an Auxillary ADD object
// We create and set up the ADC object here so that each sensor using
// the ADC may set the gain appropriately without effecting others.
#ifndef MS_USE_ADS1015
        Adafruit_ADS1115 ads(_i2cAddress);  // Use this for the 16-bit version
#else
        Adafruit_ADS1015 ads(_i2cAddress);  // Use this for the 12-bit version
#endif
        // ADS Library default settings:
        //  - TI1115 (16 bit)
        //    - single-shot mode (powers down between conversions)
        //    - 128 samples per second (8ms conversion time)
        //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)
        //  - TI1015 (12 bit)
        //    - single-shot mode (powers down between conversions)
        //    - 1600 samples per second (625µs conversion time)
        //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)

        // Bump the gain up to 1x = +/- 4.096V range
        // Sensor return range is 0-2.5V, but the next gain option is 2x which
        // only allows up to 2.048V
        ads.setGain(GAIN_SIXTEEN);
        // Begin ADC
        ads.begin();

        // Read Analog to Digital Converter (ADC)
        // Taking this reading includes the 8ms conversion delay.
        // We're allowing the ADS1115 library to do the bit-to-volts conversion
        // for us
        adcVoltage =
            ads.readADC_Differential_0_1_V();  // Getting the reading
        MS_DBG(F("  ads.readADC_Differential_0_1_V():"),
               adcVoltage);

        if (adcVoltage * 1000 < 70 && adcVoltage * 1000 > -5) {
            // Skip results out of range
            // Apogee SP-610-SS output range 0 to 70 mV
            calibResult = 1000 * adcVoltage * _calibrationFactor;
            MS_DBG(F("  calibResult:"), calibResult);
        } else {
            // set invalid voltages back to -9999
            adcVoltage = -9999;
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(SP610_OSWR_VAR_NUM, calibResult);
    verifyAndAddMeasurementResult(SP610_VOLTAGE_VAR_NUM, adcVoltage);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    if (adcVoltage * 1000 < 70 && adcVoltage * 1000 > -70) {
        return true;
    } else {
        return false;
    }
}
