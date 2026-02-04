/*
 * SPDX-FileCopyrightText: 2017 Frits Kuipers
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include "time_units.h"

/**
 * @ingroup     drivers_ds18
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for DS1822 and DS18B20 temperature sensors.
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name ds18 commands
 * @{
 */
#define DS18_CMD_CONVERT            (0x44)
#define DS18_CMD_RSCRATCHPAD        (0xbe)
#define DS18_CMD_WRITESCRATCHPAD    (0x4e)
#define DS18_CMD_COPYSCRATCHPAD     (0x48)
#define DS18_CMD_RECALLE            (0xb8)
#define DS18_CMD_RPWRSPPLY          (0xb4)
#define DS18_CMD_SEARCHROM          (0xf0)
#define DS18_CMD_READROM            (0x33)
#define DS18_CMD_MATCHROM           (0x55)
#define DS18_CMD_SEARCHROM          (0xf0)
#define DS18_CMD_ALARMSEARCH        (0xec)
#define DS18_CMD_SKIPROM            (0xcc)
/** @} */

/**
 * @name ds18 delays
 * @{
 */
#define DS18_DELAY_RESET            (480U)
#define DS18_DELAY_PRESENCE         (60U)
#define DS18_DELAY_SLOT             (60U)
#define DS18_SAMPLE_TIME            (10U)
#define DS18_DELAY_CONVERT          (750U * US_PER_MS)
#define DS18_DELAY_RW_PULSE         (1U)
#define DS18_DELAY_R_RECOVER        (DS18_DELAY_SLOT - DS18_SAMPLE_TIME)
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */
