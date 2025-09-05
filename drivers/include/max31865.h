/*
 * SPDX-FileCopyrightText: 2025 David Picard
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_max31865 MAX31865 RTD-to-Digital converter driver
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the SPI RTD-to-Digital converter MAX31865.
 *
 * \section sec_max31865_ovrvw Overview

 * The MAX31865 is a resistance-to-digital
 * converter optimized for platinum resistance temperature
 * detectors (RTDs). An external resistor sets the sensitivity
 * for the RTD being used and a precision delta-sigma ADC
 * converts the ratio of the RTD resistance to the reference
 * resistance on the board, to a 15-bit word.
 *
 * - Compatible with 2-, 3-, and 4-Wire Sensor Connections
 * - 15-Bit ADC resolution; resolution 0.03125°C
 * - Total accuracy over all operating conditions: 0.5°C (0.05% FS) max
 * - 21 ms max conversion time
 *
 * The RTD register and the threshold registers represent the ratio
 * of the RTD resistance to the reference resistance.
 *
 * @note See the
 * <a href="https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31865.pdf">
 * datasheet</a> for more information.
 *
 * \section sec_max31865_lut Lookup table
 *
 * In order to convert the ADC code to temperature with a decent precision
 * and low CPU usage by using fixed point computation, this driver uses a lookup table.
 * The values of the lookup table depend on the type of RTD, the reference
 * resistance on the board and the temperature range.
 * A default lookup table for a Pt100 sensor, a reference resistance of 330&Omega;
 * and a temperature range of -200..+650°C is provided.
 * With this lookup table, the standard deviation of the computation
 * error over the -200..+650°C range is 0.011°C and the maximum error
 * is 0.039°C.
 *
 * If the default lookup doesn't fit the application, the user can generate
 * a custom one with the Python script `drivers/max31865/dist/genlut.py`.
 * Type `genlut.py -h` for help.
 * Move the generated header file to the application project directory.
 * In the application code, include `max31865_lut.h`.
 * The header files must be included in the following order:
 *
 * @code
 * #include "max31865.h"
 * #include "max31865_lut.h"
 * #include "max31865_params.h"
 * @endcode
 *
 * The lookup table in the application directory will override the default one.
 *
 * \section sec_max31865_usage Usage
 *
 * The default configuration is set in \ref max31865_params.h as a
 * \ref max31865_params_t structure.
 * Most of them can be changed using the preprocessor definitions either
 * in the board definition in `board.h` or in the Makefile of the application
 * using the `CFLAGS` variable.
 * In particular, let the SPI device and chip select pin fit the board.
 *
 * @code
 * define MAX31865_PARAM_SPI        (SPI_DEV(0))
 * define MAX31865_PARAM_CS_PIN     (GPIO_PIN(0, 5))
 * @endcode
 *
 * The easiest way to read the temperature is to call #max31865_read().
 * In critical applications, where a sensor failure may cause damage to the
 * system, calling #max31865_read_raw(),
 * #max31865_raw_to_data() and #max31865_detect_fault() independently
 * provides a better control on fault detection.
 *
 * @{
 *
 * @file
 *
 * @author      David Picard
 */

/* Add header includes here */

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/spi.h"

/**
 * @brief Lookup table column indexes
 *
 * The lookup table is a 2D array.
 * This enum defines the indexes of the columns of the table.
 * The conversion from ADC code to temperature is done with a linear interpolation
 * between two lines of the table.
 * The coefficients at line n are valid for the temperature range between line n
 * and line n+1.
 *
 * T = a<sub>0</sub> + a<sub>1</sub> &times; code
 */
typedef enum {
    MAX31865_LUTCOL_CODE        = 0,    /**< ADC code column index */
    MAX31865_LUTCOL_TEMP        = 1,    /**< Temperature column index (µ°C) */
    MAX31865_LUTCOL_A0          = 2,    /**< a<sub>0</sub> coefficient column index */
    MAX31865_LUTCOL_A1          = 3,    /**< a<sub>1</sub> coefficient column index */
    MAX31865_LUTCOL_NUMOF               /**< Number of columns in the lookup table */
} max31865_lutcols_t;

/**
 * @brief   Device initialization parameters
 *
 * - Use the bits defined in \ref drivers_max31865_constants_regcfg to build the
 *   configuration byte #max31865_params_t.cfg_byte.
 * - The high and low thresholds must be set in units of 0.01 °C, e.g. `12000` for 120 °C.
 * - #max31865_params_t.lut_numlines is defined in the header file generated
 *   by the Python script `drivers/max31865/dist/genlut.py`.
 */
typedef struct {
    spi_t spi;                      /**< SPI device */
    spi_cs_t cs_pin;                /**< Chip select pin */
    uint8_t cfg_byte;               /**< Initial value of the configuration register */
    int32_t temp_low_threshold;     /**< Low threshold temperature (c°C) */
    int32_t temp_high_threshold;    /**< High threshold temperature (c°C) */
    const int32_t (*lut)[][MAX31865_LUTCOL_NUMOF];    /**< Lookup table */
    const int lut_numlines;         /**< Number of lines in the lookup table */
} max31865_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    const max31865_params_t *params;  /**< device configuration */
} max31865_t;

/**
 * @brief   Fault status of the MAX31865
 */
typedef enum {
    MAX31865_FAULT_NO_FAULT     = 0,    /**< No fault */
    MAX31865_FAULT_RTD_HIGH     = 1,    /**< The RTD value is too high */
    MAX31865_FAULT_RTD_LOW      = 2,    /**< The RTD value is too small */
    MAX31865_FAULT_CIRCUIT      = 3,    /**< Open or shorted circuit */
    MAX31865_FAULT_VOLTAGE      = 4     /**< Overvoltage or undervoltage on the FORCE or RTD pin */
} max31865_fault_t;

/**
 * @brief   Initialize the given device
 *
 * @param[in,out] dev    Device descriptor of the driver
 * @param[in] params     Initialization parameters
 *
 * @retval    0 on success
 * @retval    -ENXIO invalid SPI device
 * @retval    -EINVAL invalid SPI CS pin/line
 */
int max31865_init(max31865_t *dev, const max31865_params_t *params);

/**
 * @brief Clear the fault flag
 *
 * @param[in] dev       Device descriptor of the driver
 * @param[out] config   If not NULL, set to the value of the config register
 *
 * Call this function if after #max31865_read() or #max31865_detect_fault() if
 * either of them reports a fault.
 */
void max31865_clear_fault(const max31865_t *dev, uint8_t *config);

/**
 * @brief Read data from the MAX31865. This is a shortcut to read raw data
 * and parse it to the data structure.
 *
 * @param[in] dev                       Device descriptor of the driver
 * @param[out] rtd_temperature_cdegc    Temperature in centi-degrees Celsius (0.01°C)
 *
 * @pre \a dev and \a data must not be NULL
 *
 * This function does a minimal error check.
 * To get more details on the fault, call
 * #max31865_detect_fault().
 *
 * @retval 0 on success
 * @retval -EIO if an error was detected by the MAX31865
 */
int max31865_read(const max31865_t *dev, int32_t *rtd_temperature_cdegc);

/**
 * @brief Read raw data from the MAX31865
 * @param[in] dev Device descriptor of the driver
 * @param[out] raw_data Value of the RTD registers
 *
 * \a raw_data is a left-justified 15-bit word, representing the ratio
 * of the RTD resistance to the reference resistance.
 * Bit 0 is a status bit.
 *
 * @retval 0 on success.
 * @retval -EIO if an error was detected by the MAX31865.
 * In this case, the temperature is not valid.
 * Call #max31865_detect_fault() for more details.
 */
int max31865_read_raw(const max31865_t *dev, uint16_t *raw_data);

/**
 * @brief   Convert the raw data from the MAX31865 temperature
 *
 * @param[in]  dev                      Device descriptor of the driver
 * @param[in]  raw_data                 Raw data from the MAX31865
 * @param[out] rtd_temperature_cdegc    Temperature in centi-degrees Celsius (0.01°C)
 *
 * @pre                                 @p data must not be NULL
 *
 * @retval 0 on success
 * @retval -EINVAL on error
 */
int max31865_raw_to_data(const max31865_t *dev, uint16_t raw_data, int32_t *rtd_temperature_cdegc);

/**
 * @brief Run an automatic fault-detection cycle
 * @param[in] dev Device descriptor of the driver
 * @param[out] flt_code Fault code
 *
 * Run a full fault-detection cycle, in automatic mode.
 * The automatic mode implies that the input filter of the ADC has
 * a time constant smaller than 100µs.
 * This is the case if the capacitor across the RTD has the
 * value which is recommended in the datasheet.
 *
 * The execution time of this function is at least 100µs, until the
 * fault-detection cycle completes.
 * It polls the configuration register to check the completion of the cycle.
 *
 * @pre V<sub>BIAS</sub> must be on for at least 5 time constants.
 *
 * @retval 0 on success.
 * @retval -EIO on error.
 */
int max31865_detect_fault(const max31865_t *dev, max31865_fault_t *flt_code);

/**
 * @brief Switch V<sub>BIAS</sub> on or off
 * @param[in] dev Device descriptor of the driver
 * @param[in] enable Set to \c true to switch on, or to \c false to switch off V<sub>BIAS</sub>.
 *
 * The bias current of the RTD can be switched off to save power.
 * After switching it on, wait for at least 10 time constants -- 10 &times; 100µs,
 * if the capacitance across the RTD is as recommended in the datasheet --
 * before measuring a valid temperature.
 */
void max31865_switch_vbias(const max31865_t *dev, bool enable);

/**
 * @brief Start a one-shot conversion
 * @param[in] dev Device descriptor of the driver
 *
 * The user must wait at least 52ms in 60Hz or 62.5ms in 50Hz filter mode
 * for the conversion to complete, before reading the conversion result.
 */
void max31865_oneshot(const max31865_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
