/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_ina220
 * @{
 *
 * @file
 * @brief       Register definitions for Texas Instruments INA220 High or Low
 *              Side, Bi-Directional CURRENT/POWER MONITOR with Two-Wire
 *              Interface
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef INA220_REGS_H
#define INA220_REGS_H


#ifdef __cplusplus
 extern "C" {
#endif


/**
 * @brief INA220 register addresses
 *
 * All registers in the INA220 are 16 bit wide and transmitted MSB first.
 */
typedef enum ina220_reg {
    INA220_REG_CONFIGURATION = 0x00, /**< Configuration register (read/write) */
    INA220_REG_SHUNT_VOLTAGE = 0x01, /**< Shunt voltage register (read only) */
    INA220_REG_BUS_VOLTAGE   = 0x02, /**< Bus voltage register (read only) */
    INA220_REG_POWER         = 0x03, /**< Power register (read only) */
    INA220_REG_CURRENT       = 0x04, /**< Current register (read only) */
    INA220_REG_CALIBRATION   = 0x05, /**< Calibration register (read/write) */
} ina220_reg_t;


#ifdef __cplusplus
}
#endif

#endif /* __L3G4200D_REGS_H */
/** @} */
