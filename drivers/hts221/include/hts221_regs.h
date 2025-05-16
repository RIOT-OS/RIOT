/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_hts221
 *
 * @{
 * @file
 * @brief       Register definitions for ST HTS221 devices
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */


#include "board.h"
#include "hts221.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HTS221_DEVICE_ID            (0xBC)
/**
 * @name   Register Map
 * @{
 */
#define HTS221_REGS_WHO_AM_I        (0x0F)
#define HTS221_REGS_AV_CONF         (0x10)
#define HTS221_REGS_CTRL_REG1       (0x20)
#define HTS221_REGS_CTRL_REG2       (0x21)
#define HTS221_REGS_CTRL_REG3       (0x22)   /**< data ready output signal */
#define HTS221_REGS_STATUS_REG      (0x27)
#define HTS221_REGS_HUMIDITY_OUT_L  (0x28)
#define HTS221_REGS_HUMIDITY_OUT_H  (0x29)
#define HTS221_REGS_TEMP_OUT_L      (0x2A)
#define HTS221_REGS_TEMP_OUT_H      (0x2B)
/** @} */

/**
 * @name   Calibration Register Map
 * @{
 */
#define HTS221_REGS_H0_RH_X2        (0x30)
#define HTS221_REGS_H1_RH_X2        (0x31)
#define HTS221_REGS_T0_DEGC_X8      (0x32)
#define HTS221_REGS_T1_DEGC_X8      (0x33)
#define HTS221_REGS_T1_T0_MSB       (0x35)
#define HTS221_REGS_H0_T0_OUT_L     (0x36)
#define HTS221_REGS_H0_T0_OUT_H     (0x37)
#define HTS221_REGS_H1_T0_OUT_L     (0x3A)
#define HTS221_REGS_H1_T0_OUT_H     (0x3B)
#define HTS221_REGS_T0_OUT_L        (0x3C)
#define HTS221_REGS_T0_OUT_H        (0x3D)
#define HTS221_REGS_T1_OUT_L        (0x3E)
#define HTS221_REGS_T1_OUT_H        (0x3F)
/** @} */

/**
 * @brief Shift for AVG oftemperature configuration set in HTS221_REGS_AV_CONF
 *
 * Register HTS221_REGS_AV_CONF= [7:6] reserved, [5:3] AVGT2-0, [2:0] AVGH2-0
 */
#define HTS221_REGS_AVGT_SHIFT      (3U)

/**
 * @name Humidity average over number of samples 4 to 512
 * @{
 */
enum {
    HTS221_REGS_AVGH_4   = 0,
    HTS221_REGS_AVGH_8,
    HTS221_REGS_AVGH_16,
    HTS221_REGS_AVGH_32,
    HTS221_REGS_AVGH_64,
    HTS221_REGS_AVGH_128,
    HTS221_REGS_AVGH_256,
    HTS221_REGS_AVGH_512
};
/** @} */

/**
 * @name Temperature average over number of samples 2 to 256
 * @{
 */
enum {
    HTS221_REGS_AVGT_2   = 0,
    HTS221_REGS_AVGT_4,
    HTS221_REGS_AVGT_8,
    HTS221_REGS_AVGT_16,
    HTS221_REGS_AVGT_32,
    HTS221_REGS_AVGT_64,
    HTS221_REGS_AVGT_128,
    HTS221_REGS_AVGT_256
};
/** @} */

/**
 * @name Config bits of HTS221_REGS_CTRL_REG1
 * @{
 */
#define HTS221_REGS_CTRL_REG1_PD_ACTIVE (1 << 7)    /**< power-down control, set active mode */
#define HTS221_REGS_CTRL_REG1_BDU       (1 << 2)    /**< Block data update */
/** @} */

/**
 * @brief Output data rate settings, HTS221_REGS_CTRL_REG1[1:0]
 */
enum {
    HTS221_REGS_CTRL_REG1_ODR_ONE_SHOT = 0,
    HTS221_REGS_CTRL_REG1_ODR_1HZ,
    HTS221_REGS_CTRL_REG1_ODR_7HZ,
    HTS221_REGS_CTRL_REG1_ODR_12HZ
};

/**
 * @name Config bits of HTS221_REGS_CTRL_REG2
 * @{
 */
#define HTS221_REGS_CTRL_REG2_BOOT      (1 << 7)    /**< Reboot memory content */
#define HTS221_REGS_CTRL_REG2_HEATER    (1 << 1)    /**< Heater ON */
#define HTS221_REGS_CTRL_REG2_OS_EN     (1 << 0)    /**< One-shot enable, start new dataset */
/** @} */

/**
 * @name Data ready config bits of HTS221_REGS_CTRL_REG3
 * @{
 */
#define HTS221_REGS_CTRL_REG3_DRDY_HL   (1 << 7)    /**< Data Ready output signal active high, low */
#define HTS221_REGS_CTRL_REG3_PP_OD     (1 << 6)    /**< Push-pull / Open Drain selection on pin 3 */
#define HTS221_REGS_CTRL_REG3_DRDY_EN   (1 << 2)    /**< Data Ready enable */
/** @} */

/**
 * @name Status bits to check data availability, HTS221_REGS_STATUS_REG[1:0]
 * @{
 */
#define HTS221_REGS_STATUS_REG_TDA      (1 << 0)    /**< temperature data available */
#define HTS221_REGS_STATUS_REG_HDA      (1 << 1)    /**< humidity data available */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
