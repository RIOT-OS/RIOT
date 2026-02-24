/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_suit_storage_ram  ram storage backend
 * @ingroup     sys_suit_storage
 * @brief       RAM blob SUIT payload storage backends
 *
 * @{
 *
 * @brief       RAM-based storage backend for SUIT OTA updates
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * This module implements a RAM-backed storage interface for SUIT. The main
 * purpose is mock testing the SUIT implementation, however the interface could
 * also be used for to target backup ram storage by changing @ref
 * CONFIG_SUIT_STORAGE_RAM_SIZE to store it in backup ram.
 *
 * The module uses a .ram.### structure where the number indicates the index of
 * the memory region being targeted.
 *
 * @warning The install function is implemented as a noop. There is no
 * distinction between valid content and not yet invalidated content.
 */

#include <stdint.h>

#include "suit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of each memory region
 */
#ifndef CONFIG_SUIT_STORAGE_RAM_SIZE
#define CONFIG_SUIT_STORAGE_RAM_SIZE    (64U)
#endif

/**
 * @brief Number of allocated regions
 */
#ifndef CONFIG_SUIT_STORAGE_RAM_REGIONS
#define CONFIG_SUIT_STORAGE_RAM_REGIONS (2U)
#endif

/**
 * @brief Extra attributes for allocating the RAM struct
 */
#ifndef CONFIG_SUIT_STORAGE_RAM_ATTR
#define CONFIG_SUIT_STORAGE_RAM_ATTR
#endif

/**
 * @brief Storage location string separators
 */
#ifndef CONFIG_SUIT_STORAGE_RAM_LOCATION_SEPARATOR
#define CONFIG_SUIT_STORAGE_RAM_LOCATION_SEPARATOR '.'
#endif

/**
 * @brief Storage location string prefix
 *
 * Must include the leading and trailing separators
 */
#ifndef CONFIG_SUIT_STORAGE_RAM_LOCATION_PREFIX
#define CONFIG_SUIT_STORAGE_RAM_LOCATION_PREFIX  ".ram."
#endif

/**
 * @brief Single in-memory storage region
 */
typedef struct {
    size_t occupied; /**< Region space filled */
    uint8_t mem[CONFIG_SUIT_STORAGE_RAM_SIZE]; /**< RAM area */
} suit_storage_ram_region_t;

/**
 * @brief memory storage state
 */
typedef struct CONFIG_SUIT_STORAGE_RAM_ATTR {
    suit_storage_t storage;       /**< parent struct */
    /**
     * @brief ram storage regions
     */
    suit_storage_ram_region_t regions[CONFIG_SUIT_STORAGE_RAM_REGIONS];
    size_t active_region; /**< Active region to write to */
    uint32_t sequence_no; /**< Ephemeral sequence number */
} suit_storage_ram_t;

#ifdef __cplusplus
}
#endif

/** @} */
