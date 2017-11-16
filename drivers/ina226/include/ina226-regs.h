/*
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_ina226
 * @{
 *
 * @file
 * @brief       Register definitions for Texas Instruments INA226 High-Side or Low-Side Measurement,
 *              Bi-Directional Current and Power Monitor with I2C Compatible Interface
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */


#ifndef INA226_REGS_H
#define INA226_REGS_H


#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief INA226 register addresses
 *
 * All registers in the INA226 are 16 bit wide and transmitted MSB first.
 */
typedef enum ina226_reg {
    INA226_REG_CONFIGURATION   = 0x00, /**< Configuration register (read/write) */
    INA226_REG_SHUNT_VOLTAGE   = 0x01, /**< Shunt voltage register (read only) */
    INA226_REG_BUS_VOLTAGE     = 0x02, /**< Bus voltage register (read only) */
    INA226_REG_POWER           = 0x03, /**< Power register (read only) */
    INA226_REG_CURRENT         = 0x04, /**< Current register (read only) */
    INA226_REG_CALIBRATION     = 0x05, /**< Calibration register (read/write) */
    INA226_REG_MASK_ENABLE     = 0x06, /**< Mask/Enable register (read/write) */
    INA226_REG_ALERT_LIMIT     = 0x07, /**< Alert Limit register (read/write) */
    INA226_REG_MANUFACTURER_ID = 0xFE, /**< Manufacturer ID register (read only) */
    INA226_REG_DIE_ID          = 0xFF, /**< Die ID register (read only) */
} ina226_reg_t;

/**
 * @brief INA226 mask/enable register bits
 * @see Table 15 in INA226 data sheet
 */
typedef enum ina226_mask_enable_reg_bit {
    INA226_MASK_ENABLE_LEN     = 0x0001, /**< Alert Latch Enable */
    INA226_MASK_ENABLE_APOL    = 0x0002, /**< Alert Polarity bit */
    INA226_MASK_ENABLE_OVF     = 0x0004, /**< Math Overflow Flag */
    INA226_MASK_ENABLE_CVRF    = 0x0008, /**< Conversion Ready Flag */
    INA226_MASK_ENABLE_AFF     = 0x0010, /**< Alert Function Flag */
    INA226_MASK_ENABLE_CNVR    = 0x0400, /**< Conversion Ready */
    INA226_MASK_ENABLE_POL     = 0x0800, /**< Power Over-Limit */
    INA226_MASK_ENABLE_BUL     = 0x1000, /**< Bus Voltage Under-Voltage */
    INA226_MASK_ENABLE_BOL     = 0x2000, /**< Bus Voltage Over-Voltage */
    INA226_MASK_ENABLE_SUL     = 0x4000, /**< Shunt Voltage Under-Voltage */
    INA226_MASK_ENABLE_SOL     = 0x8000, /**< Shunt Voltage Over-Voltage */
} ina226_mask_enable_reg_bit_t;


#ifdef __cplusplus
}
#endif

#endif /* INA226_REGS_H */
/** @} */
