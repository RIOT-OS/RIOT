/*
 * Copyright (C) 2025 Baptiste Le Duc <baptiste.leduc38@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup   drivers_ads1115 ADS1115 Analog-to-digital converter driver
 * @ingroup    drivers_sensors
 * @ingroup    drivers_saul
 * @brief      I2C Analog-to-digital converter with programmable gain amplifier.
 *             The device has four input channels: AIN0, AIN1, AIN2, and AIN3, that can be selected by using
 *             @ref ads1115_set_ain_ch_input.
 *
 * @file
 * @brief      ADS1115 Analog-to-digital converter driver
 *
 * @author     Baptiste Le Duc <baptiste.leduc38@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"

/**
 * @brief Named return values
 */
enum {
    ADS1115_OK      = 0,        /**< Operation successful */
    ADS1115_NOI2C   = -1,       /**< I2C communication error */
    ADS1115_NODEV   = -2,       /**< No device found on the bus */
    ADS1115_NODATA  = -3,       /**< No data available */
};

/**
 * @brief Operational status bit in write operations
 */
typedef enum {
    ADS1115_WRITE_NO_EFFECT     = 0,    /**< No effect */
    ADS1115_WRITE_SINGLE_SHOT   = 1,    /**< Start a single-shot conversion when in power-down mode */
} ads1115_os_write_t;

/**
 * @brief Operational status bit in read operations
 */
typedef enum {
    ADS1115_READ_BUSY   = 0,    /**<device is currently performing a conversion> */
    ADS1115_READ_IDLE   = 1,    /**< Device is not performing a conversion */
} ads1115_os_read_t;

/**
 * @brief Input multiplexer configuration
 */
typedef enum {
    ADS1115_MUX_AIN0_AIN1   = 0,    /**< AIN0 - AIN1 differential input (AINp = AIN0 && AINn = AIN1) (default) */
    ADS1115_MUX_AIN0_AIN3   = 1,    /**< AIN0 - AIN3 differential input (AINp = AIN0 && AINn = AIN3) */
    ADS1115_MUX_AIN1_AIN3   = 2,    /**< AIN1 - AIN3 differential input (AINp = AIN1 && AINn = AIN3) */
    ADS1115_MUX_AIN2_AIN3   = 3,    /**< AIN2 - AIN3 differential input (AINp = AIN2 && AINn = AIN3) */
    ADS1115_MUX_AIN0        = 4,    /**< AIN0 single-ended input (AINp = AIN0 && AINn = GND) */
    ADS1115_MUX_AIN1        = 5,    /**< AIN1 single-ended input (AINp = AIN1 && AINn = GND) */
    ADS1115_MUX_AIN2        = 6,    /**< AIN2 single-ended input (AINp = AIN2 && AINn = GND) */
    ADS1115_MUX_AIN3        = 7     /**< AIN3 single-ended input (AINp = AIN3 && AINn = GND) */
} ads1115_mux_t;

/**
 * @brief Programmable gain amplifier configuration
 */
typedef enum {
    ADS1115_PGA_6_144V, /**< FSR = ±6.144V */
    ADS1115_PGA_4_096V, /**< FSR = ±4.096V */
    ADS1115_PGA_2_048V, /**< FSR = ±2.048V (default) */
    ADS1115_PGA_1_024V, /**< FSR = ±1.024V */
    ADS1115_PGA_0_512V, /**< FSR = ±0.512V */
    ADS1115_PGA_0_256V, /**< FSR = ±0.256V */

    // Aliases (same behavior)
    ADS1115_PGA_0_256V_B,
    ADS1115_PGA_0_256V_C
} ads1115_pga_t;

/**
 * @brief Device mode
 */
typedef enum {
    ADS1115_MODE_CONTINUOUS = 0,    /**< Continuous conversion mode */
    ADS1115_MODE_SINGLE     = 1     /**< Single-shot mode or power-down mode (default) */
} ads1115_mode_t;

/**
 * @brief Data rate configuration
 */
typedef enum {
    ADS1115_DR_8    = 0,    /**< 8 SPS */
    ADS1115_DR_16   = 1,    /**< 16 SPS */
    ADS1115_DR_32   = 2,    /**< 32 SPS */
    ADS1115_DR_64   = 3,    /**< 64 SPS */
    ADS1115_DR_128  = 4,    /**< 128 SPS (default) */
    ADS1115_DR_250  = 5,    /**< 250 SPS */
    ADS1115_DR_475  = 6,    /**< 475 SPS */
    ADS1115_DR_860  = 7     /**< 860 SPS */
} ads1115_dr_t;

/**
 * @brief Comparator mode
 */
typedef enum {
    ADS1115_COMP_MODE_TRADITIONAL   = 0,    /**< Traditional comparator (default) */
    ADS1115_COMP_MODE_WINDOW        = 1     /**< Window comparator */
} ads1115_comp_mode_t;

/**
 * @brief Comparator polarity
 */
typedef enum {
    ADS1115_COMP_POLARITY_LOW   = 0,    /**< Active low (default) */
    ADS1115_COMP_POLARITY_HIGH  = 1     /**< Active high */
} ads1115_comp_polarity_t;
/**
 * @brief Comparator latch
 */
typedef enum {
    ADS1115_COMP_LATCH_DISABLE  = 0,    /**< Comparator latch disabled (default) */
    ADS1115_COMP_LATCH_ENABLE   = 1     /**< Comparator latch enabled */
} ads1115_comp_latch_t;

/**
 * @brief Comparator queue
 */
typedef enum {
    ADS1115_COMP_QUEUE_1        = 0,    /**< Assert after one conversion */
    ADS1115_COMP_QUEUE_2        = 1,    /**< Assert after two conversions */
    ADS1115_COMP_QUEUE_4        = 2,    /**< Assert after four conversions */
    ADS1115_COMP_QUEUE_DISABLE  = 3     /**< Disable comparator (default) */
} ads1115_comp_queue_t;

/**
 * @brief ADS1115 parameters
 */
typedef struct {
    i2c_t i2c;                              /**< I2C device */
    uint8_t addr;                           /**< I2C address */
    ads1115_mux_t mux;                      /**< Input multiplexer configuration */
    ads1115_pga_t pga;                      /**< Programmable gain amplifier configuration */
    ads1115_mode_t mode;                    /**< Device mode */
    ads1115_dr_t dr;                        /**< Data rate configuration */
    ads1115_comp_mode_t comp_mode;          /**< Comparator mode */
    ads1115_comp_polarity_t comp_polarity;  /**< Comparator polarity */
    ads1115_comp_latch_t comp_latch;        /**< Comparator latch */
    ads1115_comp_queue_t comp_queue;        /**< Comparator queue */
} ads1115_params_t;

/**
 * @brief ADS1115 device descriptor
 */
typedef struct {
    ads1115_params_t params; /**< Device driver configuration */
} ads1115_t;

/**
 * @brief Initializes an ADS1115 device
 *
 * @param[in,out] dev    Device descriptor
 * @param[in] params     Device configuration parameters
 *
 * @return ADS1115_OK on success
 * @return ADS1115_NODEV if no device is found on the bus
 * @return ADS1115_NOI2C if other error occurs
 */
int ads1115_init(ads1115_t *dev, const ads1115_params_t *params);

/**
 * @brief Sets the input channel for the ADS1115 device.
 *
 * @param[in,out] dev    Device descriptor
 * @param[in] mux       Input multiplexer configuration
 *
 * @return ADS1115_OK on success
 * @return ADS1115_NODEV if no device is found on the bus
 * @return ADS1115_NOI2C if other error occurs
 */
int ads1115_set_ain_ch_input(ads1115_t *dev, ads1115_mux_t mux);

/**
 * @brief Read the conversion register of the ADS1115 device.
 * @param[in,out] dev    Device descriptor
 * @param[out] value     Pointer to store the conversion result
 * @return ADS1115_OK on success
 * @return ADS1115_NODATA if no data is available
 * @return ADS1115_NOI2C if other error occurs
 */
int ads1115_read_conversion(ads1115_t *dev, uint16_t *value);

/**
 * @brief Converts the digital value from the ADS1115 device to millivolts.
 *
 * @param[in] dev    Device descriptor
 * @param[in] value  Raw value from the ADS1115 device
 *
 * @return Converted value in millivolts
 */
int ads1115_convert_to_mv(ads1115_t *dev, uint16_t value);

#ifdef __cplusplus
}
#endif
