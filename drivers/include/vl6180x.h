/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_vl6180x VL6180X Ranging and Ambient Light Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the ST VL6180X Ranging and Ambient Light Sensor
 *
 * ### Driver Variants
 *
 * Using the according module, the driver can be used in two variants
 * which differ in functionality and size:
 *
 * Module Name         | Driver
 * :-------------------|:-------------------------------------------
 * \ref vl6180x        | Driver with standard functionality and larger size
 * \ref vl6180x_basic  | Driver with only basic functionality and smaller size
 * <br>
 *
 * All driver variants provide @ref drivers_saul capabilities for distance in
 * mm and ambient light in Lux.
 *
 * \anchor vl6180x
 * ### <a name="vl6180x"> Standard driver vl6180x </a>
 *
 * This driver variant is a compromise of size and functionality. It provides
 * the application with most functionality of the sensor for ranging and
 * ambient light sensing:
 *
 * - continuous measurements
 * - single shot measurements
 * - polling for new measurement results
 * - data ready interrupts
 * - threshold interrupts
 * - start and stop measurements
 * - measurement error status
 * - configuration of measurements during run-time
 * - power down and power up
 *
 * The driver can be used when memory requirements are not an issue.
 * The standard retrieval method for new data is to use data-ready or threshold
 * interrupts.
 *
 * \anchor vl6180x_basic
 * ### <a name="vl6180x_basic"> Basic driver vl6180x_basic </a>
 *
 * This is a smaller driver variant that only provides the most basic functions
 * for ranging and ambient light sensing such as:
 *
 * - continuous measurements
 * - polling for new measurement results
 *
 * This driver should be used when memory requirements are an issue.
 * The standard retrieval method for new data is polling.
 *
 * ### Driver Comparison Sheet
 *
 * Feature / Property                      | vl6180x_basic | vl6180x
 * :---------------------------------------|:-------------:|:--------:
 * Range measurements [mm]                 | X             | X
 * Ambient light measurements [Lux, raw]   | X             | X
 * SAUL capability                         | X             | X
 * Continuous measurements                 | X             | X
 * Single shot measurements                |               | X
 * Start and stop measurements             |               | X
 * Polling for new measurement results     | X             | X
 * Data-ready interrupts                   |               | X
 * Threshold interrupts                    |               | X
 * Measurement error status                |               | X
 * Configuration during run-time           |               | X
 * Power down and power up                 |               | X
 * Size on reference platform              | 1.7 kByte     | 3.0 kByte
 *
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef VL6180X_H
#define VL6180X_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

#define VL6180X_I2C_ADDR    (0x29)  /**< VNCL6180 default I2C slave address */

/**
 * @brief   Error codes
 */
typedef enum {
    VL6180X_OK              = 0,    /**< Success */
    VL6180X_ERROR_I2C       = 1,    /**< I2C communication error */
    VL6180X_ERROR_WRONG_ID  = 2,    /**< Wrong id read */
    VL6180X_ERROR_NO_PIN    = 3,    /**< Pin not defined */
    VL6180X_ERROR_NO_DATA   = 4,    /**< No data available */
    VL6180X_ERROR_RNG       = 5,    /**< Ranging error */
    VL6180X_ERROR_ALS       = 6,    /**< Ambient light sensing (ALS) error */
} vl6180x_error_t;

/**
 * @brief   Analogue gain of ALS measurements
 */
typedef enum {
    VL6180X_ALS_GAIN_20 = 0,    /**<   20 x gain */
    VL6180X_ALS_GAIN_10,        /**<   10 x gain */
    VL6180X_ALS_GAIN_5,         /**<    5 x gain */
    VL6180X_ALS_GAIN_2_5,       /**<  2.5 x gain */
    VL6180X_ALS_GAIN_1_67,      /**< 1.67 x gain */
    VL6180X_ALS_GAIN_1_25,      /**< 1.25 x gain */
    VL6180X_ALS_GAIN_1,         /**<    1 x gain (default) */
    VL6180X_ALS_GAIN_40,        /**<   40 x gain */
} vl6180x_als_gain_t;

#if !MODULE_VL6180X_BASIC
/**
 * @brief   Range measurement status
 */
typedef enum {
    VL6180X_RNG_OK = 0,                     /**< No error */
    VL6180X_RNG_VCSEL_CONT_TEST = 1,        /**< VCSEL continuity Test */
    VL6180X_RNG_VCSEL_WD_TEST = 2,          /**< VCSEL watchdog test */
    VL6180X_RNG_VCSEL_WD = 3,               /**< VCSEL watchdog */
    VL6180X_RNG_PLL1_LOCK = 4,              /**< PLL1 lock */
    VL6180X_RNG_PLL2_LOCK = 5,              /**< PLL2 lock */
    VL6180X_RNG_EARLY_CONV_EST = 6,         /**< Early convergence estimate */
    VL6180X_RNG_MAX_CONV = 7,               /**< Maximum convergence time reached */
    VL6180X_RNG_NO_TARGET = 8,              /**< No target, ignore */
    VL6180X_RNG_MAX_SNR = 11,               /**< Maximum SNR reached */
    VL6180X_RNG_RAW_ALGO_UNDERFLOW = 12,    /**< Raw ranging algorithm underflow */
    VL6180X_RNG_RAW_ALGO_OVERFLOW = 13,     /**< Raw ranging algorithn overflow */
    VL6180X_RNG_ALGO_UNDERFLOW = 14,        /**< Ranging algorithm underflow */
    VL6180X_RNG_ALGO_OVERFLOW = 15,         /**< Ranging algorithm overflow */
} vl6180x_rng_status_t;

/**
 * @brief   Ambient light sensing (ALS) status
 */
typedef enum {
    VL6180X_ALS_OK = 0,                    /**< No error */
    VL6180X_ALS_OVERFLOW = 1,              /**< ALS measurement overflow */
    VL6180X_ALS_UNDERFLOW = 2,             /**< ALS measurement underflow */
} vl6180x_als_status_t;

#endif /* !MODULE_VL6180X_BASIC */

/**
 * @brief   Interrupt modes
 *
 * The interrupt mode defines when an interrupt is triggered for ranging and
 * ambient light sensing (ALS). Interrupt can be triggered either
 *
 * - in each measurement cycle when new data become available
 *   (data-ready interrupts) or
 * - only when any threshold condition is fulfilled (threshold interrupts).
 *
 * For threshold interrupts, upper and lower thresholds have to be defined,
 * with the upper and lower thresholds defining a threshold window, see
 * also #vl6180x_int_config_t.
 *
 * @note Interrupts are NOT AVAILABLE when module \ref vl6180x_basic is used.
 */
typedef enum {
    VL6180X_INT_DISABLED    = 0, /**< Interrupt is disabled */
    VL6180X_INT_LEVEL_LOW   = 1, /**< Interrupt is triggered when the value
                                      falls below the lower threshold */
    VL6180X_INT_LEVEL_HIGH  = 2, /**< Interrupt is triggered when the value
                                      exceeds the upper threshold */
    VL6180X_INT_LEVEL_OUT   = 3, /**< Interrupt is triggered when the value
                                      exceeds the upper threshold or falls
                                      below the lower threshold (the value
                                      leaves the threshold window) */
    VL6180X_DATA_RDY        = 4, /**< Interrupt is triggered when new data
                                      are available */
} vl6180x_int_mode_t;

#if !MODULE_VL6180X_BASIC
/**
 * @brief   Interrupt configuration
 *
 * Interrupts can be configured separately for ranging and ambient light
 * sensing (ALS). The interrupt mode defines when an interrupt is triggered,
 * either
 *
 * - in each measurement cycle, when new data becomes available
 *   (data ready interrupts), or
 * - only when a threshold condition is met (threshold interrupts).
 *
 * Threshold configurations are used for threshold interrupts only.
 *
 * @note Interrupts are NOT AVAILABLE when module \ref vl6180x_basic is used.
 */
typedef struct {
    vl6180x_int_mode_t rng_int_mode; /**< interrupt mode for ranging */

    uint8_t rng_thresh_high;         /**< upper threshold for ranging in mm */
    uint8_t rng_thresh_low;          /**< lower threshold for ranging in mm */

    vl6180x_int_mode_t als_int_mode; /**< interrupt mode for ranging */

    uint16_t als_thresh_high;        /**< upper threshold for ALS in counts */
    uint16_t als_thresh_low;         /**< lower threshold for ALS in counts */
} vl6180x_int_config_t;

/**
 * @brief   Interrupt source
 *
 * Interrupt source contains the type of an interrupt  triggered by ranging
 * and/or ambient light sensing.
 */
typedef struct {
    vl6180x_int_mode_t rng_int; /**< type of interrupt triggered by ranging */
    vl6180x_int_mode_t als_int; /**< type of interrupt triggered by ALS */
} vl6180x_int_source_t;

#endif /* !MODULE_VL6180X_BASIC */

/**
 * @brief   VL6180X device initialization parameters
 */
typedef struct {

    /**
     * @name Device parameters
     * @{
     */
    unsigned i2c_dev;       /**< I2C device, default I2C_DEV(0) */
    uint8_t  i2c_addr;      /**< I2C slave address */

    #if !MODULE_VL6180X_BASIC
    gpio_t pin_int;         /**< Interrupt pin which is open drain, low active
                                 (default #GPIO_UNDEF). If #GPIO_UNDEF,
                                 the interrupt pin is not used. */
    gpio_t pin_shutdown;    /**< Shutdown pin which is low active
                                 (default #GPIO_UNDEF). If #GPIO_UNDEF,
                                 the shutdown pin is not used */
    #endif

    /** @} */

    /**
     * @name Range measurement parameters
     * @{
     */
    uint8_t rng_period;     /**< Range inter-measurement period in continuous
                                 measurement mode in steps of 10 ms
                                 (default 10 = 100 ms). It controls also
                                 the measurement mode. If 0, single shot
                                 measurement mode is used, otherwise the
                                 continuous measurement mode is activated. */
    uint8_t rng_max_time;   /**< Maximum convergence time in ms [1...63] given
                                 the sensor to perform range measurements
                                 (default 50 = 50 ms) */
    /** @} */

    /**
     * @name ALS measurement parameters
     * @{
     */
    uint8_t als_period;     /**< ALS inter-measurement period in continuous
                                 measurement mode in steps of 10 ms
                                 (default 50 = 500 ms). It controls also
                                 the measurement mode. If 0, single shot
                                 measurement mode is used, otherwise the
                                 continuous measurement mode is activated. */
    uint16_t als_int_time;  /**< ALS integration time in ms [1...512]
                                 (default 100 = 100 ms, **recommended**) */

    vl6180x_als_gain_t als_gain; /**< ALS analogue gain for light channel
                                     (default VL6180X_ALS_GAIN_1_0) */
    /** @} */

} vl6180x_params_t;

/**
 * @brief   VL6180X sensor device data structure type
 */
typedef struct {

    vl6180x_params_t params;     /**< Device initialization parameters */
    bool int_init;               /**< Interrupt pin is already initialized */

    #if !MODULE_VL6180X_BASIC
    vl6180x_rng_status_t rng_status;    /**< status of last range measurement */
    vl6180x_als_status_t als_status;    /**< status of last ALS measurement */
    #endif /* !MODULE_VL6180X_BASIC */

} vl6180x_t;

/**
 * @brief   Initialize the VL6180X sensor device
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor to be initialized
 * @param[in]   params  Configuration parameters, see #vl6180x_params_t
 *
 * @retval  VL6180X_OK      on success
 * @retval  VL6180X_ERROR_* a negative error code on error, see
 *                          #vl6180x_error_t
 */
int vl6180x_init(vl6180x_t *dev, const vl6180x_params_t *params);

/**
 * @brief   Range data-ready status function
 *
 * The function can be used for polling to know when new ranging data are ready.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK             new ranging data are ready
 * @retval  VL6180X_ERROR_NO_DATA  no new ranging data available
 * @retval  VL6180X_ERROR_*        a negative error code on any other error, see
 *                                 #vl6180x_error_t
 */
int vl6180x_rng_data_ready(const vl6180x_t *dev);

/**
 * @brief   ALS data-ready status function
 *
 * The function can be used for polling to know when new ALS data are ready.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK             new ALS data are ready
 * @retval  VL6180X_ERROR_NO_DATA  no new ALS data available
 * @retval  VL6180X_ERROR_*        a negative error code on any other error,
 *                                 see #vl6180x_error_t
 */
int vl6180x_als_data_ready(const vl6180x_t *dev);

/**
 * @brief   Read one ranging data sample in mm
 *
 * This function returns the ranging data in millimeters. If ranging value
 * is invalid because of a measurement error, #VL6180X_ERROR_RNG is returned.
 * The #vl6180x_rng_status function can then be used to get an error code of
 * type #vl6180x_rng_status_t.
 *
 * @note The function clears the interrupt if ambient light sensing interrupts
 * are used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @param[out]  mm      Ranging data in mm [0...100]
 *
 * @retval  VL6180X_OK         on success
 * @retval  VL6180X_ERROR_RNG  on error during range measurement
 * @retval  VL6180X_ERROR_*    a negative error code on any other error
 *                             see #vl6180x_error_t
 */
int vl6180x_rng_read(vl6180x_t *dev, uint8_t *mm);

/**
 * @brief   Read one ambient light sensing (ALS) data sample
 *
 * This function returns one ALS data sample as raw count value and, if
 * required, as illuminance in Lux. The range of the count value depends
 * on used ALS analog gain defined by vl6180x_params_t::als_gain and the
 * integration time defined by vl6180x_params_t::als_int_time.
 *
 * The count value is returned in parameter \p raw while the illuminance
 * is returned in parameter \p lux.  For either \p raw or \p lux also `NULL`
 * can be passed, if only one value is of interest.
 *
 * If ALS value is invalid because of a measurement error, #VL6180X_ERROR_ALS
 * is returned. The #vl6180x_als_status function can then be used to get an
 * error code of type #vl6180x_als_status_t.
 *
 * @note The function clears the interrupt if ambient light sensing interrupts
 * are used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @param[out]  raw     Ambient light raw data as count value
 * @param[out]  lux     Ambient light in Lux
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_als_read(vl6180x_t *dev, uint16_t *raw, uint16_t *lux);

#if !MODULE_VL6180X_BASIC
/**
 * @brief       Get status of last range measurement
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @retval      status of type vl6180x_rng_status_t
 */
vl6180x_rng_status_t vl6180x_rng_status(vl6180x_t *dev);

/**
 * @brief   Start range measurements
 *
 * This function can be used
 *
 * - to start a single shot range measurement if vl6180x_params_t::rng_period
 *   is 0 or
 * - to restart the continuous range measurement that was previously stopped
 *   with #vl6180x_rng_stop if vl6180x_params_t::rngals_period is not 0.
 *
 * Continuous range measurements are automatically started during
 * sensor initialization when vl6180x_params_t::rng_period is not 0.
 * Therefore, it is not necessary to call this function when continuous
 * range measurements are used.
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_rng_start (vl6180x_t *dev);

/**
 * @brief   Stop range measurements
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_rng_stop (vl6180x_t *dev);

/**
 * @brief   Reconfigure range measurements during run-time
 *
 * This function can be used to overwrite the default configuration of range
 * measurements defined by #vl6180x_params_t during run-time.
 *
 * For this purpose, the running range measurement is stopped and restarted
 * after the reconfiguration if continuous mode is used (\p period is not 0).
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev         Device descriptor of VL6180X sensor
 * @param[in]   period      Range inter-measurement period in continuous
 *                          measurement mode in steps of 10 ms (It controls also
 *                          the measurement mode. If 0, single shot measurement
 *                          mode is used, otherwise the continuous measurement
 *                          mode is activated.
 * @param[in]   max_time    Maximum convergence timing in ms [1...63] given
 *                          to the sensor to perform range measurements
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_rng_config(vl6180x_t *dev, uint8_t period, uint8_t max_time);

/**
 * @brief   Start ambient light sensing (ALS)
 *
 * This function can be used
 *
 * - to start a single shot ambient light sensing (ALS) if
 *   vl6180x_params_t::als_period is 0 or
 * - to restart the continuous ambient light sensing (ALS) that was
 *   previously stopped with #vl6180x_als_stop if vl6180x_params_t::als_period
 *   is not 0.
 *
 * Continuous ambient light sensing (ALS) is automatically started during
 * sensor initialization when vl6180x_params_t::als_period is not 0.
 * Therefore, it is not necessary to call this function when continuous
 * ambient light detection is used.
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_als_start (vl6180x_t *dev);

/**
 * @brief   Stop ambient light sensing (ALS)
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_als_stop (vl6180x_t *dev);

/**
 * @brief   Reconfigure ambient light sensing (ALS) during run-time
 *
 * This function can be used to overwrite the default configuration of ambient
 * light sensing defined by #vl6180x_params_t during run-time.
 *
 * For this purpose, the running ambient light sensing (ALS) is stopped and
 * restarted after the reconfiguration if continuous mode is used
 * (\p period is not 0).
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev         Device descriptor of VL6180X sensor
 * @param[in]   period      ALS inter-measurement period in continuous
 *                          measurement mode in steps of 10 ms. It controls also
 *                          the measurement mode. If 0, single shot measurement
 *                          mode is used, otherwise the continuous measurement
 *                          mode is activated.
 * @param[in]   int_time    ALS integration time in ms [0...511]
 * @param[in]   gain        ALS analogue gain for light channel
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_als_config(vl6180x_t *dev, uint8_t period, uint8_t int_time,
                                       vl6180x_als_gain_t gain);
/**
 * @brief       Get status of last ALS measurement
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @retval      status of type vl6180x_als_status_t
 */
vl6180x_als_status_t vl6180x_als_status(vl6180x_t *dev);

/**
 * @brief   Power down the sensor
 *
 * This function requires that a GPIO connected to sensor's GPIO0/CE pin is
 * defined by parameter vl6180x_params_t::pin_shutdown.
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_power_down(const vl6180x_t *dev);

/**
 * @brief   Power down the sensor
 *
 * This function requires that a GPIO connected to sensor's GPIO0/CE pin is
 * defined by parameter vl6180x_params_t::pin_shutdown.
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_power_up(vl6180x_t *dev);

/**
 * @brief   Configure interrupts
 *
 * The function
 *
 * - configures the interrupts for ranging as well as ambient light sensing,
 * - initializes the GPIO defined by configuration parameter
 *   vl6180x_params_t::pin_int, and
 * - attaches the ISR specified by the \p isr parameter to the interrupt.
 *
 * Parameter \p cfg can be `NULL`. In that case, data-ready interrupts are
 * configured for ranging as well as ambient light sensing (ALS).
 *
 * @pre
 * - Configuration parameter vl6180x_params_t::pin_int has to be defined
 * - Parameter \p isr must not be `NULL`
 *
 * @note
 * - Since the ISR is executed in the interrupt context, it must not be
 *   blocking or time consuming. In addition, it must not access the sensor
 *   directly via I2C. It should only indicate to a waiting thread that an
 *   interrupt has occurred, which is then handled in the thread context.
 * - To disable intertupts, set vl6180x_int_config_t::rng_int_mode and
 *   vl6180x_int_config_t::als_int_mode to #VL6180X_INT_DISABLED. In that case
 *   parameter \p isr can be `NULL`.
 *
 * - This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @param[in]   cfg     Interrupt configuration, see #vl6180x_int_config_t,
 *                      if `NULL`, data-ready interrupts are configured
 * @param[in]   isr     ISR called for all types of interrupts, must not be `NULL`
 * @param[in]   isr_arg ISR argument, can be `NULL`
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_int_config(vl6180x_t *dev, vl6180x_int_config_t *cfg,
                                       void (*isr)(void *),
                                       void *isr_arg);

/**
 * @brief   Get interrupt source
 *
 * The function can be used when an interrupt occurs to determine what type
 * of interrupt (#vl6180x_int_mode_t) is triggered by ranging and/or ambient
 * light measurement (ALS).
 *
 * @note This function is NOT AVAILABLE when module \ref vl6180x_basic is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @param[out]  src     Interrupt source, see #vl6180x_int_source_t
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_int_source(vl6180x_t *dev, vl6180x_int_source_t *src);

#endif /* !MODULE_VL6180X_BASIC */

#ifdef __cplusplus
}
#endif

#endif /* VL6180X_H */
/** @} */
