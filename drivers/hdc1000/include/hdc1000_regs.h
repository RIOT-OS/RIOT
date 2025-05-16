/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_hdc1000
 * @{
 *
 * @file
 * @brief       Register definitions for HDC1000 devices
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Manufacturer and Device IDs
 * @{
 */
#define HDC1000_MID_VALUE          0x5449
#define HDC1000_DID_VALUE          0x1000
/** @} */

/**
 * @name    Register Map
 * @{
 */
#define HDC1000_TEMPERATURE         (0x00)
#define HDC1000_HUMIDITY            (0x01)
#define HDC1000_CONFIG              (0x02)
#define HDC1000_SID1                (0xFB)
#define HDC1000_SID2                (0xFC)
#define HDC1000_SID3                (0xFD)
#define HDC1000_MANUFACTURER_ID     (0xFE)
#define HDC1000_DEVICE_ID           (0xFF)
/** @} */

/**
 * @name    Configuration register bitmap
 * @{
 */
#define HDC1000_RST                 (1 << 15)
#define HDC1000_HEAT                (1 << 13)
#define HDC1000_SEQ_MOD             (1 << 12)
#define HDC1000_BTST_LOW            (1 << 11)
#define HDC1000_TRES_MSK            (1 << 10)
#define HDC1000_TRES11              (1 << 10)
#define HDC1000_TRES14              (0)
#define HDC1000_HRES_MSK            (1 << 9 | 1 << 8)
#define HDC1000_HRES14              (0)
#define HDC1000_HRES11              (1 << 8)
#define HDC1000_HRES8               (1 << 9)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
