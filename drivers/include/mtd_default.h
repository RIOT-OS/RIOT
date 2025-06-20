/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_mtd
 * @{
 * @brief       Default MTD device configuration
 *
 * Helpers for generic MTD use.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "board.h"
#include "modules.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MODULE_MTD_SDCARD_DEFAULT)
#include "mtd_sdcard.h"
#endif

#if defined(MODULE_MTD_EMULATED)
#include "mtd_emulated.h"
#endif

#if defined(MODULE_MTD_FLASHPAGE)
#include "mtd_flashpage.h"
#endif

#if defined(MODULE_MTD_SDCARD_DEFAULT)
extern mtd_sdcard_t mtd_sdcard_dev0;
#endif

#if defined(MODULE_MTD_EMULATED)
extern mtd_emulated_t mtd_emulated_dev0;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
