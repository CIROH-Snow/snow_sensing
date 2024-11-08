/**
 * @file ApogeeSL510.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ApogeeSL510 sensor subclass and the variable subclasses
 * ApogeeSL510_ISWR and ApogeeSL510_Voltage.
 *
 * These are used for the Apogee SL-510 incoming shortwave radiation sensor.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */
/* clang-format off */
/**
 * @defgroup sensor_sl510 Apogee SL-510
 * Classes for the Apogee SL-510 thermopile radiation sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_sq212_intro Introduction
 * The [Apogee SQ-212 quantum light
 * sensor](https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/)
 * measures [photosynthetically active radiation
 * (PAR)](https://en.wikipedia.org/wiki/Photosynthetically_active_radiation) -
 * typically defined as total radiation across a range of 400 to 700 nm.  PAR is
 * often expressed as photosynthetic photon flux density (PPFD): photon flux in
 * units of micromoles per square meter per second (μmol m-2 s-1, equal to
 * microEinsteins per square meter per second) summed from 400 to 700 nm.  The
 * raw output from the sensor is a simple analog signal which must be converted
 * to a digital signal and then multiplied by a calibration factor to get the
 * final PAR value.  The PAR sensor requires a 5-24 V DC power source with a
 * nominal current draw of 300 μA.  The power supply to the sensor can be
 * stopped between measurements.
 *
 * To convert the sensor's analog signal to a high resolution digital signal,
 * the sensor must be attached to an analog-to-digital converter.  See the
 * [ADS1115](@ref analog_group) for details on the conversion.
 *
 * The calibration factor this library uses to convert from raw voltage to PAR
 * is that specified by Apogee for the SQ-212: 1 µmol mˉ² sˉ¹ per mV (reciprocal
 * of sensitivity).  If needed, this calibration factor can be modified by
 * compiling with the build flag ```-D SQ212_CALIBRATION_FACTOR=x``` where x is
 * the calibration factor.  This allows you to adjust the calibration or change
 * to another Apogee sensor (e.g. SQ-215 or SQ225) as needed.
 *
 * @section sensor_sq212_datasheet Sensor Datasheet
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Apogee
 * SQ-212-215 Manual.pdf)
 *
 * @section sensor_sq212_flags Build flags
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 * - ```-D SQ212_CALIBRATION_FACTOR=x```
 *      - Changes the calibration factor from 1 to x
 *
 * @section sensor_sq212_ctor Sensor Constructor
 * {{ @ref ApogeeSQ212::ApogeeSQ212 }}
 *
 * ___
 * @section sensor_sq212_examples Example Code
 * The SQ-212 is used in the @menulink{apogee_sq212} example.
 *
 * @menusnip{apogee_sq212}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_APOGEESL510_H_
#define SRC_SENSORS_APOGEESL510_H_

// Debugging Statement
// #define MS_APOGEESL510_DEBUG

#ifdef MS_APOGEESL510_DEBUG
#define MS_DEBUGGING_STD "ApogeeSL510"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_sl510 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the SL510 can report 3 values, raw
/// voltage for thermistor, raw voltage for thermopile, and calculated incoming longwave radiation.
#define SL510_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; ILWR is calculated from the raw voltage.
#define SL510_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_sl510_timing
 * @name Sensor Timing
 * The sensor timing for an Apogee SL-510
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the warm up time is unknown; using the 2ms for
 * the TI ADS1x15 to warm up
 *
 * @todo Measure warm-up time of the SQ-212
 */
#define SL510_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; the ADS1115 is stable after 2ms.
 *
 * The stabilization time of the SQ-212 itself is not known!
 *
 * @todo Measure stabilization time of the SQ-212
 */
#define SL510_STABILIZATION_TIME_MS 500
/// @brief Sensor::_measurementTime_ms; ADS1115 takes almost 2ms to complete a
/// measurement (860/sec).
#define SL510_MEASUREMENT_TIME_MS 500
/**@}*/

/**
 * @anchor sensor_sl510_ilwr
 * @name ILWR
 * The ILWR variable from an Apogee SL-510
 * - Range is 0 to 2000 W m-2
 * - Accuracy is ± 0.5%
 * - Resolution:
 *   - 16-bit ADC (ADS1115): 0.3125 µmol m-2 s-1 (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 5 µmol m-2
 * s-1 (ADS1015)
 * - Reported as microeinsteins per square meter per second (µE m-2 s-1 or µmol
 * m-2 s-1)
 *
 * {{ @ref ApogeeSL510_ILWR }}
 */
/**@{*/
/// Variable number; ILWR is stored in sensorValues[0].
#define SL510_ILWR_VAR_NUM 0
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/);
/// "radiationIncomingLongwave"
#define SL510_ILWR_VAR_NAME "radiationIncomingLongwave"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "wattsPerMeterSquared" (W m-2)
#define SL510_ILWR_UNIT_NAME "wattsPerMeterSquared"
/// @brief Default variable short code; "incomingLongwaveRadiation"
#define SL510_ILWR_DEFAULT_CODE "inLongRad"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; PAR should have 0 when
/// using an ADS1015.
#define SL510_PAR_RESOLUTION 0
#else
/// @brief Decimals places in string representation; PAR should have 4 when
/// using an ADS1115.
#define SL510_ILWR_RESOLUTION 4
#endif
/**@}*/

/**
 * @anchor sensor_sl510_thermistor_voltage
 * @name Voltage
 * The voltage variable from an Apogee SQ-212
 * - Range is 0 to 3.6V [when ADC is powered at 3.3V]
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
 * {{ @ref ApogeeSL510_Thermistor_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define SL510_THERMISTOR_VOLTAGE_VAR_NUM 1
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "thermistorVoltage"
#define SL510_THERMISTOR_VOLTAGE_VAR_NAME "thermistorVoltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define SL510_THERMISTOR_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "SL510ThermistorVoltage"
#define SL510_THERMISTOR_VOLTAGE_DEFAULT_CODE "sl510thermistorVolt"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1 when
/// used with an ADS1015.
#define SL510_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4 when
/// used with an ADS1115.
#define SL510_THERMISTOR_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/**
 * @anchor sensor_sl510_thermopile_voltage
 * @name Voltage
 * The voltage variable from an Apogee SQ-212
 * - Range is 0 to 3.6V [when ADC is powered at 3.3V]
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
 * {{ @ref ApogeeSL510_Thermistor_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[2].
#define SL510_THERMOPILE_VOLTAGE_VAR_NUM 2
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "thermopileVoltage"
#define SL510_THERMOPILE_VOLTAGE_VAR_NAME "thermopileVoltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define SL510_THERMOPILE_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "SL510ThermopileVoltage"
#define SL510_THERMOPILE_VOLTAGE_DEFAULT_CODE "sl510thermopileVolt"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1 when
/// used with an ADS1015.
#define SL510_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4 when
/// used with an ADS1115.
#define SL510_THERMOPILE_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/**
 * @brief The calibration factor between output in volts and PAR
 * (microeinsteinPerSquareMeterPerSecond) 1 µmol mˉ² sˉ¹ per mV (reciprocal of
 * sensitivity)
 */
#ifndef SL510_CALIBRATION_FACTOR_K1
#define SL510_CALIBRATION_FACTOR_K1 9.229
#endif

#ifndef SL510_CALIBRATION_FACTOR_K2
#define SL510_CALIBRATION_FACTOR_K2 1.044
#endif

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
 * @brief The Sensor sub-class for the [Apogee SP-510](@ref sensor_sl510) sensor
 *
 * @ingroup sensor_sl510
 */
class ApogeeSL510 : public Sensor {
 public:
    /**
     * @brief Construct a new Apogee SQ-212 object - need the power pin and the
     * data channel on the ADS1x15.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the Apogee
     * SQ-212.  Use -1 if it is continuously powered.
     * - The SQ-212 requires 3.3 to 24 V DC; current draw 10 µA
     * - The ADS1115 requires 2.0-5.5V but is assumed to be powered at 3.3V
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @note  The ADS is expected to be either continuously powered or have
     * its power controlled by the same pin as the SQ-212.  This library does
     * not support any other configuration.
     */
    ApogeeSL510(int8_t powerPin, uint8_t thermistorChannel,
                uint8_t thermistori2cAddress,
                uint8_t thermopilei2cAddress,
                uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the ApogeeSL510 object - no action needed
     */
    ~ApogeeSL510();

    /**
     * @brief Report the I1C address of the ADS and the channel that the SQ-212
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
    uint8_t _thermistorChannel;
    uint8_t _thermistori2cAddress;
    uint8_t _thermopilei2cAddress;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [incoming shortwave radiation (ILWR) output](@ref sensor_sl510_ilwr)
 * from an [Apogee SQ-212](@ref sensor_sl510).
 *
 * @ingroup sensor_sl510
 */
/* clang-format on */
class ApogeeSL510_ILWR : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSL510_ILWR object.
     *
     * @param parentSense The parent ApogeeSL510 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "radiationIncomingPAR".
     */
    explicit ApogeeSL510_ILWR(ApogeeSL510* parentSense, const char* uuid = "",
                             const char* varCode = SL510_ILWR_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SL510_ILWR_VAR_NUM,
                   (uint8_t)SL510_ILWR_RESOLUTION, SL510_ILWR_VAR_NAME,
                   SL510_ILWR_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSL510_ILWR object.
     *
     * @note This must be tied with a parent ApogeeSL510 before it can be used.
     */
    ApogeeSL510_ILWR()
        : Variable((const uint8_t)SL510_ILWR_VAR_NUM,
                   (uint8_t)SL510_ILWR_RESOLUTION, SL510_ILWR_VAR_NAME,
                   SL510_ILWR_UNIT_NAME, SL510_ILWR_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSL510_ILWR object - no action needed.
     */
    ~ApogeeSL510_ILWR() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_sl510_voltage) from an
 * [Apogee SL-510](@ref sensor_sl510).
 *
 * @ingroup sensor_sl510
 */
/* clang-format on */
class ApogeeSL510_Thermistor_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSL510_Thermistor_Voltage object.
     *
     * @param parentSense The parent ApogeeSL510 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SL510ThermistorVoltage".
     */
    explicit ApogeeSL510_Thermistor_Voltage(
        ApogeeSL510* parentSense, const char* uuid = "",
        const char* varCode = SL510_THERMISTOR_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SL510_THERMISTOR_VOLTAGE_VAR_NUM,
                   (uint8_t)SL510_THERMISTOR_VOLTAGE_RESOLUTION, SL510_THERMISTOR_VOLTAGE_VAR_NAME,
                   SL510_THERMISTOR_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSL510_Thermistor_Voltage object.
     *
     * @note This must be tied with a parent ApogeeSL510 before it can be used.
     */
    ApogeeSL510_Thermistor_Voltage()
        : Variable((const uint8_t)SL510_THERMISTOR_VOLTAGE_VAR_NUM,
                   (uint8_t)SL510_THERMISTOR_VOLTAGE_RESOLUTION, SL510_THERMISTOR_VOLTAGE_VAR_NAME,
                   SL510_THERMISTOR_VOLTAGE_UNIT_NAME, SL510_THERMISTOR_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSL510_Thermistor_Voltage object - no action needed.
     */
    ~ApogeeSL510_Thermistor_Voltage() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_sl510_voltage) from an
 * [Apogee SL-510](@ref sensor_sl510).
 *
 * @ingroup sensor_sl510
 */
/* clang-format on */
class ApogeeSL510_Thermopile_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSL510_Thermopile_Voltage object.
     *
     * @param parentSense The parent ApogeeSL510 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SL510ThermistorVoltage".
     */
    explicit ApogeeSL510_Thermopile_Voltage(
        ApogeeSL510* parentSense, const char* uuid = "",
        const char* varCode = SL510_THERMOPILE_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SL510_THERMOPILE_VOLTAGE_VAR_NUM,
                   (uint8_t)SL510_THERMOPILE_VOLTAGE_RESOLUTION,
                   SL510_THERMOPILE_VOLTAGE_VAR_NAME,
                   SL510_THERMOPILE_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSL510_Thermistor_Voltage object.
     *
     * @note This must be tied with a parent ApogeeSL510 before it can be used.
     */
    ApogeeSL510_Thermopile_Voltage()
        : Variable((const uint8_t)SL510_THERMOPILE_VOLTAGE_VAR_NUM,
                   (uint8_t)SL510_THERMOPILE_VOLTAGE_RESOLUTION,
                   SL510_THERMOPILE_VOLTAGE_VAR_NAME,
                   SL510_THERMOPILE_VOLTAGE_UNIT_NAME,
                   SL510_THERMOPILE_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSL510_Thermistor_Voltage object - no action
     * needed.
     */
    ~ApogeeSL510_Thermopile_Voltage() {}
};
/**@}*/
#endif  // SRC_SENSORS_APOGEESL510_H_
