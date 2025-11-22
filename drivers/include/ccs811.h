/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ccs811
 * @brief       Device Driver for AMS CCS811 digital gas sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <stdint.h>
#include "periph/gpio.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name CCS811 I2C addresses
 * @{
 */
#define CCS811_I2C_ADDRESS_1    (0x5A)  /**< default */
#define CCS811_I2C_ADDRESS_2    (0x5B)
/** @} */

/**
 * @name CCS811 IAQ value ranges
 * @{
 */
#define CCS811_ECO2_RANGE_MIN   (400)   /**< eCO2 min in ppm */
#define CCS811_ECO2_RANGE_MAX   (8192)  /**< eCO2 max in ppm */
#define CCS811_TVOC_RANGE_MIN   (0)     /**< TVOC min in ppb */
#define CCS811_TVOC_RANGE_MAX   (1187)  /**< TVOC min in ppb */
/** @} */

/**
 * @brief   Driver error codes (returned as negative values)
 */
typedef enum {
    CCS811_OK,                   /**< no error */
    CCS811_ERROR_I2C,            /**< I2C communication failure */
    CCS811_ERROR_NO_DEV,         /**< device not available */
    CCS811_ERROR_NO_APP,         /**< could not start application */
    CCS811_ERROR_NO_NEW_DATA,    /**< no new data (last valid data returned) */
    CCS811_ERROR_NO_IAQ_DATA,    /**< IAQ data not available in this mode */
    CCS811_ERROR_WRITE_REG_INV,  /**< invalid register address on write */
    CCS811_ERROR_READ_REG_INV,   /**< invalid register address on read */
    CCS811_ERROR_MEASMODE_INV,   /**< invalid measurement mode */
    CCS811_ERROR_THRESH_INV,     /**< invalid threshold parameters */
    CCS811_ERROR_MAX_RESISTANCE, /**< maximum sensor resistance exceeded */
    CCS811_ERROR_HEATER_FAULT,   /**< heater current not in range */
    CCS811_ERROR_HEATER_SUPPLY,  /**< heater voltage not applied correctly */
    CCS811_ERROR_NO_INT_PIN,     /**< nINT signal pin not configured */
    CCS811_ERROR_NO_WAKE_PIN,    /**< nWAKE signal pin not configured */
    CCS811_ERROR_NO_RESET_PIN,   /**< nRESET signal pin not configured */
    CCS811_ERROR_NOT_SUPPORTED,  /**< function is not supported */
} ccs811_error_codes_t;

/**
 * @brief   CCS811 operation modes
 */
typedef enum {
    CCS811_MODE_IDLE  = 0, /**< Idle, low current mode */
    CCS811_MODE_1S    = 1, /**< Constant Power mode, IAQ values every 1 s */
    CCS811_MODE_10S   = 2, /**< Pulse Heating mode, IAQ values every 10 s */
    CCS811_MODE_60S   = 3, /**< Low Power Pulse Heating, IAQ values every 60 s */
    CCS811_MODE_250MS = 4  /**< Constant Power mode, only RAW data every 250 ms */
} ccs811_mode_t;

/**
 * @brief   CCS811 interrupt mode
 */
typedef enum {
    CCS811_INT_NONE = 0,    /**< interrupt generation is disabled (default) */
    CCS811_INT_DATA_READY,  /**< nINT signal when new data are reade to read */
    CCS811_INT_THRESHOLD,   /**< nINT signal when new data reach thresholds */
} ccs811_int_mode_t;

/**
 * @brief   CCS811 device initialization parameters
 */
typedef struct {

    i2c_t   i2c_dev;            /**< I2C device, clock stretching required (default I2C_DEV(0)) */
    uint8_t i2c_addr;           /**< I2C address (default CCS811_I2C_ADDRESS_1) */
    ccs811_mode_t mode;         /**< measurement mode used (default #CCS811_MODE_IDLE) */
#if MODULE_CCS811_FULL || DOXYGEN
    gpio_t  int_pin;            /**< nINT signal pin (default GPIO_PIN(0, 0) */
    ccs811_int_mode_t int_mode; /**< interrupt mode used (default #CCS811_INT_NONE) */
#endif
    gpio_t  wake_pin;           /**< nWAKE signal pin (default GPIO_UNDEF) */
    gpio_t  reset_pin;          /**< nRESET signal pin (default GPIO_UNDEF) */
} ccs811_params_t;

/**
 * @brief   CCS811 sensor device data structure
 */
typedef struct {
    ccs811_params_t params;     /**< device initialization parameters */
} ccs811_t;

/**
 * @brief   Initialize a CCS811 sensor device
 *
 * The function resets the CCS811 sensor, checks its availability and
 * initializes it according to the given configuration parameters.
 *
 * If #ccs811_params_t::reset_pin is configured
 * - the pin is used for the hardware reset of the sensor, otherwise
 *   only a software reset is tried, and
 * - the #ccs811_init function can be used at any time to reset the sensor.
 *
 * If #ccs811_params_t::wake_pin is configured, it use to switch the sensor
 * into the sleep mode while the I2C interface is not used.
 *
 * @param[in]   dev     Device descriptor of CCS811 device to be initialized
 * @param[in]   params  Configuration parameters used by initialization
 *
 * @note The I2C implementation of the MCU has to support clock stretching
 * to get CCS811 working.
 *
 * @retval  CCS811_OK      on success
 * @retval  CCS811_ERROR_* on error, see #ccs811_error_codes_t
 */
int ccs811_init (ccs811_t *dev, const ccs811_params_t *params);

/**
 * @brief   Read IAQ sensor values and/or RAW sensor data
 *
 * The function reads the IAQ sensor values (TVOC and eCO2) and/or the raw
 * sensor data. For either \p iaq_tvoc2, \p iaq_eco2, \p raw_i, or \p raw_v
 * also `NULL` can be passed, if their value are not of interest.
 *
 * @note
 * - If the function is called and no new data are available, the function
 *   returns the results of the last measurement.
 * - The data-ready status function #ccs811_data_ready or the data-ready
 *   interrupt (#CCS811_INT_DATA_READY) can be used to determine whether
 *   new data are available.
 * - In #CCS811_MODE_250MS, only RAW data are available. In
 *   that case, the function fails with error_code #CCS811_ERROR_NO_IAQ_DATA
 *   if \p iaq_tvoc and \p iaq_eco2 parameters are not `NULL`.
 *
 * @param[in]  dev      Device descriptor of CCS811 device to read from
 * @param[out] iaq_tvoc TVOC total volatile organic compound (0..1187 ppb)
 * @param[out] iaq_eco2 eCO2 equivalent CO2 (400 - 8192 ppm)
 * @param[out] raw_i    Current through the sensor used for measuring (0..63 uA)
 * @param[out] raw_v    Voltage across the sensor measured (0..1023 = 1.65 V)
 *
 * @retval  CCS811_OK                on success and new data are returned
 * @retval  CCS811_ERROR_*           otherwise, see #ccs811_error_codes_t.
 */
int ccs811_read_iaq (const ccs811_t *dev,
                     uint16_t *iaq_tvoc, uint16_t *iaq_eco2,
                     uint16_t *raw_i, uint16_t *raw_v);

#if MODULE_CCS811_FULL || DOXYGEN

/**
 * @brief    Read the resistance of connected NTC thermistor
 *
 * CCS811 supports an external interface for connecting a negative thermal
 * coefficient thermistor (R_NTC) to provide a cost effective and power
 * efficient means of calculating the local ambient temperature. The sensor
 * measures the voltage V_NTC across the R_NTC as well as the voltage V_REF
 * across a connected reference resistor (R_REF).
 *
 * The function returns the current resistance of R_NTC using the equation
 *
 *          R_NTC = R_REF / V_REF * V_NTC
 *
 * Using the data sheet of the NTC, the ambient temperature can be calculated.
 *
 * @param[in]   dev     Device descriptor of CCS811 device to read from
 * @param[in]   r_ref   Resistance of R_REF in Ohm
 * @param[out]  r_ntc   Resistance of R_NTC in Ohm
 *
 * @retval  CCS811_OK      on success
 * @retval  CCS811_ERROR_* on error, see #ccs811_error_codes_t
 */
int ccs811_read_ntc (const ccs811_t *dev, uint32_t r_ref, uint32_t *r_ntc);

#endif /* MODULE_CCS811_FULL || DOXYGEN */

/**
 * @brief   Data-ready status function
 *
 * The function reads the status register and returns CSS811_OK when new
 * data are available. The function is useful for polling the sensor.
 *
 * @param[in]   dev     Device descriptor of CCS811 device to read from
 *
 * @retval  CCS811_OK                when new data are available
 * @retval  CCS811_ERROR_NO_NEW_DATA when no new data are available
 * @retval  CCS811_ERROR_*           otherwise, see #ccs811_error_codes_t.
 */
int ccs811_data_ready (const ccs811_t *dev);

/**
 * @brief   Power down the sensor
 *
 * The feature disables sensor measurements by entering idle mode
 * (#CCS811_MODE_IDLE). In addition, the function sets the low active
 * signal ** nWAKE ** to high to completely deactivate the sensor.
 * The sensor is then no longer accessible via I2C. The last sensor
 * measurement mode is saved.
 *
 * The low active **nWAKE** signal pin has to be configured
 * (#ccs811_params_t::wake_pin) accordingly. Otherwise, the function fails
 * and returns with #CCS811_ERROR_NO_WAKE_PIN.
 *
 * @param[in]   dev     Device descriptor of CCS811 device to read from
 *
 * @retval  CCS811_OK                on success
 * @retval  CCS811_ERROR_NO_WAKE_PIN #ccs811_params_t::wake_pin not configured
 * @retval  CCS811_ERROR_*           on error, see #ccs811_error_codes_t
 */
int ccs811_power_down (ccs811_t *dev);

/**
 * @brief   Power up the sensor
 *
 * The function sets the low active signal ** nWAKE ** to low to activate the
 * sensor and switches back from the idle mode (#CCS811_MODE_IDLE) to the
 * last measurement mode.
 *
 * The low active **nWAKE** signal pin has to be configured
 * (#ccs811_params_t::wake_pin) accordingly. Otherwise, the function fails
 * and returns with #CCS811_ERROR_NO_WAKE_PIN.
 *
 * @note It may take several minutes before accurate readings are generated
 * when the sensor switches back from idle mode to the previous
 * measurement mode.
 *
 * @param[in]   dev     Device descriptor of CCS811 device to read from
 *
 * @retval  CCS811_OK                on success
 * @retval  CCS811_ERROR_NO_WAKE_PIN #ccs811_params_t::wake_pin not configured
 * @retval  CCS811_ERROR_*           on error, see #ccs811_error_codes_t
 */
int ccs811_power_up (ccs811_t *dev);

/**
 * @brief   Set the operation mode of the sensor
 *
 * The function sets the operating mode of the sensor.
 *
 * The sensor starts periodic measurements with the specified period if the
 * \p mode parameter is either
 *
 * - #CCS811_MODE_1S,
 * - #CCS811_MODE_10S,
 * - #CCS811_MODE_60S, or
 * - #CCS811_MODE_250MS.
 *
 * The #ccs811_read_iaq function can then be used to get sensor data at the
 * same rate to get the results.
 *
 * In case, the \p mode parameter is #CCS811_MODE_IDLE, the sensor does not
 * perform any measurements.
 *
 * @note
 * - In #CCS811_MODE_250MS, only raw data are available. IAQ values would
 *   have to be calculated by the host in this mode.
 * - Mode timings (the period) are subject to typical 2% tolerance due to
 *   accuracy of internal sensor clock.
 * - After setting the sensor mode, the sensor needs up to 20 minutes, before
 *   accurate readings are generated.
 * - When the sensor operating mode is changed to a new mode with
 *   a lower sample rate, e.g., from #CCS811_MODE_60S to #CCS811_MODE_1S, it
 *   should be placed in #CCS811_MODE_IDLE for at least 10 minutes before
 *   enabling the new mode.
 *
 * @param[in]   dev     Device descriptor of CCS811 device
 * @param[in]   mode    CCS811 operation mode
 *
 * @retval  CCS811_OK      on success
 * @retval  CCS811_ERROR_* on error, see #ccs811_error_codes_t
 */
int ccs811_set_mode (ccs811_t *dev, ccs811_mode_t mode);

#if MODULE_CCS811_FULL || DOXYGEN
/**
 * @brief   Enable/disable data ready or threshold interrupt signal **nINT**
 *
 * CCS811 can trigger either
 *
 * - a data-ready interrupt (#CCS811_INT_DATA_READY) when new data become
 *   available or
 * - a threshold interrupt (#CCS811_INT_THRESHOLD) if the new eCO2 data
 *   exceed defined thresholds (see #ccs811_set_eco2_thresholds).
 *
 * As soon as an interrupt condition occurs, the signal **nINT** is driven
 * low. It stops being driven low when the sensor data is read with the
 * #ccs811_read_iaq function. #ccs811_params_t::int_pin parameter has
 * to be configured.
 *
 * With #CCS811_INT_NONE (the default), interrupt generation is disabled.
 * The interrupt generation is disabled by default.
 *
 * @param[in]   dev     Device descriptor of CCS811 device
 * @param[in]   mode    Enable the interrupt if true, otherwise disable it
 *
 * @retval  CCS811_OK               on success
 * @retval  CCS811_ERROR_NO_INT_PIN #ccs811_params_t::int_pin not configured
 * @retval  CCS811_ERROR_*          on error, see #ccs811_error_codes_t
 */
int ccs811_set_int_mode (ccs811_t *dev, ccs811_int_mode_t mode);

/**
 * @brief   Set environmental data
 *
 * If information about the environment are available from another sensor,
 * they can be used by CCS811 to compensate gas readings due to
 * temperature and humidity changes.
 *
 * @param[in]   dev     Device descriptor of CCS811 device
 * @param[in]   temp    Temperature [hundredths of a degree Celsius]
 * @param[in]   hum     Relative Humidity [hundredths of a percent]
 *
 * @retval  CCS811_OK      on success
 * @retval  CCS811_ERROR_* on error, see #ccs811_error_codes_t
 */
int ccs811_set_environmental_data (const ccs811_t *dev,
                                   int16_t temp, int16_t hum);

/**
 * @brief   Set eCO2 thresholds for threshold interrupts
 *
 * Threshold interrupts, if enabled (#ccs811_int_mode_t), are generated when
 * new eCO2 value moves from the current range (LOW, MEDIUM or HIGH) to
 * another range by more than one hysteresis value. The hysteresis is used
 * to prevent multiple interrupts near a threshold.
 *
 * Ranges are defined as following:
 *
 * Name   | Range                                     | Value  | Default
 * :------|:------------------------------------------|:-------|:-------
 * LOW    | below the \p low parameter                | > 400  | 1500
 * MEDIUM | between the \p low and \p high parameters |        | |
 * HIGH   | above the value of the \p high parameter  | < 8192 | 2500
 *
 * @param[in]   dev     Device descriptor of CCS811 device
 * @param[in]   low     Threshold LOW to MEDIUM
 * @param[in]   high    Threshold MEDIUM to HIGH
 * @param[in]   hyst    Hysteresis value (default 50)
 *
 * @retval  CCS811_OK      on success
 * @retval  CCS811_ERROR_* on error, see #ccs811_error_codes_t
 */
int ccs811_set_eco2_thresholds (const ccs811_t *dev,
                                uint16_t low, uint16_t high, uint8_t hyst);

/**
 * @brief   Get the current baseline value from sensor
 *
 * The sensor supports automatic baseline correction over a minimum time of
 * 24 hours. Using this function, the current baseline value can be saved
 * before the sensor is powered down. This baseline can then be restored using
 * the #ccs811_set_baseline function after sensor is powered up again to
 * continue the automatic baseline process.
 *
 * @param[in]   dev         Device descriptor of CCS811 device
 * @param[in]   baseline    Current baseline value
 *
 * @retval  CCS811_OK      on success
 * @retval  CCS811_ERROR_* on error, see #ccs811_error_codes_t
 */
int ccs811_get_baseline (const ccs811_t *dev, uint16_t *baseline);

/**
 * @brief   Write a previously stored baseline value to the sensor
 *
 * The sensor supports automatic baseline correction over a minimum time of
 * 24 hours. Using this function, a previously saved baseline
 * (#ccs811_get_baseline) value can be restored after the sensor is powered
 * up to continue the automatic baseline process.
 *
 * @note The baseline must be written after the conditioning period
 * of 20 min after power up.
 *
 * @param[in]   dev         Device descriptor of CCS811 device
 * @param[in]   baseline    Stored baseline value
 *
 * @retval  CCS811_OK      on success
 * @retval  CCS811_ERROR_* on error, see #ccs811_error_codes_t
 */
int ccs811_set_baseline (const ccs811_t *dev, uint16_t baseline);
#endif /* MODULE_CCS811_FULL || DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
