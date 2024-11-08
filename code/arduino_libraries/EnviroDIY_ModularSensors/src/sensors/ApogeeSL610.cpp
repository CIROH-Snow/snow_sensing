/**
 * @file ApogeeSL610.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.cpp by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Implements the ApogeeSL610 class.
 */


#include "ApogeeSL610.h"
#include <Adafruit_ADS1015.h>


// The constructor - need the power pin and address pin of thermistor
ApogeeSL610::ApogeeSL610(int8_t powerPin, uint8_t thermistorChannel,
                         uint8_t thermistori2cAddress,
                         uint8_t thermopilei2cAddress, uint8_t measurementsToAverage)
    : Sensor("ApogeeSL610", SL610_NUM_VARIABLES, SL610_WARM_UP_TIME_MS,
             SL610_STABILIZATION_TIME_MS, SL610_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage, SL610_INC_CALC_VARIABLES),
      _thermistorChannel(thermistorChannel),  
      _thermistori2cAddress(thermistori2cAddress),
      _thermopilei2cAddress(thermopilei2cAddress) {}

// Destructor
ApogeeSL610::~ApogeeSL610() {}


String ApogeeSL610::getSensorLocation(void) {
#ifndef MS_USE_ADS1015
    String sensorLocation = F("ADS1115_0x");
#else
    String sensorLocation = F("ADS1015_0x");
#endif
    sensorLocation += String(_thermopilei2cAddress, HEX);
    sensorLocation += F("_Channels_2_3");
    return sensorLocation;
}


bool ApogeeSL610::addSingleMeasurementResult(void) {
    // Variables to store the results in
    float thermistorVoltage = -9999;
    float thermopileVoltage = -9999;
    float calibResult = -9999;

    // Rt variable for determining if temperature is above or below 0 deg C
    float Rt;

    // Pygreometer temperature variable
    float sl610Temp;

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

        // Bump the gain up to 1x = +/- 4.096V range for thermistor
        // Thermistor return range is dependent on the excitation voltage supplied
        // Using 3.3 switched votls, GAIN_ONE is appropriate
        thermistor_ads.setGain(GAIN_ONE);

        // Bump the gain up to 16x = +/- 0.256V range for thermopile
        // Thermopile can return +/- 23.5 mV (0.023V)
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
        thermopileVoltage = thermopile_ads.readADC_Differential_2_3_V();  // Getting the reading
        MS_DBG(F("  thermopile_ads.readADC_Differential_2_3_V():"), thermopileVoltage);

        if ((thermistorVoltage < 3.6 && thermistorVoltage > -0.3) && (thermopileVoltage * 1000 > -23.5 && thermopileVoltage * 1000 < 23.5)) {
            // Skip results out of range
            Rt = 24900 * (thermistorVoltage / (3.3 - thermistorVoltage));
            if (Rt > 94980) { 
                sl610Temp = 1 / (A_LESS + B_LESS * log(Rt) + C_LESS * pow(log(Rt), 3));
            } else {
                sl610Temp = 1 / (A_MORE + B_MORE * log(Rt) + C_MORE * pow(log(Rt), 3));
            }
            calibResult = SL610_CALIBRATION_FACTOR_K1 * 1000 * thermopileVoltage + 
                          SL610_CALIBRATION_FACTOR_K2 * STEFAN_BOLTZMANN_CONST *
                    pow(sl610Temp, 4);
            MS_DBG(F("  calibResult:"), calibResult);
        } else {
            // set invalid voltages back to -9999
            thermistorVoltage = -9999;
            thermopileVoltage = -9999;
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(SL610_OLWR_VAR_NUM, calibResult);
    verifyAndAddMeasurementResult(SL610_THERMISTOR_VOLTAGE_VAR_NUM, thermistorVoltage);
    verifyAndAddMeasurementResult(SL610_THERMOPILE_VOLTAGE_VAR_NUM, thermopileVoltage);

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
