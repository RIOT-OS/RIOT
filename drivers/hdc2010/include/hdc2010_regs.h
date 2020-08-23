/*
 * Copyright (C) 2020 Viktor Gal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hdc2010
 * @{
 *
 * @file
 * @brief       Register definitions for HDC2010 devices
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef HDC2010_REGS_H
#define HDC2010_REGS_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Manufacturer and Device IDs
 * @{
 */
#define HDC2010_MID_VALUE          0x4954
#define HDC2010_DID_VALUE          0x07D0
/** @} */

/**
 * @name    Register Map
 * @{
 */
#define HDC2010_TEMPERATURE         (0x00)
#define HDC2010_HUMIDITY            (0x02)
#define HDC2010_TEMPERATURE_MAX     (0x05)
#define HDC2010_HUMIDITY_MAX        (0x06)
#define HDC2010_INTERRUPT_ENABLE    (0x07)
#define HDC2010_TEMP_THR_LOW        (0x0A)
#define HDC2010_TEMP_THR_HIGH       (0x0B)
#define HDC2010_RH_THR_LOW          (0x0C)
#define HDC2010_RH_THR_HIGH         (0x0D)
#define HDC2010_CONFIG              (0x0E)
#define HDC2010_MEASUREMENT_CONFIG  (0x0F)
#define HDC2010_MANUFACTURER_ID     (0xFC)
#define HDC2010_DEVICE_ID           (0xFE)
/** @} */

/**
 * @name    Configuration register bitmap
 * @{
 */
#define HDC2010_TEMP_MODE           (1 << 1)
#define HDC2010_RST                 (1 << 7)
#define HDC2010_HEAT                (1 << 3)
#define HDC2010_TRES_MSK            (1 << 7 | 1 << 6)
#define HDC2010_TRES9               (1 << 7)
#define HDC2010_TRES11              (1 << 6)
#define HDC2010_TRES14              (0)
#define HDC2010_HRES_MSK            (1 << 5 | 1 << 4)
#define HDC2010_HRES14              (0)
#define HDC2010_HRES11              (1 << 4)
#define HDC2010_HRES9               (1 << 5)
#define HDC2010_SAMPLE_2M           (1 << 4)
#define HDC2010_SAMPLE_1M           (1 << 5)
#define HDC2010_SAMPLE_10S          (3 << 4)
#define HDC2010_SAMPLE_5S           (1 << 6)
#define HDC2010_SAMPLE_1HZ          (5 << 4)
#define HDC2010_SAMPLE_2HZ          (6 << 4)
#define HDC2010_SAMPLE_5HZ          (7 << 4)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HDC2010_REGS_H */
/** @} */
