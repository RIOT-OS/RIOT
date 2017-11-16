/*
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ina226 INA226 current/power monitor
 * @ingroup     drivers_sensors
 * @brief       Device driver for Texas Instruments INA226 High-Side or Low-Side Measurement,
 *              Bi-Directional Current and Power Monitor with I2C Compatible Interface
 * @{
 *
 * @file
 * @brief       Device driver interface for Texas Instruments INA226 High-Side or Low-Side Measurement,
 *              Bi-Directional Current and Power Monitor with I2C Compatible Interface.
 *              All references to the INA226 data sheet are related to the document revision SBOS547A
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef INA226_H
#define INA226_H

#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "ina226-regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device descriptor for INA226 sensors
 */
typedef struct {
    i2c_t i2c;              /**< I2C device the sensor is connected to */
    uint8_t addr;           /**< the slave address of the sensor on the I2C bus */
} ina226_t;

/** INA226 reset command bit in configuration register (setting this bit to '1' generates a system reset) */
#define INA226_RESET_BIT (0x8000)

/** INA226 full scale (max. value) of the bus voltage register. Equivalent to 40.96 V (LSB = 1.25 mV)*/
#define INA226_FULL_VOLTAGE_SCALE (0x7FFF)

/** INA226 bus voltage LSB in microvolts = 1.25 mV*/
#define INA226_BUS_VOLTAGE_LSB_UV (1250)

/** INA226 shunt voltage LSB in nanovolts = 2.5 uV*/
#define INA226_SHUNT_VOLTAGE_LSB_NV (2500)

/** INA226 maximum voltage drop over shunt-resistor see "Shunt voltage input range" in data sheet section 6.5*/
#define INA226_MAX_SHUNT_VOLTAGE_UV (81920)

/** 
 * INA226 power register value LSB is based on the programmed current register LSB. 
 * For the ina226 this fixed ratio is 25. => 25 * CurrentLSB = PowerLSB */
#define INA226_POWER_LSB_CURRENT_LSB_RATIO (25)

/** INA226 Current_LSB = MAX_EXPECTED_CURRENT / 2^15 (see ina226 datasheet section 7.5)*/
#define INA226_MAX_CURRENT_TO_LSB_RATIO (32768)

/** 
 * INA226 CAL = 0.00512 / Current_LSB * R_shunt (see ina226 datasheet section 7.5) 
 * this value equals 1,000,000,000,000 * 0.00512. This value is scaled up to ensure 
 * maximum accuracy for macro-based calculation of CAL value without using floating point.
 * For proper calculation of CAL value based on this define use [nA] for Current_LSB and [mOhm] for R_shunt*/
#define INA226_CAL_SCALE_FACTOR_INT (5120000000LL)


/**
 * @brief Determines the number of samples that are collected and averaged. (Bits 9–11 in configuration register)
 * @see Table 6 in INA226 data sheet
 */
typedef enum ina226_avg {
    INA226_AVG_1     = 0x0000, /**no averaging, default */
    INA226_AVG_4     = 0x0200, /**averaging 4 measurements */
    INA226_AVG_16    = 0x0400, /**averaging 16 measurements */
    INA226_AVG_64    = 0x0600, /**averaging 64 measurements */
    INA226_AVG_128   = 0x0800, /**averaging 128 measurements */
    INA226_AVG_256   = 0x0A00, /**averaging 256 measurements */
    INA226_AVG_512   = 0x0C00, /**averaging 512 measurements */
    INA226_AVG_1024  = 0x0E00, /**averaging 1024 measurements */
} ina226_avg_t;

/**
 * @brief Determines the bus voltage measurement conversion time. (Bits 6–8 in configuration register)
 * @see Table 7 in INA226 data sheet
 */
typedef enum ina226_vbusct {
    INA226_VBUSCT_140_US   = 0x0000, /**conversion time 140 us */
    INA226_VBUSCT_204_US   = 0x0040, /**conversion time 204 us */
    INA226_VBUSCT_332_US   = 0x0080, /**conversion time 332 us */
    INA226_VBUSCT_588_US   = 0x00C0, /**conversion time 588 us */
    INA226_VBUSCT_1_1_MS   = 0x0100, /**conversion time 1.1 ms, default */
    INA226_VBUSCT_2_116_MS = 0x0140, /**conversion time 2.116 ms */
    INA226_VBUSCT_4_156_MS = 0x0180, /**conversion time 4.156 ms */
    INA226_VBUSCT_8_244_MS = 0x01C0, /**conversion time 8.244 ms */
} ina226_vbusct_t;

/**
 * @brief Determines the shunt voltage measurement conversion time. (Bits 3–5 in configuration register)
 * @see Table 8 in INA226 data sheet
 */
typedef enum ina226_vshct {
    INA226_VSHCT_140_US   = 0x0000, /**conversion time 140 us */
    INA226_VSHCT_204_US   = 0x0008, /**conversion time 204 us */
    INA226_VSHCT_332_US   = 0x0010, /**conversion time 332 us */
    INA226_VSHCT_588_US   = 0x0018, /**conversion time 588 us */
    INA226_VSHCT_1_1_MS   = 0x0020, /**conversion time 1.1 ms, default */
    INA226_VSHCT_2_116_MS = 0x0028, /**conversion time 2.116 ms */
    INA226_VSHCT_4_156_MS = 0x0030, /**conversion time 4.156 ms */
    INA226_VSHCT_8_244_MS = 0x0038, /**conversion time 8.244 ms */
} ina226_vshct_t;

/**
 * @brief INA226 possible mode settings. (Bits 0-2 in configuration register)
 * @see Table 9 in INA226 data sheet
 */
typedef enum ina226_OP_mode {
    INA226_OP_MODE_POWERDOWN                = 0x0000, /**< Power-Down (or Shutdown) */
    INA226_OP_MODE_TRIGGER_SHUNT_ONLY       = 0x0001, /**< Shunt Voltage, Triggered */
    INA226_OP_MODE_TRIGGER_BUS_ONLY         = 0x0002, /**< Bus Voltage, Triggered */
    INA226_OP_MODE_TRIGGER_SHUNT_AND_BUS    = 0x0003, /**< Shunt and Bus, Triggered */
    INA226_OP_MODE_POWERDOWN_2              = 0x0004, /**< Power-Down (or Shutdown) */
    INA226_OP_MODE_CONTINUOUS_SHUNT_ONLY    = 0x0005, /**< Shunt Voltage, Continuous */
    INA226_OP_MODE_CONTINUOUS_BUS_ONLY      = 0x0006, /**< Bus Voltage, Continuous */
    INA226_OP_MODE_CONTINUOUS_SHUNT_AND_BUS = 0x0007, /**< Shunt and Bus, Continuous, default */
} ina226_mode_t;

/**
 * @brief Read one 16 bit register of INA226 device
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[in]  reg          register to read
 * @param[out] out          pointer to where the register value will be stored
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina226_read_reg(ina226_t *dev, uint8_t reg, uint16_t *out);

/**
 * @brief Write one 16 bit register of INA226 device
 *
 * @param[in] dev           device descriptor of sensor
 * @param[in] reg           register to write
 * @param[in] in            value to write to register
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int ina226_write_reg(ina226_t *dev, uint8_t reg, uint16_t in);

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
int ina226_init(ina226_t *dev, i2c_t i2c, uint8_t address);

/**
 * @brief enable interrupt for the alert pin
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[in]  me_config    the configuration of the mask/enable register (specifies 
                            which condition triggers the interrupt)
 * @param[in]  pin          the gpio pin that is connected to the alert pin 
                            of the ina226 device
 * @param[in]  callback     function that is called when an interrupt occurs
 *
 * @return                     0 on success
 *                             <0 on error
 */
int ina226_activate_int(ina226_t *dev, uint16_t me_config, gpio_t pin, gpio_cb_t callback);

/**
 * @brief Read calibration register
 *
 * @param[in]   dev          device descriptor of sensor to configure
 * @param[out]  calibration  calibration register settings, see data sheet
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_read_calibration(ina226_t *dev, uint16_t *calibration)
{
    return ina226_read_reg(dev, INA226_REG_CALIBRATION, calibration);
}

/**
 * @brief Write to calibration register
 *
 * @param[in]  dev          device descriptor of sensor to configure
 * @param[in]  calibration  calibration register settings, see data sheet
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_write_calibration(ina226_t *dev, uint16_t calibration)
{
    return ina226_write_reg(dev, INA226_REG_CALIBRATION, calibration);
}

/**
 * @brief Read configuration register
 *
 * @param[in]   dev          device descriptor of sensor to configure
 * @param[out]  config       configuration register settings, see data sheet
 *
 * @return                   0 on success
 * @return                   <0 on error
 */
inline int ina226_read_config(ina226_t *dev, uint16_t *config)
{
    return ina226_read_reg(dev, INA226_REG_CONFIGURATION, config);   
}

/**
 * @brief Write to configuration register
 *
 * @param[in]  dev          device descriptor of sensor to configure
 * @param[in]  config       configuration register settings, see data sheet
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_write_config(ina226_t *dev, uint16_t config)
{
    return ina226_write_reg(dev, INA226_REG_CONFIGURATION, config);
}

/**
 * @brief Read shunt voltage
 *
 * The Shunt Voltage Register stores the current shunt voltage reading. 
 * Negative numbers are represented in two's complement format.
 * If averaging is enabled, this register displays the averaged value. 
 * Full-scale range = 81.92 mV (decimal = 7FFF); LSB: 2.5 uV
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] voltage      measured voltage across shunt resistor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_read_shunt(ina226_t *dev, int16_t *voltage)
{
    return ina226_read_reg(dev, INA226_REG_SHUNT_VOLTAGE, (uint16_t *)voltage);
}

/**
 * @brief Read bus voltage register
 *
 * The Bus Voltage Register stores the most recent bus voltage reading, VBUS.
 * If averaging is enabled, this register displays the averaged value.
 * Full-scale range = 40.96 V (decimal = 7FFF); LSB = 1.25 mV.
 *
 * See the device data sheet for details.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] voltage      measured bus voltage
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_read_bus(ina226_t *dev, int16_t *voltage)
{
    return ina226_read_reg(dev, INA226_REG_BUS_VOLTAGE, (uint16_t *)voltage);
}

/**
 * @brief Read shunt current
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] current      measured current through shunt resistor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_read_current(ina226_t *dev, int16_t *current)
{
    return ina226_read_reg(dev, INA226_REG_CURRENT, (uint16_t *)current);
}

/**
 * @brief Read power consumption
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] power        measured power consumption
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_read_power(ina226_t *dev, int16_t *power)
{
    return ina226_read_reg(dev, INA226_REG_POWER, (uint16_t *)power);
}

/**
 * @brief read die ID Register
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] id           the unique identification number and the revision ID for the die
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_read_die_id(ina226_t *dev, uint16_t *id)
{
    return ina226_read_reg(dev, INA226_REG_DIE_ID, id);
}

/**
 * @brief read manufacturer ID Register
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] id           the manufacturer ID (if the INA226 is set up correctly 
                            this value should be 0x5449)
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_read_manufacturer_id(ina226_t *dev, uint16_t *id)
{
    return ina226_read_reg(dev, INA226_REG_MANUFACTURER_ID, id);
}

/**
 * @brief read the mask/enable register
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] val          the value of the mask/enable register
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_read_mask_enable(ina226_t *dev, uint16_t *val)
{
    return ina226_read_reg(dev, INA226_REG_MASK_ENABLE, val);
}

/**
 * @brief write the mask/enable register
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[in]  val          the value of the mask/enable register
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_write_mask_enable(ina226_t *dev, uint16_t val)
{
    return ina226_write_reg(dev, INA226_REG_MASK_ENABLE, val);
}

/**
 * @brief read alert limit register
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] val          the value of the alert limit register
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_read_alert_limit(ina226_t *dev, uint16_t *val)
{
    return ina226_read_reg(dev, INA226_REG_ALERT_LIMIT, val);
}

/**
 * @brief write alert limit register
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[in]  val          the value of the alert limit register
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
inline int ina226_write_alert_limit(ina226_t *dev, uint16_t val)
{
    return ina226_write_reg(dev, INA226_REG_ALERT_LIMIT, val);
}

#ifdef __cplusplus
}
#endif

#endif /* INA226_H */
/** @} */
