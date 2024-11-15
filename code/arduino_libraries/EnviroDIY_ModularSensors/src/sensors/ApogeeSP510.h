/**
 * @file ApogeeSP510.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * This specific sensor's code was created by Braedon Dority based on the Apogee SQ-212 code already made for Modular Sensors by the EnviroDIY team.
 * 
 * The original authorship and editorial credits are listed below from the Apogee SQ-212 code that this is all based on:
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ApogeeSP510 sensor subclass and the variable subclasses
 * ApogeeSP510_ISWR and ApogeeSP510_Voltage.
 *
 * These are used for the Apogee SP-510 incoming shortwave radiation sensor.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */
/* clang-format off */
/**
 * @defgroup sensor_sp510 Apogee SP-510
 * Classes for the Apogee SP-510 thermopile radiation sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_sp510_intro Introduction
 * The [Apogee SP-510 upward-looking thermopile pyranometer
 * sensor](https://www.apogeeinstruments.com/sp-510-ss-upward-looking-thermopile-pyranometer/)
 * measures [incoming shortwave radiation
 * (ISWR)](https://en.wikipedia.org/wiki/Shortwave_radiation_(optics)) -
 * Incoming shortwave radiation is usually measured in watts per square meter.
 * It classically thought of as the total electromagnetic radiation emenating from the sun
 * on a flat surface. At night, incoming shortwave radiation should be zero. A clear day
 * with high sun can usually be measured around 1000 W/m^2. The output of this sensor is an
 * analog differential signal.
 *
 * To convert the sensor's analog signal to a high resolution digital signal,
 * the sensor must be attached to an analog-to-digital converter.  See the
 * [ADS1115](@ref analog_group) for details on the conversion.
 *
 * The calibration factor needs to be adjusted to what you received from Apogee your sensor's certicate of calibration.
 * The line where that calibration factor is found just before the SP510 class is declared.
 *
 * @section sensor_sp510_datasheet Sensor Datasheet
 * [Datasheet](https://www.apogeeinstruments.com/content/SP-510-610-manual.pdf)
 *
 * @section sensor_sp510_flags Build flags
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 * - ```-D SP510_CALIBRATION_FACTOR=x```
 *      - Changes the calibration factor from 1 to x
 *
 * @section sensor_sp510_ctor Sensor Constructor
 * {{ @ref ApogeeSP510::ApogeeSP510 }}
 *
 * This section just below may need to just be deleted
 * ___
 * @section sensor_sp510_examples Example Code
 * The SP-510 is used in the @menulink{apogee_sp510} example.
 *
 * @menusnip{apogee_sp510}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_APOGEESP510_H_
#define SRC_SENSORS_APOGEESP510_H_

// Debugging Statement
// #define MS_APOGEESP510_DEBUG

#ifdef MS_APOGEESP510_DEBUG
#define MS_DEBUGGING_STD "ApogeeSP510"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_sp510 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the SP510 can report 2 values, raw
/// voltage and calculated incoming shortwave radiation.
#define SP510_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; ISWR is calculated from the raw voltage.
#define SP510_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_sp510_timing
 * @name Sensor Timing
 * The sensor timing for an Apogee SP-510
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the warm up time is unnecessary for this analog sensor; using the 2ms for
 * the TI ADS1x15 to warm up
 */
#define SP510_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; the ADS1115 is stable after 2ms.
 *
 * The stabilization time of the SP-510 is not really a set value because it is self-powered
 *
 * @todo Measure stabilization time of the SP-510
 */
#define SP510_STABILIZATION_TIME_MS 500
/// @brief Sensor::_measurementTime_ms; ADS1115 takes almost 2ms to complete a
/// measurement (860/sec).
#define SP510_MEASUREMENT_TIME_MS 500
/**@}*/

/**
 * @anchor sensor_sp510_iswr
 * @name ISWR
 * The ISWR variable from an Apogee SP-510
 * - Range is 0 to 2000 W m-2
 * - Accuracy is ± 0.5%
 * - Resolution (this needs to be updated for the SP-510):
 *   - 16-bit ADC (ADS1115): 0.3125 µmol m-2 s-1 (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 5 µmol m-2
 * s-1 (ADS1015)
 * - Reported as microeinsteins per square meter per second (µE m-2 s-1 or µmol
 * m-2 s-1)
 *
 * {{ @ref ApogeeSP510_ISWR }}
 */
/**@{*/
/// Variable number; ISWR is stored in sensorValues[0].
#define SP510_ISWR_VAR_NUM 0
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/);
/// "radiationIncomingShortwave"
#define SP510_ISWR_VAR_NAME "radiationIncomingShortwave"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "wattsPerMeterSquared" (W m-2)
#define SP510_ISWR_UNIT_NAME "wattsPerMeterSquared"
/// @brief Default variable short code; "incomingShortwaveRadiation"
#define SP510_ISWR_DEFAULT_CODE "inShortRad"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; PAR should have 0 when
/// using an ADS1015.
#define SP510_PAR_RESOLUTION 0
#else
/// @brief Decimals places in string representation; PAR should have 4 when
/// using an ADS1115.
#define SP510_ISWR_RESOLUTION 4
#endif
/**@}*/

/**
 * @anchor sensor_sp510_voltage
 * @name Voltage
 * The voltage variable from an Apogee SP-510
 * - Range is 0 to 90 mV
 * - Uncertainty in daily total less than 5%
 *   - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset error)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset error)
 * - Resolution [assuming the ADC is powered at 3.3V with inbuilt gain set to 1
 * (0-4.096V)]:
 *   - 16-bit ADC (ADS1115): 0.125 mV (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 2 mV
 * (ADS1015)
 *
 * {{ @ref ApogeeSP510_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define SP510_VOLTAGE_VAR_NUM 1
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "voltage"
#define SP510_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define SP510_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "SP510Voltage"
#define SP510_VOLTAGE_DEFAULT_CODE "sp510thermopileVolt"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1 when
/// used with an ADS1015.
#define SP510_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4 when
/// used with an ADS1115.
#define SP510_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/**
 * @brief The calibration factor between output in volts and W m-2
 * (wattspermetersquared)
 */
#ifndef SP510_CALIBRATION_FACTOR
#define SP510_CALIBRATION_FACTOR 22.47
#endif

/// The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/**
 * @brief The Sensor sub-class for the [Apogee SP-510](@ref sensor_sp510) sensor
 *
 * @ingroup sensor_sp510
 */
class ApogeeSP510 : public Sensor {
 public:
    /**
     * @brief Construct a new Apogee SP-510 object - need the power pin and the
     * data channel on the ADS1x15.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The SP-510 does not need any powering up to make a measurement.
     * The only power requirements are if you run the heaters. This power pin parameter could potentially be deleted, as it doesn't effect functionality.
     * - The ADS1115 requires 2.0-5.5V but is assumed to be powered at 3.3V
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @note  The ADS is expected to be either continuously powered or have
     * its power controlled by the same pin as the SP-510.  This library does
     * not support any other configuration.
     */
    ApogeeSP510(int8_t powerPin,
                uint8_t i2cAddress            = ADS1115_ADDRESS,
                uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the ApogeeSP510 object - no action needed
     */
    ~ApogeeSP510();

    /**
     * @brief Report the I1C address of the ADS and the channel that the SP-510
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
 * [incoming shortwave radiation (ISWR) output](@ref sensor_sp510_iswr)
 * from an [Apogee SP-510](@ref sensor_sp510).
 *
 * @ingroup sensor_sp510
 */
/* clang-format on */
class ApogeeSP510_ISWR : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSP510_ISWR object.
     *
     * @param parentSense The parent ApogeeSP510 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "radiationIncomingShortwave".
     */
    explicit ApogeeSP510_ISWR(ApogeeSP510* parentSense, const char* uuid = "",
                             const char* varCode = SP510_ISWR_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SP510_ISWR_VAR_NUM,
                   (uint8_t)SP510_ISWR_RESOLUTION, SP510_ISWR_VAR_NAME,
                   SP510_ISWR_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSP510_ISWR object.
     *
     * @note This must be tied with a parent ApogeeSP510 before it can be used.
     */
    ApogeeSP510_ISWR()
        : Variable((const uint8_t)SP510_ISWR_VAR_NUM,
                   (uint8_t)SP510_ISWR_RESOLUTION, SP510_ISWR_VAR_NAME,
                   SP510_ISWR_UNIT_NAME, SP510_ISWR_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSP510_ISWR object - no action needed.
     */
    ~ApogeeSP510_ISWR() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_sp510_voltage) from an
 * [Apogee SP-510](@ref sensor_sp510).
 *
 * @ingroup sensor_sp510
 */
/* clang-format on */
class ApogeeSP510_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSP510_Voltage object.
     *
     * @param parentSense The parent ApogeeSP510 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SP510Voltage".
     */
    explicit ApogeeSP510_Voltage(
        ApogeeSP510* parentSense, const char* uuid = "",
        const char* varCode = SP510_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SP510_VOLTAGE_VAR_NUM,
                   (uint8_t)SP510_VOLTAGE_RESOLUTION, SP510_VOLTAGE_VAR_NAME,
                   SP510_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSP510_Voltage object.
     *
     * @note This must be tied with a parent ApogeeSP510 before it can be used.
     */
    ApogeeSP510_Voltage()
        : Variable((const uint8_t)SP510_VOLTAGE_VAR_NUM,
                   (uint8_t)SP510_VOLTAGE_RESOLUTION, SP510_VOLTAGE_VAR_NAME,
                   SP510_VOLTAGE_UNIT_NAME, SP510_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSP510_Voltage object - no action needed.
     */
    ~ApogeeSP510_Voltage() {}
};
/**@}*/
#endif  // SRC_SENSORS_APOGEESP510_H_
