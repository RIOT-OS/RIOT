/*
 * SPDX-FileCopyrightText: 2014-2015 Martine S. Lenders <m.lenders@fu-berlin.de>
 * SPDX-FileCopyrightText: 2014-2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup net_gnrc_pktbuf
 * @brief   Internal definitions of the static implementation of
 *          @ref net_gnrc_pktbuf
 * @{
 *
 * @file
 * @brief   Definitions of types and their alignment for usage in tests
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mask to align packet buffer allocations with size of @ref _unused_t
 */
#define GNRC_PKTBUF_STATIC_ALIGN_MASK   (sizeof(_unused_t) - 1)

/**
 * @brief   Marks an unused section of the packet buffer arena array
 */
typedef struct _unused {
    struct _unused *next;   /**< the next unused section */
    unsigned int size;      /**< the size of the unused section */
} _unused_t;

/**
 * @brief   Calculates the required space of a number of bytes including
 *          alignment to the size of @ref _unused_t
 */
static inline size_t _align(size_t size)
{
    return (size + GNRC_PKTBUF_STATIC_ALIGN_MASK) &
          ~(GNRC_PKTBUF_STATIC_ALIGN_MASK);
}

#ifdef __cplusplus
}
#endif

/** @} */
