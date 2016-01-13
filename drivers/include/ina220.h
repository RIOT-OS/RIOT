/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ina220 INA220 current/power monitor
 * @ingroup     drivers_sensors
 * @brief       Device driver for Texas Instruments INA220 High or Low Side,
 *              Bi-Directional CURRENT/POWER MONITOR with Two-Wire Interface
 * @{
 *
 * @file
 * @brief       Device driver interface for Texas Instruments INA220 High or Low
 *              Side, Bi-Directional CURRENT/POWER MONITOR with Two-Wire
 *              Interface
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef INA220_H
#define INA220_H

#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device descriptor for INA220 sensors
 */
typedef struct {
    i2c_t i2c;              /**< I2C device the sensor is connected to */
    uint8_t addr;           /**< the slave address of the sensor on the I2C bus */
} ina220_t;

/** @brief INA220 possible mode settings */
typedef enum ina220_mode {
    INA220_MODE_POWERDOWN             = 0x0000, /**< Power down */
    INA220_MODE_TRIGGER_SHUNT_ONLY    = 0x0001, /**< Shunt Voltage, Triggered */
    INA220_MODE_TRIGGER_BUS_ONLY      = 0x0002, /**< Bus Voltage, Triggered */
    INA220_MODE_TRIGGER_SHUNT_BUS     = 0x0003, /**< Shunt and Bus, Triggered */
    INA220_MODE_ADC_DISABLE           = 0x0004, /**< ADC Off (disabled) */
    INA220_MODE_CONTINUOUS_SHUNT_ONLY = 0x0005, /**< Shunt Voltage, Continuous */
    INA220_MODE_CONTINUOUS_BUS_ONLY   = 0x0006, /**< Bus Voltage, Continuous */
    INA220_MODE_CONTINUOUS_SHUNT_BUS  = 0x0007, /**< Shunt and Bus, Continuous, default */
} ina220_mode_t;

/** @brief Shunt voltage measurement range (PGA settings) */
typedef enum ina220_range {
    INA220_RANGE_40MV  = 0x0000, /**< +/- 40 mV range */
    INA220_RANGE_80MV  = 0x0800, /**< +/- 80 mV range */
    INA220_RANGE_160MV = 0x1000, /**< +/- 160 mV range */
    INA220_RANGE_320MV = 0x1800, /**< +/- 320 mV range, default */
} ina220_range_t;

/** @brief Bus voltage measurement range */
typedef enum ina220_brng {
    INA220_BRNG_16V_FSR = 0x0000, /**< 16 V bus voltage full scale range */
    INA220_BRNG_32V_FSR = 0x0200, /**< 32 V bus voltage full scale range, default. */
} ina220_brng_t;

/**
 * @brief Shunt ADC settings
 *
 * @see Table 5 in INA220 data sheet
 */
typedef enum ina220_sadc {
    /** 9 bit resolution, 84 us conversion time */
    INA220_SADC_9BIT            = 0x0000,
    /** 10 bit resolution, 148 us conversion time */
    INA220_SADC_10BIT           = 0x0008,
    /** 11 bit resolution, 276 us conversion time */
    INA220_SADC_11BIT           = 0x0010,
    /** 12 bit resolution, 532 us conversion time, default */
    INA220_SADC_12BIT           = 0x0018,
    /** 12 bit resolution, 532 us conversion time, same as INA220_SADC_12BIT */
    INA220_SADC_AVG_1_SAMPLE    = 0x0040,
    /** 2 sample average, 1.06 ms conversion time */
    INA220_SADC_AVG_2_SAMPLES   = 0x0048,
    /** 4 sample average, 2.13 ms conversion time */
    INA220_SADC_AVG_4_SAMPLES   = 0x0050,
    /** 8 sample average, 4.26 ms conversion time */
    INA220_SADC_AVG_8_SAMPLES   = 0x0058,
    /** 16 sample average, 8.51 ms conversion time */
    INA220_SADC_AVG_16_SAMPLES  = 0x0060,
    /** 32 sample average, 17.02 ms conversion time */
    INA220_SADC_AVG_32_SAMPLES  = 0x0068,
    /** 64 sample average, 34.05 ms conversion time */
    INA220_SADC_AVG_64_SAMPLES  = 0x0070,
    /** 128 sample average, 68.10 ms conversion time */
    INA220_SADC_AVG_128_SAMPLES = 0x0078,
} ina220_sadc_t;

/**
 * @brief Bus ADC settings
 *
 * @see Table 5 in INA220 data sheet
 */
typedef enum ina220_badc {
    /** 9 bit resolution, 84 us conversion time */
    INA220_BADC_9BIT            = 0x0000,
    /** 10 bit resolution, 148 us conversion time */
    INA220_BADC_10BIT           = 0x0080,
    /** 11 bit resolution, 276 us conversion time */
    INA220_BADC_11BIT           = 0x0100,
    /** 12 bit resolution, 532 us conversion time, default */
    INA220_BADC_12BIT           = 0x0180,
    /** 12 bit resolution, 532 us conversion time, same as INA220_BADC_12BIT */
    INA220_BADC_AVG_1_SAMPLE    = 0x0400,
    /** 2 sample average, 1.06 ms conversion time */
    INA220_BADC_AVG_2_SAMPLES   = 0x0480,
    /** 4 sample average, 2.13 ms conversion time */
    INA220_BADC_AVG_4_SAMPLES   = 0x0500,
    /** 8 sample average, 4.26 ms conversion time */
    INA220_BADC_AVG_8_SAMPLES   = 0x0580,
    /** 16 sample average, 8.51 ms conversion time */
    INA220_BADC_AVG_16_SAMPLES  = 0x0600,
    /** 32 sample average, 17.02 ms conversion time */
    INA220_BADC_AVG_32_SAMPLES  = 0x0680,
    /** 64 sample average, 34.05 ms conversion time */
    INA220_BADC_AVG_64_SAMPLES  = 0x0700,
    /** 128 sample average, 68.10 ms conversion time */
    INA220_BADC_AVG_128_SAMPLES = 0x0780,
} ina220_badc_t;

/** INA220 reset command bit (in configuration register) */
#define INA220_RESET_BIT (0x8000)

/** Location of the bus voltage in the INA220 bus voltage register */
#define INA220_BUS_VOLTAGE_SHIFT (3)

/**
 * @brief Initialize a current sensor
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  i2c          I2C bus the sensor is connected to
 * @param[in]  address      I2C slave address of the sensor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina220_init(ina220_t *dev, i2c_t i2c, uint8_t address);

/**
 * @brief Write to calibration register
 *
 * @param[in]  dev          device descriptor of sensor to configure
 * @param[in]  calibration  calibration register settings, see data sheet
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina220_set_calibration(ina220_t *dev, uint16_t calibration);

/**
 * @brief Write to configuration register
 *
 * @param[in]  dev          device descriptor of sensor to configure
 * @param[in]  config       configuration register settings, see data sheet
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina220_set_config(ina220_t *dev, uint16_t config);

/**
 * @brief Read shunt voltage
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] voltage      measured voltage across shunt resistor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina220_read_shunt(ina220_t *dev, int16_t *voltage);

/**
 * @brief Read bus voltage register
 *
 * The bus voltage can be found in the most significant bits of the bus voltage
 * register, the lower three bits are flags/reserved.
 *
 * See the device data sheet for details.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] voltage      measured bus voltage
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina220_read_bus(ina220_t *dev, int16_t *voltage);

/**
 * @brief Read shunt current
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] current      measured current through shunt resistor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina220_read_current(ina220_t *dev, int16_t *current);

/**
 * @brief Read power consumption
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] power        measured power consumption
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina220_read_power(ina220_t *dev, int16_t *power);

#ifdef __cplusplus
}
#endif

#endif /* INA220_H */
/** @} */
