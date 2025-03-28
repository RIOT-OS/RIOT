/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef HMC5883L_H
#define HMC5883L_H

/**
 * @defgroup    drivers_hmc5883l HMC5883L 3-axis digital compass
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the Honeywell HMC5883L 3-axis digital compass
 *
 * The driver implements basic polling mode. The application can use
 * different approaches to get new data, either
 *
 * - using the #hmc5883l_read function at a lower rate than the DOR, or
 * - using the data-ready interrupt (**DRDY**), see #hmc5883l_init_int.
 *
 * The data-ready interrupt (**DRDY**) is only be available when module
 * `hmc5883l_int` is enabled.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

/** Definition of error codes */
typedef enum {
    HMC5883L_OK             =  0,  /**< success */
    HMC5883L_ERROR_I2C      = -1,  /**< any I2C communication error */
    HMC5883L_ERROR_WRONG_ID = -2,  /**< wrong id read */
    HMC5883L_ERROR_NO_DATA  = -3,  /**< no data are available */
    HMC5883L_ERROR_RAW_DATA = -4,  /**< reading raw data failed */
    HMC5883L_ERROR_COMMON   = -5,  /**< common error */
} hmc5883l_error_codes_t;

/**
 * @brief   Data output rates (DOR)
 *
 * The values correspond to bits <4:2> of the HMC5883L_REG_CFG_A register.
 */
typedef enum {
    HMC5883L_DOR_0_75 = 0x00,   /**< 0.75 Hz           */
    HMC5883L_DOR_1_5  = 0x04,   /**<  1.5 Hz           */
    HMC5883L_DOR_3    = 0x08,   /**<    3 Hz           */
    HMC5883L_DOR_7_5  = 0x0c,   /**<  7.5 Hz           */
    HMC5883L_DOR_15   = 0x10,   /**<   15 Hz (default) */
    HMC5883L_DOR_30   = 0x14,   /**<   30 Hz           */
    HMC5883L_DOR_75   = 0x18,   /**<   75 Hz           */
} hmc5883l_dor_t;

/**
 * @brief   Measurement modes
 *
 * The values correspond to bits <1:0> of the HMC5883L_REG_CFG_A register.
 */
typedef enum {
    HMC5883L_MEAS_MODE_NORMAL   = 0x00, /**< Normal measurement config */
    HMC5883L_MEAS_MODE_BIAS_POS = 0x01, /**< Positive bias config for all axes */
    HMC5883L_MEAS_MODE_BIAS_NEG = 0x02, /**< Negative bias config for all axes */
} hmc5883l_meas_mode_t;

/**
 * @brief   Measurement avaraging (number of samples are averaged for output)
 *
 * The values correspond to bits <6:5> of the HMC5883L_REG_CFG_A register.
 */
typedef enum {
    HMC5883L_MEAS_AVG_NONE = 0x00,  /**< No averaging */
    HMC5883L_MEAS_AVG_2    = 0x20,  /**< 2 samples are averaged */
    HMC5883L_MEAS_AVG_4    = 0x40,  /**< 4 samples are averaged */
    HMC5883L_MEAS_AVG_8    = 0x60,  /**< 8 samples are averaged */
} hmc5883l_meas_avg_t;

/**
 * @brief   Operation modes
 *
 * Values correspond to bits <1:0> of HMC5883L_REG_MODE register
 */
typedef enum {
    HMC5883L_OP_MODE_CONTINUOUS = 0x00, /**< Continuous measurement */
    HMC5883L_OP_MODE_SINGLE     = 0x01, /**< Single measurement */
    HMC5883L_OP_MODE_IDLE       = 0x02, /**< Idle mode */
} hmc5883l_op_mode_t;

/**
 * @brief   Gain (determines the sensitivity and the range)
 *
 * The values correspond to bits <7:5> of the HMC5883L_REG_CFG_B_GN register.
 */
typedef enum {
    HMC5883L_GAIN_1370 = 0x00,  /**< Range +-0.88 Gs, Resolution 0.73 mGs/LSB */
    HMC5883L_GAIN_1090 = 0x20,  /**< Range +-1.3 Gs,  Resolution 0.92 mGs/LSB */
    HMC5883L_GAIN_820  = 0x40,  /**< Range +-1.9 Gs,  Resolution 1.22 mGs/LSB */
    HMC5883L_GAIN_660  = 0x60,  /**< Range +-2.5 Gs,  Resolution 1.52 mGs/LSB */
    HMC5883L_GAIN_440  = 0x80,  /**< Range +-4.0 Gs,  Resolution 2.27 mGs/LSB */
    HMC5883L_GAIN_390  = 0xa0,  /**< Range +-4.7 Gs,  Resolution 2.56 mGs/LSB */
    HMC5883L_GAIN_330  = 0xc0,  /**< Range +-5.6 Gs,  Resolution 3.03 mGs/LSB */
    HMC5883L_GAIN_230  = 0xe0,  /**< Range +-8.1 Gs,  Resolution 4.35 mGs/LSB */
} hmc5883l_gain_t;

/**
 * @brief   Magnetic field values in milli-Gauss (mGs)
 */
typedef struct {
    int16_t x;  /**< magnetic field x-axis */
    int16_t y;  /**< magnetic field y-axis */
    int16_t z;  /**< magnetic field y-axis */
} hmc5883l_data_t;

/**
 * @brief   Raw data set as two complements
 */
typedef struct {
    int16_t x; /**< magnetic field x-axis as 16 bit two's complements */
    int16_t y; /**< magnetic field y-axis as 16 bit two's complements */
    int16_t z; /**< magnetic field z-axis as 16 bit two's complements */
} hmc5883l_raw_data_t;

#if MODULE_HMC5883L_INT || DOXYGEN

/**
 * @brief   HMC5883L DRDY interrupt callback function type
 *
 * Function prototype for the function which is called on DRDY interrupt if
 * the interrupt is activated by #hmc5883l_init_int and the interrupt pin is
 * defined in device initialization parameters.
 *
 * @note The @p cb function is called in interrupt context. The application
 *       should do nothing time consuming and not directly access sensor data.
 */
typedef void (*hmc5883l_drdy_int_cb_t)(void *);

#endif /* MODULE_HMC5883L_INT || DOXYGEN */

/**
 * @brief   HMC5883L device initialization parameters
 */
typedef struct {
    unsigned dev;    /**< I2C device */
#if MODULE_HMC5883L_INT
    gpio_t int_pin;  /**< DRDY interrupt pin: if #GPIO_UNDEF, interrupts are not used */
#endif
    hmc5883l_meas_mode_t meas_mode; /**< Measurement mode (default #HMC5883L_MEAS_MODE_NORMAL) */
    hmc5883l_meas_avg_t  meas_avg;  /**< Measurement avaraging (default #HMC5883L_MEAS_AVG_NONE) */
    hmc5883l_dor_t       dor;       /**< Data output rate (default #HMC5883L_DOR_15) */
    hmc5883l_op_mode_t   op_mode;   /**< Operation mode (#HMC5883L_OP_MODE_CONTINUOUS) */
    hmc5883l_gain_t      gain;      /**< Gain (default #HMC5883L_GAIN_1090) */
} hmc5883l_params_t;

/**
 * @brief   HMC5883L sensor device data structure type
 */
typedef struct {
    unsigned dev;    /**< I2C device */
#if MODULE_HMC5883L_INT
    gpio_t int_pin;  /**< DRDY interrupt pin: if #GPIO_UNDEF, interrupts are not used */
#endif
    hmc5883l_op_mode_t   op_mode;   /**< Operation mode (#HMC5883L_OP_MODE_CONTINUOUS) */
    hmc5883l_gain_t      gain;      /**< Gain (default #HMC5883L_GAIN_1090) */
} hmc5883l_t;

/**
 * @brief	Initialize the HMC5883L sensor device
 *
 * This function resets the sensor and initializes the sensor according to
 * given initialization parameters. All registers are reset to default values.
 *
 * @param[in]   dev     device descriptor of HMC5883L sensor to be initialized
 * @param[in]   params  HMC5883L initialization parameters
 *
 * @retval  HMC5883L_OK             on success
 * @retval  HMC5883L_ERROR_*        a negative error code on error,
 *                                  see #hmc5883l_error_codes_t
 */
int hmc5883l_init(hmc5883l_t *dev, const hmc5883l_params_t *params);

#if MODULE_HMC5883L_INT || DOXYGEN

/**
 * @brief	Initialize and activate the DRDY interrupt of HMC5883L sensor device
 *
 * This function activates the DRDY interrupt and initializes the pin defined
 * as the interrupt pin in the initialization parameters of the device. The
 * @p cb parameter specifies the function, along with an optional argument
 * @p arg, which is called when a DRDY interrupt is triggered.
 *
 * @warning The given callback function @p cb is executed in interrupt context.
 *          Make sure not to call any driver API function in that context.
 * @note This function is only available when module `hmc5883l_int` is enabled.
 *
 * @param[in]   dev     device descriptor of HMC5883L sensor
 * @param[in]   cb      function called when DRDY interrupt is triggered
 * @param[in]   arg     argument for the callback function
 *
 * @retval  HMC5883L_OK             on success
 * @retval  HMC5883L_ERROR_*        a negative error code on error,
 *                                  see #hmc5883l_error_codes_t
 */
int hmc5883l_init_int(hmc5883l_t *dev, hmc5883l_drdy_int_cb_t cb, void *arg);

#endif /* MODULE_HMC5883L_INT || DOXYGEN */

/**
 * @brief    Data-ready status function
 *
 * The function checks the status register and returns
 *
 * @param[in]   dev     device descriptor of HMC5883L sensor
 *
 * @retval  HMC5883L_OK             new data available
 * @retval  HMC5883L_ERROR_NO_DATA  no new data available
 * @retval  HMC5883L_ERROR_*        negative error code,
 *                                  see #hmc5883l_error_codes_t
 */
int hmc5883l_data_ready(const hmc5883l_t *dev);

/**
 * @brief   Read one sample of magnetic field values in milli-Gauss (mGs)
 *
 * Raw magnetometer data are read from the sensor and normalized them
 * with respect to configured gain. Magnetic field values are given in
 * milli-Gauss (mGs) to preserve full resolution:
 *
 * @param[in]   dev     device descriptor of HMC5883L sensor
 * @param[out]  data    result vector in milli-Gauss (mGs) per axis
 *
 * @retval  HMC5883L_OK         on success
 * @retval  HMC5883L_ERROR_*    a negative error code on error,
 *                              see #hmc5883l_error_codes_t
 */
int hmc5883l_read(const hmc5883l_t *dev, hmc5883l_data_t *data);

/**
 * @brief   Read one sample of raw sensor data as 16 bit two's complements
 *
 * @param[in]   dev     device descriptor of HMC5883L sensor
 * @param       raw     raw data vector
 *
 * @retval  HMC5883L_OK         on success
 * @retval  HMC5883L_ERROR_*    a negative error code on error,
 *                              see #hmc5883l_error_codes_t
 */
int hmc5883l_read_raw(const hmc5883l_t *dev, hmc5883l_raw_data_t *raw);

/**
 * @brief   Power down the sensor
 *
 * Changes the sensor operation mode to #HMC5883L_OP_MODE_IDLE in which
 * almost all internal blocks are switched off. I2C interface is
 * still active. The content of the configuration registers is preserved.
 *
 * @param[in]   dev     Device descriptor of HMC5883L device to read from
 *
 * @retval  HMC5883L_OK      on success
 * @retval  HMC5883L_ERROR_* negative error code, see #hmc5883l_error_codes_t
 */
int hmc5883l_power_down(hmc5883l_t *dev);

/**
 * @brief   Power up the sensor
 *
 * Swichtes the sensor back into the last active operation mode.
 *
 * @param[in]   dev     Device descriptor of HMC5883L device to read from
 *
 * @retval  HMC5883L_OK      on success
 * @retval  HMC5883L_ERROR_* negative error code, see #hmc5883l_error_codes_t
 */
int hmc5883l_power_up(hmc5883l_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HMC5883L_H */
