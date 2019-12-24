/*
 * Copyright (C) 2018-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_nimble_netif_conn Connection State Management for netif
 * @ingroup     pkg_nimble_netif
 * @brief       Helper module for managing the memory needed to store the
 *              BLE connection state for the netif wrapper
 * @{
 *
 * @file
 * @brief       Connection allocation and maintenance for NimBLE netif
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_NETIF_CONN_H
#define NIMBLE_NETIF_CONN_H

#include <stdint.h>

#include "nimble_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Value for marking a handle invalid
 */
#define NIMBLE_NETIF_CONN_INVALID       (-1)

/**
 * @brief   Memory layout for holding the relevant connection information
 */
typedef struct {
    struct ble_l2cap_chan *coc;     /**< l2cap context as exposed by NimBLE */
    uint16_t gaphandle;             /**< GAP handle exposed by NimBLE */
    uint16_t state;                 /**< the current state of the context */
    uint8_t addr[BLE_ADDR_LEN];     /**< BLE address of connected peer
                                         (in network byte order) */
} nimble_netif_conn_t;

/**
 * @brief   Iterator function signature used by @ref nimble_netif_conn_foreach()
 *
 * @param[in] conn          connection context of the current entry
 * @param[in] handle        handle of the current entry
 * @param[in] arg           user supplied argument
 *
 * @return  0 to continue
 * @return  != 0 to stop iterating
 */
typedef int (*nimble_netif_conn_iter_t)(nimble_netif_conn_t *conn,
                                        int handle, void *arg);

/**
 * @brief   Initialize the connection state manager
 *
 * This functions is typically called by @ref nimble_netif_init().
 */
void nimble_netif_conn_init(void);

/**
 * @brief   Get the connection context corresponding to the given handle
 *
 * @param[in] handle        handle to a connection context
 *
 * @return  pointer to the corresponding connection context
 * @return  NULL if handle in invalid
 */
nimble_netif_conn_t *nimble_netif_conn_get(int handle);

/**
 * @brief   Get the handle to the context that is currently advertising
 *
 * @return  handle to the currently advertising context
 * @return  NIMBLE_NETIF_CONN_INVALID if not advertising
 */
int nimble_netif_conn_get_adv(void);

/**
 * @brief   Get the handle to the context that is busy connecting
 *
 * @return  handle to the busy context
 * @return  NIMBLE_NETIF_CONN_INVALID if not busy connecting
 */
int nimble_netif_conn_get_connecting(void);

/**
 * @brief   Find the connection to the peer with the given BLE address
 *
 * @param[in] addr          BLE address, in network byte order
 *
 * @return  handle to the matching connection context
 * @return  NIMBLE_NETIF_CONN_INVALID if no matching connection was found
 */
int nimble_netif_conn_get_by_addr(const uint8_t *addr);

/**
 * @brief   Find the connection using the given NimBLE GAP handle
 *
 * @param[in] gaphandle     GAP handle as exposed by NimBLE
 *
 * @return  handle to the matching connection context
 * @return  NIMBLE_NETIF_CONN_INVALID if no matching connection was found
 */
int nimble_netif_conn_get_by_gaphandle(uint16_t gaphandle);


/**
 * @brief   Iterate over all connection contexts that match the filter condition
 *
 * @warning Do not call any other nimble_netif_conn function from within the
 *          callback, this will lead to a deadlock!
 *
 * @param[in] filter        filter mask
 * @param[in] cb            callback called on each filtered entry
 * @param[in] arg           user argument
 */
void nimble_netif_conn_foreach(uint16_t filter,
                               nimble_netif_conn_iter_t cb, void *arg);

/**
 * @brief   Count the number of connections contexts for which the given filter
 *          applies
 *
 * @param[in] filter        filter mask
 *
 * @return  number of contexts for which the filter applied
 */

unsigned nimble_netif_conn_count(uint16_t filter);

/**
 * @brief   Allocate an unused context for starting a connection
 *
 * @param[in] addr          the BLE address of the peer node, in network byte
 *                          order
 *
 * @return handle used for the new connection
 */
int nimble_netif_conn_start_connection(const uint8_t *addr);

/**
 * @brief   Reserve a unused context for the purpose of accepting a new
 *          connection
 *
 * @return  handle of the reserved context
 * @return  NIMBLE_NETIF_CONN_INVALID if no unused context was available
 */
int nimble_netif_conn_start_adv(void);

/**
 * @brief   Free the connection context with the given handle
 */
void nimble_netif_conn_free(int handle, uint8_t *addr);

/**
 * @brief   Find the connection context with a given GAP handle and return a
 *          pointer to it
 *
 * @param[in] gh            GAP handle used by NimBLE
 *
 * @return  Pointer to the selected context
 * @return  NULL if no fitting context was found
 */
static inline
nimble_netif_conn_t *nimble_netif_conn_from_gaphandle(uint16_t gh)
{
    return nimble_netif_conn_get(nimble_netif_conn_get_by_gaphandle(gh));
}

/**
 * @brief   Convenience function to check if any context is currently in the
 *          connecting state (@ref NIMBLE_NETIF_CONNECTING)
 *
 * @return  != 0 if true
 * @return  0 if false
 */
static inline int nimble_netif_conn_connecting(void)
{
    return (nimble_netif_conn_get_connecting() != NIMBLE_NETIF_CONN_INVALID);
}

/**
 * @brief   Convenience function to check if we are currently connected to a
 *          peer with the given address
 *
 * @param[in] addr          BLE address, in network byte order
 *
 * @return  != 0 if true
 * @return  0 if false
 */
static inline int nimble_netif_conn_connected(const uint8_t *addr)
{
    return (nimble_netif_conn_get_by_addr(addr) != NIMBLE_NETIF_CONN_INVALID);
}


/**
 * @brief   Convenience function to check if any context is currently in the
 *          advertising state (@ref NIMBLE_NETIF_ADV)
 *
 * @return  != 0 if true
 * @return  0 if false
 */
static inline int nimble_netif_conn_is_adv(void)
{
    return (nimble_netif_conn_get_adv() != NIMBLE_NETIF_CONN_INVALID);
}

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_NETIF_CONN_H */
/** @} */
