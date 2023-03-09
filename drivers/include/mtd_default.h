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
#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of MTD devices
 */
#ifndef MTD_NUMOF
#if defined(MTD_3)
#define MTD_NUMOF 4
#elif defined(MTD_2)
#define MTD_NUMOF 3
#elif defined(MTD_1)
#define MTD_NUMOF 2
#elif defined(MTD_0)
#define MTD_NUMOF 1
#else
#define MTD_NUMOF 0
#endif
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
#ifdef MTD_0
    case 0: return MTD_0;
#endif
#ifdef MTD_1
    case 1: return MTD_1;
#endif
#ifdef MTD_2
    case 2: return MTD_2;
#endif
#ifdef MTD_3
    case 3: return MTD_3;
#endif
    }

    return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* MTD_DEFAULT_H */
/** @} */
