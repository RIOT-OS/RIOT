/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef MTD_DEFAULT_H
#define MTD_DEFAULT_H
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

#if defined(MODULE_MTD_SDCARD_DEFAULT)
extern mtd_sdcard_t mtd_sdcard_dev0;
#endif

#if defined(MODULE_MTD_EMULATED)
extern mtd_emulated_t mtd_emulated_dev0;
#endif

/**
 * @brief   Get the default MTD device by index
 *
 * @deprecated  Use @ref mtd_dev_get instead
 *
 * @param[in] idx   Index of the MTD device
 *
 * @return  MTD_0 for @p idx 0 and so on
 *          NULL if no MTD device exists for the given index
 */
static inline mtd_dev_t *mtd_default_get_dev(unsigned idx)
{
    return ((MTD_NUMOF != 0) && (idx < MTD_NUMOF)) ? mtd_dev_xfa[idx] : NULL;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MTD_DEFAULT_H */
