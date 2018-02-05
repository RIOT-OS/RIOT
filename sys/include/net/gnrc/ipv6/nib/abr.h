/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef NET_GNRC_IPV6_NIB_ABR_H
#define NET_GNRC_IPV6_NIB_ABR_H

#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nib/conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (GNRC_IPV6_NIB_CONF_6LBR && GNRC_IPV6_NIB_CONF_MULTIHOP_P6C) || defined(DOXYGEN)
/**
 * @brief   Adds the address of an authoritative border router to the NIB
 *
 * @param[in] addr  The address of an authoritative border router.
 *
 * @return  0 on success.
 * @return  -ENOMEM, if no space is left in the neighbor cache.
 */
int gnrc_ipv6_nib_abr_add(const ipv6_addr_t *addr);

/**
 * @brief   Removes an authoritative border router from the NIB
 *
 * @param[in] addr  The address of an authoritative border router.
 */
void gnrc_ipv6_nib_abr_del(const ipv6_addr_t *addr);
#endif  /* (GNRC_IPV6_NIB_CONF_6LBR && GNRC_IPV6_NIB_CONF_MULTIHOP_P6C) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_NIB_ABR_H */
/** @} */
