/**
 * @file ApogeeST110.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ApogeeST110 sensor subclass and the variable subclasses
 * ApogeeST110_AIRT and ApogeeST110_ThermistorVoltage.
 *
 * These are used for the Apogee SL-510 incoming shortwave radiation sensor.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */
/* clang-format off */
/**
 * @defgroup sensor_st110 Apogee SL-510
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
#ifndef SRC_SENSORS_APOGEEST110_H_
#define SRC_SENSORS_APOGEEST110_H_

// Debugging Statement
// #define MS_APOGEEST110_DEBUG

#ifdef MS_APOGEEST110_DEBUG
#define MS_DEBUGGING_STD "ApogeeST110"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_st110 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the ST110 can report 2 values, raw
/// voltage for thermistor and air temperature.
#define ST110_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; airTemp is calculated from the raw voltage.
#define ST110_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_st110_timing
 * @name Sensor Timing
 * The sensor timing for an Apogee ST-110
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the warm up time is unknown; using the 2ms for
 * the TI ADS1x15 to warm up
 *
 * @todo Measure warm-up time of the ST-110
 */
#define ST110_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; the ADS1115 is stable after 2ms.
 *
 * The stabilization time of the ST-110 itself is not known!
 *
 * @todo Measure stabilization time of the ST-11-
 */
#define ST110_STABILIZATION_TIME_MS 500
/// @brief Sensor::_measurementTime_ms; ADS1115 takes almost 2ms to complete a
/// measurement (860/sec).
#define ST110_MEASUREMENT_TIME_MS 500
/**@}*/

/**
 * @anchor sensor_st110_airt
 * @name AIRT
 * The AIRT variable from an Apogee ST-110
 * - Range is -40 to 70 degC
 * - Accuracy is 0.1 degC for temperatures 0-70 degC and 0.15 degC
 *     for temperatures -50-0 degC
 * - Resolution:
 *   - 16-bit ADC (ADS1115): 0.3125 µmol m-2 s-1 (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 5 µmol m-2
 * s-1 (ADS1015)
 * - Reported as degrees celsius (degC)
 *
 * {{ @ref ApogeeST110_AIRT }}
 */
/**@{*/
/// Variable number; AIRT is stored in sensorValues[0].
#define ST110_AIRT_VAR_NUM 0
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperatureAir"
#define ST110_AIRT_VAR_NAME "airTemperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (degC)
#define ST110_AIRT_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "incomingLongwaveRadiation"
#define ST110_AIRT_DEFAULT_CODE "airTemp"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; PAR should have 0 when
/// using an ADS1015.
#define ST110_PAR_RESOLUTION 0
#else
/// @brief Decimals places in string representation; PAR should have 4 when
/// using an ADS1115.
#define ST110_AIRT_RESOLUTION 4
#endif
/**@}*/

/**
 * @anchor sensor_st110_thermistor_voltage
 * @name Voltage
 * The voltage variable from an Apogee ST-110
 * - Recommended excitation voltage of 2.5 degC
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
 * {{ @ref ApogeeST110_Thermistor_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define ST110_THERMISTOR_VOLTAGE_VAR_NUM 1
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "thermistorVoltage"
#define ST110_THERMISTOR_VOLTAGE_VAR_NAME "thermistorVoltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define ST110_THERMISTOR_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "ST110ThermistorVoltage"
#define ST110_THERMISTOR_VOLTAGE_DEFAULT_CODE "st110thermistorVolt"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1 when
/// used with an ADS1015.
#define ST110_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4 when
/// used with an ADS1115.
#define ST110_THERMISTOR_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/**
 * @brief The Steinhart-Hart coefficients found on page 11 of the sensor manual
 */

#ifndef A
#define A 0.001129241
#endif

#ifndef B
#define B 0.0002341077
#endif

#ifndef C
#define C 0.00000008775468
#endif

/// The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/**
 * @brief The Sensor sub-class for the [Apogee ST-110](@ref sensor_st110) sensor
 *
 * @ingroup sensor_st110
 */
class ApogeeST110 : public Sensor {
 public:
    /**
     * @brief Construct a new Apogee ST-110 object - need the power pin and the
     * data channel on the ADS1x15.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the Apogee
     * ST-110.  Use -1 if it is continuously powered.
     * - The recommended excitation voltage for the ST-110 is 2.5 V DC; current draw 0.1 mA at 2.5 V
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
    ApogeeST110(int8_t powerPin, uint8_t thermistorChannel,
                uint8_t i2cAddress            = ADS1115_ADDRESS,
                uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the ApogeeST110 object - no action needed
     */
    ~ApogeeST110();

    /**
     * @brief Report the I2C address of the ADS and the channel that the ST-110
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
    uint8_t _i2cAddress;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [air temperature (AIRT) output](@ref sensor_st110_airt)
 * from an [Apogee ST-110](@ref sensor_st110).
 *
 * @ingroup sensor_st110
 */
/* clang-format on */
class ApogeeST110_AIRT : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeST110_AIRT object.
     *
     * @param parentSense The parent ApogeeST110 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "airTemperature".
     */
    explicit ApogeeST110_AIRT(ApogeeST110* parentSense, const char* uuid = "",
                             const char* varCode = ST110_AIRT_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ST110_AIRT_VAR_NUM,
                   (uint8_t)ST110_AIRT_RESOLUTION, ST110_AIRT_VAR_NAME,
                   ST110_AIRT_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeST110_AIRT object.
     *
     * @note This must be tied with a parent ApogeeST110 before it can be used.
     */
    ApogeeST110_AIRT()
        : Variable((const uint8_t)ST110_AIRT_VAR_NUM,
                   (uint8_t)ST110_AIRT_RESOLUTION, ST110_AIRT_VAR_NAME,
                   ST110_AIRT_UNIT_NAME, ST110_AIRT_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeST110_AIRT object - no action needed.
     */
    ~ApogeeST110_AIRT() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_st110_voltage) from an
 * [Apogee ST-110](@ref sensor_st110).
 *
 * @ingroup sensor_st110
 */
/* clang-format on */
class ApogeeST110_Thermistor_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeST110_Thermistor_Voltage object.
     *
     * @param parentSense The parent ApogeeST110 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ST110ThermistorVoltage".
     */
    explicit ApogeeST110_Thermistor_Voltage(
        ApogeeST110* parentSense, const char* uuid = "",
        const char* varCode = ST110_THERMISTOR_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ST110_THERMISTOR_VOLTAGE_VAR_NUM,
                   (uint8_t)ST110_THERMISTOR_VOLTAGE_RESOLUTION, ST110_THERMISTOR_VOLTAGE_VAR_NAME,
                   ST110_THERMISTOR_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeST110_Thermistor_Voltage object.
     *
     * @note This must be tied with a parent ApogeeST110 before it can be used.
     */
    ApogeeST110_Thermistor_Voltage()
        : Variable((const uint8_t)ST110_THERMISTOR_VOLTAGE_VAR_NUM,
                   (uint8_t)ST110_THERMISTOR_VOLTAGE_RESOLUTION, ST110_THERMISTOR_VOLTAGE_VAR_NAME,
                   ST110_THERMISTOR_VOLTAGE_UNIT_NAME, ST110_THERMISTOR_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeST110_Thermistor_Voltage object - no action needed.
     */
    ~ApogeeST110_Thermistor_Voltage() {}
};

/**@}*/
#endif  // SRC_SENSORS_APOGEEST110_H_
