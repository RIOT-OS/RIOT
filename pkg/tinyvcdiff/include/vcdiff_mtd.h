/*
 * Copyright (C) 2022 Juergen Fitschen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @{
 *
 * @file
 * @ingroup  pkg_tinyvcdiff
 *
 * @author  Juergen Fitschen <me@jue.yt>
 */

#include "vcdiff.h"
#include "mtd.h"

#ifndef CONFIG_TINYVCDIFF_MTD_WRITE_SIZE
/**
 * @brief Alignment and minimum size for MTD write access
 */
#define CONFIG_TINYVCDIFF_MTD_WRITE_SIZE 4
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Driver for accessing MTD devices
 */
extern const vcdiff_driver_t vcdiff_mtd_driver;

/**
 * @brief Context for the underlying MTD device
 */
typedef struct {
    /**
     * @brief Instance of the backing MTD device
     */
    mtd_dev_t *dev;

    /**
     * @brief Number of the next sector that must be erased
     */
    size_t next_erase_sector;

    /**
     * @brief Buffer for aligned writes
     */
    uint8_t write_buffer[CONFIG_TINYVCDIFF_MTD_WRITE_SIZE];

    /**
     * @brief Current offset on the MTD device
     */
    size_t offset;
} vcdiff_mtd_t;

/**
 * @brief Initializes vcdiff_mtd_t
 */
#define VCDIFF_MTD_INIT(DEV) { .dev = DEV }

#ifdef __cplusplus
}
#endif

/** @} */
