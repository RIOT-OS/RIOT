/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef EEPROM_NATIVE_H
#define EEPROM_NATIVE_H

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

#endif /* EEPROM_NATIVE_H */
/** @} */
