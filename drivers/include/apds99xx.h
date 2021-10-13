/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_apds99xx APDS99XX proximity and ambient light sensors
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for Broadcom APDS99XX proximity and ambient light sensors
 *
 * The driver can be used with following Broadcom sensors:
 * APDS9900, APDS9901, APDS9930, APDS9950, APDS9960
 *
 * The driver is divided into two parts:
 *
 * - **Basic driver** ```apds99xx```
 *
 *   The base driver ```apds99xx``` only supports a basic set of functions and
 *   has therefore a small size. The procedure for retrieving new data is
 *   polling. Ambient light and proximity sensing are supported.
 *
 * - **Fully functional driver** ```apds99xx_full```
 *
 *   The fully functional driver ```apds99xx_full``` supports all the features
 *   supported by the base driver, as well as all other sensor features,
 *   including interrupts and their configuration. Data-ready interrupts can be
 *   used to retrieve data. In addition, threshold interrupts can be used and
 *   configured.
 *
 * @note In addition to specifying whether the base driver ```apds99xx``` or
 * the fully featured driver ```apds99xx_full``` should be used, the
 * application has to declare used sensor by means of various
 * pseudomodules as follows:
 *
 *      APDS9900:     USEMODULE=apds9900
 *      APDS9901:     USEMODULE=apds9901
 *      APDS9930:     USEMODULE=apds9930
 *      APDS9950:     USEMODULE=apds9950
 *      APDS9960:     USEMODULE=apds9960
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * # Measurement Cycle
 *
 * APDS99XX sensor **measurement cycles** consists of the
 * following **three steps** in the given order:
 *
 * - **Proximity Sensing**
 *
 *   The sensor generates a number of IR LED pulses and measures the amount
 *   of the IR energy reflected by an object to determine its distance. The
 *   time required for proximity sensing (```t_prx``` ) results from the
 *   time it takes to generate the IR LED pulses and to accumulate the
 *   reflected IR energy (```t_prx_acc```) as well as the time for the ADC
 *   conversion (```t_prx_cnv```).
 *
 *      t_prx = t_prx_acc + t_prx_cnv
 *
 *   The time to generate the IR LED pulses and accumulate reflected IR
 *   energy ```t_prx_acc``` is defined by the number of pulses (parameter
 *   apds99xx_params_t::prx_pulses) and the period of one pulse ```t_prx_pulse```.
 *
 *       t_prx_acc = prx_pulses * t_prx_pulse
 *
 *   The ADC conversion time ```t_prx_cnv``` and the period of one
 *   pulse ```t_prx_pulse``` depend on used sensor and are available as the
 *   defines #APDS99XX_T_PRX_CNV and #APDS99XX_T_PRX_PULSE for calculations
 *   by the application.
 *   Sensor<br> | t_prx_pulse<br>APDS99XX_T_PRX_PULSE | t_prx_cnv<br>APDS99XX_T_PRX_CNV
 *   ---------- | -------- | -----------------------
 *   APDS9900   | 0.016 ms | 2.720 ms
 *   APDS9901   | 0.016 ms | 2.720 ms
 *   APDS9930   | 0.016 ms | 2.730 ms
 *   APDS9950   | 0.014 ms | 2.400 ms
 *   APDS9960   | 0.036 ms | 0.814 ms
 *   <br>
 *
 *   Proximity sensing uses the gain specified by parameter
 *   apds99xx_params_t::prx_gain and the LED current specified by
 *   parameter apds99xx_params_t::prx_drive.
 *
 * - **Wait**
 *
 *   The sensor waits for ```t_wait``` which is defined by the number of
 *   waiting steps ```wait_steps``` (apds99xx_params_t::wait_steps), the time
 *   per step ```t_step```, and the wait long flag
 *   (apds99xx_params_t::wait_long);
 *
 *       t_wait = wait_steps * t_wait_step            (if wait_long is false)
 *       t_wait = wait_steps * t_wait_step * 12       (if wait_long is true)
 *
 *   Parameter apds99xx_params_t::wait_steps can range from 0 to 256.
 *   If apds99xx_params_t::wait_steps is 0, waiting is disabled. The time per
 *   step ```t_wait_step``` depends on used sensor and is available as the
 *   define #APDS99XX_T_WAIT_STEP for calculations by the application.
 *   Sensor<br> | t_als_step<br>APDS99XX_T_WAIT_STEP | t_wait (wait_long=0) <br> |  t_wait (wait_long=1) <br>
 *   ---------- | ------- | ---------------------------| ------------------------
 *   APDS9900   | 2.72 ms | ```wait_steps``` * 2.72 ms | ```wait_steps``` * 12 * 2.72 ms
 *   APDS9901   | 2.72 ms | ```wait_steps``` * 2.72 ms | ```wait_steps``` * 12 * 2.72 ms
 *   APDS9930   | 2.73 ms | ```wait_steps``` * 2.73 ms | ```wait_steps``` * 12 * 2.73 ms
 *   APDS9950   | 2.40 ms | ```wait_steps``` * 2.40 ms | ```wait_steps``` * 12 * 2.40 ms
 *   APDS9960   | 2.78 ms | ```wait_steps``` * 2.78 ms | ```wait_steps``` * 12 * 2.78 ms
 *   <br>
 *
 * - **Ambient Light Sensing (ALS)**
 *
 *   The sensor converts the amplified photodiode currents using integrating
 *   ADCs. The time required for ALS is determined by the ALS integration
 *   time ```t_als_int```, which is defined as the number of integration
 *   steps ```als_steps``` (parameter apds99xx_params_t::als_steps) and the
 *   time per step ```t_als_step```.
 *
 *       t_als_int = als_steps * t_als_step
 *
 *   Parameter apds99xx_params_t::als_steps can range from 0 to 256.
 *   If apds99xx_params_t::als_steps is 0, ALS is disabled. The time per
 *   step ```t_als_step``` depends on used sensor and is available as the
 *   define #APDS99XX_T_ALS_STEP for calculations by the application.
 *
 *   The ALS integration time in steps ```als_steps``` (parameter
 *   apds99xx_params_t::als_steps) affects the resolution and the full scale
 *   range ```cnt_als_max``` of ALS data.
 *
 *       cnt_als_max = als_steps * cnts_p_step
 *
 *   The counts per step ```cnts_p_step``` depend on used sensor and is
 *   available as defines #APDS99XX_CNTS_P_STEP for calculations by the
 *   application
 *
 *   ALS uses the gain specified by parameter apds99xx_params_t::als_gain.
 *   Sensor<br> | t_als_step<br>APDS99XX_T_ALS_STEP | cnts_p_step<br>APDS99XX_CNTS_P_STEP | t_als_int<br> | cnt_als_max<br>
 *   -------- | ------- | ---- | ----------------------- | ----------------
 *   APDS9900 | 2.72 ms | 1023 | ```als_steps``` * 2.72 ms | ```als_steps``` * 1023
 *   APDS9901 | 2.72 ms | 1023 | ```als_steps``` * 2.72 ms | ```als_steps``` * 1023
 *   APDS9930 | 2.73 ms | 1023 | ```als_steps``` * 2.73 ms | ```als_steps``` * 1023
 *   APDS9950 | 2.40 ms | 1024 | ```als_steps``` * 2.40 ms | ```als_steps``` * 1024
 *   APDS9960 | 2.78 ms | 1025 | ```als_steps``` * 2.78 ms | ```als_steps``` * 1025
 *
 *
 * The overall measurement cycle time is given by:
 *
 *     t_cycle = t_prx + t_wait + t_als_int
 *
 * For a given ALS integration time and a given proximity sensing time,
 * the waiting time can be used to tune the overall measurement cycle time.
 *
 *
 * # Interrupts
 *
 * With the ```apds99xx_full``` driver, interrupts can be used either to
 * fetch ALS and proximity data or to detect when these data exceed
 * defined thresholds.
 *
 * To use interrupts, the application must call the #apds99xx_int_config
 * function to specify an #apds99xx_int_config_t interrupt configuration and
 * an ISR with an optional argument that is invoked when an interrupt is
 * raised. For details about configuring and enabling the interrupts, see
 * #apds99xx_int_config_t.
 *
 * @note The ISR of the application is executed in the interrupt context.
 * Therefore, it must not be blocking or time consuming. In addition, it
 * must not access the sensor directly via I2C. It should only indicate to
 * the waiting application that an interrupt has occurred, which is then
 * handled in the thread context.
 *
 * While an interrupt is being serviced, the application can use the
 * #apds99xx_int_source function to query the type of interrupts triggered
 * by the sensor. In addition, the function resets the interrupt signal.
 *
 * @note Since the interrupt signal is only reset by the function
 * #apds99xx_int_source, this function #apds99xx_int_source must be executed
 * by application, even if the type of the triggered interrupt is not of
 * interest.
 *
 * For using interrupts, the GPIO to which the sensor's **INT** output pin
 * is connected has to be defined by the application in configuration
 * parameters. The GPIO is initialized by the driver as soon as the interrupt
 * configuration with the function # apds99xx_int_config is specified.
 *
 * # Illuminance in Lux and RGB count values
 *
 * For all sensors, the clear channel and the RGB channels provide only count
 * values. APDS9900, APDS9901, and APDS9930 have an IR photodiode in addition
 * to the clear channel photodiode, which can be used to determine the
 * illuminance in Lux with an algorithm described in their datasheets.
 *
 * Unfortunately, APDS9950 and APDS9960 do not have such an additional IR
 * photodiode, and there is no document which describes an approach to
 * calculate the illuminance from the RGB channels. Therefore, it is not
 * possible to determine the illuminance for these sensors.
 *
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef APDS99XX_H
#define APDS99XX_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

/**
 * @name    APDS99XX device properties
 *
 * Defines for different device properties for supported sensor types. These
 * properties can be used by the application for calculations.
 * @{
 */
#if MODULE_APDS9900
#define APDS99XX_ID          (0x29)  /**< Device ID of ADPS-9900 */
#define APDS99XX_T_PRX_PULSE (16)    /**< LED IR pulse period ```t_pulse``` in us */
#define APDS99XX_T_PRX_CNV   (2720)  /**< Proximity ADC conversion time ```t_prx_conv``` in us */
#define APDS99XX_T_WAIT_STEP (2720)  /**< Wait time step size ```t_step``` in us */
#define APDS99XX_T_ALS_STEP  (2720)  /**< ALS integration time step size ```t_step``` in */
#define APDS99XX_CNTS_P_STEP (1023)  /**< Counts per step ```cnts_p_step``` */

#elif MODULE_APDS9901
#define APDS99XX_ID          (0x20)  /**< Device ID of ADPS-9901 */
#define APDS99XX_T_PRX_PULSE (16)    /**< LED IR pulse period ```t_pulse``` in us */
#define APDS99XX_T_PRX_CNV   (2720)  /**< Proximity ADC conversion time ```t_prx_conv``` in us */
#define APDS99XX_T_WAIT_STEP (2720)  /**< Wait time step size ```t_step``` in us */
#define APDS99XX_T_ALS_STEP  (2720)  /**< ALS integration time step size ```t_step``` in */
#define APDS99XX_CNTS_P_STEP (1023)  /**< Counts per step ```cnts_p_step``` */

#elif MODULE_APDS9930
#define APDS99XX_ID          (0x39)  /**< Device ID of ADPS-9930 */
#define APDS99XX_T_PRX_PULSE (16)    /**< LED IR pulse period ```t_pulse``` in us */
#define APDS99XX_T_PRX_CNV   (2730)  /**< Proximity ADC conversion time ```t_prx_conv``` in us */
#define APDS99XX_T_WAIT_STEP (2730)  /**< Wait time step size ```t_step``` in us */
#define APDS99XX_T_ALS_STEP  (2730)  /**< ALS integration time step size ```t_step``` in */
#define APDS99XX_CNTS_P_STEP (1023)  /**< Counts per step ```cnts_p_step``` */

#elif MODULE_APDS9950
#define APDS99XX_ID          (0x69)  /**< Device ID of ADPS-9950 */
#define APDS99XX_T_PRX_PULSE (14)    /**< LED IR pulse period ```t_pulse``` in us */
#define APDS99XX_T_PRX_CNV   (2400)  /**< Proximity ADC conversion time ```t_prx_conv``` in us */
#define APDS99XX_T_WAIT_STEP (2400)  /**< Wait time step size ```t_step``` in us */
#define APDS99XX_T_ALS_STEP  (2400)  /**< ALS integration time step size ```t_step``` in */
#define APDS99XX_CNTS_P_STEP (1024)  /**< Counts per step ```cnts_p_step``` */

#elif MODULE_APDS9960 || DOXYGEN
#define APDS99XX_ID          (0xab)  /**< Device ID of ADPS-9960 */
#define APDS99XX_T_PRX_PULSE (36)    /**< LED IR pulse period ```t_pulse``` in us
                                          (for PPLEN=8 us) */
#define APDS99XX_T_PRX_CNV   (841)   /**< Proximity ADC conversion time ```t_prx_conv``` in us
                                          (tINIT + tCNVT for PPLEN=8 us) */
#define APDS99XX_T_WAIT_STEP (2780)  /**< Wait time step size ```t_step``` in us */
#define APDS99XX_T_ALS_STEP  (2780)  /**< ALS integration time step size ```t_step``` in */
#define APDS99XX_CNTS_P_STEP (1025)  /**< Counts per step ```cnts_p_step``` */

#else
#error "Please provide a valid aps99xx variant (apds9900, apds9901, adps9930, apds9950, apds9960)"
#endif
/** @} */

/**
 * @brief   APDS99XX I2C addresses
 */
#define APDS99XX_I2C_ADDRESS    (0x39)

/**
 * @brief   Definition of error codes
 */
typedef enum {
    APDS99XX_OK,                /**< success */
    APDS99XX_ERROR_I2C,         /**< I2C communication error */
    APDS99XX_ERROR_WRONG_ID,    /**< wrong id read */
    APDS99XX_ERROR_NO_DATA,     /**< no data are available */
    APDS99XX_ERROR_RAW_DATA,    /**< reading raw data failed */
} apds99xx_error_codes_t;

/**
 * @brief   Ambient light sensing (ALS) gain
 */
typedef enum {
    APDS99XX_ALS_GAIN_1 = 0, /**<   1 x gain (default) */
    APDS99XX_ALS_GAIN_8,     /**<   8 x gain */
    APDS99XX_ALS_GAIN_16,    /**<  16 x gain */
    #if MODULE_APDS9950 || MODULE_APDS9960 || DOXYGEN
    APDS99XX_ALS_GAIN_64,    /**<  64 x gain (APDS9950, APDS9960 only) */
    #endif /* MODULE_APDS9950 || MODULE_APDS9960 || DOXYGEN */
    #if MODULE_APDS9900 || MODULE_APDS9901 || APDS9930 || DOXYGEN
    APDS99XX_ALS_GAIN_120,   /**< 120 x gain (APDS9900, APDS9901, APDS9930 only) */
    #endif /*   MODULE_APDS9900 || MODULE_APDS9901 || APDS9930 || DOXYGEN */
} apds99xx_als_gain_t;

/**
 * @brief   Proximity sensing (PRX) gain
 */
typedef enum {
    APDS99XX_PRX_GAIN_1 = 0,    /**< 1 x gain (default) */
    #if MODULE_APDS9930 || MODULE_APDS9960 || DOXYGEN
    APDS99XX_PRX_GAIN_2,        /**< 2 x gain (APDS9930, APDS9960 only) */
    APDS99XX_PRX_GAIN_4,        /**< 4 x gain (APDS9930, APDS9960 only) */
    APDS99XX_PRX_GAIN_8,        /**< 8 x gain (APDS9930, APDS9960 only) */
    #endif /* MODULE_APDS9930 || MODULE_APDS9960 || DOXYGEN */
} apds99xx_prx_gain_t;

/**
 * @brief   Proximity sensing (PRX) LED drive strength
 */
typedef enum {
    APDS99XX_PRX_DRIVE_100 = 0,  /**< 100.0 mA (default) */
    APDS99XX_PRX_DRIVE_50,       /**<  50.0 mA */
    APDS99XX_PRX_DRIVE_25,       /**<  25.0 mA */
    APDS99XX_PRX_DRIVE_12_5,     /**<  12.5 mA */
} apds99xx_prx_drive_t;

/**
 * @brief   APDS99XX device initialization parameters
 */
typedef struct {

    unsigned dev;          /**< I2C device (default I2C_DEV(0)) */

    uint16_t als_steps;             /**< ALS integration time in steps. If 0,
                                         ALS is disabled. (default 64) */
    apds99xx_als_gain_t  als_gain;  /**< Gain used for ALS.
                                         (default #APDS99XX_ALS_GAIN_1) */

    uint8_t prx_pulses;             /**< IR LED pulses for proximity sensing.
                                         If 0, proximity sensing is disabled.
                                         (default 8 as recommended) */
    apds99xx_prx_drive_t prx_drive; /**< IR LED current for proximity sensing
                                         (default #APDS99XX_PRX_DRIVE_100) */
    apds99xx_prx_gain_t  prx_gain;  /**< Gain used for proximity sensing.
                                         (default #APDS99XX_PRX_GAIN_1) */

    uint16_t wait_steps;    /**< Waiting time in steps. If 0, waiting is
                                 disabled. (default 0) */
    bool     wait_long;     /**< Long waiting time. If true, waitng time is
                                 increased by a factor 12. (default false) */

    gpio_t  int_pin;        /**< interrupt pin: #GPIO_UNDEF if not used */

} apds99xx_params_t;

#if MODULE_APDS99XX_FULL || DOXYGEN

/**
 * @brief   Interrupt configuration
 *
 * The interrupt enable flags apds99xx_int_config_t::als_int_en and
 * apds99xx_int_config_t::prx_int_en control whether ALS and proximity sensor
 * interrupts are enable.
 *
 * The persistence values apds99xx_int_config_t::als_pers and
 * apds99xx_int_config_t::prx_pers specify how many ALS or proximity
 * values have to be outside of the thresholds defined by
 * apds99xx_int_config_t::als_thresh_low and
 * apds99xx_int_config_t::als_thresh_high or
 * apds99xx_int_config_t::prx_thresh_low and
 * apds99xx_int_config_t::prx_thresh_high.
 *
 * @note If the persistence values are 0, an interrupt is generated in each
 * cycle at the end of the corresponding measurement step, regardless of the
 * values and the defined threshold. The corresponding interrupt is thus used
 * as a data-ready interrupt.
 */
typedef struct {
    bool als_int_en;    /**< ALS interrupt enabled */
    uint8_t als_pers;   /**< Number of consecutive ALS values that have to be
                             outside the thresholds to generate an interrupt:
                             Value   | Interrupt is generated
                             --------| -------
                             0       | every cycle (ALS data-ready interrupt)
                             1...15  | when n values are outside the thresholds */
    uint16_t als_thresh_low;  /**< Low threshold value for ALS interrupts */
    uint16_t als_thresh_high; /**< High threshold value for ALS interrupts */

    bool prx_int_en;    /**< Proximity interrupt enabled */
    uint8_t prx_pers;   /**< Number of consecutive proximity values that have
                             to be outside the thresholds to generate an
                             interrupt:
                             Value   | Interrupt is generated
                             --------| -------
                             0       | every cycle (PRX data-ready interrupt)
                             1, 2, 3 | when 1, 2, or 3 values are outside the thresholds
                             4...15  | when (n - 3) * 5 values are outside the thresholds */

    uint16_t prx_thresh_low;  /**< Low threshold for proximity values
                                   (only the low byte is used for APDS9960) */
    uint16_t prx_thresh_high; /**< High threshold for proximity values
                                   (only the low byte is used for APDS9960) */

} apds99xx_int_config_t;

/**
 * @brief   Interrupt source
 *
 * The type is used to
 */
typedef struct {
    bool als_int;    /**< ALS interrupt happened */
    bool prx_int;    /**< Proximity interrupt happened */
} apds99xx_int_source_t;

/**
 * @brief   Interrupt service routine function prototype
 */
typedef void (*apds99xx_isr_t)(void *arg);

#endif /* MODULE_APDS99XX_FULL */

/**
 * @brief   APDS99XX sensor device data structure type
 */
typedef struct {

    apds99xx_params_t params;          /**< device initialization parameters */

#if MODULE_APDS99XX_FULL || DOXYGEN
    apds99xx_isr_t isr;                /**< user ISR */
    void* isr_arg;                     /**< user ISR argument */

    bool gpio_init;                    /**< GPIO is already initialized */
#endif /* MODULE_APDS99XX_FULL */

} apds99xx_t;

#if MODULE_APDS9950 || MODULE_APDS9960 || DOXYGEN
/**
 * @brief   RGB count value data structure (APDS9950 and APDS9960 only)
 */
typedef union {

    struct {
        uint16_t    red;    /**< R photodiode count value (red) */
        uint16_t    green;  /**< G photodiode count value (green) */
        uint16_t    blue;   /**< B photodiode count value (blue) */
    };
    uint16_t val[3];        /**< RGB count values as array */

} apds99xx_rgb_t;
#endif

/**
 * @brief   Initialize the APDS99XX sensor device
 *
 * This function resets the sensor and initializes the sensor according to
 * given initialization parameters. All registers are reset to default values.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor to be initialized
 * @param[in]   params  configuration parameters, see #apds99xx_params_t
 *
 * @retval  APDS99XX_OK      on success
 * @retval  APDS99XX_ERROR_* a negative error code on error,
 *                           see #apds99xx_error_codes_t
 */
int apds99xx_init(apds99xx_t *dev, const apds99xx_params_t *params);

/**
 * @brief   Ambient light sensing (ALS) data-ready status function
 *
 * The function reads the ALS valid flag in status register and returns.
 * It can be used for polling new ambient light sensing data.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 *
 * @retval  APDS99XX_OK             new abmient light data available
 * @retval  APDS99XX_ERROR_NO_DATA  no new abmient light data available
 * @retval  APDS99XX_ERROR_*        negative error code,
 *                                  see #apds99xx_error_codes_t
 */
int apds99xx_data_ready_als(const apds99xx_t *dev);

/**
 * @brief   Read one raw data sample of ambient light sensing (ALS)
 *
 * Raw ambient light sensing (ALS) data are available as 16-bit count values
 * (cnt_als). The range of these count values depends on the ALS integration
 * time apds99xx_params_t::als_steps and the ALS gain
 * apds99xx_params_t::als_gain. The maximum count value (cnt_als_max) is:
 *
 *      cnt_als_max = APDS99XX_CNTS_P_STEP * als_steps * als_gain
 *
 * If there are no new data ready to read, last valid data sample is returned.
 * Function #apds99xx_data_ready_als could be used to check whether new data
 * are available before this function is called.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 * @param[out]  raw     raw ambient light sensing data as count value
 *
 * @retval  APDS99XX_OK         on success
 * @retval  APDS99XX_ERROR_*    negative error code,
 *                              see #apds99xx_error_codes_t
 */
int apds99xx_read_als_raw(const apds99xx_t *dev, uint16_t *raw);

#if MODULE_APDS9900 || MODULE_APDS9901 || MODULE_APDS9930 || DOXYGEN
/**
 * @brief   Read one data sample of illuminance in lux
 *
 * Illuminance in lux is computed from raw ambient light sensing (ALS) data
 * which are measured in counts. Since the range of ALS data depend on ALS
 * integration time apds99xx_params_t::als_steps and the ALS gain
 * apds99xx_params_t::als_gain, these parameters also affect the sensitivity
 * of the illuminance.
 *
 * @note This function is only available for APDS9900, APDS9901 and APD9930.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 * @param[out]  lux     illuminance in lux
 *
 * @retval  APDS99XX_OK         on success
 * @retval  APDS99XX_ERROR_*    negative error code,
 *                              see #apds99xx_error_codes_t
 */
int apds99xx_read_illuminance(const apds99xx_t *dev, uint16_t *lux);

#endif /* MODULE_APDS9900 || MODULE_APDS9901 || MODULE_APDS9930 || DOXYGEN */

#if MODULE_APDS9950 || MODULE_APDS9960 || DOXYGEN
/**
 * @brief   Read one raw RGB color data sample (APDS9950 and APDS9960 only)
 *
 * In APDS9950 and APDS9960 sensors, ambient light sensing (ALS) also detects
 * spectral components of the light as RGB count values. This function can
 * be used to fetch raw RGB data.
 *
 * Raw RGB data are available as 16-bit count values (cnt_als).
 * The range of these count values depends on the ALS integration time
 * apds99xx_params_t::als_steps and the ALS gain apds99xx_params_t::als_gain.
 * The maximum count value (cnt_rgb_max) is:
 *
 *      cnt_rgb_max = APDS99XX_CNTS_P_STEP * als_steps * als_gain
 *
 * If there are no data ready to read, last valid data sample is returned.
 * Function #apds99xx_data_ready_als could be used to check whether new data
 * are available before this function is called.
 *
 * @note This function is only available for APDS9950 and APD9960.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 * @param[out]  rgb     RGB color data sample as count values
 *
 * @retval  APDS99XX_OK         on success
 * @retval  APDS99XX_ERROR_*    negative error code,
 *                              see #apds99xx_error_codes_t
 */
int apds99xx_read_rgb_raw(const apds99xx_t *dev, apds99xx_rgb_t *rgb);

#endif /* MODULE_APDS9950 || MODULE_APDS9960 || DOXYGEN */

/**
 * @brief   Proximity sensing (PRX) data-ready status function
 *
 * The function reads the proximity valid flag in status register and returns.
 * It can be used for polling new proximity sensing data.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 *
 * @retval  APDS99XX_OK             new proximity data available
 * @retval  APDS99XX_ERROR_NO_DATA  no new proximity data available
 * @retval  APDS99XX_ERROR_*        negative error code,
 *                                  see #apds99xx_error_codes_t
 */
int apds99xx_data_ready_prx(const apds99xx_t *dev);

/**
 * @brief   Read one data sample of proximity sensing (PRX)
 *
 * Proximity data samples are given as a 16-bit count values (cnt_prx).
 *
 * @note APDS9960 returns only 8-bit values in the range of 0 to 255.
 *
 * The range of the count values depends on the PRX LED drive strength
 * apds99xx_params_t::prx_drive the PRX gain apds99xx_params_t::prx_gain,
 * and if used, the PRX integration time apds99xx_params_t::prx_time.
 *
 * @note A number of disturbing effects such as DC noise, sensor coverage,
 * or surrounding objects cause an offset in the measured proximity values.
 * The application should remove this offset, for example, by finding the
 * minimum value ever measured and subtracting it from the current reading.
 * The minimum value is then assumed to be 0 (no proximity).
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 * @param[out]  prx     proximity sensing data as count value
 *
 * @retval  APDS99XX_OK      on success
 * @retval  APDS99XX_ERROR_* a negative error code on error,
 *                           see #apds99xx_error_codes_t
 */
int apds99xx_read_prx_raw(const apds99xx_t *dev, uint16_t *prx);

/**
 * @brief   Power down the sensor
 *
 * The sensor is switched into sleep mode. It remains operational on the I2C
 * interface. Depending on the sensor used, it consumes only about 1 to 3 uA
 * in this mode.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 *
 * @retval  APDS99XX_OK      on success
 * @retval  APDS99XX_ERROR_* a negative error code on error,
 *                           see #apds99xx_error_codes_t
 */
int apds99xx_power_down(const apds99xx_t *dev);

/**
 * @brief   Power up the sensor
 *
 * The sensor is woken up from sleep mode.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 *
 * @retval  APDS99XX_OK      on success
 * @retval  APDS99XX_ERROR_* a negative error code on error,
 *                           see #apds99xx_error_codes_t
 */
int apds99xx_power_up(const apds99xx_t *dev);

#if MODULE_APDS99XX_FULL || DOXYGEN

/**
 * @brief   Configure the interrupts of the sensor
 *
 * The function configures the interrupts of the sensor and sets the ISR
 * as well as its argument for handling the interrupts.
 *
 * If any interrupt is enabled by the configuration
 * (apds99xx_int_config_t::als_int_en or apds99xx_int_config_t::als_int_en are
 * set), the function
 *
 * - initializes the GPIO defined by apds99xx_params_t::int_pin, and
 * - attaches the ISR specified by the @p isr parameter to the interrupt.
 *
 * @note Since the ISR is executed in the interrupt context, it must not be
 * blocking or time consuming. In addition, it must not access the sensor
 * directly via I2C. It should only indicate to a waiting thread that an
 * interrupt has occurred, which is then handled in the thread context.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 * @param[in]   cfg     interrupt configuration, see #apds99xx_int_config_t
 * @param[in]   isr     ISR called for all types of interrupts
 * @param[in]   isr_arg ISR argument, can be NULL
 *
 * @retval  APDS99XX_OK         on success
 * @retval  APDS99XX_ERROR_*    negative error code on error,
 *                              see #apds99xx_error_codes_t
 */
int apds99xx_int_config(apds99xx_t *dev, apds99xx_int_config_t* cfg,
                        apds99xx_isr_t isr, void *isr_arg);

/**
 * @brief   Get the source of an interrupt
 *
 * The function clears the interrupt signal and returns the source of the
 * interrupt. Since the interrupt signal is only reset by this function,
 * it must be executed to reset the interrupt signal, even if the type of the
 * triggered interrupt is not of interest.
 *
 * @note It must not be called from the ISR to avoid I2C bus accesses in
 * the interrupt context.
 *
 * @param[in]   dev     device descriptor of APDS99XX sensor
 * @param[out]  src     interrupt source, see #apds99xx_int_source_t
 *
 * @retval  APDS99XX_OK         on success
 * @retval  APDS99XX_ERROR_*    negative error code on error,
 *                              see #apds99xx_error_codes_t
 */
int apds99xx_int_source(apds99xx_t *dev, apds99xx_int_source_t* src);

#endif /* MODULE_APDS99XX_FULL */

#ifdef __cplusplus
}
#endif

#endif /* APDS99XX_H */
/** @} */
