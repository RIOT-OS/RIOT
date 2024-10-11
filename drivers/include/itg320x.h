/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_itg320x ITG320X 3-axis gyroscope
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for InvenSense ITG320X 3-axis gyroscope
 *
 * The driver can be used with InvenSense ITG3200, ITG3205, and MPU3050.
 * The latter one can be used only with reduced feature set.
 *
 * The driver implements polling mode as well as interrupt mode. Thus,
 * the application may use two different approaches to retrieve new data,
 * either
 *
 * - periodically fetching the data at a rate lower than the sensor's
 *   output data rate (ODR), or
 * - fetching the data when the data-ready interrupt is triggered.
 *
 * To use the latter approach, module `itg320x_int` has to be enabled and the
 * GPIO to which the sensor's **INT** output pin is connected has to be
 * configured.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @file
 * @brief       Device driver for InvenSense ITG320X 3-axis gyroscope
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ITG320X_H
#define ITG320X_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

/**
 * @name I2C addresses
 * @{
 */
#define ITG320X_I2C_ADDRESS_1    (0x68)     /**< AD0 pin low */
#define ITG320X_I2C_ADDRESS_2    (0x69)     /**< AD0 pin high */
/** @} */

/** ITG320X chip id defined in Who Am I */
#define ITG320X_ID               (0x68)

/**
 * @brief   Named return values
 */
typedef enum {
    ITG320X_OK               =  0,      /**< success */
    ITG320X_ERROR_I2C        = -1,      /**< I2C communication error */
    ITG320X_ERROR_WRONG_ID   = -2,      /**< wrong id read */
    ITG320X_ERROR_NO_DATA    = -3,      /**< no data are available */
    ITG320X_ERROR_RAW_DATA   = -4,      /**< reading raw data failed */
} itg320x_error_codes_t;

/**
 * @brief   Low pass filter bandwidth
 *
 * @note Low pass filter bandwidth determines the internal sample rate (ISR).
 * The internal sample rate (ISR) together with sample rate divider
 * (ISR_DIV) determines the output data rate ODR = ISR / (ISR_DIV + 1)
 * where internal sample rate (ISR) is 8 kHz for #ITG320X_LPF_BW_256,
 * or 1 kHz otherwise.
 */
typedef enum {
    ITG320X_LPF_BW_256 = 0, /**< 256 Hz, ISR = 8 kHz */
    ITG320X_LPF_BW_188,     /**< 188 Hz, ISR = 1 kHz */
    ITG320X_LPF_BW_98,      /**<  98 Hz, ISR = 1 kHz */
    ITG320X_LPF_BW_42,      /**<  42 Hz, ISR = 1 kHz */
    ITG320X_LPF_BW_20,      /**<  20 Hz, ISR = 1 kHz */
    ITG320X_LPF_BW_10,      /**<  10 Hz, ISR = 1 kHz */
    ITG320X_LPF_BW_5,       /**<   5 Hz, ISR = 1 kHz (default) */
} itg320x_lpf_bw_t;

/**
 * @brief   Logic level for INT output pin (ITG320X_REG_INT_CFG<7>)
 */
typedef enum {
    ITG320X_INT_HIGH = 0x00,    /**< INT output is active high (default) */
    ITG320X_INT_LOW  = 0x80,    /**< INT output is active low */
} itg320x_int_level_t;

/**
 * @brief   Drive type for INT output pin (ITG320X_REG_INT_CFG<6>)
 */
typedef enum {
    ITG320X_INT_PUSH_PULL  = 0x00,  /**< INT output is of type push/pull (default) */
    ITG320X_INT_OPEN_DRAIN = 0x40   /**< INT output is of type open drain */
} itg320x_int_drive_t;

/**
 * @brief   Clock source selection (ITG320X_REG_PWR_MGM<2:0>)
 */
typedef enum {
    ITG320X_CLK_INTERNAL = 0,   /**< Internal oscillator */
    ITG320X_CLK_PLL_X_GYRO,     /**< PLL with X Gyro reference (default) */
    ITG320X_CLK_PLL_Y_GYRO,     /**< PLL with X Gyro reference */
    ITG320X_CLK_PLL_Z_GYRO,     /**< PLL with X Gyro reference */
    ITG320X_CLK_PLL_32K,        /**< PLL with external 32.768 kHz reference */
    ITG320X_CLK_PLL_19M,        /**< PLL with external 19.2 MHz reference */
} itg320x_clk_sel_t;

/**
 * @brief   Angular rate values in tenths of a degree per second
 */
typedef struct {
    int16_t x;  /**< angular rate x-axis (roll) */
    int16_t y;  /**< angular rate y-axis (pitch) */
    int16_t z;  /**< angular rate y-axis (yaw) */
} itg320x_data_t;

/**
 * @brief   Raw data set as two complements
 *
 * According to the sensor sensitivity of 1/14.375 degrees per second, the
 * raw data values have to be divided by 14.375 to obtain the measurements
 * in degrees per second.
 */
typedef struct {
    int16_t x; /**< angular rate x-axis as 16 bit two's complements (roll) */
    int16_t y; /**< angular rate y-axis as 16 bit two's complements (pitch) */
    int16_t z; /**< angular rate z-axis as 16 bit two's complements (yaw) */
} itg320x_raw_data_t;

/**
 * @brief   ITG320X device initialization parameters
 */
typedef struct {
    i2c_t dev;     /**< I2C device (default I2C_DEV(0)) */
    uint8_t addr;  /**< I2C slave address (default #ITG320X_I2C_ADDRESS_1) */

    uint8_t isr_div; /**< Internal sample rate divider ISR_DIV (default 99)
                          ODR = ISR / (ISR_DIV + 1) where internal sample rate
                          is 1 kHz or 8 kHz dependent on the low pass filter
                          bandwidth #lpf_bw */

    itg320x_lpf_bw_t lpf_bw; /**< Low pass filter bandwidth
                                  (default #ITG320X_LPF_BW_5, ISR 1 kHz) */
    itg320x_clk_sel_t clk_sel; /**< Clock source selection
                                    (default ITG320X_CLK_PLL_X_GYRO) */

#if MODULE_ITG320X_INT || DOXYGEN
    gpio_t int_pin;                /**< DRDY interrupt pin: #GPIO_UNDEF if
                                        not used */
    itg320x_int_level_t int_level; /**< Logic level for INT output pin
                                        (default #ITG320X_INT_LOW) */
    itg320x_int_drive_t int_drive; /**< Drive type for INT output pin
                                        (default #ITG320X_INT_PUSH_PULL */
#endif /* MODULE_ITG320X_INT || DOXYGEN */
} itg320x_params_t;

/**
 * @brief   ITG320X sensor device data structure type
 */
typedef struct {
    itg320x_params_t params;    /**< device initialization parameters */
} itg320x_t;

#if MODULE_ITG320X_INT || DOXYGEN
/**
 * @brief   ITG320X data ready interrupt (DRDY) callback function type
 *
 * Function prototype for the function which is called on DRDY interrupt if
 * the interrupt is activated by #itg320x_init_int
 *
 * @note The @p cb function is called in interrupt context. The application
 *       should do nothing time consuming and not directly access sensor data.
 */
typedef void (*itg320x_drdy_int_cb_t)(void *);

#endif /* MODULE_ITG320X_INT || DOXYGEN */

/**
 * @brief	Initialize the ITG320X sensor device
 *
 * This function resets the sensor and initializes the sensor according to
 * given initialization parameters. All registers are reset to default values.
 *
 * @param[in]   dev     device descriptor of ITG320X sensor to be initialized
 * @param[in]   params  ITG320X initialization parameters
 *
 * @retval  ITG320X_OK      on success
 * @retval  ITG320X_ERROR_* a negative error code on error,
 *                          see #itg320x_error_codes_t
 */
int itg320x_init(itg320x_t *dev, const itg320x_params_t *params);

#if MODULE_ITG320X_INT || DOXYGEN
/**
 * @brief	Initialize and activate the DRDY interrupt of ITG320X sensor device
 *
 * This function activates the DRDY interrupt and initializes the pin defined
 * as the interrupt pin in the initialization parameters of the device. The
 * @p cb parameter specifies the function, along with an optional argument
 * @p arg, which is called when a DRDY interrupt is triggered.
 *
 * @warning The given callback function @p cb is executed in interrupt context.
 *          Make sure not to call any driver API function in that context.
 * @note This function is only available when module `itg320x_int` is enabled.
 *
 * @param[in]   dev     device descriptor of ITG320X sensor
 * @param[in]   cb      function called when DRDY interrupt is triggered
 * @param[in]   arg     argument for the callback function
 */
int itg320x_init_int(const itg320x_t *dev, itg320x_drdy_int_cb_t cb, void *arg);

#endif /* MODULE_ITG320X_INT || DOXYGEN */

/**
 * @brief    Data-ready status function
 *
 * The function checks the status register and returns
 *
 * @param[in]   dev     device descriptor of ITG320X sensor
 *
 * @retval  ITG320X_OK             new data available
 * @retval  ITG320X_ERROR_NO_DATA  no new data available
 * @retval  ITG320X_ERROR_*        negative error code,
 *                                 see #itg320x_error_codes_t
 */
int itg320x_data_ready(const itg320x_t *dev);

/**
 * @brief   Read one sample of angular rates in tenths of a degree per second
 *
 * Raw magnetometer data are read from the sensor and normalized with
 * respect to full scale +-2000 dps. Angular rate values are given in
 * tenths of a degrees per second:
 *
 * @param[in]   dev     device descriptor of ITG320X sensor
 * @param[out]  data    result vector in tenths of a degrees per second
 *
 * @retval  ITG320X_OK      on success
 * @retval  ITG320X_ERROR_* a negative error code on error,
 *                          see #itg320x_error_codes_t
 */
int itg320x_read(const itg320x_t *dev, itg320x_data_t *data);

/**
 * @brief   Read one sample of raw sensor data as 16 bit two's complements
 *
 * @param[in]   dev     device descriptor of ITG320X sensor
 * @param[out]  raw     raw data vector
 *
 * @retval  ITG320X_OK      on success
 * @retval  ITG320X_ERROR_* a negative error code on error,
 *                          see #itg320x_error_codes_t
 */
int itg320x_read_raw(const itg320x_t *dev, itg320x_raw_data_t *raw);

/**
 * @brief   Read temperature in tenths of a degree Celsius
 *
 * @param[in]   dev     device descriptor of ITG320X sensor
 * @param[out]  temp    temperature tenths of a degree Celsius
 *
 * @retval  ITG320X_OK      on success
 * @retval  ITG320X_ERROR_* a negative error code on error,
 *                          see #itg320x_error_codes_t
 */
int itg320x_read_temp(const itg320x_t *dev, int16_t* temp);

/**
 * @brief   Power down the sensor
 *
 * Changes the sensor operation mode to sleep mode in which  almost all
 * including the gyros are switched off.
 *
 * @param[in]   dev     Device descriptor of ITG320X device to read from
 *
 * @retval  ITG320X_OK      on success
 * @retval  ITG320X_ERROR_* a negative error code on error,
 *                          see #itg320x_error_codes_t
 */
int itg320x_power_down(itg320x_t *dev);

/**
 * @brief   Power up the sensor
 *
 * Swichtes the sensor back into active operation mode.  It takes
 * up to 20 ms since the gyros have to be switched on again.
 *
 * @param[in]   dev     Device descriptor of ITG320X device to read from
 *
 * @retval  ITG320X_OK      on success
 * @retval  ITG320X_ERROR_* a negative error code on error,
 *                          see #itg320x_error_codes_t
 */
int itg320x_power_up(itg320x_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ITG320X_H */
