/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tmp006
 *
 * @{
 * @file
 * @brief       Register definitions for TMP006 devices
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef TMP006_REGS_H
#define TMP006_REGS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Register Map
 * @{
 */
 #define TMP006_REGS_V_OBJECT           0x00 /**< Sensor Voltage Register */
 #define TMP006_REGS_T_AMBIENT          0x01 /**< Ambient Temperature Register */
 #define TMP006_REGS_CONFIG             0x02 /**< Configuration Register */
 #define TMP006_REGS_MANUFACTURER_ID    0xFE /**< Manufacturer ID Register */
 #define TMP006_REGS_DEVICE_ID          0xFF /**< Device ID Register */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* TMP006_REGS_H */
/** @} */
