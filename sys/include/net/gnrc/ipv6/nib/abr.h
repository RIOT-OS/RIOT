/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_gnrc_ipv6_nib_abr   Authoritative border router list
 * @ingroup     net_gnrc_ipv6_nib
 * @brief       Authoritative border router list component of neighbor
 *              information base
 * @{
 *
 * @file
 * @brief   Authoritative border router list definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "modules.h"

/* prevent cascading include error to xtimer if it is not compiled in or not
 * supported by board */
#if IS_USED(MODULE_EVTIMER)
#include "timex.h"
#include "evtimer.h"
#endif
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nib/conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Authoritative border router list entry view on NIB
 */
typedef struct {
    ipv6_addr_t addr;       /**< The address of the border router */
    uint32_t version;       /**< last received version */
    uint32_t valid_until_ms;   /**< timestamp (in ms) until which the information is valid
                                *   (needs resolution in minutes an 16 bits of them)*/
} gnrc_ipv6_nib_abr_t;

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C) || defined(DOXYGEN)
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR) || defined(DOXYGEN)
/**
 * @brief   Adds the address of an authoritative border router to the NIB
 *
 * @param[in] addr  The address of an authoritative border router.
 *
 * @return  0 on success.
 * @return  -ENOMEM, if no space is left in the neighbor cache.
 * @return  -ENOTSUP, if @ref CONFIG_GNRC_IPV6_NIB_6LBR or
 *          @ref CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C is not defined
 */
int gnrc_ipv6_nib_abr_add(const ipv6_addr_t *addr);

/**
 * @brief   Removes an authoritative border router from the NIB
 *
 * @param[in] addr  The address of an authoritative border router.
 */
void gnrc_ipv6_nib_abr_del(const ipv6_addr_t *addr);
#else   /* CONFIG_GNRC_IPV6_NIB_6LBR || defined(DOXYGEN) */
#define gnrc_ipv6_nib_abr_add(addr)     (-ENOTSUP)
#define gnrc_ipv6_nib_abr_del(addr)     (void)(addr)
#endif  /* CONFIG_GNRC_IPV6_NIB_6LBR || defined(DOXYGEN) */

/**
 * @brief   Iterates over all authoritative border router in the NIB
 *
 * @pre `(state != NULL) && (abr != NULL)`
 *
 * @param[in,out] state Iteration state of the authoritative border router list.
 *                      Must point to NULL pointer to start iteration
 * @param[out] abr      The next authoritative border router list entry.
 *
 * Usage example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/gnrc/ipv6/nib/abr.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     gnrc_ipv6_nib_abr_t abr;
 *
 *     puts("My border routers:");
 *     while (gnrc_ipv6_nib_abr_iter(&state, &abr)) {
 *         gnrc_ipv6_nib_abr_print(&abr);
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note    The list may change during iteration.
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p abr is the last authoritative border router entry in
 *          the NIB.
 */
bool gnrc_ipv6_nib_abr_iter(void **state, gnrc_ipv6_nib_abr_t *abr);

#if IS_USED(MODULE_EVTIMER) || defined(DOXYGEN)
/**
 * @brief   Provides the time in minutes for which the authoritative border
 *          router entry is valid
 *
 * @param[in] abr   An authoritative border router entry.
 *
 * @return  The time in minutes for which the authoritative border router entry
 *          is valid.
 */
static inline uint32_t gnrc_ipv6_nib_abr_valid_offset(const gnrc_ipv6_nib_abr_t *abr)
{
    return (abr->valid_until_ms - evtimer_now_msec()) / ( MS_PER_SEC * SEC_PER_MIN);
}
#endif

/**
 * @brief   Prints an authoritative border router list entry
 *
 * @pre `abr != NULL`
 *
 * @param[in] abr   An authoritative border router list entry
 */
void gnrc_ipv6_nib_abr_print(gnrc_ipv6_nib_abr_t *abr);
#else   /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C || defined(DOXYGEN) */
#define gnrc_ipv6_nib_abr_add(addr)         (-ENOTSUP)
#define gnrc_ipv6_nib_abr_del(addr)         (void)(addr)
#define gnrc_ipv6_nib_abr_iter(state, abr)  (false)
#define gnrc_ipv6_nib_abr_print(abr)        (void)(abr)
#endif  /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/** @} */
