/*
 * Copyright (C) 2015 Eistec AB
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ina2xx INA2XX current/power monitor
 * @ingroup     drivers_sensors
 * @brief       Device driver for Texas Instruments INA219/INA2XX
 *              Bi-Directional CURRENT/POWER MONITOR with Two-Wire Interface
 *
 * # Supported Hardware
 * This driver currently supports the INA219 and the INA2XX - and possibly
 * other devices of the INA2xx family. The popular INA219 breakout boards are
 * supported out of the box.
 *
 * # Choosing the Right Shunt Resistor
 * The shunt resistor should generate a voltage drop of at least 40 mV and of
 * at most 320 mV at maximum current (add some safety margin to the maximum
 * current). E.g. when the expected maximum current is 500 mA and we add a
 * safety margin of 33 mA, a 75 mΩ shunt resistor would cause a drop of 40 mV
 * at about 533 mA, and a 600 mΩ shunt resistor would cause a drop of 320 mV.
 * So every shunt resistor between 75 mΩ and 600 mΩ would result in a decent
 * resolution, but shunt resistors that almost max out one of the selectable
 * shunt voltage ranges (40 mV, 80 mV, 160 mV and 320 mV) would be ideal.
 * Often the voltage drop should be as low as possible, therefore the lowest
 * reasonable shunt resistor has to be chosen in that case.
 *
 * The popular INA219 breakout boards have a shunt resistor of 100 mΩ, which
 * allows to measure a current of up to 400 mA at PGA = /1, and of up two 3.2 A
 * at PGA = /8.
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for Texas Instruments INA219/INA2XX
 *              Bi-Directional CURRENT/POWER MONITOR with Two-Wire Interface
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef INA2XX_H
#define INA2XX_H

#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   INA2XX possible mode settings
 */
typedef enum ina2xx_mode {
    INA2XX_MODE_POWERDOWN             = 0x0000, /**< Power down */
    INA2XX_MODE_TRIGGER_SHUNT_ONLY    = 0x0001, /**< Shunt Voltage, Triggered */
    INA2XX_MODE_TRIGGER_BUS_ONLY      = 0x0002, /**< Bus Voltage, Triggered */
    INA2XX_MODE_TRIGGER_SHUNT_BUS     = 0x0003, /**< Shunt and Bus, Triggered */
    INA2XX_MODE_ADC_DISABLE           = 0x0004, /**< ADC Off (disabled) */
    INA2XX_MODE_CONTINUOUS_SHUNT_ONLY = 0x0005, /**< Shunt Voltage, Continuous */
    INA2XX_MODE_CONTINUOUS_BUS_ONLY   = 0x0006, /**< Bus Voltage, Continuous */
    INA2XX_MODE_CONTINUOUS_SHUNT_BUS  = 0x0007, /**< Shunt and Bus, Continuous, default */
} ina2xx_mode_t;

/**
 * @brief   Shunt ADC settings
 *
 * @see Table 4 in INA2XX data sheet
 */
typedef enum ina2xx_sadc {
    /** 9 bit resolution, 84 us conversion time */
    INA2XX_SADC_9BIT            = 0x0000,
    /** 10 bit resolution, 148 us conversion time */
    INA2XX_SADC_10BIT           = 0x0008,
    /** 11 bit resolution, 276 us conversion time */
    INA2XX_SADC_11BIT           = 0x0010,
    /** 12 bit resolution, 532 us conversion time, default */
    INA2XX_SADC_12BIT           = 0x0018,
    /** 12 bit resolution, 532 us conversion time, same as INA2XX_SADC_12BIT */
    INA2XX_SADC_AVG_1_SAMPLE    = 0x0040,
    /** 2 sample average, 1.06 ms conversion time */
    INA2XX_SADC_AVG_2_SAMPLES   = 0x0048,
    /** 4 sample average, 2.13 ms conversion time */
    INA2XX_SADC_AVG_4_SAMPLES   = 0x0050,
    /** 8 sample average, 4.26 ms conversion time */
    INA2XX_SADC_AVG_8_SAMPLES   = 0x0058,
    /** 16 sample average, 8.51 ms conversion time */
    INA2XX_SADC_AVG_16_SAMPLES  = 0x0060,
    /** 32 sample average, 17.02 ms conversion time */
    INA2XX_SADC_AVG_32_SAMPLES  = 0x0068,
    /** 64 sample average, 34.05 ms conversion time */
    INA2XX_SADC_AVG_64_SAMPLES  = 0x0070,
    /** 128 sample average, 68.10 ms conversion time */
    INA2XX_SADC_AVG_128_SAMPLES = 0x0078,
} ina2xx_sadc_t;

/**
 * @brief   Bus ADC settings
 *
 * @see Table 4 in INA2XX data sheet
 */
typedef enum ina2xx_badc {
    /** 9 bit resolution, 84 us conversion time */
    INA2XX_BADC_9BIT            = 0x0000,
    /** 10 bit resolution, 148 us conversion time */
    INA2XX_BADC_10BIT           = 0x0080,
    /** 11 bit resolution, 276 us conversion time */
    INA2XX_BADC_11BIT           = 0x0100,
    /** 12 bit resolution, 532 us conversion time, default */
    INA2XX_BADC_12BIT           = 0x0180,
    /** 12 bit resolution, 532 us conversion time, same as INA2XX_BADC_12BIT */
    INA2XX_BADC_AVG_1_SAMPLE    = 0x0400,
    /** 2 sample average, 1.06 ms conversion time */
    INA2XX_BADC_AVG_2_SAMPLES   = 0x0480,
    /** 4 sample average, 2.13 ms conversion time */
    INA2XX_BADC_AVG_4_SAMPLES   = 0x0500,
    /** 8 sample average, 4.26 ms conversion time */
    INA2XX_BADC_AVG_8_SAMPLES   = 0x0580,
    /** 16 sample average, 8.51 ms conversion time */
    INA2XX_BADC_AVG_16_SAMPLES  = 0x0600,
    /** 32 sample average, 17.02 ms conversion time */
    INA2XX_BADC_AVG_32_SAMPLES  = 0x0680,
    /** 64 sample average, 34.05 ms conversion time */
    INA2XX_BADC_AVG_64_SAMPLES  = 0x0700,
    /** 128 sample average, 68.10 ms conversion time */
    INA2XX_BADC_AVG_128_SAMPLES = 0x0780,
} ina2xx_badc_t;

/**
 * @brief   Shunt voltage measurement range (PGA settings)
 */
typedef enum ina2xx_pga {
    INA2XX_SHUNT_RANGE_40MV     = 0x0000, /**< +/- 40 mV range */
    INA2XX_SHUNT_RANGE_80MV     = 0x0800, /**< +/- 80 mV range */
    INA2XX_SHUNT_RANGE_160MV    = 0x1000, /**< +/- 160 mV range */
    INA2XX_SHUNT_RANGE_320MV    = 0x1800, /**< +/- 320 mV range, default */
} ina2xx_pga_t;

/**
 * @brief   Bus voltage measurement range
 */
typedef enum ina2xx_brng {
    INA2XX_BUS_RANGE_16V        = 0x0000, /**< 16 V bus voltage full scale range */
    INA2XX_BUS_RANGE_32V        = 0x2000, /**< 32 V bus voltage full scale range, default. */
} ina2xx_brng_t;

/**
 * @brief   Current measurement range
 * @warning Choosing a low range and a small shunt resistor can cause
 *          @ref ina2xx_init to fail. But for high resolution measurements of
 *          low currents a "big" shunt resistor (e.g. 100 mΩ) should be chosen
 *          anyway.
 *
 * Choosing the lowest sufficient range for your use case will result in the
 * optimal resolution
 */
typedef enum ina2xx_i_range {
    INA2XX_CURRENT_RANGE_327_MA,     /**<   ±327.68 mA, 0.01mA resolution */
    INA2XX_CURRENT_RANGE_655_MA,     /**<   ±655.36 mA, 0.02mA resolution */
    INA2XX_CURRENT_RANGE_1310_MA,    /**<  ±1310.72 mA, 0.04mA resolution */
    INA2XX_CURRENT_RANGE_2621_MA,    /**<  ±2621.44 mA, 0.08mA resolution */
    INA2XX_CURRENT_RANGE_5242_MA,    /**<  ±5242.88 mA, 0.16mA resolution */
    INA2XX_CURRENT_RANGE_10485_MA,   /**< ±10485.76 mA, 0.32mA resolution */
} ina2xx_i_range_t;

/**
 * @brief   Configuration parameters of the INA2xx driver
 */
typedef struct {
    i2c_t i2c;                  /**< I2C device the sensor is connected to */
    uint8_t addr;               /**< I2C address of the sensr */
    uint16_t config;            /**< Configuration to apply */
    uint16_t rshunt_mohm;       /**< Size of the shunt resistor in mΩ */
    ina2xx_i_range_t i_range;   /**< Range of the measured current */
} ina2xx_params_t;

/**
 * @brief   Device descriptor for INA2XX sensors
 */
typedef struct {
    ina2xx_params_t params;     /**< Configuration parameters of the driver */
} ina2xx_t;

/**
 * @brief   Initialize a current sensor
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       Configuration parameters to use
 *
 * @return                  `0` on success, `<0` on error.
 * @retval  0               Success
 * @retval -ENODEV          Device seems not to be an INA2XX device
 * @retval -EINVAL          @p dev or @p params is NULL
 * @retval -ERANGE          @p params contained invalid setting: Increase
 *                          current range
 * @retval <0               Failure, error of @ref i2c_read_regs or
 *                          @ref i2c_write_regs passed through
 */
int ina2xx_init(ina2xx_t *dev, const ina2xx_params_t *params);

/**
 * @brief   Read shunt voltage in E-05 V
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] voltage      measured voltage across shunt resistor in E-05 V
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina2xx_read_shunt(const ina2xx_t *dev, int16_t *voltage);

/**
 * @brief   Read bus voltage in mV
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] voltage      The measured bus voltage in mV
 *
 * @return                  `<0` on error, `>= 0` on success
 * @retval  0               Success, no new power value available
 * @retval  1               Success, new value for @ref ina2xx_read_power ready
 * @retval  -EDOM           Overflow during power/current calculations.
 * @retval  <0              Error code of @ref i2c_read_regs passed through
 */
int ina2xx_read_bus(const ina2xx_t *dev, uint16_t *voltage);

/**
 * @brief   Read shunt current in E-05 A
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] current      measured current through shunt resistor in E-05 A
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina2xx_read_current(const ina2xx_t *dev, int32_t *current);

/**
 * @brief   Read power consumption in E-04 W
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] power        measured power consumption in E-04 W
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina2xx_read_power(const ina2xx_t *dev, uint32_t *power);

#ifdef __cplusplus
}
#endif

#endif /* INA2XX_H */
/** @} */
