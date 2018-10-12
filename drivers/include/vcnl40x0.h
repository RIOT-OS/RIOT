/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_vcnl40x0 VCNL40X0 proximity and ambient light sensors
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for Vishay VCNL40X0 proximity and ambient light sensors
 *
 * The driver can be used with Vishay VCNL4010, VCNL4020, and VCNL3020.
 *
 * The driver is divided into two parts
 *
 * - a basic driver ```vcnl40x0```, and
 * - a fully functional driver ```vcnl40x0_full```.
 *
 * The base driver ```vcnl40x0``` only supports a basic set of functions and
 * therefore has a small size. The procedure for retrieving new data is simply
 * polling. Ambient light and distance measurements are supported.
 *
 * The fully functional driver `` `vcnl40x0_full``` supports all the features
 * supported by the base driver, as well as all other sensor features,
 * including interrupts and their configuration. The approach to retrieving
 * data is to use data-ready interrupts. In addition, threshold interrupts
 * can be used and configured. 
 *
 * For using interrupts, the according GPIO to which the sensor's
 * **INT** output pin is connected has to be defined by the application in
 * configuration parameters.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef VCNL40X0_H
#define VCNL40X0_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

/**< VCNL40X0 I2C addresses */
#define VCNL40X0_I2C_ADDRESS    (0x13)

/** VCNL40X0 Produc Revision ID */
#define VCNL40X0_ID             (0x21)

/** Definition of error codes */
typedef enum {
    VCNL40X0_OK,                /**< success */
    VCNL40X0_ERROR_I2C,         /**< I2C communication error */
    VCNL40X0_ERROR_WRONG_ID,    /**< wrong id read */
    VCNL40X0_ERROR_NO_DATA,     /**< no data are available */
    VCNL40X0_ERROR_RAW_DATA,    /**< reading raw data failed */
    VCNL40X0_ERROR_INV_RATE,    /**< invalid measurement rate */
    VCNL40X0_ERROR_INV_MODE,    /**< invalid measurement mode */
} vcnl40x0_error_codes_t;

/**
 * @brief   Ambient light sensor (ALS) measurement rate
 *
 * Defines the measurement rate for Ambient light sensor (ALS) measurements.
 *
 * @note If a different ambient light measurement rate than
 * #VCNL40X0_ALS_RATE_NONE is used, periodic selftimed measurements
 * are activated. In this case, on-demand measurements are not possible
 * for proximity and for ambient light.
 */
typedef enum {
    VCNL40X0_ALS_RATE_1 = 0,    /**<  1 sample per second */
    VCNL40X0_ALS_RATE_2,        /**<  2 samples per second (default) */
    VCNL40X0_ALS_RATE_3,        /**<  3 samples per second */
    VCNL40X0_ALS_RATE_4,        /**<  4 samples per second */
    VCNL40X0_ALS_RATE_5,        /**<  5 samples per second */
    VCNL40X0_ALS_RATE_6,        /**<  6 samples per second */
    VCNL40X0_ALS_RATE_8,        /**<  8 samples per second */
    VCNL40X0_ALS_RATE_10,       /**< 10 samples per second */
    VCNL40X0_ALS_RATE_NONE      /**< not enabled, on-demand measurements only */
} vcnl40x0_als_rate_t;

/**
 * @brief   Averaging function for ambient light measurements
 *
 * Defines the number of conversions during one measurement cycle.
 */
typedef enum {
    VCNL40X0_ALS_AVG_NONE = 0,  /**< no averaging */
    VCNL40X0_ALS_AVG_2,         /**< averaging uses   2 conversions */
    VCNL40X0_ALS_AVG_4,         /**< averaging uses   4 conversions */
    VCNL40X0_ALS_AVG_8,         /**< averaging uses   8 conversions */
    VCNL40X0_ALS_AVG_16,        /**< averaging uses  16 conversions */
    VCNL40X0_ALS_AVG_32,        /**< averaging uses  32 conversions (default) */
    VCNL40X0_ALS_AVG_64,        /**< averaging uses  64 conversions */
    VCNL40X0_ALS_AVG_128,       /**< averaging uses 128 conversions */
} vcnl40x0_als_avg_t;

/**
 * @brief   Proximity (PRX) measurement rate
 *
 * Defines the measurement rate for proximity measurements.
 *
 * @note If a different proximity measurement rate than
 * #VCNL40X0_PRX_RATE_NONE is used, periodic selftimed measurements
 * are activated. In this case, on-demand measurements are not possible
 * for proximity and for ambient light.
 */
typedef enum {
    VCNL40X0_PRX_RATE_1_95 = 0, /**< 1.95    measurements per second (default) */
    VCNL40X0_PRX_RATE_3_90625,  /**< 3.90625 measurements per second */
    VCNL40X0_PRX_RATE_7_8125,   /**< 7.8125  measurements per second */
    VCNL40X0_PRX_RATE_16_625,   /**< 16.625  measurements per second */
    VCNL40X0_PRX_RATE_31_25,    /**< 31.25   measurements per second */
    VCNL40X0_PRX_RATE_62_5,     /**< 62.5    measurements per second */
    VCNL40X0_PRX_RATE_125,      /**< 125     measurements per second */
    VCNL40X0_PRX_RATE_250,      /**< 250     measurements per second */
    VCNL40X0_PRX_RATE_NONE      /**< not enabled, on-demand measurements only */
} vcnl40x0_prx_rate_t;

/**
 * @brief   Proximity (PRX) IR test signal frequency
 */
typedef enum {
    VCNL40X0_PRX_FREQ_390_K_625 = 0, /**< 390.625 kHz IR test signal */
    VCNL40X0_PRX_FREQ_780_K_25,      /**< 780.25 kHz  IR test signal */
    VCNL40X0_PRX_FREQ_1_M_5625,      /**< 1.5625 MHz  IR test signal */
    VCNL40X0_PRX_FREQ_3_M_125,       /**< 3.125  MHz  IR test signal */
} vcnl40x0_prx_freq_t;

/**
 * @brief   VCNL40X0 device initialization parameters
 */
typedef struct {

    unsigned  dev;      /**< I2C device (default I2C_DEV(0)) */

    vcnl40x0_als_rate_t als_rate;  /**< Ambient light measurement (ALS) rate */
    vcnl40x0_als_avg_t  als_avg;   /**< Averaging function for ALS */

    vcnl40x0_prx_rate_t prx_rate;  /**< Proximity measurementrate */
    uint8_t prx_ir_led;            /**< IR LED current for proximity measurements
                                        range is 0 ... 200 mA in steps of 10 mA */

    gpio_t  int_pin;    /**< interrupt pin: #GPIO_UNDEF if not used */

} vcnl40x0_params_t;

#if MODULE_VCNL40X0_FULL || DOXYGEN

/**
 * @brief   Interrupt types
 */
typedef enum {
    VCNL40X0_INT_ALS_DRDY,  /**< ambient light data ready interrupt */
    VCNL40X0_INT_PRX_DRDY,  /**< proximity data ready interrupt */
    VCNL40X0_INT_THS_LO,    /**< low threshold exceeded interrupt */
    VCNL40X0_INT_THS_HI,    /**< high threshold exceeded interrupt */
} vcnl40x0_int_t;

/**
 * @brief   Threshold interrupt types
 */
typedef enum {
    VCNL40X0_INT_THS_PRX = 0,   /**< threshold applied to proximity measurements */
    VCNL40X0_INT_THS_ALS,       /**< threshold applied to ambient light measurements */
} vcnl40x0_int_ths_t;

/**
 * @brief   Threshold interrupt counts, the number of consecutive measurements
 *          needed above/below the threshold
 */
typedef enum {
    VCNL40X0_INT_THS_1 = 0,     /**<   1 count (default) */
    VCNL40X0_INT_THS_2,         /**<   2 counts */
    VCNL40X0_INT_THS_4,         /**<   4 counts */
    VCNL40X0_INT_THS_8,         /**<   8 counts */
    VCNL40X0_INT_THS_16,        /**<  16 counts */
    VCNL40X0_INT_THS_32,        /**<  32 counts */
    VCNL40X0_INT_THS_64,        /**<  64 counts */
    VCNL40X0_INT_THS_128,       /**< 128 counts */
} vcnl40x0_int_ths_cnt_t;

/**
 * @brief   Interrupt service routine function prototype
 */
typedef void (*vcnl40x0_isr_t)(void *arg);

/**
 * @brief   Interrupt context
 */
typedef struct {
    vcnl40x0_isr_t isr;     /**< interrupt service routine */
    void* arg;              /**< interrupt service routine argument */
} vcnl40x0_int_ctx_t;

#endif /* MODULE_VCNL40X0_FULL */

/**
 * @brief   VCNL40X0 sensor device data structure type
 */
typedef struct {

    vcnl40x0_params_t params;          /**< device initialization parameters */

#if MODULE_VCNL40X0_FULL || DOXYGEN
    vcnl40x0_int_ctx_t isr_als_drdy;   /**< ambient light data ready ISR */
    vcnl40x0_int_ctx_t isr_prx_drdy;   /**< proximity data ready ISR */
    vcnl40x0_int_ctx_t isr_ths_lo;     /**< low threshold exceeded ISR */
    vcnl40x0_int_ctx_t isr_ths_hi;     /**< high threshold exceeded ISR */

    bool gpio_init;                    /**< GPIO already initialized */
#endif /* MODULE_VCNL40X0_FULL */

} vcnl40x0_t;

/**
 * @brief	Initialize the VCNL40X0 sensor device
 *
 * This function resets the sensor and initializes the sensor according to
 * given intialization parameters. All registers are reset to default values.
 *
 * @param[in]   dev     device descriptor of VCNL40X0 sensor to be initialized
 * @param[in]   params  configuration parameters, see #vcnl40x0_params_t
 *
 * @retval  VCNL40X0_OK      on success
 * @retval  VCNL40X0_ERROR_* a negative error code on error,
 *                           see #vcnl40x0_error_codes_t
 */
int vcnl40x0_init(vcnl40x0_t *dev, const vcnl40x0_params_t *params);

/**
 * @brief   Read one sample of ambient light measurement (ALS)
 *          in quarters of a Lux
 *
 * Ambient light data are given in quarters of a Lux in the range
 * 0.25 lx to 16,383.00 lx. The resolution is 0.25 lx
 *
 * If an ALS rate different from #VCNL40X0_ALS_RATE_NONE is used as
 * parameter #vcnl40x0_params_t::als_rate, the last available data sample
 * is returned.
 *
 * If #VCNL40X0_ALS_RATE_NONE is used as parameter
 * #vcnl40x0_params_t::als_rate, a single on-demand measurement is started
 * in continuous conversion mode. In this case, the function waits for
 * available data. Depending on #vcnl40x0_params_t::als_avg this can take
 * from 23 ms when averaging is disabled (#VCNL40X0_ALS_AVG_NONE) up to
 * 90 ms for averaging over 128 conversions (#VCNL40X0_ALS_AVG_128).
 *
 * @param[in]   dev     device descriptor of VCNL40X0 sensor to be initialized
 * @param[out]  als     ambient light measurement data
 *
 * @retval  VCNL40X0_OK      on success
 * @retval  VCNL40X0_ERROR_* a negative error code on error,
 *                           see #vcnl40x0_error_codes_t
 */
int vcnl40x0_read_als(const vcnl40x0_t *dev, uint16_t *als);

/**
 * @brief   Read one data sample of proximity measurement (PRX)
 *
 * Proximity data samples are given as a 16-bit values. The range of these
 * values depends on the IR LED current used for measurement.
 *
 * @note A number of disturbing effects such as DC noise, sensor coverage,
 * or surrounding objects cause an offset in the measured proximity values.
 * The application should remove this offset, for example, by finding the
 * minimum value ever measured and subtracting it from the current reading.
 * The minimum value is then assumed to be 0 (no proximity).
 *
 * @param[in]   dev     device descriptor of VCNL40X0 sensor to be initialized
 * @param[out]  prx     proximity measurement data
 *
 * @retval  VCNL40X0_OK      on success
 * @retval  VCNL40X0_ERROR_* a negative error code on error,
 *                           see #vcnl40x0_error_codes_t
 */
int vcnl40x0_read_prox(vcnl40x0_t *dev, uint16_t *prx);

/**
 * @brief   Ambient light measurement (ALS) data-ready status function
 *
 * The function checks the ASL data ready flag and returns. It can be
 * used for polling new ambient light measurement data.
 *
 * @param[in]   dev     device descriptor of VCNL40X0 sensor
 *
 * @retval  VCNL40X0_OK             new data available
 * @retval  VCNL40X0_ERROR_NO_DATA  no new data available
 * @retval  VCNL40X0_ERROR_*        negative error code,
 *                                  see #vcnl40x0_error_codes_t
 */
int vcnl40x0_als_data_ready (const vcnl40x0_t *dev);

/**
 * @brief   Proximity data-ready status function
 *
 * The function checks the proximity data ready flag and returns. It can be
 * used for polling new proximity data.
 *
 * @param[in]   dev     device descriptor of VCNL40X0 sensor
 *
 * @retval  VCNL40X0_OK             new data available
 * @retval  VCNL40X0_ERROR_NO_DATA  no new data available
 * @retval  VCNL40X0_ERROR_*        negative error code,
 *                                  see #vcnl40x0_error_codes_t
 */
int vcnl40x0_prx_data_ready (const vcnl40x0_t *dev);

#if MODULE_VCNL40X0_FULL || DOXYGEN

/**
 * @brief   Enable an interrupt
 *
 * If required, this function initializes the GPIO defined by
 * #vcnl40x0_params_t::int_pin and activates the interrupt of type
 * #vcnl40x0_int_t specified by the @p intr parameter. The interrupt
 * service routine specified by the @p isr parameter is attached
 * to this interrupt.
 *
 * @note The interrupt service routine specified by parameter @p isr is called
 * in the interrupt context and shouldn't block or do anything time consuming.
 *
 * @note  Unlike other sensor drivers, the GPIO used as the interrupt signal
 * pin is initialized by the driver as soon as an interrupt is activated.
 * The driver intercepts all interrupts and calls the user function registered
 * by the parameter @p isr for the interrupt.
 *
 * @param[in]   dev     device descriptor of VCNL40X0 sensor
 * @param[in]   intr    interrupt to be enabled, see #vcnl40x0_int_t
 * @param[in]   isr     interrupt service routine called for the interrupt
 * @param[in]   isr_arg interrupt service routine argument
 *
 * @retval  VCNL40X0_OK         on success
 * @retval  VCNL40X0_ERROR_*    negative error code on error,
 *                              see #vcnl40x0_error_codes_t
 */
int vcnl40x0_enable_int(vcnl40x0_t *dev, vcnl40x0_int_t intr,
                        vcnl40x0_isr_t isr, void *isr_arg);

/**
 * @brief   Disable an interrupt
 *
 * @param[in]   dev     device descriptor of VCNL40X0 sensor
 * @param[in]   intr    interrupt to be disabled, see #vcnl40x0_int_t
 *
 * @retval  VCNL40X0_OK         on success
 * @retval  VCNL40X0_ERROR_*    negative error code on error,
 *                              see #vcnl40x0_error_codes_t
 */
int vcnl40x0_disable_int(vcnl40x0_t *dev, vcnl40x0_int_t intr);

/**
 * @brief   Config threshold interrupt
 *
 * This function configures the thresholds that are used for the generation
 * of threshold interrupts (#VCNL40X0_INT_THS_LO or #VCNL40X0_INT_THS_HI).
 *
 * @param[in]   dev     device descriptor of VCNL40X0 sensor
 * @param[in]   sel     selects to which measurements thresholds are applied,
 *                      see #vcnl40x0_int_ths_t
 * @param[in]   low     low threshold
 * @param[in]   high    high threshold
 * @param[in]   count   number of consecutive measurements needed above/below
 *                      the threshold
 */
int vcnl40x0_set_int_thresh(vcnl40x0_t *dev, vcnl40x0_int_ths_t sel,
                            uint16_t low, uint16_t high,
                            vcnl40x0_int_ths_t count);

/**
 * @brief   Config the modulator timings for proximity measurements
 *
 * When proximity measurements are enabled, a number of parameters define
 * the timing of the IR LED test signal. These are the frequency (default
 * #VCNL40X0_PRX_FREQ_390_K_625), the modulation delay time (default 0),
 * and the modulation dead time (default 1). Normally, the values are
 * determined by the sensor so that it is normally not needed to call
 * this function.
 *
 * @param[in]   dev      device descriptor of VCNL40X0 sensor
 * @param[in]   freq     IR LED test signal frequency, see #vcnl40x0_prx_freq_t
 * @param[in]   t_delay  time between the IR LED signal and the IR input signal
 *                       evaluation [0...7]
 * @param[in]   t_dead   dead time in evaluation of IR signal at the IR signal
 *                       slopes [0...7]
 *
 * @retval  VCNL40X0_OK         on success
 * @retval  VCNL40X0_ERROR_*    negative error code on error,
 *                              see #vcnl40x0_error_codes_t
 */
int vcnl40x0_set_prx_mod (vcnl40x0_t *dev, vcnl40x0_prx_freq_t freq,
                          uint8_t t_delay, uint8_t t_dead);

#endif /* MODULE_VCNL40X0_FULL */

#ifdef __cplusplus
}
#endif

#endif /* VCNL40X0_H */
/** @} */
