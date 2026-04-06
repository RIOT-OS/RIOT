/*
 * SPDX-FileCopyrightText: 2020 Viktor Gal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_seesaw_soil
 * @{
 *
 * @file
 * @brief       Register definitions for Seesaw Soil devices
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Manufacturer ID
 * @{
 */
#define SEESAW_SOIL_MID_VALUE          0x55
/** @} */

/**
 * @name    Register Map
 * @{
 */
#define SEESAW_SOIL_MANUFACTURER_ID     (0x01)
#define SEESAW_SOIL_TEMPERATURE         (0x04)
#define SEESAW_SOIL_MOISTURE            (0x0F10)
#define SEESAW_SOIL_RST                 (0x7F)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
