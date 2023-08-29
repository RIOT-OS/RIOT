/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     drivers_mtd
 * @{
 * @brief       Default MTD device configuration
 *
 * Helpers for generic MTD use.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef MTD_DEFAULT_H
#define MTD_DEFAULT_H

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

#if !defined(MTD_NUMOF) && !DOXYGEN

#if defined(MTD_3)
#define MTD_BOARD_NUMOF     4
#elif defined(MTD_2)
#define MTD_BOARD_NUMOF     3
#elif defined(MTD_1)
#define MTD_BOARD_NUMOF     2
#elif defined(MTD_0)
#define MTD_BOARD_NUMOF     1
#else
#define MTD_BOARD_NUMOF     0
#endif

#define MTD_SDCARD_NUMOF    IS_USED(MODULE_MTD_SDCARD_DEFAULT)
#define MTD_EMULATED_NUMOF  IS_USED(MODULE_MTD_EMULATED)

/**
 * @brief   Number of MTD devices
 */
#define MTD_NUMOF           (MTD_BOARD_NUMOF + MTD_SDCARD_NUMOF + MTD_EMULATED_NUMOF)

#else
#define MTD_BOARD_NUMOF     MTD_NUMOF
#endif /*  !defined(MTD_NUMOF) && !DOXYGEN */

#if !DOXYGEN

/**
 * @brief   Declare `mtd*` according to the number of MTD devices
 */
#if MTD_NUMOF > 0
extern mtd_dev_t *mtd0;
#endif
#if MTD_NUMOF > 1
extern mtd_dev_t *mtd1;
#endif
#if MTD_NUMOF > 2
extern mtd_dev_t *mtd2;
#endif
#if MTD_NUMOF > 3
extern mtd_dev_t *mtd3;
#endif
#if MTD_NUMOF > 4
extern mtd_dev_t *mtd4;
#endif
#if MTD_NUMOF > 5
extern mtd_dev_t *mtd5;
#endif
#endif /* !DOXYGEN */

#if defined(MODULE_MTD_SDCARD_DEFAULT)
extern mtd_sdcard_t mtd_sdcard_dev0;
#endif

#if defined(MODULE_MTD_EMULATED)
extern mtd_emulated_t mtd_emulated_dev0;
#endif

/**
 * @brief   Get the default MTD device by index
 *
 * @param[in] idx   Index of the MTD device
 *
 * @return  MTD_0 for @p idx 0 and so on
 *          NULL if no MTD device exists for the given index
 */
static inline mtd_dev_t *mtd_default_get_dev(unsigned idx)
{
    switch (idx) {
#if MTD_BOARD_NUMOF > 0
    case 0: return MTD_0;
#endif
#if MTD_BOARD_NUMOF > 1
    case 1: return MTD_1;
#endif
#if MTD_BOARD_NUMOF > 2
    case 2: return MTD_2;
#endif
#if MTD_BOARD_NUMOF > 3
    case 3: return MTD_3;
#endif
#if MTD_SDCARD_NUMOF > 0
    case MTD_BOARD_NUMOF: return (mtd_dev_t *)&mtd_sdcard_dev0;
#endif
#if MTD_EMULATED_NUMOF > 0
    case MTD_BOARD_NUMOF + MTD_SDCARD_NUMOF: return (mtd_dev_t *)&mtd_emulated_dev0;
#endif
    }
    return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* MTD_DEFAULT_H */
/** @} */
