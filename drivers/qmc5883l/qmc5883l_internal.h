/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_qmc5883l
 * @{
 *
 * @file
 * @brief       Register definitions for the QMC5883L device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name   Register map
 * @{
 */
#define QMC5883L_DOXL       (0x00)
#define QMC5883L_DOXH       (0x01)
#define QMC5883L_DOYL       (0x02)
#define QMC5883L_DOYH       (0x03)
#define QMC5883L_DOZL       (0x04)
#define QMC5883L_DOZH       (0x05)
#define QMC5883L_STATUS     (0x06)
#define QMC5883L_TOUTL      (0x07)
#define QMC5883L_TOUTH      (0x08)
#define QMC5883L_CTRL1      (0x09)
#define QMC5883L_CTRL2      (0x0a)
#define QMC5883L_SETRESET   (0x0b)
/** @} */

/**
 * @name   Device modes
 * @{
 */
#define QMC5883L_STANDBY    (0x00)
#define QMC5883L_CONT       (0x01)
/** @} */

/**
 * @name   Device status flags
 * @{
 */
#define QMC5883L_DRDY       (0x01)
#define QMC5883L_OVL        (0x02)
#define QMC5883L_DOR        (0x04)
/** @} */

/**
 * @name   Configuration bitfields
 * @{
 */
#define QMC5883L_INT_ENB    (0x01)
#define QMC5883L_ROL_PNT    (0x40)
#define QMC5883L_SOFT_RST   (0x80)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
