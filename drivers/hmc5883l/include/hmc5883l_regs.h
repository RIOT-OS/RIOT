/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hmc5883l
 * @brief       Register definitions for the Honeywell HMC5883L 3-axis digital compass
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef HMC5883L_REGS_H
#define HMC5883L_REGS_H

#ifdef __cplusplus
extern "C"
{
#endif

/** HMC5883L I2C address*/
#define HMC5883L_I2C_ADDRESS    (0x1e)

/** HMC5883L chip id defined in Identification Registers A..C */
#define HMC5883L_ID                 { 0x48, 0x34, 0x33 }

/** HMC5883L chip id length */
#define HMC5883L_ID_LEN             (3)

/**
 * @name    Register addresses
 * @{
 */
#define HMC5883L_REG_CFG_A          (0x00)
#define HMC5883L_REG_CFG_B          (0x01)
#define HMC5883L_REG_MODE           (0x02)
#define HMC5883L_REG_OUT_X_MSB      (0x03)
#define HMC5883L_REG_OUT_X_LSB      (0x04)
#define HMC5883L_REG_OUT_Y_MSB      (0x05)
#define HMC5883L_REG_OUT_Y_LSB      (0x06)
#define HMC5883L_REG_OUT_Z_MSB      (0x07)
#define HMC5883L_REG_OUT_Z_LSB      (0x08)
#define HMC5883L_REG_STATUS         (0x09)
#define HMC5883L_REG_ID_A           (0x0a)
#define HMC5883L_REG_ID_B           (0x0b)
#define HMC5883L_REG_ID_C           (0x0c)
/** @} */

/**
 * @name    Register structure definitions
 * @{
 */
#define HMC5883L_REG_CFG_A_MA       (0x60)  /**< HMC5883L_REG_CFG_A<6:5> */
#define HMC5883L_REG_CFG_A_DO       (0x1c)  /**< HMC5883L_REG_CFG_A<4:2> */
#define HMC5883L_REG_CFG_A_MS       (0x03)  /**< HMC5883L_REG_CFG_A<1:0> */

#define HMC5883L_REG_CFG_B_GN       (0xe0)  /**< HMC5883L_REG_CFG_N<7:5> */
#define HMC5883L_REG_CFG_B_GN_S     (5)     /**< HMC5883L_REG_CFG_N<7:5> shift */

#define HMC5883L_REG_MODE_HS        (0x80)  /**< HMC5883L_REG_MODE<7> */
#define HMC5883L_REG_MODE_MD        (0x03)  /**< HMC5883L_REG_MODE<1:0> */

#define HMC5883L_REG_STATUS_LOCK    (0x02)  /**< HMC5883L_REG_STATUS<1> */
#define HMC5883L_REG_STATUS_RDY     (0x01)  /**< HMC5883L_REG_STATUS<0> */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HMC5883L_REGS_H */
