/*
 * Copyright (C) 2020 Viktor Gal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
