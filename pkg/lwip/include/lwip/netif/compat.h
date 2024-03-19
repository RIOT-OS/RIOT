/*
 * Copyright (C) 2021 Google LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lwip
 *
 * @{
 *
 * @file
 * @brief   lwIP definition based on common netif_t struct for network
 *          stack-independent API.
 *
 * @author  Erik Ekman <eekman@google.com>
 */
#ifndef LWIP_NETIF_COMPAT_H
#define LWIP_NETIF_COMPAT_H

#include "lwip/netif.h"
#include "net/netif.h"
#include "bhp/msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Representation of a network interface
 */
typedef struct {
    netif_t common_netif;                /**< network interface descriptor */
    struct netif lwip_netif;             /**< lwIP interface data */
    rmutex_t lock;                       /**< lock for the interface */
#if IS_USED(MODULE_BHP_MSG)
    bhp_msg_t bhp;                       /**< IPC Bottom Half Processor */
#endif
} lwip_netif_t;

/**
 * @brief   Init the LWIP netif network device lock.
 *
 * @param[in]   netif pointer to the LWIP network interface
 */
static inline void lwip_netif_dev_lock_init(struct netif *netif)
{
    lwip_netif_t *compat_netif = container_of(netif, lwip_netif_t, lwip_netif);
    rmutex_init(&compat_netif->lock);
}

/**
 * @brief   Acquire the LWIP netif network device.
 *
 * @param[out]   netif pointer to the LWIP network interface
 */
static inline void lwip_netif_dev_acquire(struct netif *netif)
{
    lwip_netif_t *compat_netif = container_of(netif, lwip_netif_t, lwip_netif);
    rmutex_lock(&compat_netif->lock);
}

/**
 * @brief   Release the LWIP netif network device.
 *
 * @param[out]   netif pointer to the LWIP network interface
 */
static inline void lwip_netif_dev_release(struct netif *netif)
{
    lwip_netif_t *compat_netif = container_of(netif, lwip_netif_t, lwip_netif);
    rmutex_unlock(&compat_netif->lock);
}

/**
 * @brief Get the IPC based Bottom Half Processor for LWIP
 *
 * @param[in]   netif pointer to the LWIP network interface
 *
 * @return      pointer to the IPC based Bottom Half Processor descriptor, if
 *              @ref sys_bhp_msg is present.
 * @return      NULL otherwise
 */
static inline bhp_msg_t *lwip_netif_get_bhp(struct netif *netif)
{
#if IS_USED(MODULE_BHP_MSG)
    lwip_netif_t *compat_netif = container_of(netif, lwip_netif_t, lwip_netif);
    return &compat_netif->bhp;
#else
    (void) netif;
    return NULL;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETIF_COMPAT_H */
/** @} */
