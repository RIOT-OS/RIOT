/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup drivers_eeprom_native Native extra API for EEPROM
 * @ingroup  cpu_native
 * @brief    Implementation of EEPROM buffer persistence in file.
 * @{
 */

/**
 * @file
 * @author Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(EEPROM_FILEPATH_MAX_LEN) || defined(DOXYGEN)
/**
 * @brief Maximum path len to store the EEPROM filepath
 */
#  define EEPROM_FILEPATH_MAX_LEN     (128U)
#endif

/**
 * @brief   Read the configured file containing the EEPROM content
 *
 * This function is called once during the native instance startup.
 *
 * The file can be configured in a macro `EPPROM_FILE` via CFLAGS.
 */
void eeprom_native_read(void);

#ifdef __cplusplus
}
#endif

/** @} */
