/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_pcd8544
 *
 * @{
 *
 * @file
 * @brief       Internal definitions for PCD8544 displays
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Delay for resetting the device
 */
#define RESET_DELAY         (5000)  /* keep reset low for 5ms */

/**
 * @name    Communication modes: command or data
 * @{
 */
#define MODE_CMD            (0)     /**< we are sending a command */
#define MODE_DTA            (1)     /**< we are sending some data */
/** @} */

/**
 * @name    Maximum parameter values
 * @{
 */
#define CONTRAST_MAX        (127U)  /**< maximum possible contrast value */
#define TEMP_MAX            (3U)    /**< maximum possible temp coefficient */
#define BIAS_MAX            (7U)    /**< maximum possible bias value */
/** @} */

/**
 * @name    Display commands
 * @{
 */
#define CMD_DISABLE         (0x24)  /**< set LCD into power down mode */
#define CMD_ENABLE_H        (0x20)  /**< ON using horizontal addressing */
#define CMD_ENABLE_V        (0x22)  /**< ON using vertical addressing */
#define CMD_EXTENDED        (0x21)  /**< enter extended instruction set mode */

#define CMD_MODE_BLANK      (0x08)  /**< display nothing */
#define CMD_MODE_NORMAL     (0x0c)  /**< display memory content */
#define CMD_MODE_ALLON      (0x09)  /**< display 'black' screen */
#define CMD_MODE_INVERSE    (0x0d)  /**< display inverted memory content */

#define CMD_SET_Y           (0x40)  /**< set 3-bit y address, OR with Y value */
#define CMD_SET_X           (0x80)  /**< set 7-bit x address, OR with X value */

#define CMD_EXT_TEMP        (0x04)  /**< select temperature coefficient */
#define CMD_EXT_BIAS        (0x10)  /**< select BIAS value */
#define CMD_EXT_CONTRAST    (0x80)  /**< set contrast, OR with [0 - 127] */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
