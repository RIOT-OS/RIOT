/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_eepreg EEPROM registration
 * @ingroup     sys
 * @brief       eepreg provides a facility to easily manage the locations of
 *              data stored in EEPROM via a meta-data registry.
 *
 * The structure of the meta-data registry is intended to make it easy to
 * detect the exact layout of existent data so that automatic tools may be
 * written to migrate legacy data to new formats. It also allows the addition
 * and removal of new entries dynamically.
 *
 * @note   Names are used as identifiers and must be unique! It is also
 * recommended to keep them as short as possible (while still being unique and
 * human readable), as many systems have very small amounts of EEPROM.
 * Disemvowelment can shorten long names while still retaining readability.
 *
 * @code {unparsed}
 * The layout of the EEPROM used looks like this:
 *    EEPROM_RESERV_CPU_LOW
 *    EEPROM_RESERV_BOARD_LOW
 *    Registry magic number ("RIOTREG")
 *    Registry end pointer
 *    Registry entry 1 meta-data length (1 byte)
 *    Registry entry 1 name (unterminated)
 *    Registry entry 1 data pointer
 *    Registry entry 2 meta-data length
 *    Registry entry 2 name
 *    Registry entry 2 data pointer
 *    ... (new registry meta-data may be added in ascending order)
 *    unused space
 *    ... (new data locations may be added in descending order)
 *    Entry 2 data
 *    Entry 1 data
 *    EEPROM_RESERV_BOARD_HI
 *    EEPROM_RESERV_CPU_HI
 * @endcode
 *
 * Pointer length is dependent on the size of the available EEPROM (see
 * EEPREG_PTR_LEN below).
 *
 * @{
 *
 * @file
 * @brief       eepreg interface definitions
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */


#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EEPROM_RESERV_CPU_LOW
/**
 * @brief   EEPROM reserved near beginning for use by CPU and related
 *
 * Change with care, as it may make existing data difficult to migrate
 */
#define EEPROM_RESERV_CPU_LOW    (0U)
#endif

#ifndef EEPROM_RESERV_CPU_HI
/**
 * @brief   EEPROM reserved near end for use by CPU and related
 *
 * Change with care, as it may make existing data difficult to migrate
 */
#define EEPROM_RESERV_CPU_HI    (0U)
#endif

#ifndef EEPROM_RESERV_BOARD_LOW
/**
 * @brief   EEPROM reserved near beginning for use by board and related
 *
 * Change with care, as it may make existing data difficult to migrate
 */
#define EEPROM_RESERV_BOARD_LOW    (0U)
#endif

#ifndef EEPROM_RESERV_BOARD_HI
/**
 * @brief   EEPROM reserved near end for use by board and related
 *
 * Change with care, as it may make existing data difficult to migrate
 */
#define EEPROM_RESERV_BOARD_HI    (0U)
#endif

/**
 * @brief   Size in bytes of pointer meta-data in EEPROM
 */
#if (EEPROM_SIZE > 0x1000000)
#define EEPREG_PTR_LEN    (4U)
#elif (EEPROM_SIZE > 0x10000)
#define EEPREG_PTR_LEN    (3U)
#elif (EEPROM_SIZE > 0x100)
#define EEPREG_PTR_LEN    (2U)
#else
#define EEPREG_PTR_LEN    (1U)
#endif

/**
 * @brief   Signature of callback for iterating over entries in EEPROM registry
 *
 * @param[in] name    name of an entry in the registry
 * @param[in] arg     argument for cb
 *
 * @return    0 on success
 * @return    < 0 on failure
 */
typedef int (*eepreg_iter_cb_t)(char *name, void *arg);

/**
 * @brief   Load or write meta-data in EEPROM registry
 *
 * This checks to see if relevant meta-data exists in the EEPROM registry, and
 * returns that data position if it exists. If an entry does not exist in the
 * registry, meta-data is written and allocated data space if there is enough
 * remaining. Requesting a different length for an existent entry returns an
 * error.
 *
 * @param[out] pos    pointer to position variable
 * @param[in] name    name of entry to load or write
 * @param[in] len     requested amount of data storage
 *
 * @return    0 on success
 * @return    -EIO on EEPROM I/O error
 * @return    -ENOSPC on insufficient EEPROM for entry
 * @return    -EADDRINUSE on existing entry with different length
 */
int eepreg_add(uint32_t *pos, const char *name, uint32_t len);

/**
 * @brief   Read position meta-data from EEPROM registry
 *
 * This is similar to eepreg_add, except it never writes meta-data.
 *
 * @param[out] pos    pointer to position variable
 * @param[in] name    name of entry to load
 *
 * @return    0 on success
 * @return    -EIO on EEPROM I/O error
 * @return    -ENOENT on non-existent registry or entry
 */
int eepreg_read(uint32_t *pos, const char *name);

/**
 * @brief   Write meta-data to EEPROM registry
 *
 * This ignores existing meta-data and always makes a new entry in the
 * registry. Typical use should be through eepreg_add and not eepreg_write.
 * If multiple entries with the same name exist, eepreg functions will find
 * the oldest. Mainly intended for use by migration utilities.
 *
 * @param[out] pos    pointer to position variable
 * @param[in] name    name of entry to write
 * @param[in] len     requested amount of data storage
 *
 * @return    0 on success
 * @return    -EIO on EEPROM I/O error
 * @return    -ENOSPC on insufficient EEPROM for entry
 */
int eepreg_write(uint32_t *pos, const char *name, uint32_t len);

/**
 * @brief   Remove entry from EEPROM registry and free space
 *
 * This removes an entry from the EEPROM registry and its corresponding data
 * and moves the data and meta-data of entries after removed entry to occupy
 * the freed space. This preserves the structure of the EEPROM registry.
 * Warning: this is a read/write intensive operation! Mainly intended for use
 * by migration utilities.
 *
 * @param[in] name    name of entry to remove
 *
 * @return    0 on success
 * @return    -EIO on EEPROM I/O error
 * @return    -ENOENT on non-existent registry or entry
 */
int eepreg_rm(const char *name);

/**
 * @brief   Iterate over meta-data entries in EEPROM registry
 *
 * This executes a callback over each name in the EEPROM registry. The intended
 * work-flow for migration is to: iterate over each entry, check to see if
 * migration is needed, duplicate using eepreg_write if needed, migrate data to
 * duplicate entry, then delete old entry using eepreg_rm.
 *
 * @note   It is safe for the callback to remove the entry it is called with,
 * or to add new entries.
 *
 * @param[in] cb     callback to iterate over entries
 * @param[in] arg    argument for cb
 *
 * @return    0 on success
 * @return    -EIO on EEPROM I/O error
 * @return    -ENOENT on non-existent registry
 * @return    return value of cb when cb returns < 0
 */
int eepreg_iter(eepreg_iter_cb_t cb, void *arg);

/**
 * @brief   Check for the presence of meta-data registry
 *
 * @return    0 on success
 * @return    -EIO on EEPROM I/O error
 * @return    -ENOENT on non-existent registry
 */
int eepreg_check(void);

/**
 * @brief   Clear existing meta-data registry
 *
 * This removes any existing meta-data registry by writing a new registry with
 * no entries.
 *
 * @return    0 on success
 * @return    -EIO on EEPROM I/O error
 */
int eepreg_reset(void);

/**
 * @brief   Calculate data length from meta-data in EEPROM registry
 *
 * @note   This information is typically already available to code that has
 * called eepreg_add.
 *
 * @param[out] len    pointer to length variable
 * @param[in] name    name of entry to load or write
 *
 * @return    0 on success
 * @return    -EIO on EEPROM I/O error
 * @return    -ENOENT on non-existent registry or entry
 */
int eepreg_len(uint32_t *len, const char *name);

/**
 * @brief   Calculate length of remaining EEPROM free space
 *
 * @param[out] len    pointer to length variable
 *
 * @return    0 on success
 * @return    -EIO on EEPROM I/O error
 * @return    -ENOENT on non-existent registry
 */
int eepreg_free(uint32_t *len);

#ifdef __cplusplus
}
#endif

/** @} */
