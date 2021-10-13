/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_pktbuf
 * @brief       Internal stuff for gnrc_pktbuf
 *
 * @{
 *
 * @file
 * @brief   Interface definition for the global network buffer. Network devices
 *          and layers can allocate space for packets here.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 */
#ifndef PKTBUF_INTERNAL_H
#define PKTBUF_INTERNAL_H

#include <stdbool.h>
#include <stdlib.h>

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mutex used to provide mutually exclusive access to packet buffer
 *          data structures
 *
 * @warning This mutex will be ***internally*** locked and unlocked and should
 *          not be touched by external code
 */
extern mutex_t gnrc_pktbuf_mutex;

#if IS_USED(MODULE_GNRC_PKTBUF_STATIC) || DOXYGEN
/**
 * @brief   The actual static buffer used when module gnrc_pktbuf_static is used
 *
 * @warning This is an internal buffer and should not be touched by external code
 */
extern uint8_t *gnrc_pktbuf_static_buf;
#endif

/**
 * @brief   Check if the given pointer is indeed part of the packet buffer
 *
 * @warning This function is ***internal***.
 *
 * @param   ptr     pointer to check
 *
 * @retval  true    It is plausible, that this pointer belongs to data in
 *                  the packet buffer
 * @retval  false   @p ptr does not point to data in the packet buffer
 */
static inline bool gnrc_pktbuf_contains(void *ptr)
{
#if IS_USED(MODULE_GNRC_PKTBUF_STATIC)
    return (unsigned)((uint8_t *)ptr - gnrc_pktbuf_static_buf) < CONFIG_GNRC_PKTBUF_SIZE;
#else
    (void)ptr;
    return true;
#endif
}

/**
 * @brief   Release an internal buffer
 *
 * @warning This function is ***internal***. Use @ref gnrc_pktbuf_release to
 *          release a packet buffer
 *
 * @param   data    pointer to the internal buffer to release
 * @param   size    size of @p data in bytes
 */
void gnrc_pktbuf_free_internal(void *data, size_t size);

/* for testing */
#ifdef TEST_SUITES
/**
 * @brief   Checks if packet buffer is empty
 *
 * @return  true, if packet buffer is empty
 * @return  false, if packet buffer is not empty
 */
bool gnrc_pktbuf_is_empty(void);

/**
 * @brief   Checks if the implementation's internal invariants still uphold
 *
 * @return  true, the packet buffer is sane.
 * @return  false, the packet buffer is insane.
 */
bool gnrc_pktbuf_is_sane(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PKTBUF_INTERNAL_H */
/** @} */
