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

#include "bhp/event.h"
#include "event.h"
#include "lwip/netif.h"
#include "net/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Representation of a network interface
 */
typedef struct {
    netif_t common_netif;               /**< network interface descriptor */
    struct netif lwip_netif;            /**< lwIP interface data */
    rmutex_t lock;                      /**< lock for the interface */
    event_t ev_isr;                     /**< ISR event */
#ifdef MODULE_BHP_EVENT
    bhp_event_t bhp;                    /**< IPC Bottom Half Processor */
#endif
#if (IS_USED(MODULE_NETDEV_NEW_API))
    thread_t *thread_doing_tx;          /**< The thread currently doing TX */
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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LWIP_NETIF_COMPAT_H */
