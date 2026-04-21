/*
 * SPDX-FileCopyrightText: 2016 University of California, Berkeley
 * SPDX-FileCopyrightText: 2016 Michael Andersen <m.andersen@cs.berkeley.edu>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_mma7660
 * @{
 *
 * @file
 * @brief       Register definition for the MMA7660 accelerometer driver.
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name   Registers
 * @{
 */
#define MMA7660_XOUT    (0x00)   /**< X-axis out */
#define MMA7660_YOUT    (0x01)   /**< Y-axis out */
#define MMA7660_ZOUT    (0x02)   /**< Z-axis out */
#define MMA7660_TILT    (0x03)   /**< Tilt, Shake, Tap, Alert */
#define MMA7660_SRST    (0x04)   /**< Sample rate status */
#define MMA7660_SPCNT   (0x05)   /**< Sleep count register */
#define MMA7660_INTSU   (0x06)   /**< Interrupt setup */
#define MMA7660_MODE    (0x07)   /**< Mode register */
#define MMA7660_SR      (0x08)   /**< Sample rate register */
#define MMA7660_PDET    (0x09)   /**< Tap / Pulse detection */
#define MMA7660_PD      (0x0A)   /**< Tap / Pulse debounce */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
