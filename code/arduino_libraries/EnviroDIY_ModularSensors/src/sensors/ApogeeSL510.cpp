/**
 * @file ApogeeSL510.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.cpp by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Implements the ApogeeSL510 class.
 */


#include "ApogeeSL510.h"
#include <Adafruit_ADS1015.h>


// The constructor - need the power pin and address pin of thermistor
ApogeeSL510::ApogeeSL510(int8_t powerPin, float k1calib, float k2calib,
                         uint8_t thermistorChannel,
                         uint8_t thermistori2cAddress,
                         uint8_t thermopilei2cAddress,
                         uint8_t measurementsToAverage)
    : Sensor("ApogeeSL510", SL510_NUM_VARIABLES, SL510_WARM_UP_TIME_MS,
             SL510_STABILIZATION_TIME_MS, SL510_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage, SL510_INC_CALC_VARIABLES),
      _k1calib(k1calib),
      _k2calib(k2calib),
      _thermistorChannel(thermistorChannel),  
      _thermistori2cAddress(thermistori2cAddress),
      _thermopilei2cAddress(thermopilei2cAddress) {}

// Destructor
ApogeeSL510::~ApogeeSL510() {}


String ApogeeSL510::getSensorLocation(void) {
#ifndef MS_USE_ADS1015
    String sensorLocation = F("ADS1115_0x");
#else
    String sensorLocation = F("ADS1015_0x");
#endif
    sensorLocation += String(_thermopilei2cAddress, HEX);
    sensorLocation += F("_Channels_0_1");
    return sensorLocation;
}


bool ApogeeSL510::addSingleMeasurementResult(void) {
    // Variables to store the results in
    float thermistorVoltage = -9999;
    float thermopileVoltage = -9999;
    float calibResult = -9999;

    // Rt variable for determining if temperature is above or below 0 deg C
    float Rt;

    // Pygreometer temperature variable
    float sl510Temp;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

// Create an Auxillary ADD object
// We create and set up the ADC object here so that each sensor using
// the ADC may set the gain appropriately without effecting others.
#ifndef MS_USE_ADS1015
        Adafruit_ADS1115 thermistor_ads(_thermistori2cAddress);  // Use this for the 16-bit version
        Adafruit_ADS1115 thermopile_ads(_thermopilei2cAddress);
#else
        Adafruit_ADS1015 thermistor_ads(_thermistori2cAddress);  // Use this for the 12-bit version
        Adafruit_ADS1015 thermopile_ads(_thermopilei2cAddress);
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
        thermistor_ads.setGain(GAIN_ONE);
        thermopile_ads.setGain(GAIN_SIXTEEN);
        // Begin ADCs
        thermistor_ads.begin();
        thermopile_ads.begin();

        // Read Analog to Digital Converter (ADC)
        // Taking this reading includes the 8ms conversion delay.
        // We're allowing the ADS1115 library to do the bit-to-volts conversion
        // for us
        thermistorVoltage =
            thermistor_ads.readADC_SingleEnded_V(_thermistorChannel);  // Getting the reading
        MS_DBG(F("  thermistor_ads.readADC_SingleEnded_V("), _thermistorChannel, F("):"),
               thermistorVoltage);
        thermopileVoltage = thermopile_ads.readADC_Differential_0_1_V();  // Getting the reading
        MS_DBG(F("  thermopile_ads.readADC_Differential_0_1_V():"), thermopileVoltage);

        if ((thermistorVoltage < 3.6 && thermistorVoltage > -0.3) && (thermopileVoltage * 1000 > -23.5 && thermopileVoltage *1000 < 23.5)) {
            // Skip results out of range
            // Apogee SL-510 calibration factors are dependent on each sensor and are noted in the sensor's calibration certificate
            Rt = 24900 * (thermistorVoltage / (3.3 - thermistorVoltage));
            if (Rt > 94980) { 
                sl510Temp = 1 / (A_LESS + B_LESS * log(Rt) + C_LESS * pow(log(Rt), 3));
            } else {
                sl510Temp = 1 / (A_MORE + B_MORE * log(Rt) + C_MORE * pow(log(Rt), 3));
            }
            calibResult = _k1calib * 1000 * thermopileVoltage + 
                          _k2calib * STEFAN_BOLTZMANN_CONST * pow(sl510Temp, 4);
            MS_DBG(F("  calibResult:"), calibResult);
        } else {
            // set invalid voltages back to -9999
            thermistorVoltage = -9999;
            thermopileVoltage = -9999;
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(SL510_ILWR_VAR_NUM, calibResult);
    verifyAndAddMeasurementResult(SL510_THERMISTOR_VOLTAGE_VAR_NUM, thermistorVoltage);
    verifyAndAddMeasurementResult(SL510_THERMOPILE_VOLTAGE_VAR_NUM, thermopileVoltage);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    if ((thermistorVoltage < 3.6 && thermistorVoltage > -0.3) &&
        (thermopileVoltage * 1000 > -23.5 && thermopileVoltage * 1000 < 23.5)) {
        return true;
    } else {
        return false;
    }
}
