/*
 * Copyright 2015 Ludwig Knüpfer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_isl29125
 * @{
 *
 * @file
 * @brief       Definitions for the ISL29125 RGB light sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#ifndef ISL29125_INTERNAL_H
#define ISL29125_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The sensors hard coded I2C address
 */
#define ISL29125_I2C_ADDRESS 0x44

/**
 * @name ISL29125 constants
 * @{
 */
#define ISL29125_ID             0x7D
/** @} */

/**
 * @name ISL29125 register map
 * @{
 */
/* main register */
#define ISL29125_REG_ID         0x00
#define ISL29125_REG_RESET      0x00
/* configuration registers */
#define ISL29125_REG_CONF1      0x01
#define ISL29125_REG_CONF2      0x02
#define ISL29125_REG_CONF3      0x03
/* interrupt mode threshold registers */
#define ISL29125_REG_LTHLB      0x04
#define ISL29125_REG_LTHHB      0x05
#define ISL29125_REG_HTHLB      0x06
#define ISL29125_REG_HTHHB      0x07
/* status register */
#define ISL29125_REG_STATUS     0x08
/* sensor readout registers (double buffered) */
#define ISL29125_REG_GDLB       0x09
#define ISL29125_REG_GDHB       0x0A
#define ISL29125_REG_RDLB       0x0B
#define ISL29125_REG_RDHB       0x0C
#define ISL29125_REG_BDLB       0x0D
#define ISL29125_REG_BDHB       0x0E
/** @} */

/**
 * @name ISL29125 commands
 * @{
 */
#define ISL29125_CMD_RESET      0x46
/** @} */

/**
 * @name ISL29125 configuration masks and bits
 * @{
 */
/* ISL29125_REG_CONF1 B2:B0 */
#define ISL29125_CON1_MASK_MODE 0x07
/* ISL29125_REG_CONF1 B3 */
#define ISL29125_CON1_MASK_RANGE 0x08
/* ISL29125_REG_CONF1 B4 */
#define ISL29125_CON1_MASK_RES  0x10
/* ISL29125_REG_CONF1 B5 */
#define ISL29125_CON1_MASK_SYNC 0x20
#define ISL29125_CON1_SYNCOFF   0x00
#define ISL29125_CON1_SYNCON    0x20
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ISL29125_INTERNAL_H */
/** @} */
