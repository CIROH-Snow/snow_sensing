/**
 * @file ApogeeSL610.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * This file was created based on the Apogee SP510/610 files created by Braedon
 * Dority, based on the Apogee SQ-212 files created by the people listed below:
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ApogeeSL610 sensor subclass and the variable subclasses
 * ApogeeSL610_OLWR, ApogeeSL610_Thermopile_Voltage, and ApogeeSL610_Thermistor_Voltage.
 *
 * These are used for the Apogee SL-610 outgoing longwave radiation sensor.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */
/* clang-format off */
/**
 * @defgroup sensor_sl610 Apogee SL-610
 * Classes for the Apogee SL-610 thermopile radiation sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_sl610_intro Introduction
 * The [Apogee SL-610 pyrgeometer downward looking
 * sensor](https://www.apogeeinstruments.com/sl-610-ss-pyrgeometer-downward-looking/)
 * measures [outgoing longwave radiation
 * (OLWR)](https://www.britannica.com/science/atmosphere/Carbon-budget) -
 * and is typically measured in watts per square meter. The
 * raw output from the sensor is twofold, one is a differential analog signal from the sensor's
 * thermopile, and the other is a single-ended analog reading from the thermistor. Both must be converted
 * to a digital signal and then used in an equation to get the OLWR. 
 * An excitation voltage is required for the thermistor. The power supply to the sensor can be
 * stopped between measurements.
 *
 * To convert the sensor's analog signal to a high resolution digital signal,
 * the sensor must be attached to an analog-to-digital converter.  See the
 * [ADS1115](@ref analog_group) for details on the conversion.
 *
 * The calibration factors for this sensor comes with the purchase of the sensor on its certificate of calibration.
 *
 * @section sensor_sl610_datasheet Sensor Datasheet
 * [Datasheet](https://www.apogeeinstruments.com/content/SL-510-610-manual.pdf)
 *
 * @section sensor_sl610_flags Build flags
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 * - ```-D SL610_CALIBRATION_FACTOR_K1=x```
 *      - Changes the calibration factor from 1 to x
 *  * - ```-D SL510_CALIBRATION_FACTOR_K2=x```
 *      - Changes the calibration factor from 1 to x
 *
 * @section sensor_sl610_ctor Sensor Constructor
 * {{ @ref ApogeeSL610::ApogeeSL610 }}
 *
 * This section with the examples code may need to be deleted.
 * ___
 * @section sensor_sq212_examples Example Code
 * The SQ-212 is used in the @menulink{apogee_sq212} example.
 *
 * @menusnip{apogee_sl610}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_APOGEESL610_H_
#define SRC_SENSORS_APOGEESL610_H_

// Debugging Statement
// #define MS_APOGEESL610_DEBUG

#ifdef MS_APOGEESL610_DEBUG
#define MS_DEBUGGING_STD "ApogeeSL610"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_sl610 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the SL610 can report 3 values, raw
/// voltage for thermistor, raw voltage for thermopile, and calculated incoming longwave radiation.
#define SL610_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; OLWR is calculated from the raw voltage.
#define SL610_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_sl610_timing
 * @name Sensor Timing
 * The sensor timing for an Apogee SL-610
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the warm up time is unknown; using the 2ms for
 * the TI ADS1x15 to warm up
 *
 * @todo Measure warm-up time of the SL-610
 */
#define SL610_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; the ADS1115 is stable after 2ms.
 *
 * The stabilization time of the SL-610 itself is not known!
 *
 * @todo Measure stabilization time of the SL-610
 */
#define SL610_STABILIZATION_TIME_MS 500
/// @brief Sensor::_measurementTime_ms; ADS1115 takes almost 2ms to complete a
/// measurement (860/sec).
#define SL610_MEASUREMENT_TIME_MS 500
/**@}*/

/**
 * @anchor sensor_sl610_olwr
 * @name OLWR
 * The OLWR variable from an Apogee SL-610
 * - Range is -200 to 200 W m-2
 * - Uncertainty in daily total is less than 5%
 * - Resolution (need to update for SL-610):
 *   - 16-bit ADC (ADS1115): 0.3125 µmol m-2 s-1 (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 5 µmol m-2
 * s-1 (ADS1015)
 * - Reported as microeinsteins per square meter per second (µE m-2 s-1 or µmol
 * m-2 s-1)
 *
 * {{ @ref ApogeeSL610_OLWR }}
 */
/**@{*/
/// Variable number; OLWR is stored in sensorValues[0].
#define SL610_OLWR_VAR_NUM 0
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/);
/// "radiationOutgoingLongwave"
#define SL610_OLWR_VAR_NAME "radiationOutgoingLongwave"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "wattsPerMeterSquared" (W m-2)
#define SL610_OLWR_UNIT_NAME "wattsPerMeterSquared"
/// @brief Default variable short code; "outgoingLongwaveRadiation"
#define SL610_OLWR_DEFAULT_CODE "outLongRad"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; OLWR should have 0 when
/// using an ADS1015.
#define SL610_OLWR_RESOLUTION 0
#else
/// @brief Decimals places in string representation; PAR should have 4 when
/// using an ADS1115.
#define SL610_OLWR_RESOLUTION 4
#endif
/**@}*/

/**
 * @anchor sensor_sl610_thermistor_voltage
 * @name Voltage
 * The voltage variable from an Apogee SL-610
 * - Range is 0 to 3300mV [when excited with 3.3V]
 * - Accuracy is ± 0.5%
 *   - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset error)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset error)
 * - Resolution [assuming the ADC is powered at 3.3V with inbuilt gain set to 1
 * (0-4.096V)]:
 *   - 16-bit ADC (ADS1115): 0.125 mV (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 2 mV
 * (ADS1015)
 *
 * {{ @ref ApogeeSL610_Thermistor_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define SL610_THERMISTOR_VOLTAGE_VAR_NUM 1
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "thermistorVoltage"
#define SL610_THERMISTOR_VOLTAGE_VAR_NAME "thermistorVoltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define SL610_THERMISTOR_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "SL610ThermistorVoltage"
#define SL610_THERMISTOR_VOLTAGE_DEFAULT_CODE "sl610thermistorVolt"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1 when
/// used with an ADS1015.
#define SL610_THERMISTOR_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4 when
/// used with an ADS1115.
#define SL610_THERMISTOR_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/**
 * @anchor sensor_sl610_thermopile_voltage
 * @name Voltage
 * The voltage variable from an Apogee SL-610
 * - Range is -23.5 mV to 23.5 mV [when ADC is powered at 3.3V]
 * - Accuracy is ± 0.5%
 *   - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset error)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset error)
 * - Resolution [assuming the ADC is powered at 3.3V with inbuilt gain set to 1
 * (0-4.096V)]:
 *   - 16-bit ADC (ADS1115): 0.125 mV (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 2 mV
 * (ADS1015)
 *
 * {{ @ref ApogeeSL610_Thermistor_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[2].
#define SL610_THERMOPILE_VOLTAGE_VAR_NUM 2
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "thermopileVoltage"
#define SL610_THERMOPILE_VOLTAGE_VAR_NAME "thermopileVoltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define SL610_THERMOPILE_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "SL610ThermopileVoltage"
#define SL610_THERMOPILE_VOLTAGE_DEFAULT_CODE "sl610thermopileVolt"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1 when
/// used with an ADS1015.
#define SL610_THERMOPILE_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4 when
/// used with an ADS1115.
#define SL610_THERMOPILE_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/**
 * @brief The calibration factors
 */
//#ifndef SL610_CALIBRATION_FACTOR_K1
//#define SL610_CALIBRATION_FACTOR_K1 9.181
//#endif

//#ifndef SL610_CALIBRATION_FACTOR_K2
//#define SL610_CALIBRATION_FACTOR_K2 1.018
//#endif

#ifndef STEFAN_BOLTZMANN_CONST
#define STEFAN_BOLTZMANN_CONST 0.000000056704
#endif

// Steinhart-Hart equation constants for thermistor with 30 kOhm resistance at 25 deg C
// Found on page 14 of SL-510-610 manual
#ifndef A_LESS
#define A_LESS 0.000932960
#endif

#ifndef B_LESS
#define B_LESS 0.000221424
#endif

#ifndef C_LESS
#define C_LESS 0.000000126329
#endif

#ifndef A_MORE
#define A_MORE 0.000932794
#endif

#ifndef B_MORE
#define B_MORE 0.000221451
#endif

#ifndef C_MORE
#define C_MORE 0.000000126233
#endif

/// The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/**
 * @brief The Sensor sub-class for the [Apogee SL-610](@ref sensor_sl610) sensor
 *
 * @ingroup sensor_sl610
 */
class ApogeeSL610 : public Sensor {
 public:
    /**
     * @brief Construct a new Apogee SL-610 object - need the power pin and the
     * data channels on the ADS1x15.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the Apogee
     * SL-610 thermistor.  Use -1 if it is continuously powered.
     * - The ADS1115 requires 2.0-5.5V but is assumed to be powered at 3.3V
     * @param thermistorChannel The ADC pin connected to the sensor's thermistor
     * analog output
     * @param thermistori2cAddress The I2C address of the ADC for the sensor's
     * thermistor
     * @param thermopilei2CAddress The I2C address of the ADC for the sensor's
     * thermopile
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @note  The ADS is expected to be either continuously powered or have
     * its power controlled by the same pin as the SL-610.  This library does
     * not support any other configuration.
     */
    ApogeeSL610(int8_t powerPin, float k1calib, float k2calib,
                uint8_t thermistorChannel,
                uint8_t thermistori2cAddress,
                uint8_t thermopilei2cAddress,
                uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the ApogeeSL610 object - no action needed
     */
    ~ApogeeSL610();

    /**
     * @brief Report the I1C address of the ADS and the channel that the SL-610
     * is attached to.
     *
     * @return **String** Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    float   _k1calib;
    float   _k2calib;
    uint8_t _thermistorChannel;
    uint8_t _thermistori2cAddress;
    uint8_t _thermopilei2cAddress;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [outgoing longwave radiation (OLWR) output](@ref sensor_sl610_olwr)
 * from an [Apogee SL-610](@ref sensor_sl610).
 *
 * @ingroup sensor_sl610
 */
/* clang-format on */
class ApogeeSL610_OLWR : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSL610_OLWR object.
     *
     * @param parentSense The parent ApogeeSL610 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "radiationOutgoingLongwave".
     */
    explicit ApogeeSL610_OLWR(ApogeeSL610* parentSense, const char* uuid = "",
                             const char* varCode = SL610_OLWR_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SL610_OLWR_VAR_NUM,
                   (uint8_t)SL610_OLWR_RESOLUTION, SL610_OLWR_VAR_NAME,
                   SL610_OLWR_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSL610_OLWR object.
     *
     * @note This must be tied with a parent ApogeeSL610 before it can be used.
     */
    ApogeeSL610_OLWR()
        : Variable((const uint8_t)SL610_OLWR_VAR_NUM,
                   (uint8_t)SL610_OLWR_RESOLUTION, SL610_OLWR_VAR_NAME,
                   SL610_OLWR_UNIT_NAME, SL610_OLWR_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSL610_OLWR object - no action needed.
     */
    ~ApogeeSL610_OLWR() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_sl610_voltage) from an
 * [Apogee SL-610](@ref sensor_sl610).
 *
 * @ingroup sensor_sl610
 */
/* clang-format on */
class ApogeeSL610_Thermistor_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSL610_Thermistor_Voltage object.
     *
     * @param parentSense The parent ApogeeSL610 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SL610_Thermistor_Voltage".
     */
    explicit ApogeeSL610_Thermistor_Voltage(
        ApogeeSL610* parentSense, const char* uuid = "",
        const char* varCode = SL610_THERMISTOR_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SL610_THERMISTOR_VOLTAGE_VAR_NUM,
                   (uint8_t)SL610_THERMISTOR_VOLTAGE_RESOLUTION, SL610_THERMISTOR_VOLTAGE_VAR_NAME,
                   SL610_THERMISTOR_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSL610_Thermistor_Voltage object.
     *
     * @note This must be tied with a parent ApogeeSL610 before it can be used.
     */
    ApogeeSL610_Thermistor_Voltage()
        : Variable((const uint8_t)SL610_THERMISTOR_VOLTAGE_VAR_NUM,
                   (uint8_t)SL610_THERMISTOR_VOLTAGE_RESOLUTION, SL610_THERMISTOR_VOLTAGE_VAR_NAME,
                   SL610_THERMISTOR_VOLTAGE_UNIT_NAME, SL610_THERMISTOR_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSL610_Thermistor_Voltage object - no action needed.
     */
    ~ApogeeSL610_Thermistor_Voltage() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_sl610_voltage) from an
 * [Apogee SL-610](@ref sensor_sl610).
 *
 * @ingroup sensor_sl610
 */
/* clang-format on */
class ApogeeSL610_Thermopile_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSL610_Thermopile_Voltage object.
     *
     * @param parentSense The parent ApogeeSL610 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SL610ThermistorVoltage".
     */
    explicit ApogeeSL610_Thermopile_Voltage(
        ApogeeSL610* parentSense, const char* uuid = "",
        const char* varCode = SL610_THERMOPILE_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SL610_THERMOPILE_VOLTAGE_VAR_NUM,
                   (uint8_t)SL610_THERMOPILE_VOLTAGE_RESOLUTION,
                   SL610_THERMOPILE_VOLTAGE_VAR_NAME,
                   SL610_THERMOPILE_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSL610_Thermistor_Voltage object.
     *
     * @note This must be tied with a parent ApogeeSL610 before it can be used.
     */
    ApogeeSL610_Thermopile_Voltage()
        : Variable((const uint8_t)SL610_THERMOPILE_VOLTAGE_VAR_NUM,
                   (uint8_t)SL610_THERMOPILE_VOLTAGE_RESOLUTION,
                   SL610_THERMOPILE_VOLTAGE_VAR_NAME,
                   SL610_THERMOPILE_VOLTAGE_UNIT_NAME,
                   SL610_THERMOPILE_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSL610_Thermistor_Voltage object - no action
     * needed.
     */
    ~ApogeeSL610_Thermopile_Voltage() {}
};
/**@}*/
#endif  // SRC_SENSORS_APOGEESL610_H_
