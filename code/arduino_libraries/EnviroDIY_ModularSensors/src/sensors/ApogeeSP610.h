/**
 * @file ApogeeSP610.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * This specific sensor's code was created by Braedon Dority based on the Apogee
 * SQ-212 code already made for Modular Sensors by the EnviroDIY team.
 *
 * The original authorship and editorial credits are listed below from the
 * Apogee SQ-212 code that this is all based on:
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ApogeeSP610 sensor subclass and the variable subclasses
 * ApogeeSP610_OSWR and ApogeeSP610_Voltage.
 *
 * These are used for the Apogee SP-610 outgoing shortwave radiation sensor.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */
/* clang-format off */
/**
 * @defgroup sensor_sp610 Apogee SP-610
 * Classes for the Apogee SP-610 thermopile radiation sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_sp610_intro Introduction
 * The [Apogee SP-610 downward-looking thermopile pyranometer
 * sensor](https://www.apogeeinstruments.com/sp-610-ss-downward-looking-thermopile-pyranometer/)
 * measures [outgoing shortwave radiation
 * (OSWR)](https://en.wikipedia.org/wiki/Shortwave_radiation_(optics)) -
 * typically defined as total elctronmagnetic radiation from the sun reflected off the ground.
 * The raw output from the sensor is a differential analog signal which must be converted
 * to a digital signal and then multiplied by a calibration factor to get the
 * final OSWR value.
 *
 * To convert the sensor's analog signal to a high resolution digital signal,
 * the sensor must be attached to an analog-to-digital converter.  See the
 * [ADS1115](@ref analog_group) for details on the conversion.
 *
 * The calibration factor needs to be adjusted to what you received from Apogee your sensor's certicate of calibration.
 * The line where that calibration factor is found just before the SP610 class is declared.
 *
 * @section sensor_sp610_datasheet Sensor Datasheet
 * [Datasheet](https://www.apogeeinstruments.com/content/SP-510-610-manual.pdf)
 *
 * @section sensor_sp610_flags Build flags
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 * - ```-D SP610_CALIBRATION_FACTOR=x```
 *      - Changes the calibration factor from 1 to x
 *
 * @section sensor_sp610_ctor Sensor Constructor
 * {{ @ref ApogeeSP610::ApogeeSP610 }}
 *
 * This section just below may need to just be deleted
 * ___
 * @section sensor_sq212_examples Example Code
 * The SQ-212 is used in the @menulink{apogee_sq212} example.
 *
 * @menusnip{apogee_sp610}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_APOGEESP610_H_
#define SRC_SENSORS_APOGEESP610_H_

// Debugging Statement
// #define MS_APOGEESP610_DEBUG

#ifdef MS_APOGEESP610_DEBUG
#define MS_DEBUGGING_STD "ApogeeSP610"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_sp610 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the SP610 can report 2 values, raw
/// voltage and calculated outgoing shortwave radiation.
#define SP610_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; OSWR is calculated from the raw voltage.
#define SP610_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_sp610_timing
 * @name Sensor Timing
 * The sensor timing for an Apogee SP-610
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the warm up time is unnecessary for this
 * analog sensor; using the 2ms for the TI ADS1x15 to warm up
 */
#define SP610_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; the ADS1115 is stable after 2ms.
 *
 * The stabilization time of the SP-510 is not really a set value because it is self-powered
 *
 * @todo Measure stabilization time of the SP-610
 */
#define SP610_STABILIZATION_TIME_MS 500
/// @brief Sensor::_measurementTime_ms; ADS1115 takes almost 2ms to complete a
/// measurement (860/sec).
#define SP610_MEASUREMENT_TIME_MS 500
/**@}*/

/**
 * @anchor sensor_sp610_oswr
 * @name OSWR
 * The OSWR variable from an Apogee SP-510
 * - Range is 0 to 2000 W m-2
 * - Accuracy is ± 0.5%
 * - Resolution (this needs to be updated for the SP-610):
 *   - 16-bit ADC (ADS1115): 0.3125 µmol m-2 s-1 (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 5 µmol m-2
 * s-1 (ADS1015)
 * - Reported as microeinsteins per square meter per second (µE m-2 s-1 or µmol
 * m-2 s-1)
 *
 * {{ @ref ApogeeSP610_OSWR }}
 */
/**@{*/
/// Variable number; OSWR is stored in sensorValues[0].
#define SP610_OSWR_VAR_NUM 0
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/);
/// "radiationOutgoingShortwave"
#define SP610_OSWR_VAR_NAME "radiationOutgoingShortwave"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "wattsPerMeterSquared" (W m-2)
#define SP610_OSWR_UNIT_NAME "wattsPerMeterSquared"
/// @brief Default variable short code; "outgoingShortwaveRadiation"
#define SP610_OSWR_DEFAULT_CODE "outShortRad"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; OSWR should have 0 when
/// using an ADS1015.
#define SP610_OSWR_RESOLUTION 0
#else
/// @brief Decimals places in string representation; OSWR should have 4 when
/// using an ADS1115.
#define SP610_OSWR_RESOLUTION 4
#endif
/**@}*/

/**
 * @anchor sensor_sp610_voltage
 * @name Voltage
 * The voltage variable from an Apogee SP-610
 * - Range is 0 to 70 mV
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
 * {{ @ref ApogeeSP610_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define SP610_VOLTAGE_VAR_NUM 1
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "voltage"
#define SP610_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define SP610_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "SP610Voltage"
#define SP610_VOLTAGE_DEFAULT_CODE "sp610thermopileVolt"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1 when
/// used with an ADS1015.
#define SP610_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4 when
/// used with an ADS1115.
#define SP610_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/**
 * @brief The calibration factor between output in volts and W m-2
 * (wattspermetersquared)
 */
#ifndef SP610_CALIBRATION_FACTOR
#define SP610_CALIBRATION_FACTOR 30.50
#endif

/// The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/**
 * @brief The Sensor sub-class for the [Apogee SP-610](@ref sensor_sp610) sensor
 *
 * @ingroup sensor_sp610
 */
class ApogeeSP610 : public Sensor {
 public:
    /**
     * @brief Construct a new Apogee SQ-212 object - need the power pin and the
     * data channel on the ADS1x15.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The SP-510 does not need any powering up to make a
     * measurement. The only power requirements are if you run the heaters. This
     * power pin parameter could potentially be deleted, as it doesn't effect
     * functionality.
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
    ApogeeSP610(int8_t powerPin,
                uint8_t i2cAddress            = ADS1115_ADDRESS,
                uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the ApogeeSP610 object - no action needed
     */
    ~ApogeeSP610();

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
    uint8_t _i2cAddress;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [outgoing shortwave radiation (OSWR) output](@ref sensor_sp610_oswr)
 * from an [Apogee SP-610](@ref sensor_sp610).
 *
 * @ingroup sensor_sp610
 */
/* clang-format on */
class ApogeeSP610_OSWR : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSP610_OSWR object.
     *
     * @param parentSense The parent ApogeeSP610 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "radiationOutgoingShortwave".
     */
    explicit ApogeeSP610_OSWR(ApogeeSP610* parentSense, const char* uuid = "",
                             const char* varCode = SP610_OSWR_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SP610_OSWR_VAR_NUM,
                   (uint8_t)SP610_OSWR_RESOLUTION, SP610_OSWR_VAR_NAME,
                   SP610_OSWR_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSP610_OSWR object.
     *
     * @note This must be tied with a parent ApogeeSP610 before it can be used.
     */
    ApogeeSP610_OSWR()
        : Variable((const uint8_t)SP610_OSWR_VAR_NUM,
                   (uint8_t)SP610_OSWR_RESOLUTION, SP610_OSWR_VAR_NAME,
                   SP610_OSWR_UNIT_NAME, SP610_OSWR_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSP610_OSWR object - no action needed.
     */
    ~ApogeeSP610_OSWR() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_sp610_voltage) from an
 * [Apogee SP-610](@ref sensor_sp610).
 *
 * @ingroup sensor_sp610
 */
/* clang-format on */
class ApogeeSP610_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSP610_Voltage object.
     *
     * @param parentSense The parent ApogeeSP610 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SP610Voltage".
     */
    explicit ApogeeSP610_Voltage(
        ApogeeSP610* parentSense, const char* uuid = "",
        const char* varCode = SP610_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SP610_VOLTAGE_VAR_NUM,
                   (uint8_t)SP610_VOLTAGE_RESOLUTION, SP610_VOLTAGE_VAR_NAME,
                   SP610_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSP610_Voltage object.
     *
     * @note This must be tied with a parent ApogeeSP610 before it can be used.
     */
    ApogeeSP610_Voltage()
        : Variable((const uint8_t)SP610_VOLTAGE_VAR_NUM,
                   (uint8_t)SP610_VOLTAGE_RESOLUTION, SP610_VOLTAGE_VAR_NAME,
                   SP610_VOLTAGE_UNIT_NAME, SP610_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSP610_Voltage object - no action needed.
     */
    ~ApogeeSP610_Voltage() {}
};
/**@}*/
#endif  // SRC_SENSORS_APOGEESP610_H_
