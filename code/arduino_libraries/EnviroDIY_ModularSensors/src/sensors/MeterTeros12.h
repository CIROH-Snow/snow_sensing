/**
 * @file MeterTeros12.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MeterTeros12 sensor subclass and the variable subclasses
 * MeterTeros12_Ea, MeterTeros12_Temp, MeterTeros12_VWC, and MeterTeros12_EC.
 *
 * These are for the Meter Teros 12 Advanced Soil Moisture probe.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 */
/* clang-format off */
/**
 * @defgroup sensor_teros12 Meter Teros 12
 * Classes for the Meter Teros 12 soil moisture probe.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_teros12_intro Introduction
 *
 * Meter Environmental makes two series of soil moisture sensors, the
 * [ECH2O series](https://www.metergroup.com/environment/products/?product_category=9525) and the
 * [Teros series](https://www.metergroup.com/environment/products/teros-12/).
 * __This page is for the Teros series.__
 *
 * Both series of sensors operate as sub-classes of the SDI12Sensors class.
 * They require a 3.5-12V power supply, which can be turned off between
 * measurements. While contrary to the manual, they will run with power as low
 * as 3.3V. On the 5TM with a stereo cable, the power is connected to the tip,
 * data to the ring, and ground to the sleeve. On the bare-wire version, the
 * power is connected to the _white_ cable, data to _red_, and ground to the
 * unshielded cable.
 *
 * @warning Coming from the factory, METER sensors are set at SDI-12 address
 * '0'.  They also output a "DDI" serial protocol string on each power up.
 * This library *disables the DDI output string* on all newer METER sensors
 * that support disabling it.  After using a METER sensor with ModularSensors,
 * you will need to manually re-enable the DDI output if you wish to use it.
 *
 * @section sensor_teros12_datasheet Sensor Datasheet
 * Documentation for the SDI-12 Protocol commands and responses for the Meter
 * Teros 12 can be found at:
 * http://publications.metergroup.com/Manuals/20587_TEROS11-12_Manual_Web.pdf
 *
 * @section sensor_teros12_voltages Voltage Ranges
 * - Supply Voltage (VCC to GND), 4.0 to 15.0 VDC
 * - Digital Input Voltage (logic high), 2.8 to 3.9 V (3.6 typical)
 * - Digital Output Voltage (logic high), 3.6 typical
 *
 * @section sensor_teros12_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_teros12_ctor Sensor Constructor
 * {{ @ref MeterTeros12::MeterTeros12 }}
 *
 * ___
 * @section sensor_teros12_examples Example Code
 * The Meter Teros is used in the @menulink{meter_teros12} example.
 *
 * @menusnip{meter_teros12}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_METERTEROS12_H_
#define SRC_SENSORS_METERTEROS12_H_

// Debugging Statement
//#define MS_MeterTeros12_DEBUG

#ifdef MS_METERTEROS12_DEBUG
#define MS_DEBUGGING_STD "MeterTeros12"
#endif

#ifdef MS_SDI12SENSORS_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "SDI12Sensors"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_teros12 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Teros 12 can report 3 raw values -
/// counts, temperature, and electrical conductivity.
#define TEROS12_NUM_VARIABLES 5
/// @brief Sensor::_incCalcValues; We calculate permittivity and water content
/// from the raw counts and temperature reported by the Teros 12.
#define TEROS12_INC_CALC_VARIABLES 2

/**
 * @anchor sensor_teros12_timing
 * @name Sensor Timing
 * The sensor timing for a Meter Teros 12
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the Teros 12 warm-up time in SDI-12 mode:
/// 245ms typical
#define TEROS12_WARM_UP_TIME_MS 250
/// @brief Sensor::_stabilizationTime_ms; the Teros 12 is stable after 50ms.
#define TEROS12_STABILIZATION_TIME_MS 50
/// @brief Sensor::_measurementTime_ms; the Teros 12 takes 25 ms to 50 ms to
/// complete a measurement.
#define TEROS12_MEASUREMENT_TIME_MS 50
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The Terros-12 requires no extra time.
#define TEROS12_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_teros12_counts
 * @name Raw Counts
 * The raw VWC counts variable from a Meter Teros 12
 * - Range and accuracy of the raw count values are not specified
 *
 * {{ @ref MeterTeros12_Count::MeterTeros12_Count }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; EA should have 1.
 */
#define TEROS12_COUNT_RESOLUTION 0
/// @brief Sensor variable number; EA is stored in sensorValues[0].
#define TEROS12_COUNT_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "counter"
#define TEROS12_COUNT_VAR_NAME "counter"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "count"
#define TEROS12_COUNT_UNIT_NAME "count"
/// @brief Default variable short code; "RawVWCCounts"
#define TEROS12_COUNT_DEFAULT_CODE "RawVWCCounts"
/**@}*/

/**
 * @anchor sensor_teros12_temp
 * @name Temperature
 * The temperature variable from a Meter Teros 12
 * - Range is -40°C to 60°C
 * - Accuracy is:
 *     - ± 1°C, from -40°C to 0°C
 *     - ± 0.5°C, from 0°C to + 60°C
 *
 * {{ @ref MeterTeros12_Temp::MeterTeros12_Temp }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; temperature should have 2.
 *
 * 1 is reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.1°C
 */
#define TEROS12_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define TEROS12_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define TEROS12_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define TEROS12_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "SoilTemp"
#define TEROS12_TEMP_DEFAULT_CODE "SoilTemp"
/**@}*/

/**
 * @anchor sensor_teros12_ea
 * @name EA
 * The EA variable from a Meter Teros 12
 * - Range is 1 (air) to 80 (water)
 * - Accuracy is:
 *     - 1–40 (soil range), ±1 εa (unitless)
 *     - 40–80, 15% of measurement
 *
 * {{ @ref MeterTeros12_Ea::MeterTeros12_Ea }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; EA should have 5.
 *
 * 4 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.00001
 */
#define TEROS12_EA_RESOLUTION 5
/// @brief Sensor variable number; EA is stored in sensorValues[0].
#define TEROS12_EA_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "permittivity"
#define TEROS12_EA_VAR_NAME "permittivity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "faradPerMeter" (F/m)
#define TEROS12_EA_UNIT_NAME "faradPerMeter"
/// @brief Default variable short code; "SoilEa"
#define TEROS12_EA_DEFAULT_CODE "SoilEa"
/**@}*/

/**
 * @anchor sensor_teros12_vwc
 * @name Volumetric Water Content
 * The VWC variable from a Meter Teros 12
 *   - Range is:
 *     - Mineral soil calibration: 0.00–0.70 m3/m3 (0 – 70% VWC)
 *     - Soilless media calibration: 0.0–1.0 m3/m3 (0 – 100% VWC)
 *   - Accuracy is:
 *     - Generic calibration: ±0.03 m3/m3 (± 3% VWC) typical in mineral soils
 * that have solution electrical conductivity <8 dS/m
 *     - Medium specific calibration: ±0.01–0.02 m3/m3 (± 1-2% VWC)in any porous
 * medium
 *
 * {{ @ref MeterTeros12_VWC::MeterTeros12_VWC }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; VWC should have 3.
 *
 * 2 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - Resolution is 0.001 m3/m3 (0.1% VWC)
 * from 0 – 70% VWC
 */
#define TEROS12_VWC_RESOLUTION 3
/// @brief Sensor variable number; VWC is stored in sensorValues[2].
#define TEROS12_VWC_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "volumetricWaterContent"
#define TEROS12_VWC_VAR_NAME "volumetricWaterContent"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent" -
/// volumetric percent water content (%, m3/100m3)
#define TEROS12_VWC_UNIT_NAME "percent"
/// @brief Default variable short code; "SoilVWC"
#define TEROS12_VWC_DEFAULT_CODE "SoilVWC"
/**@}*/

/**
 * @anchor sensor_teros12_ec
 * @name Electrical Conductivity
 * The EC variable from a Meter Teros 12
 *   - Range is:
 *     0 - 20,000 uS/cm (bulk)
 *   - Accuracy is:
 *     0 - 10,000 uS/cm: ±(5% + 10uS/cm)
 *     10,000 - 20,000 uS/cm: ±8%
 *
 * {{ @ref MeterTeros12_VWC::MeterTeros12_VWC }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation.
 *
 * 2 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - Resolution is 0.001 m3/m3 (0.1% VWC)
 * from 0 – 70% VWC
 */
#define TEROS12_EC_RESOLUTION 2
/// @brief Sensor variable number; VWC is stored in sensorValues[2].
#define TEROS12_EC_VAR_NUM 4
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "volumetricWaterContent"
#define TEROS12_EC_VAR_NAME "electricalConductivity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent" -
/// volumetric percent water content (%, m3/100m3)
#define TEROS12_EC_UNIT_NAME "Microsiemen per Centimeter"
/// @brief Default variable short code; "SoilVWC"
#define TEROS12_EC_DEFAULT_CODE "SoilEC"
/**@}*/

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter Teros 12 sensor](@ref sensor_teros12)
 *
 * @ingroup sensor_teros12
 */
/* clang-format on */
class MeterTeros12 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Meter Teros 12 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the Teros 12; can be a char,
     * char*, or int.
     * @warning The SDI-12 address **must** be changed from the factory
     * programmed value of "0" before the Teros 12 can be used with
     * ModularSensors!
     * @param powerPin The pin on the mcu controlling power to the Teros 12
     * Use -1 if it is continuously powered.
     * - The Teros 12 requires a 3.5-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    MeterTeros12(char SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterTeros12", TEROS12_NUM_VARIABLES,
                       TEROS12_WARM_UP_TIME_MS, TEROS12_STABILIZATION_TIME_MS,
                       TEROS12_MEASUREMENT_TIME_MS, TEROS12_EXTRA_WAKE_TIME_MS,
                       TEROS12_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterTeros12::MeterTeros12
     */
    MeterTeros12(char* SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterTeros12", TEROS12_NUM_VARIABLES,
                       TEROS12_WARM_UP_TIME_MS, TEROS12_STABILIZATION_TIME_MS,
                       TEROS12_MEASUREMENT_TIME_MS, TEROS12_EXTRA_WAKE_TIME_MS,
                       TEROS12_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterTeros12::MeterTeros12
     */
    MeterTeros12(int SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterTeros12", TEROS12_NUM_VARIABLES,
                       TEROS12_WARM_UP_TIME_MS, TEROS12_STABILIZATION_TIME_MS,
                       TEROS12_MEASUREMENT_TIME_MS, TEROS12_EXTRA_WAKE_TIME_MS,
                       TEROS12_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Meter Teros 12 object
     */
    ~MeterTeros12() {}

    /**
     * @copydoc SDI12Sensors::getResults()
     */
    bool getResults(void) override;
};


// Defines the raw VWC count Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw calibrated VWC counts](@ref sensor_teros12_counts)
 * from a [Meter Teros soil moisture/water content sensor](@ref sensor_teros12).
 *
 * @ingroup sensor_teros12
 */
/* clang-format on */
class MeterTeros12_Count : public Variable {
 public:
    /**
     * @brief Construct a new MeterTeros12_Count object.
     *
     * @param parentSense The parent MeterTeros12 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RawVWCCounts".
     */
    explicit MeterTeros12_Count(
        MeterTeros12* parentSense, const char* uuid = "",
        const char* varCode = TEROS12_COUNT_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TEROS12_COUNT_VAR_NUM,
                   (uint8_t)TEROS12_COUNT_RESOLUTION, TEROS12_COUNT_VAR_NAME,
                   TEROS12_COUNT_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterTeros12_Count object.
     *
     * @note This must be tied with a parent MeterTeros12 before it can be used.
     */
    MeterTeros12_Count()
        : Variable((const uint8_t)TEROS12_COUNT_VAR_NUM,
                   (uint8_t)TEROS12_COUNT_RESOLUTION, TEROS12_COUNT_VAR_NAME,
                   TEROS12_COUNT_UNIT_NAME, TEROS12_COUNT_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterTeros12_Count object - no action needed.
     */
    ~MeterTeros12_Count() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_teros12_temp) output from a
 * [Teros soil moisture/water content sensor](@ref sensor_teros12).
 *
 * @ingroup sensor_teros12
 */
/* clang-format on */
class MeterTeros12_Temp : public Variable {
 public:
    /**
     * @brief Construct a new MeterTeros12_Temp object.
     *
     * @param parentSense The parent MeterTeros12 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilTemp".
     */
    explicit MeterTeros12_Temp(MeterTeros12* parentSense, const char* uuid = "",
                               const char* varCode = TEROS12_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TEROS12_TEMP_VAR_NUM,
                   (uint8_t)TEROS12_TEMP_RESOLUTION, TEROS12_TEMP_VAR_NAME,
                   TEROS12_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterTeros12_Temp object.
     *
     * @note This must be tied with a parent MeterTeros12 before it can be used.
     */
    MeterTeros12_Temp()
        : Variable((const uint8_t)TEROS12_TEMP_VAR_NUM,
                   (uint8_t)TEROS12_TEMP_RESOLUTION, TEROS12_TEMP_VAR_NAME,
                   TEROS12_TEMP_UNIT_NAME, TEROS12_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterTeros12_Temp object - no action needed.
     */
    ~MeterTeros12_Temp() {}
};


// Defines the Ea/Matric Potential Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [apparent dielectric permittivity (εa, matric potential)](@ref sensor_teros12_ea)
 * from a [Meter Teros soil moisture/water content sensor](@ref sensor_teros12).
 *
 * @ingroup sensor_teros12
 */
/* clang-format on */
class MeterTeros12_Ea : public Variable {
 public:
    /**
     * @brief Construct a new MeterTeros12_Ea object.
     *
     * @param parentSense The parent MeterTeros12 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilEa".
     */
    explicit MeterTeros12_Ea(MeterTeros12* parentSense, const char* uuid = "",
                             const char* varCode = TEROS12_EA_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TEROS12_EA_VAR_NUM,
                   (uint8_t)TEROS12_EA_RESOLUTION, TEROS12_EA_VAR_NAME,
                   TEROS12_EA_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterTeros12_Ea object.
     *
     * @note This must be tied with a parent MeterTeros12 before it can be used.
     */
    MeterTeros12_Ea()
        : Variable((const uint8_t)TEROS12_EA_VAR_NUM,
                   (uint8_t)TEROS12_EA_RESOLUTION, TEROS12_EA_VAR_NAME,
                   TEROS12_EA_UNIT_NAME, TEROS12_EA_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterTeros12_Ea object - no action needed.
     */
    ~MeterTeros12_Ea() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [volumetric water content](@ref sensor_teros12_vwc) output from a
 * [Teros soil moisture/water content sensor](@ref sensor_teros12).
 *
 * @ingroup sensor_teros12
 */
/* clang-format on */
class MeterTeros12_VWC : public Variable {
 public:
    /**
     * @brief Construct a new MeterTeros12_VWC object.
     *
     * @param parentSense The parent MeterTeros12 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilVWC".
     */
    explicit MeterTeros12_VWC(MeterTeros12* parentSense, const char* uuid = "",
                              const char* varCode = TEROS12_VWC_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TEROS12_VWC_VAR_NUM,
                   (uint8_t)TEROS12_VWC_RESOLUTION, TEROS12_VWC_VAR_NAME,
                   TEROS12_VWC_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterTeros12_VWC object.
     *
     * @note This must be tied with a parent MeterTeros12 before it can be used.
     */
    MeterTeros12_VWC()
        : Variable((const uint8_t)TEROS12_VWC_VAR_NUM,
                   (uint8_t)TEROS12_VWC_RESOLUTION, TEROS12_VWC_VAR_NAME,
                   TEROS12_VWC_UNIT_NAME, TEROS12_VWC_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterTeros12_VWC object - no action needed.
     */
    ~MeterTeros12_VWC() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [electrical conductivity output](@ref sensor_teros12_temp) output from a
 * [Teros soil moisture/water content sensor](@ref sensor_teros12).
 *
 * @ingroup sensor_teros12
 */
/* clang-format on */
class MeterTeros12_EC : public Variable {
 public:
    /**
     * @brief Construct a new MeterTeros12_EC object.
     *
     * @param parentSense The parent MeterTeros12 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilEC".
     */
    explicit MeterTeros12_EC(MeterTeros12* parentSense, const char* uuid = "",
                               const char* varCode = TEROS12_EC_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TEROS12_EC_VAR_NUM,
                   (uint8_t)TEROS12_EC_RESOLUTION, TEROS12_EC_VAR_NAME,
                   TEROS12_EC_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterTeros12_EC object.
     *
     * @note This must be tied with a parent MeterTeros12 before it can be used.
     */
    MeterTeros12_EC()
        : Variable((const uint8_t)TEROS12_EC_VAR_NUM,
                   (uint8_t)TEROS12_EC_RESOLUTION, TEROS12_EC_VAR_NAME,
                   TEROS12_EC_UNIT_NAME, TEROS12_EC_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterTeros12_EC object - no action needed.
     */
    ~MeterTeros12_EC() {}
};
/**@}*/
#endif  // SRC_SENSORS_METERTEROS12_H_
