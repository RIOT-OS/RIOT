/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_si70xx
 *
 * @{
 * @file
 * @brief       Internal definitions for Si7006/13/20/21
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Si70xx chip addresses.
 */
#define SI70XX_I2C_ADDRESS          (0x40)

/**
 * @name Si70xx device commands.
 * @{
 */
#define SI70XX_MEASURE_RH_HOLD      (0xE5)
#define SI70XX_MEASURE_RH           (0xF5)
#define SI70XX_MEASURE_TEMP_HOLD    (0xE3)
#define SI70XX_MEASURE_TEMP         (0xF3)
#define SI70XX_MEASURE_TEMP_PREV    (0xE0)
#define SI70XX_RESET                (0xFE)
#define SI70XX_WRITE_USER_REG       (0xE6)
#define SI70XX_READ_USER_REG        (0xE7)
#define SI70XX_WRITE_HEATER_REG     (0x51)
#define SI70XX_READ_HEATER_REG      (0x11)
#define SI70XX_READ_ID_FIRST_A      (0xFA)
#define SI70XX_READ_ID_FIRST_B      (0x0F)
#define SI70XX_READ_ID_SECOND_A     (0xFC)
#define SI70XX_READ_ID_SECOND_B     (0xC9)
#define SI70XX_READ_REVISION_A      (0x84)
#define SI70XX_READ_REVISION_B      (0xB8)
/** @} */

/**
 * @name Si70xx register values.
 * @{
 */
#if defined(MODULE_SI7006)
#define SI70XX_ID                   (6)
#elif defined(MODULE_SI7013)
#define SI70XX_ID                   (13)
#elif defined(MODULE_SI7020)
#define SI70XX_ID                   (20)
#elif defined(MODULE_SI7021)
#define SI70XX_ID                   (21)
#elif defined(MODULE_SI7050)
#define SI70XX_ID                   (50)
#elif defined(MODULE_SI7051)
#define SI70XX_ID                   (51)
#elif defined(MODULE_SI7053)
#define SI70XX_ID                   (53)
#elif defined(MODULE_SI7054)
#define SI70XX_ID                   (54)
#elif defined(MODULE_SI7055)
#define SI70XX_ID                   (55)
#else
#error "Please provide a valid Si70xx variant (Si7006/13/20/21/5X)"
#endif

#define SI70XX_REVISION_1           (0xFF)
#define SI70XX_REVISION_2           (0x20)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
