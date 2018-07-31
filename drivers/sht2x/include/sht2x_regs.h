/*
 * Copyright (C) 2017 George Psimenos (gp7g14@soton.ac.uk)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht2x
 * @{
 *
 * @file
 * @brief       Register definition for the SHT2x sensor driver
 *
 * @author      George Psimenos <gp7g14@soton.ac.uk>
 *
 */

#ifndef SHT2X_REG_H
#define SHT2X_REG_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Device commands
 * @{
 */
#define SHT2X_TRIG_T_MEASUREMENT_HM 	 	0xE3 /**< trigger temp measurement, hold master */
#define SHT2X_TRIG_RH_MEASUREMENT_HM 	 	0xE5 /**< trigger humidity measurement, hold master */
#define SHT2X_TRIG_T_MEASUREMENT_POLL 		0xF3 /**< trigger temp measurement, no hold master */
#define SHT2X_TRIG_RH_MEASUREMENT_POLL 		0xF5 /**< trigger humidity measurement, no hold master */
#define SHT2X_USER_REG_W 	 				0xE6 /**< write user register */
#define SHT2X_USER_REG_R 	 				0xE7 /**< read user register */
#define SHT2X_SOFT_RESET 	 				0xFE /**< soft reset */
/** @} */

/**
 * @brief   User register masks
 * @{
 */
#define SHT2X_USER_RESOLUTION_MASK			0x81
#define SHT2X_USER_EOB_MASK					0x40
#define SHT2X_USER_HEATER_MASK				0x04
#define SHT2X_USER_RESERVED_MASK			0x38
#define SHT2X_USER_OTP_MASK					0x02
/** @} */

#ifdef __cplusplus
}
#endif

#endif /** @} */
