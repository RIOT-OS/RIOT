/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l3gd20h
 * @brief       Device Driver for ST L3GD20H 3-axis digital output gyroscope
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef L3GD20H_H
#define L3GD20H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/spi.h"

/**
  * @name L3GD20H addresses
  * @{
  */
#define L3GD20H_I2C_ADDRESS_1   (0x6a)    /**< SDO pin is low */
#define L3GD20H_I2C_ADDRESS_2   (0x6b)    /**< SDO pin is high */
/** @} */

/**
  * @name L3GD20 addresses
  * @{
  */
#define L3GD20_I2C_ADDRESS_1    (0x6a)    /**< SDO pin is low */
#define L3GD20_I2C_ADDRESS_2    (0x6b)    /**< SDO pin is high */
/** @} */

/**
  * @name L3G4200D addresses
  * @{
  */
#define L3G4200D_I2C_ADDRESS_1  (0x68)    /**< SDO pin is low */
#define L3G4200D_I2C_ADDRESS_2  (0x69)    /**< SDO pin is high */
/** @} */

/** L3GD20H chip id */
#define L3GD20H_CHIP_ID         (0xd7)  /**< L3GD20H_REG_WHO_AM_I<7:0> */

/** L3GD20 chip id */
#define L3GD20_CHIP_ID          (0xd4)  /**< L3GD20H_REG_WHO_AM_I<7:0> */

/** L3G4200D chip id */
#define L3G4200D_CHIP_ID        (0xd3)  /**< L3GD20H_REG_WHO_AM_I<7:0> */

/** Definition of error codes */
typedef enum {
    L3GD20H_OK,                   /**< success */
    L3GD20H_ERROR_I2C,            /**< I2C communication error */
    L3GD20H_ERROR_SPI,            /**< SPI communication error */
    L3GD20H_ERROR_WRONG_CHIP_ID,  /**< wrong chip id read from WHO_AM_I reg */
    L3GD20H_ERROR_INV_MODE,       /**< sensor mode is invalid or not available */
    L3GD20H_ERROR_INV_FIFO_MODE,  /**< FIFO mode is invalid or not available */
    L3GD20H_ERROR_INV_INT_TYPE,   /**< invalid interrupt type used */
    L3GD20H_ERROR_NO_NEW_DATA,    /**< no new data are available */
    L3GD20H_ERROR_RAW_DATA,       /**< reading raw data failed */
    L3GD20H_ERROR_RAW_DATA_FIFO,  /**< reading raw data from FIFO failed */
    L3GD20H_ERROR_NO_INT1_PIN,    /**< INT1 signal pin not configured */
    L3GD20H_ERROR_NO_INT2_PIN,    /**< INT2/DRDY signal pin not configured */
    L3GD20H_ERROR_BYPASS_MODE,    /**< sensor is in bypass mode */
    L3GD20H_ERROR_FIFO_MODE,      /**< sensor is in FIFO mode */
} l3gd20h_error_codes_t;

/**
 * @brief   Sensor modes with output data rates (ODR) and LPF2 cutoff frequencies
 *
 * The sensor mode determines used output data rate (ODR) and the LPF2 cutoff
 * frequency as following:
 *
 * Mode | ODR [Hz] | LPF1 cutoff [Hz] | LPF2 cutoff [Hz]
 * ---- |:--------:|:----------------:|:---------------:
 * L3GD20H_MODE_100_12  | 100  | 32   | 12.5
 * L3GD20H_MODE_100_25  | 100  | 32   | 25
 * L3GD20H_MODE_200_12  | 200  | 63.3 | 12.5
 * L3GD20H_MODE_200     | 200  | 63.3 | -
 * L3GD20H_MODE_200_70  | 200  | 63.3 | 70
 * L3GD20H_MODE_400_20  | 400  | 128  | 20
 * L3GD20H_MODE_400_25  | 400  | 128  | 25
 * L3GD20H_MODE_400_50  | 400  | 128  | 50
 * L3GD20H_MODE_400_110 | 400  | 128  | 110
 * L3GD20H_MODE_800_30  | 400  | 211  | 30
 * L3GD20H_MODE_800_35  | 400  | 211  | 35
 * L3GD20H_MODE_800     | 400  | 211  | -
 * L3GD20H_MODE_800_100 | 400  | 211  | 100
 * L3GD20H_MODE_12      | 12.5 | 3.9  | -
 * L3GD20H_MODE_25      | 25   | 7.8  | -
 * L3GD20H_MODE_50_16   | 50   | 16   | 16.6
 *
 * @note While LPF1 is always used, LPF2 has to be enabled with function
 * #l3gd20h_select_output_filter.
 */
typedef enum {

    L3GD20H_MODE_100_12  = 0x00,  /**< high ODR 100 Hz, LPF1 cutoff 32 Hz,   LPF2 cutoff 12.5 Hz */
    L3GD20H_MODE_100_25  = 0x01,  /**< high ODR 100 Hz, LPF1 cutoff 32 Hz,   LPF2 cutoff 25 Hz */
    L3GD20H_MODE_200_12  = 0x04,  /**< high ODR 200 Hz, LPF1 cutoff 63.3 Hz, LPF2 cutoff 12.5 Hz */
    L3GD20H_MODE_200     = 0x05,  /**< high ODR 200 Hz, LPF1 cutoff 63.3 Hz, LPF2 not used */
    L3GD20H_MODE_200_70  = 0x07,  /**< high ODR 200 Hz, LPF1 cutoff 63.3 Hz, LPF2 cutoff 70 Hz */
    L3GD20H_MODE_400_20  = 0x08,  /**< high ODR 400 Hz, LPF1 cutoff 128 Hz,  LPF2 cutoff 20 Hz */
    L3GD20H_MODE_400_25  = 0x09,  /**< high ODR 400 Hz, LPF1 cutoff 128 Hz,  LPF2 cutoff 25 Hz */
    L3GD20H_MODE_400_50  = 0x0a,  /**< high ODR 400 Hz, LPF1 cutoff 128 Hz,  LPF2 cutoff 50 Hz */
    L3GD20H_MODE_400_110 = 0x0b,  /**< high ODR 400 Hz, LPF1 cutoff 128 Hz,  LPF2 cutoff 110 Hz */
    L3GD20H_MODE_800_30  = 0x0c,  /**< high ODR 400 Hz, LPF1 cutoff 211 Hz,  LPF2 cutoff 30 Hz */
    L3GD20H_MODE_800_35  = 0x0d,  /**< high ODR 400 Hz, LPF1 cutoff 211 Hz,  LPF2 cutoff 35 Hz */
    L3GD20H_MODE_800     = 0x0e,  /**< high ODR 400 Hz, LPF1 cutoff 211 Hz,  LPF2 not used */
    L3GD20H_MODE_800_100 = 0x0f,  /**< high ODR 400 Hz, LPF1 cutoff 211 Hz,  LPF2 cutoff 100 Hz */

    L3GD20H_MODE_12      = 0x10,  /**< low ODR 12.5 Hz, LPF1 cutoff 3.9 Hz, LPF2 not used */
    L3GD20H_MODE_25      = 0x14,  /**< low ODR 25 Hz,   LPF1 cutoff 7.8 Hz, LPF2 not used */
    L3GD20H_MODE_50_16   = 0x18,  /**< low ODR 50 Hz,   LPF1 cutoff 16 Hz,  LPF2 cutoff 16.6 Hz */

    L3GD20H_MODE_POWER_DOWN  = 0xff,   /**< power down mode */

} l3gd20h_mode_t;

/**
 * @brief   Sensitivity level in degrees per second (DPS)
 */
typedef enum {
    L3GD20H_SCALE_245_DPS = 0,  /**< 245 dps (default) */
    L3GD20H_SCALE_500_DPS,      /**< 500 dps */
    L3GD20H_SCALE_2000_DPS      /**< 2000 dps */
} l3gd20h_scale_t;

/**
 * @brief   FIFO mode
 *
 * The integrated FIFO with up to 32 data samples can be used in different
 * modes. The mode defines the behavior of FIFO when it becomes full.
 *
 * Mode   | Description
 * ------ | ---------
 * Bypass | FIFO is not used at all
 * FIFO   | Data samples are stored in the FIFO until it is full
 * Stream | FIFO is used as ring buffer and newest data samples are stored continuously
 * Stream-to-FIFO   | FIFO is used in Stream mode until an interrupt, switches then to FIFO mode
 * Bypass-to-Stream | FIFO is not used until an interrupt, switches then to Stream mode
 * Dynamic Stream   | like Stream mode, but differs in reading the first data sample after emptying
 * Bypass-to-FIFO   | FIFO is not used until an interrupt, switches then to FIFO mode
 */
typedef enum {
    L3GD20H_BYPASS = 0,           /**< bypass the FIFO (default) */
    L3GD20H_FIFO = 1,             /**< normal FIFO used until it is full */
    L3GD20H_STREAM = 2,           /**< continuous FIFO used as ring buffer */
    L3GD20H_STREAM_TO_FIFO = 3,   /**< switch from Stream to FIFO mode on event */
    L3GD20H_BYPASS_TO_STREAM = 5, /**< switch from Bypass to Stream mode on event */
    L3GD20H_DYNAMIC_STREAM = 6,   /**< dynamic Stream mode */
    L3GD20H_BYPASS_TO_FIFO = 7    /**< switch from Bypass to FIFO mode on event */
} l3gd20h_fifo_mode_t;

/**
 * @brief   High pass filter (HPF) and low pass filter 2 (LPF2) modes
 *
 * The L3GD20H integrates a combination of two low pass filters (LPF) and
 * one high pass filter (HPF).
 *
 * First, raw sensor data are always filtered by LPF1 at a cutoff frequency
 * which is fixed for the selected output data rate (ODR), see #l3gd20h_mode_t.
 * The resulting data can then optionally be filtered by HPF and LPF2. Both
 * filters can be used or bypassed.
 *
 * This type defines the possible filter combinations that can be
 * set separately for the sensor output data and the output data for
 * interrupt generation. The filter selection for output data is set
 * with function #l3gd20h_select_output_filter.
 *
 * The cutoff frequency of LPF2 is determined by used sensor
 * mode #l3gd20h_mode_t, while the cutoff frequency of HPF is set with
 * function #l3gd20h_config_hpf.
 */
typedef enum {
    L3GD20H_NO_FILTER = 0,  /**< HPF not used, LPF2 not used (default) */
    L3GD20H_HPF_ONLY,       /**< HPF used, LPF2 not used */
    L3GD20H_LPF2_ONLY,      /**< HPF not used, LPF2 used */
    L3GD20H_HPF_AND_LPF2    /**< HPF used, LPF2 used */
} l3gd20h_filter_sel_t;

/**
 * @brief   HPF (high pass filter) modes
 *
 * The high pass filter can be used in different modes.
 *
 * Mode      | Description
 * --------- | -----------
 * Normal    | HPF is reset by reading the REFERENCE register
 * Reference | output data are calculated as the difference between the input register REFERENCE
 * Autoreset | HPF is automatically reset when the configured interrupt event occurs.
 */
typedef enum {
    L3GD20H_HPF_NORMAL    = 0, /**< Normal mode, reset by reading REFERENCE */
    L3GD20H_HPF_REFERENCE = 1, /**< Reference signal for filtering */
    L3GD20H_HPF_AUTORESET = 3  /**< Autoreset on interrupt event */
} l3gd20h_hpf_mode_t;

/**
 * @brief   Interrupt types
 *
 * The LG3D20H suports different types of interrupts.
 */
typedef enum {

    L3GD20H_INT_DATA_READY,     /**< data are ready to read (INT2/DRDY) */

    L3GD20H_INT_FIFO_THRESHOLD, /**< FIFO filling exceds FTH level (INT2/DRDY) */
    L3GD20H_INT_FIFO_OVERRUN,   /**< FIFO is completely filled (INT2/DRDY) */
    L3GD20H_INT_FIFO_EMPTY,     /**< FIFO becomes empty (INT2/DRDY) */

    L3GD20H_INT_EVENT           /**< angular rate of one or more axes becomes
                                     lower or higher than threshold (INT1) */
} l3gd20h_int_types_t;

/**
 * @brief   Event interrupt generator configuration (axis movement and wake up)
 *
 * memset to 0 to disable all interrupt conditions (default)
 */
typedef struct {
    uint16_t x_threshold;    /**< x threshold value */
    uint16_t y_threshold;    /**< y threshold value */
    uint16_t z_threshold;    /**< z threshold value */

    bool x_low_enabled;      /**< x lower  than threshold interrupt enabled */
    bool x_high_enabled;     /**< x higher than threshold interrupt enabled */

    bool y_low_enabled;      /**< y lower  than threshold interrupt enabled */
    bool y_high_enabled;     /**< y higher than threshold interrupt enabled */

    bool z_low_enabled;      /**< z lower  than threshold interrupt enabled */
    bool z_high_enabled;     /**< z higher than threshold interrupt enabled */

    l3gd20h_filter_sel_t filter; /**< HPF and LPF2 filter selection used
                                      for threshold comparison */

    bool and_or;             /**< interrupt combination true=AND, false=OR
                                  AND: all enabled axes passed the treshold
                                  OR: at least one axes passed the threshold */

    bool latch;              /**< latch the interrupt when true until the
                                  interrupt source has been read */

    uint8_t duration;        /**< duration in 1/ODR an interrupt condition
                                  has to be given before the interrupt is
                                  generated */

    bool wait;               /**< when true, duration is also used when
                                  interrupt condition in no longer given
                                  before interrupt signal is reset */

    bool counter_mode;       /**< DCRM is not documented and not used
                                  therefore */
} l3gd20h_int_event_config_t;

/**
 * @brief   Event interrupt source (axis movement and wake up)
 */
typedef struct {

    bool x_low : 1; /**< true on x lower  than threshold event */
    bool x_high: 1; /**< true on x higher than threshold event */

    bool y_low : 1; /**< true on z lower  than threshold event */
    bool y_high: 1; /**< true on z higher than threshold event */

    bool z_low : 1; /**< true on z lower  than threshold event */
    bool z_high: 1; /**< true on z higher than threshold event */

    bool active: 1; /**< true when one ore more events have been generated */

} l3gd20h_int_event_source_t;

/**
 * @brief   Data interrupt source type (data ready and FIFO status)
 */
typedef struct {

    bool data_ready;        /**< true when data are ready to read */

    bool fifo_threshold;    /**< true when FIFO filling >= FTH level */
    bool fifo_overrun;      /**< true when FIFO is completely filled */
    bool fifo_empty;        /**< true when FIFO is empty */

} l3gd20h_int_data_source_t;

/**
 * @brief   INT1, INT2/DRDY sensor signal activity level
 */
typedef enum {

    L3GD20H_HIGH = 0,       /**< INT signals are High active (default) */
    L3GD20H_LOW             /**< INT signals are Low active */

} l3gd20h_int_pin_level_t;

/**
 * @brief   INT1, INT2/DRDY sensor signal type
 */
typedef enum {

    L3GD20H_PUSH_PULL = 0,  /**< INT pins push/pull outputs (default) */
    L3GD20H_OPEN_DRAIN      /**< INT pins open-drain (OD) */

} l3gd20h_int_pin_type_t;

/**
 * @brief   Raw data set as two complements
 */
typedef struct {

    int16_t x; /**< x angular rate (roll) as as 16 bit two's complements */
    int16_t y; /**< y angular rate (pitch) as as 16 bit two's complements */
    int16_t z; /**< z angular rate (yaw) as as 16 bit two's complements */

} l3gd20h_raw_data_t;

/**
 * @brief   Angular rates in milli-degrees per seconds (mdps)
 */
typedef struct {

    int32_t x; /**< x angular rate (roll) */
    int32_t y; /**< y angular rate (pitch) */
    int32_t z; /**< z angular rate (yaw) */

} l3gd20h_data_t;

/**
 * @brief   Raw data FIFO type
 */
typedef l3gd20h_raw_data_t l3gd20h_raw_data_fifo_t[32];

/**
 * @brief   Angular rates FIFO type
 */
typedef l3gd20h_data_t l3gd20h_data_fifo_t[32];

/**
 * @brief   L3GD20H device initialization parameters
 */
typedef struct {

    #if MODULE_L3GD20H_I2C
    unsigned  dev;      /**< I2C device */
    uint8_t   addr;     /**< I2C slave address */
    #endif

    #if MODULE_L3GD20H_SPI
    unsigned  dev;      /**< SPI device */
    spi_clk_t clk;      /**< SPI clock */
    gpio_t    cs;       /**< SPI chip select pin */
    #endif

    gpio_t int1;        /**< INT1 pin: #GPIO_UNDEF if INT1 is not used */
    gpio_t int2;        /**< INT2/DRDY pin: #GPIO_UNDEF if INT2 is not used */

    l3gd20h_mode_t  mode;   /**< data rate (default #L3GD20H_MODE_100_25)*/
    l3gd20h_scale_t scale;  /**< full scale (default #L3GD20H_SCALE_245_DPS) */

    l3gd20h_fifo_mode_t fifo_mode;      /**< FIFO operation mode (default #L3GD20H_BYPASS) */

    l3gd20h_int_pin_level_t int_level;  /**< INT pin activity level (default #L3GD20H_HIGH) */
    l3gd20h_int_pin_type_t  int_type;   /**< INT pin type (default #L3GD20H_PUSH_PULL) */

} l3gd20h_params_t;

/**
 * @brief   L3GD20H sensor device data structure type
 */
typedef struct {

    l3gd20h_params_t params;    /**< device initialization parameters */

    enum {                      /**< used sensor */
        L3GD20H,
        L3GD20,
        l3G4200D
    } sensor;                   /**< recognized sensor type */

} l3gd20h_t;

/**
 * @name    Sensor initialization and configuration
 * @{
 */

/**
 * @brief	Initialize the L3GD20H sensor device
 *
 * This function resets the sensor and initializes the sensor according to
 * given intialization parameters. All registers are reset to default values.
 * The FIFO is cleared.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor to be initialized
 * @param[in]   params  L3GD20H initialization parameters
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_init(l3gd20h_t *dev, const l3gd20h_params_t *params);

/**
 * @brief   Set sensor mode
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[in]   mode    sensor mode with given ODR and Cut-off bandwidth
 * @param[in]   x       enable x-axis if true
 * @param[in]   y       enable y-axis if true
 * @param[in]   z       enable z-axis if true
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_set_mode(l3gd20h_t *dev,
                     l3gd20h_mode_t mode, bool x, bool y, bool z);

/**
 * @brief   Set scale (full scale)
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[in]   scale   fulle scale
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_set_scale(l3gd20h_t *dev, l3gd20h_scale_t scale);

/** @} */

/**
 * @name    Power saving functions
 * @{
 */
/**
 * @brief   Power down the sensor
 *
 * Changes the sensor operation mode to #L3GD20H_MODE_POWER_DOWN in which
 * almost all internal blocks are switched off. I2C and SPI interface are
 * still active. The content of the configuration registers is preserved.
 *
 * @param[in]   dev     Device descriptor of L3GD20H device to read from
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_power_down (l3gd20h_t *dev);

/**
 * @brief   Power up the sensor
 *
 * Swichtes the sensor back into the last active operation mode. It takes
 * up to 100 ms since the gyros have to be switched on.
 *
 * @param[in]   dev     Device descriptor of L3GD20H device to read from
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_power_up (l3gd20h_t *dev);

/**
 * @brief   Sleep mode
 *
 * Activates the sleep mode of the sensor. That is, it deactivates the
 * measurement for all axes but the gyros are kept switched on. To return
 * from sleep mode, function #l3gd20h_wake_up is used. It takes only
 * 1/ODR when LPF2 is disabled and 6/ODR when LPF2 is enabled to
 * continue measurements.
 *
 * @param[in]   dev     Device descriptor of L3GD20H device to read from
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_sleep (l3gd20h_t *dev);

/**
 * @brief   Wake up the sensor
 *
 * Swichtes the sensor back into the last active operation mode. It takes only
 * 1/ODR when LPF2 is disabled and 6/ODR when LPF2 is enabled to
 * continue measurements.
 *
 * @param[in]   dev     Device descriptor of L3GD20H device to read from
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_wake_up (l3gd20h_t *dev);

/** @} */

/**
 * @name    Basic sensor data handling
 * @{
 */

/**
 * @brief    Data-ready status function
 *
 * The function returns the number of new available data samples or 0 if no
 * new data samples are available. In default bypass mode (#L3GD20H_BYPASS),
 * the maximum number of available data samples is 1; in the FIFO modes, it
 * is equal to the number of new data samples in the FIFO.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 *
 * @return  number of data samples available for read or negative error code,
 *          see #l3gd20h_error_codes_t
 */
int l3gd20h_data_ready(const l3gd20h_t *dev);

/**
 * @brief   Read one sample of angular rates in milli-degree per second (mpds)
 *
 * Raw gyroscope data are read from the sensor and normalized with respect to
 * the configured full scale of the gyriscope. Angular rates are given in
 * milli-degrees per second (mdps) with the following resolutions:
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[out]  data    result vector in mdps per axis
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_read(const l3gd20h_t *dev, l3gd20h_data_t *data);

/**
 * @brief   Read one sample of raw sensor data as 16 bit two's complements
 *
 * Function works only in bypass mode and fails in FIFO modes. In FIFO modes,
 * function *l3gd20h_get_raw_data_fifo* has to be used instead to get data.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param       raw     raw data vector
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_read_raw(const l3gd20h_t *dev, l3gd20h_raw_data_t *raw);

/** @} */

#if MODULE_L3GD20H_FULL || DOXYGEN

/**
 * @name    FIFO data handling
 * @{
 */

/**
 * @brief   Set FIFO mode
 *
 * The use of the integrated FIFO of the sensor allows to reduce the
 * interaction events of the MCU with the sensor and thus to save power.
 * Data samples can be fetched as a burst if there is a sufficient number of
 * data samples in the FIFO. This number is defined as threshold or
 * watermark and an interrupt can be triggered when number of data samples
 * in the FIFO exceeds this threshold.
 *
 * Futhermore, different FIFO modes can be defined which determine the
 * behavior in the case the FIFO is full, see #l3gd20h_fifo_mode_t.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[in]   mode    FIFO mode
 * @param[in]   thresh  FIFO watermark (ignored in bypass mode)
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_set_fifo_mode(l3gd20h_t *dev,
                          l3gd20h_fifo_mode_t mode, uint8_t thresh);

/**
 * @brief   Get all samples of sensor data stored in the FIFO (unit dps)
 *
 * In bypass mode, it returns only one sensor data sample.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[out]  data    array of 32 data structures
 *
 * @return  number of data samples read on success, or negative error code,
 *          see #l3gd20h_error_codes_t
 */
int l3gd20h_read_fifo(const l3gd20h_t *dev,
                      l3gd20h_data_fifo_t data);

/**
 * @brief   Get all samples of raw sensor data stored in the FIFO
 *
 * In bypass mode, it returns only one raw data sample.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[out]  raw     array of 32 raw data structures
 *
 * @return  number of data samples read on success, or negative error code,
 *          see #l3gd20h_error_codes_t
 */
int l3gd20h_read_raw_fifo(const l3gd20h_t *dev,
                          l3gd20h_raw_data_fifo_t raw);
/** @} */

/**
 * @name    Filter configuration and handling
 * @{
 */

/**
 * @brief   Filter selection for raw data output values
 *
 * L3GD20H supports combination of a high pass filter (HPF) and a second
 * low pass filter (LPF2). This function selects the combination of HPF
 * and LPF2 applied to raw output data-
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[in]   filter  selected filters for output values
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_select_output_filter(const l3gd20h_t *dev,
                                 l3gd20h_filter_sel_t filter);

/**
 * @brief   Config HPF (high pass filter)
 *
 * The cutoff frequency depends on the selected sensor mode and thus the output
 * data rate. The following table shows the possible values of parameter
 * \p cutoff and the cutoff for the according ODRs. All frequencies are given
 * in Hz.
 *
 * cutoff / ODR | 12.5  | 25    | 50    | 100  | 200  | 400  | 800
 * ------------:|:-----:|:-----:|:-----:|:----:|:----:|:----:|:---:
 *  0           | 1     | 2     | 4     | 8    | 15   | 30   | 56
 *  1           | 0.5   | 1     | 2     | 4    | 8    | 15   | 30
 *  2           | 0.2   | 0.5   | 1     | 2    | 4    | 8    | 15
 *  3           | 0.1   | 0.2   | 0.5   | 1    | 2    | 4    | 8
 *  4           | 0.05  | 0.1   | 0.2   | 0.5  | 1    | 2    | 4
 *  5           | 0.02  | 0.05  | 0.1   | 0.2  | 0.5  | 1    | 2
 *  6           | 0.01  | 0.02  | 0.05  | 0.1  | 0.2  | 0.5  | 1
 *  7           | 0.005 | 0.01  | 0.02  | 0.05 | 0.1  | 0.2  | 0.5
 *  8           | 0.002 | 0.005 | 0.01  | 0.02 | 0.05 | 0.1  | 0.2
 *  9           | 0.001 | 0.002 | 0.005 | 0.01 | 0.02 | 0.05 | 0.1
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[in]   mode    high pass filter mode, see #l3gd20h_hpf_mode_t
 * @param[in]   cutoff  cutoff frequency (depends on ODR) [0 ... 9]
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_config_hpf(const l3gd20h_t *dev,
                       l3gd20h_hpf_mode_t mode, uint8_t cutoff);

/**
 * @brief   Set HPF (high pass filter) reference
 *
 * Used to set the reference of HPF in reference mode #L3GD20H_HPF_REFERENCE.
 * and also to reset the HPF in autoreset mode #L3GD20H_HPF_AUTORESET.
 * Reference is given as two's complement.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[in]   ref     reference in #L3GD20H_HPF_REFERENCE mode, otherwise ignored
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_set_hpf_ref(const l3gd20h_t *dev, int8_t ref);

/**
 * @brief   Get HPF (high pass filter) reference
 *
 * Returns the content of the REFERENCE register. In normal mode
 * #L3GD20H_HPF_NORMAL, it is also used to reset the HPF.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[out]  ref     reference
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_get_hpf_ref(const l3gd20h_t *dev, int8_t *ref);
/** @} */

/**
 * @name    Interrupt configuration and handling
 * @{
 */

/**
 * @brief   Enable / disable data or event interrupts on signal INT1, INT2/DRDY
 *
 * This function is used to enable or disable one of the possible
 * interrupt types. By default all interrupts are disabled. The according
 * interrupt signal pin #l3gd20h_params_t::int1 or #l3gd20h_params_t::int2 has
 * to be confgigured, otherwise the function fails.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[in]   type    the interrupt to enabled or disabled
 * @param[in]   enable  enable the interrupt if true, otherwise disable it
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 * @retval  L3GD20H_ERROR_NO_INT1_PIN   INT1 signal pin not configured
 * @retval  L3GD20H_ERROR_NO_INT2_PIN   INT2 signal pin not configured
 */
int l3gd20h_enable_int(const l3gd20h_t *dev,
                       l3gd20h_int_types_t type, bool enable);

/**
 * @brief   Configuration of INT1 and INT2/DRDY interrupt signal outputs
 *
 * This function is used to set the properties of the INT1 and INT2/DRDY
 * interrupt signal outputs of the sensor. Interrupt signal outputs of the
 * sensor can be of type push/pull (#L3GD20H_PUSH_PULL) or open-drain
 * (#L3GD20H_OPEN_DRAIN). The interrupt signals can be high (#L3GD20H_HIGH)
 * or low active (#L3GD20H_LOW).
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[in]   type    define interrupt signal output as push/pull or open drain
 * @param[in]   level   define interrupt signal as high or low active
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_config_int_signals(const l3gd20h_t *dev,
                               l3gd20h_int_pin_type_t type,
                               l3gd20h_int_pin_level_t level);

/**
 * @brief   Set a new configuration of the event interrupt generator
 *
 * The event interrupt generator produces interrupts (axis movement and wake up)
 * on signal INT1 whenever the angular rate of one or more axes becomes higher
 * or lower than defined thresholds.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[in]   config  event interrupt generator configuration (#l3gd20h_int_event_config_t)
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_set_int_event_config(const l3gd20h_t *dev,
                                 const l3gd20h_int_event_config_t *config);

/**
 * @brief   Get the current configuration of the event interrupt generator
 *
 * The event interrupt generator produces interrupts (axis movement and wake up)
 * on signal INT1 whenever the angular rate of one or more axes becomes higher
 * or lower than defined thresholds.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[out]  config  event interrupt generator configuration (#l3gd20h_int_event_config_t)
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_get_int_event_config(const l3gd20h_t *dev,
                                 l3gd20h_int_event_config_t *config);


/**
 * @brief   Get the source of an event interrupt (axis movement and wake up)
 *
 * When an event interrupt is generated at INT1 interrupt signal pin, this
 * function can be used to determine the source of the event interrupt.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[out]  source  interrupt source
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_get_int_event_source(const l3gd20h_t *dev,
                                 l3gd20h_int_event_source_t *source);

/**
 * @brief   Get the source of a data interrupt (data ready or FIFO status)
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param[out]  source  interrupt source
 *
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_get_int_data_source(const l3gd20h_t *dev,
                                l3gd20h_int_data_source_t *source);

/** @} */

#endif /* MODULE_L3GD20H_FULL */

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
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param   reg      address of the first register to be changed
 * @param   data     pointer to the data to be written to the register
 * @param   len      number of bytes to be written to the register
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_reg_write(const l3gd20h_t *dev,
                      uint8_t reg, uint8_t *data, uint16_t len);

/**
 * @brief   Direct read from register
 *
 * @note This function should only be used to do something special that
 * is not covered by the high level interface AND if you exactly know what you
 * do and what effects it might have. Please be aware that it might affect the
 * high level interface.
 *
 * @param[in]   dev     device descriptor of L3GD20H sensor
 * @param   reg      address of the first register to be read
 * @param   data     pointer to the data to be read from the register
 * @param   len      number of bytes to be read from the register
 * @retval  L3GD20H_OK      on success
 * @retval  L3GD20H_ERROR_* negative error code, see #l3gd20h_error_codes_t
 */
int l3gd20h_reg_read(const l3gd20h_t *dev,
                     uint8_t reg, uint8_t *data, uint16_t len);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* L3GD20H_H */
/** @} */
