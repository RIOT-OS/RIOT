/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_vl6180x VL6180X Ranging and Ambient Light Sensing (ALS) module
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the ST VL6180X Ranging and Ambient Light Sensing (ALS) module
 *
 * # Overview {#vl6180x_overview}
 *
 * ## About the sensor {#vl6180x_about}
 *
 * The ST VL6180X is a low-power **proximity** and **ambient light** sensor
 * with an I2C interface that uses time-to-flight technology for distance
 * measurements. It can be used for ranging and/or ambient light sensing (ALS).
 * Measurements can be automatically performed at user-defined intervals.
 *
 * To minimize host operations, interrupts can be used either when
 * new sensor data are ready to be read or when sensor values exceed
 * configured thresholds.
 *
 * ## Supported Features {#vl6180x_supported}
 *
 * The driver supports different levels of functionality, which can be
 * enabled by using pseudomodules according to the requirements of the
 * application. This ensures that the driver only uses as much ROM/RAM
 * as really needed.
 *
 * As basic functionality the driver supports:
 *
 * - Ranging and ambient light sensing (ALS) in single-shot or
 *   continuous mode with polling for new sensor data
 * - Fixed configuration of the sensor by a default parameter set of
 *   type #vl6180x_params_t as defined in the file `vl6180x_params.h
 * - SAUL sensor interface
 *
 * The following pseudomodules are used to enable additional functionalities:
 * <center>
 * Pseudomodule        | Functionality
 * :-------------------|:-------------------------------------------------------
 * `vl6180x_irq`       | Data ready and event interrupt handling
 * `vl6180x_suhtdown`  | Power-down and power-up functions
 * `vl6180x_config`    | Functions for changing configurations at runtime
 * </center>
 * <br>
 *
 * The following table shows the mapping of which modules have to be used
 * to enable which functions of the VL6180X.
 *
 * <center>
 * Feature                                                       | Module
 * :------------------------------------------------------------ |:-------------
 * Ranging in single-shot or continuous mode                     | `vl6180x_rng`
 * Ambient light sensing (ALS) in single-shot or continuous mode | `vl6180x_als`
 * Data ready and event interrupt handling                       | `vl6180x_irq`
 * Power-down and power-up functions                             | `vl6180x_shutdown`
 * Configuration of the sensor at runtime                        | `vl6180x_config`
 * </center><br>
 *
 * @note
 * - If the handling of interrupts for data ready and event interrupts
 *   is enabled by module `vl6180x_irq`, the GPIO pin for the interrupt
 *   signal (sensor pin GPIO1) must be defined by the configuration parameter
 *   vl6180x_params_t::int_pin. The default configuration of this GPIO pin
 *   is defined by #VL6180X_PARAM_INT_PIN that can be overridden by the
 *   board definition. The interrupt signal is LOW active.
 * - If power-down and power-up functions are enabled by module
 *   `vl6180x_shutdown`, the GPIO pin for the shutdown signal (sensor pin
 *   GPIO0/CE) must be defined by the configuration parameter
 *   vl6180x_params_t::shutdown_pin. The default configuration of this GPIO pin
 *   is defined by #VL6180X_PARAM_SHUTDOWN_PIN that can be overridden by the
 *   board definition. The shutdown signal is LOW active.
 *
 * # Using the driver {#vl6180x_using_driver}
 *
 * ## Initialization {#vl6180x_initialization}
 *
 * The **easiest way to use the driver** is simply to initialize the sensor
 * with function #vl6180x_init using the default configuration parameter set
 * #vl6180x_params as defined in file vl6180x_params.h.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * static vl6180x_t dev;
 *
 * if (vl6180x_init(&dev, &vl6180x_params[0]) != VL6180X_OK) {
 *     ... // error handling
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * After initialization, the sensor is configured according to the standard
 * configuration parameters and is fully operational.
 *
 * ## Operation modes {#vl6180x_operation_mode}
 *
 * The VL6180X can be used in two modes
 *
 * - **Single-shot mode**<br>
 *   In this mode the sensor is in software standby and a single measurement
 *   is started explicitly either with the function #vl6180x_rng_start_single
 *   or the function #vl6180x_als_start_single. After finishing the single
 *   measurement the sensor returns to software standby. In software standby,
 *   the power consumption of the sensor is less than 1 uA.
 * - **Continuous mode**<br>
 *   In this mode the sensor automatically performs measurements with the
 *   measurement period configured by parameter vl6180x_params_t::period.
 *   Between these measurements it returns to the software standby.<br>
 *   If range and ALS measurements are used (the modules `vl6180x_rng` and
 *   `vl6180x_als` are both used), the so-called **interleaved mode** is
 *   automatically used, where an ALS measurement is immediately followed
 *   by a range measurement and repeated with the defined period.
 *   The continuous mode can be stopped with function #vl6180x_stop_cont,
 *   for example to start single measurements. It is also possible to
 *   restart it using function #vl6180x_start_cont.
 *
 * @note If the configured measurement period is 0, the single-shot mode
 * is enabled after initialization for both the range and ALS measurements.
 * The functions #vl6180x_rng_start_single and #vl6180x_als_start_single must
 * then be used to start a single measurement.<br><br>
 * Otherwise, the continuous mode is activated for both measurements and
 * continuous measurements started automatically after sensor initialization
 * with the configured measurement period.
 *
 * ## Fetching data {#vl6180x_fetching_data}
 *
 * To get data, the user task can use either
 *
 * - the #vl6180x_rng_data_ready and #vl6180x_als_data_ready functions to
 *   periodically check if new data are ready to be read, and the
 *   #vl6180x_rng_read and #vl6180x_als_read functions to read the data
 *   (following example), or
 * - the data ready interrupt which is triggered as soon as new output data
 *   are available, see section [Using Interrupts](#vl6180x_using_interrupts).
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * while (1)
 * {
 *     uint16_t als;
 *     uint16_t lux;
 *     uint8_t  rng;
 *
 *     // execute task every 20 ms
 *     xtimer_usleep(20 * US_PER_MS);
 *     ...
 *     // test for new data and fetch them if available
 *     if (vl6180x_als_data_ready(&dev) == VL6180X_OK &&
 *         vl6180x_als_read(&dev, &als, &lux) == VL6180X_OK) {
 *         ...
 *     }
 *     if (vl6180x_rng_data_ready(&dev) == VL6180X_OK) {
 *         if (vl6180x_rng_read(&dev, &rng) == VL6180X_OK) {
 *         ...
 *     }
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ## Output data format
 *
 * Function #vl6180x_als_read returns one ALS data sample as raw count value
 * and, if required, as illuminance in Lux. The range of the count value
 * depends on
 *
 * - the ALS analog gain defined by vl6180x_params_t::als_gain,
 * - the integration time defined by vl6180x_params_t::als_int_time, and
 * - the lux resolution defined by vl6180x_params_t::als_lux_res.
 *
 * The count value is returned in parameter \p raw while the illuminance
 * is returned in parameter \p lux.  For either \p raw or \p lux also `NULL`
 * can be passed, if only one value is of interest.
 *
 * If ALS value is invalid because of a measurement error, #VL6180X_ERROR_ALS
 * is returned. The #vl6180x_als_status function can then be used to get an
 * error code of type #vl6180x_als_status_t.
 *
 * Function vl6180x_rng_read returns the ranging data in millimeters. If
 * ranging value is invalid because of a measurement error, #VL6180X_ERROR_RNG
 * is returned and function #vl6180x_rng_status function can then be used to
 * get an error code of type #vl6180x_rng_status_t.
 *
 * # Using Interrupts {#vl6180x_using_interrupts}
 *
 * The VL6180X sensor allows the use of different types of interrupts on signal
 * GPIO1 for range and ALS measurements:
 *
 * - data ready interrupts when data become available
 * - different event interrupts when sensor data cross configured thresholds
 *
 * @note Interrupts are only supported when module `vl6180x_irq` is used.
 *
 * ## Interrupt configuration {#vl6180x_interrupt_configuration}
 *
 * These interrupts can be enabled separately for the range and ALS
 * measurements by the interrupt mode of type #vl6180x_int_mode_t
 *
 * <center>
 * | Interrupt mode                                                         | Driver symbol     |
 * |:-----------------------------------------------------------------------|:------------------|
 * | New data are ready to be read                                          | #VL6180X_INT_DRDY |
 * | Sensor data are below the lower threshold                              | #VL6180X_INT_LOW  |
 * | Sensor data are above the upper threshold                              | #VL6180X_INT_HIGH |
 * | Sensor data are below the lower threshold or above the upper threshold | #VL6180X_INT_OUT  |
 * </center><br>
 *
 * @warning Only one of the interrupt modes must be enabled at the same time
 * for the same measurement.
 *
 * For event interrupts, upper and lower thresholds have to be defined,
 * with the upper and lower thresholds defining a threshold window of type
 * #vl6180x_int_thresh_t.
 *
 * In default configuration, #VL6180X_INT_DRDY is used both for range and
 * ALS measurements if module `vl6180x_irq` is used.
 *
 * The enabled interrupts can be changed with the #vl6180x_int_enable
 * function which takes a parameter of type #vl6180x_int_config_t which
 * simply contains the interrupt mode of type #vl6180x_int_mode_t for range
 * and ALS measurements.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * vl6180x_int_config_t mode = { .rng_int = VL6180X_INT_OUT,
 *                               .als_int = VL6180X_INT_DRDY };
 * vl6180x_int_enable(&dev, mode);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * If module `vl6180x_config` is used, the thresholds for event interrupts
 * can be changed using function #vl6180x_int_config, for example:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * vl6180x_int_thresh_t thresh;
 *
 * thresh.rng_low = 30;
 * thresh.rng_high = 100;
 * thresh.als_low = 10;
 * thresh.als_high = 1000;
 *
 * vl6180x_int_config(&dev, thresh);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ## Interrupt usage {#vl6180x_interrupt_sources}
 *
 * All functions of the driver require direct access to the sensor via
 * I2C which does not work in interrupt context.
 *
 * Therefore, the driver prevents the direct use of the interrupts and
 * application specific ISRs. The only way to use interrupts is to call
 * the function #vl6180x_int_wait which enables the interrupt signal
 * for the configured MCU GPIO and then blocks the calling thread
 * until an interrupt is triggered.
 *
 * Once an interrupt is triggered, the driver handles the interrupt with
 * an internal ISR and then returns from the #vl6180x_int_wait function
 * with the interrupt source. The interrupt mode of type #vl6180x_int_mode_t
 * respectively the composite type #vl6180x_int_config_t which is used for
 * defining enabled interrupts is also used for specifying the interrupt
 * source. It contains a flag for each possible interrupt source which
 * can be tested for true.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * vl6180x_int_config_t src;
 *
 * vl6180x_int_wait(&dev, &src);
 *
 * if (src.rng_int == VL6180X_INT_DRDY) {
 *     vl6180x_rng_read(&dev, &rng);
 *     printf("RNG: %u [mm]\n", rng);
 * }
 * else if (src.rng_int == VL6180X_INT_OUT) {
 *     printf("RNG: out of window\n");
 * }
 * else if (src.rng_int == VL6180X_INT_RNG_LOW) {
 *     printf("RNG: low level\n");
 * }
 * else if (src.rng_int == VL6180X_INT_RNG_HIGH) {
 *     printf("RNG: high level\n");
 * }
 *
 * if (src.als_int == VL6180X_INT_DRDY) {
 *     vl6180x_als_read(&dev, &als, &lux);
 *     printf("ALS: %u [cnts], %u [lux]\n", als, lux);
 * }
 * else if (src.als_int == VL6180X_INT_OUT) {
 *     printf("ALS: out of window\n");
 * }
 * else if (src.als_int == VL6180X_INT_LOW) {
 *     printf("ALS: low level\n");
 * }
 * else if (src.als_int == VL6180X_INT_HIGH) {
 *     printf("ALS: high level\n");
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * # Power Saving {#vl6180x_power_saving}
 *
 * If module `vl6180x_shutdown` is used, the VL6180X sensor can be shutdown
 * when no measurements are required using the function #vl6180x_power_down.
 * The power consumption is then reduced to less than 1 uA. To restart the
 * VL6180X in previous measurement mode, the #vl6180x_power_up function can
 * be used.
 *
 * @note To use these functions, the MCU GPIO pin connected to the GPIO0/CE
 * pin of the sensor has to be defined by the vl6180x_params_t::pin_shutdown
 * parameter.
 *
 * # Low level functions {#vl6180x_low_level}
 *
 * Low level level interface functions that allow direct read and write
 * access to the registers of the sensor.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * bool vl6180x_reg_read(const vl6180x_t* dev, uint16_t reg, uint8_t *data, uint8_t len);
 * bool vl6180x_reg_write(const vl6180x_t* dev, uint16_t reg, const uint8_t *data, uint8_t len);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @warning
 * These functions should only be used to do something special that
 * is not covered by the high level interface AND if you exactly
 * know what you do and what it might affect. Please be aware that
 * it might affect the high level interface.
 *
 * # Configuration
 *
 * ## Default configuration
 *
 * Default sensor hardware configurations are set in file `vl6180x_params.h`
 * using the following defines:
 *
 * <center>
 * | Hardware configuration | Driver name                 | Default Value     |
 * |:-----------------------|:----------------------------|:------------------|
 * | I2C device             | #VL6180X_PARAM_DEV          | I2C_DEV(0)        |
 * | I2C address            | #VL6180X_PARAM_ADDR         | #VL6180X_I2C_ADDR |
 * | Interrupt pin          | #VL6180X_PARAM_INT_PIN      | GPIO_PIN(0,1)     |
 * | Shutdown pin           | #VL6180X_PARAM_SHUTDOWN_PIN | GPIO_PIN(0,2)     |
 * </center><br>
 *
 * These hardware configurations can be overridden either by the board
 * definition or by defining them in the `CFLAGS` variable in the make
 * command, for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USEMODULE='vl6180x_rng vl6180x_als vl6180x_irq` \
 * CLFAGS='-DVL6180X_PARAM_INT_PIN=GPIO_PIN\(0,5\)' \
 * BOARD=... make -C tests/drivers/vl6180x
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  The default configuration of the sensor is defined in file
 * `vl6180x_params.h` using the following defines:
 *
 * Parameter                          |  Default Value     | Define to be overridden
 * :----------------------------------|:-------------------|:---------------------------
 * Period of continuous measurements  | 200 ms             | #CONFIG_VL6180X_MEAS_PERIOD
 * Ranging maximum convergence time   | 50 ms              | #CONFIG_VL6180X_RNG_MAX_TIME
 * Ranging interrupt mode             | VL6180X_INT_DRDY   | #CONFIG_VL6180X_RNG_INT
 * Ranging lower threshold            | 20 mm              | #CONFIG_VL6180X_RNG_THRESH_LOW
 * Ranging upper threshold            | 90 mm              | #CONFIG_VL6180X_RNG_THRESH_HIGH
 * ALS integration time               | 100 ms             | #CONFIG_VL6180X_ALS_INT_TIME
 * ALS analogue gain                  | VL6180X_ALS_GAIN_1 | #CONFIG_VL6180X_ALS_GAIN
 * ALS lux resolution lux/count*1000  | 320                | #CONFIG_VL6180X_ALS_LUX_RES
 * ALS interrupt mode                 | VL6180X_INT_DRDY   | #CONFIG_VL6180X_ALS_INT
 * ALS lower threshold                | 50 counts          | #CONFIG_VL6180X_ALS_THRESH_LOW
 * ALS upper threshold                | 2000 counts        | #CONFIG_VL6180X_ALS_THRESH_HIGH
 *
 * Single or all parameters of the default configuration can be overridden
 * either by placing a modified file `vl6180x_params.h` in the application
 * directory or by defining them in the variable `CFLAGS` in the make command
 * line. For example to configure a measurement period of 500 ms and a maximum
 * convergence time for ranging of 60 ms, the following command could be used:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USEMODULE='vl6180x_rng vl6180x_als` \
 * CLFAGS='-DCONFIG_VL6180X_MEAS_PERIOD=50 -DCONFIG_VL6180X_RNG_MAX_TIME=60' \
 * BOARD=... make -C tests/drivers/vl6180x
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ## Configuration at runtime
 *
 * If module `vl6180x_config` is used, the following functions can be used to
 * change the default sensor configuration at runtime.
 *
 * | Function            | Functionality                                         |
 * |:--------------------|:------------------------------------------------------|
 * | #vl6180x_rng_config | Changes the range measurement parameter configuration |
 * | #vl6180x_als_config | Changes the ALS measurement parameter configuration   |
 * | #vl6180x_int_config | Changes the thresholds for event interrupts           |
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

#include "vl6180x_regs.h"

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
    VL6180X_ERROR_NOT_READY = 7,    /**< Device not ready */
} vl6180x_error_t;

/**
 * @brief   Analogue gain for ALS measurements
 */
typedef enum {
    VL6180X_ALS_GAIN_20   = 0,  /**<   20 x gain (actual analogue gain of 20) */
    VL6180X_ALS_GAIN_10   = 1,  /**<   10 x gain (actual analogue gain of 10.32) */
    VL6180X_ALS_GAIN_5    = 2,  /**<    5 x gain (actual analogue gain of 5.21) */
    VL6180X_ALS_GAIN_2_5  = 3,  /**<  2.5 x gain (actual analogue gain of 2.6) */
    VL6180X_ALS_GAIN_1_67 = 4,  /**< 1.67 x gain (actual analogue gain of 1.72) */
    VL6180X_ALS_GAIN_1_25 = 5,  /**< 1.25 x gain (actual analogue gain of 1.28) */
    VL6180X_ALS_GAIN_1    = 6,  /**<    1 x gain (actual analogue gain of 1.01), default */
    VL6180X_ALS_GAIN_40   = 7,  /**<   40 x gain (actual analogue gain of 40) */
} vl6180x_als_gain_t;

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

/**
 * @brief   Interrupt mode
 *
 * The interrupt mode defines the different sources that can trigger an
 * interrupt on the GPIO1 pin of the sensor. The interrupt mode is defined for
 * range and ALS measurements separately. Interrupts can be triggered either
 *
 * - in each measurement cycle when new data become available (data ready interrupts) or
 * - only when values exceed a threshold configured (event interrupts).
 *
 * For threshold interrupts, upper and lower thresholds have to be defined,
 * with the upper and lower thresholds defining a threshold window, see
 * also #vl6180x_int_thresh_t.
 *
 * @note Interrupts are only supported when module `vl6180x_irq` is used.
 * @warning Only one of the interrupt modes must be enabled at the same time.
 */
typedef enum {
    VL6180X_INT_NONE = 0, /**< Interrupt is disabled */

    VL6180X_INT_LOW  = 1, /**< Interrupt is triggered when values are below
                               the lower threshold */
    VL6180X_INT_HIGH = 2, /**< Interrupt is triggered when values are above
                               the upper threshold */
    VL6180X_INT_OUT  = 3, /**< Interrupt is triggered when values are below
                               the lower threshold or above the upper threshold
                               (value leave the threshold window) */
    VL6180X_INT_DRDY = 4, /**< Interrupt is triggered when new data are ready
                               to be read */
} vl6180x_int_mode_t;

/**
 * @brief   Interrupt config
 *
 * This type defines the interrupt mode for both range measurements and
 * ALS measurements. It is used on the one hand as parameter \p mode in the
 * function #vl6180x_int_enable to enable the interrupt for the respective
 * measurement and on the other hand to return the source of an interrupt
 * in function #vl6180x_int_wait when an interrupt was triggered.
 *
 * The interrupt mode is defined for range and ALS measurements separately.
 *
 * @note Interrupts are only supported when module `vl6180x_irq` is used.
 */
typedef struct {
#if IS_USED(MODULE_VL6180X_RNG) || DOXYGEN
    vl6180x_int_mode_t rng_int; /**< Interrupt mode for range measurements */
#endif
#if IS_USED(MODULE_VL6180X_ALS) || DOXYGEN
    vl6180x_int_mode_t als_int; /**< Interrupt mode for ALS measurements */
#endif
} vl6180x_int_config_t;

/**
 * @brief   Interrupt threshold configuration
 *
 * Threshold configurations are used for event interrupts only.
 * If event interrupts are enabled by the corresponding interrupt
 * mode for range and/or ALS measurements, the lower and/or upper threshold
 * values are used to generate an interrupt if the values of the respective
 * measurement exceed these threshold values.
 *
 * The unit of threshold values for range measurements is millimeters.
 * The unit of threshold values for ALS measurements is counts.
 *
 * @note Interrupts are only supported when module `vl6180x_irq` is used.
 */
typedef struct {
#if IS_USED(MODULE_VL6180X_RNG) || DOXYGEN
    uint8_t rng_high;   /**< upper threshold for range values */
    uint8_t rng_low;    /**< lower threshold for range values */
#endif
#if IS_USED(MODULE_VL6180X_ALS) || DOXYGEN
    uint16_t als_high;  /**< upper threshold for ALS values */
    uint16_t als_low;   /**< lower threshold for ALS values */
#endif
} vl6180x_int_thresh_t;

/**
 * @brief   VL6180X device configuration
 */
typedef struct {
    /**
     * @name Hardware configuration
     * @{
     */
    unsigned i2c_dev;        /**< I2C device, default I2C_DEV(0) */
    uint8_t  i2c_addr;       /**< I2C slave address */

#if IS_USED(MODULE_VL6180X_SHUTDOWN) || DOXYGEN
    gpio_t   shutdown_pin;  /**< Shutdown pin, LOW active */
#endif /* IS_USED(MODULE_VL6180X_SHUTDOWN) || DOXYGEN */

#if IS_USED(MODULE_VL6180X_IRQ) || DOXYGEN
    gpio_t               int_pin;     /**< Interrupt pin, LOW active */
    vl6180x_int_config_t int_cfg;     /**< Interrupt mode configuration */
    vl6180x_int_thresh_t int_thresh;  /**< Interrupt threshold configuration */
#endif /* IS_USED(MODULE_VL6180X_IRQ) || DOXYGEN */

    /** @} */

    /**
     * @brief   Measurement period in continuous mode in
     *          steps of 10 ms (default 20 = 200 ms).
     *
     * The measurement period also controls the measurement mode used after
     * sensor initialization. If the configured measurement period is 0,
     * the single-shot mode is enabled for both the range and ALS
     * measurements. The functions vl6180x_rng_start_single and
     * vl6180x_als_start_single must then be used to start a single measurement.
     * Otherwise, the continuous mode is activated for both measurements,
     * which are started immediately after sensor initialization with the
     * configured measurement period. This also applies to the initialization
     * after a power-down and power-up cycle.
     *
     * @note When ALS and range measurements are used in continuous mode,
     * the so-called interleaved mode is used automatically, where an ALS
     * measurement is immediately followed by a range measurement and
     * repeated with the defined period.
     */
    uint8_t period;

#if IS_USED(MODULE_VL6180X_RNG) || DOXYGEN
    /**
     * @name Range measurement configuration
     * @{
     */
    uint8_t rng_max_time;  /**< Maximum convergence time in ms [1...63] given
                                to the sensor to perform a range measurement
                                (default 50 = 50 ms) */
    /** @} */
#endif /* IS_USED(MODULE_VL6180X_RNG) || DOXYGEN */

#if IS_USED(MODULE_VL6180X_ALS) || DOXYGEN
    /**
     * @name ALS measurement configuration
     * @{
     */
    uint16_t als_int_time; /**< ALS integration time in ms [1...512]
                                (default 100 = 100 ms, **recommended**) */
    uint16_t als_lux_res;  /**< ALS lux resolution multiplied by factor 1000
                                (default 0.32 lux/count is the factory
                                calibrated lux resolution without cover glas) */
    vl6180x_als_gain_t als_gain; /**< ALS analogue gain for light channel
                                      (default VL6180X_ALS_GAIN_1_0) */
    /** @} */
#endif /* IS_USED(MODULE_VL6180X_ALS) || DOXYGEN */

} vl6180x_params_t;

/**
 * @brief   VL6180X sensor device data structure type
 */
typedef struct {

    vl6180x_params_t params;  /**< Device initialization parameters */
    bool cont_meas;           /**< Continuous mode running */
#if IS_USED(MODULE_VL6180X_RNG) || DOXYGEN
    vl6180x_rng_status_t rng_status; /**< Status of last range measurement */
#endif /* IS_USED(MODULE_VL6180X_RNG) || DOXYGEN */
#if IS_USED(MODULE_VL6180X_ALS) || DOXYGEN
    vl6180x_als_status_t als_status; /**< Status of last ALS measurement */
#endif /* IS_USED(MODULE_VL6180X_ALS) || DOXYGEN */
} vl6180x_t;

/**
 * @brief   Initialize the VL6180X sensor device
 *
 * After initialization, the sensor is configured according to the standard
 * configuration parameters and is fully functional.
 *
 * If the configured measurement period is 0, the single-shot mode
 * is enabled for both the range and ALS measurements. The functions
 * vl6180x_rng_start_single and vl6180x_als_start_single must then be used
 * to start a single measurement. Otherwise, the continuous mode is activated
 * for both measurements, which are started immediately after sensor
 * initialization with the configured measurement period.
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
 * @brief   Start measurements in continuous mode
 *
 * Range and/or ALS measurements are started in continuous mode with same
 * measurement period as defined in configuration parameter
 * vl6180x_params_t::period.
 *
 * @note Continuous mode cannot be started separately for range and ALS
 * measurements.
 *
 * @pre
 * - Measurement period vl6180x_params_t::period must not be zero.
 * - Measurements must not yet be started in continuous mode when called.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor to be initialized
 *
 * @retval  VL6180X_OK      on success
 * @retval  VL6180X_ERROR_* a negative error code on error, see
 *                          #vl6180x_error_t
 */
int vl6180x_start_cont(vl6180x_t *dev);

/**
 * @brief   Stop measurements in continuous mode
 *
 * Continuous range and ALS measurements are stopped. Once continuous
 * measurements are stopped, vl6180x_rng_start_single or
 * vl6180x_als_start_single can be used to start single-shot measurements
 * separately.
 *
 * @pre
 * - Measurement period vl6180x_params_t::period must not be zero.
 * - Measurements must be started in continuous mode when called.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor to be initialized
 *
 * @retval  VL6180X_OK      on success
 * @retval  VL6180X_ERROR_* a negative error code on error, see
 *                          #vl6180x_error_t
 */
int vl6180x_stop_cont(vl6180x_t *dev);

#if IS_USED(MODULE_VL6180X_RNG) || DOXYGEN
/**
 * @brief   Range data ready status function
 *
 * The function can be used for polling to know when new ranging data are ready.
 *
 * @note This function is only available when module `vl6180x_rng` is used.
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
 * @brief   Read one ranging data sample in mm
 *
 * This function returns the ranging data in millimeters. If ranging value
 * is invalid because of a measurement error, #VL6180X_ERROR_RNG is returned.
 * The #vl6180x_rng_status function can then be used to get an error code of
 * type #vl6180x_rng_status_t.
 *
 * @note
 * - This function is only available when module `vl6180x_rng` is used.
 * - The function clears the interrupt if ambient light sensing interrupts
 *   are used.
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
 * @brief   Get status of last range measurement
 *
 * @note This function is only available when module `vl6180x_rng` is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @retval      status of type vl6180x_rng_status_t
 */
vl6180x_rng_status_t vl6180x_rng_status(const vl6180x_t *dev);

/**
 * @brief   Start a single-shot range measurement
 *
 * @pre Measurements must not be started in continuous mode when called.
 *
 * @note This function is only available when module `vl6180x_rng` is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_rng_start_single(const vl6180x_t *dev);

#if IS_USED(MODULE_VL6180X_CONFIG) || DOXYGEN
/**
 * @brief   Reconfigure range measurements at runtime
 *
 * This function can be used to overwrite the default configuration of range
 * measurements defined by #vl6180x_params_t at runtime.
 *
 * For this purpose, the running range measurement is stopped and restarted
 * after the reconfiguration if continuous mode is used (\p period is not 0).
 *
 * @note
 * - This function is only available when modules `vl6180x_rng` and
 *   `vl6180x_config` are used.
 * - Since parameter \p period is used for continuous mode, in which
 *   measurements are performed in interleaved mode, setting the period
 *   with this function also affects the ALS measurements in continuous mode.
 *
 * @param[in]   dev         Device descriptor of VL6180X sensor
 * @param[in]   period      Period in continuous measurement mode in steps
 *                          of 10 ms. It controls also the measurement mode
 *                          enabled after the initialization. If 0, single-shot
 *                          mode is enabled, otherwise the continuous
 *                          mode is enabled and measurement are started
 *                          automatically.
 * @param[in]   max_time    Maximum convergence time in ms [1...63] given
 *                          to the sensor to perform range measurements
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_rng_config(vl6180x_t *dev, uint8_t period, uint8_t max_time);

#endif /* IS_USED(MODULE_VL6180X_CONFIG) || DOXYGEN */
#endif /* IS_USED(MODULE_VL6180X_RNG) || DOXYGEN */

#if IS_USED(MODULE_VL6180X_ALS) || DOXYGEN
/**
 * @brief   ALS data ready status function
 *
 * @note This function is only available when module `vl6180x_als` is used.
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
 * @brief   Read one ambient light sensing (ALS) data sample
 *
 * This function returns one ALS data sample as raw count value and, if
 * required, as illuminance in Lux. The range of the count value
 * depends on
 *
 * - the ALS analog gain defined by vl6180x_params_t::als_gain,
 * - the integration time defined by vl6180x_params_t::als_int_time, and
 * - the lux resolution defined by vl6180x_params_t::als_res.
 *
 * The count value is returned in parameter \p raw while the illuminance
 * is returned in parameter \p lux.  For either \p raw or \p lux also `NULL`
 * can be passed, if only one value is of interest.
 *
 * If ALS value is invalid because of a measurement error, #VL6180X_ERROR_ALS
 * is returned. The #vl6180x_als_status function can then be used to get an
 * error code of type #vl6180x_als_status_t.
 *
 * @note
 * - This function is only available when module `vl6180x_als` is used.
 * - The function clears the interrupt if ambient light sensing interrupts
 *   are used.
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

/**
 * @brief   Get status of last ALS measurement
 *
 * @note This function is only available when module `vl6180x_als` is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @retval      status of type vl6180x_als_status_t
 */
vl6180x_als_status_t vl6180x_als_status(const vl6180x_t *dev);

/**
 * @brief   Start a single-shot ALS measurement
 *
 * @pre Measurements must not be started in continuous mode when called.
 *
 * @note This function is only available when module `vl6180x_als` is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_als_start_single(const vl6180x_t *dev);

#if IS_USED(MODULE_VL6180X_CONFIG) || DOXYGEN
/**
 * @brief   Reconfigure ambient light sensing (ALS) during runtime
 *
 * This function can be used to overwrite the default configuration of ambient
 * light sensing defined by #vl6180x_params_t during runtime.
 *
 * For this purpose, the running ambient light sensing (ALS) is stopped and
 * restarted after the reconfiguration if continuous mode is used
 * (\p period is not 0).
 *
 * @note
 * - This function is only available when modules `vl6180x_als` and
 *   `vl6180x_config` are used.
 * - Since parameter \p period is used for continuous mode, in which
 *   measurements are performed in interleaved mode, setting the period
 *   with this function also affects the range measurements in continuous mode.
 *
 * @param[in]   dev         Device descriptor of VL6180X sensor
 * @param[in]   period      Period in continuous measurement mode in steps
 *                          of 10 ms. It controls also the measurement mode
 *                          enabled after the initialization. If 0, single-shot
 *                          mode is enabled, otherwise the continuous
 *                          mode is enabled and measurement are started
 *                          automatically.
 * @param[in]   int_time    ALS integration time in ms [0...511]
 * @param[in]   gain        ALS analogue gain for light channel
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_als_config(vl6180x_t *dev, uint8_t period, uint8_t int_time,
                                       vl6180x_als_gain_t gain);

#endif /* IS_USED(MODULE_VL6180X_CONFIG) || DOXYGEN */
#endif /* IS_USED(MODULE_VL6180X_ALS) || DOXYGEN */

#if IS_USED(MODULE_VL6180X_SHUTDOWN) || DOXYGEN
/**
 * @brief   Power down the sensor
 *
 * @pre This function requires that a GPIO connected to sensor's GPIO0/CE pin is
 * defined by parameter vl6180x_params_t::pin_shutdown.
 *
 * @note This function is only available if the `vl6180x_shutdown` module
 * is used.
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
 * @pre This function requires that a GPIO connected to sensor's GPIO0/CE pin is
 * defined by parameter vl6180x_params_t::pin_shutdown.
 *
 * @note This function is only available if the `vl6180x_shutdown` module
 * is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_power_up(vl6180x_t *dev);

#endif /* IS_USED(MODULE_VL6180X_SHUTDOWN) || DOXYGEN  */

#if IS_USED(MODULE_VL6180X_IRQ) || DOXYGEN

/**
 * @brief   Wait for configured interrupts and return the interrupt sources
 *
 * To avoid I2C bus access in interrupt context, the driver prevents the
 * direct use of interrupts and application specific ISRs. Rather, this
 * function has to be used to wait for an interrupt. It enables the interrupt
 * signal for the configured MCU GPIO and then blocks the calling thread
 * until an interrupt is triggered.
 *
 * Once an interrupt is triggered, the driver handles the interrupt with an
 * internal ISR and then returns. When the function returns, the data structure
 * of type vl6180x_int_config_t to which the \p src parameter points contains
 * the source of the triggered interrupt. It contains a flag for each possible
 * interrupt source which can be tested for true.
 *
 * @pre
 * - Configuration parameter vl6180x_params_t::int_pin has to be defined.
 * - vl6180x_int_config_t::rng_int and vl6180x_int_config_t::als_int
 *   must only define one interrupt mode each.
 * - If threshold interrupts are enabled, thresholds have to be valid.
 *
 * @note This function is only available when module `vl6180x_irq` is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @param[out]  src     Interrupt sources, see #vl6180x_int_config_t
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_int_wait(const vl6180x_t *dev, vl6180x_int_config_t *src);

/**
 * @brief   Enable and disable interrupts
 *
 * Configured interrupts can be enabled or disabled with this function.
 *
 * @pre
 * - Configuration parameter vl6180x_params_t::int_pin has to be defined
 * - vl6180x_int_config_t::rng_int and vl6180x_int_config_t::als_int
 *   must only define one interrupt mode each.
 *
 * @note
 * - To disable intertupts, set vl6180x_int_config_t::rng_int and
 *   vl6180x_int_config_t::als_int to #VL6180X_INT_NONE.
 * - This function is only available when module `vl6180x_irq` is used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @param[in]   mode    Interrupts to be enabled, must be only one for each
 *                      measurement type (range and ALS)
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_int_enable(vl6180x_t *dev, vl6180x_int_config_t mode);

#if IS_USED(MODULE_VL6180X_CONFIG) || DOXYGEN
/**
 * @brief   Configure thresholds for event interrupts at runtime
 *
 * @note This function is only available when modules `vl6180x_irq` and
 * `vl6180x_config` are used.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @param[in]   thresh  Threshold configuration for event interrupts,
 *                      see #vl6180x_int_thresh_t
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_int_config(vl6180x_t *dev, vl6180x_int_thresh_t thresh);

#endif /* IS_USED(MODULE_VL6180X_CONFIG) || DOXYGEN  */
#endif /* IS_USED(MODULE_VL6180X_IRQ) || DOXYGEN */

/**
 * @name   Low level interface functions
 * @{
 */

/**
 * @brief   Direct write to register
 *
 * @note This function should only be used to do something special that
 * is not covered by the high level interface AND if you exactly know what you
 * do and what effects it might have. Please be aware that it might affect the
 * high level interface.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @param[in]   reg     Address of the first register to be changed
 * @param[in]   data    Pointer to the data to be written to the register
 * @param[in]   len     Number of bytes to be written to the register
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_reg_write(const vl6180x_t *dev,
                      uint16_t reg, const uint8_t *data, uint8_t len);

/**
 * @brief   Direct read from register
 *
 * @note This function should only be used to do something special that
 * is not covered by the high level interface AND if you exactly know what you
 * do and what effects it might have. Please be aware that it might affect the
 * high level interface.
 *
 * @param[in]   dev     Device descriptor of VL6180X sensor
 * @param[in]   reg     address of the first register to be read
 * @param[out]  data    pointer to the data to be read from the register
 * @param[in]   len     number of bytes to be read from the register
 *
 * @retval  VL6180X_OK          on success
 * @retval  VL6180X_ERROR_*     a negative error code on error, see
 *                              #vl6180x_error_t
 */
int vl6180x_reg_read(const vl6180x_t *dev,
                     uint16_t reg, uint8_t *data, uint8_t len);
/** @} */
#ifdef __cplusplus
}
#endif

/** @} */
#endif /* VL6180X_H */
