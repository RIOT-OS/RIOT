/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netif   Network interfaces
 * @ingroup     net
 * @brief       Common network interface API
 *
 * This allows access to network interfaces regardless of the network stack
 * implementation. The network stack must provide definitions for:
 *
 * - @p netif_get_name
 * - @p netif_get_opt
 * - @p netif_set_opt
 * - @p netif_send
 * - @p netif_receive
 * - @p netif_rx_done
 * - @p netif_tx_done
 * - @p netif_alloc_rx_pkt
 * - @p netif_link_up
 * - @p netif_link_down
 *
 * The network stack should also register each interface via @p netif_register.
 *
 * @{
 *
 * @file
 * @brief   Common network interface API definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef NET_NETIF_H
#define NET_NETIF_H

#include <stdint.h>

#include "net/netopt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum length for an interface name
 */
#ifndef NETIF_NAMELENMAX
#define NETIF_NAMELENMAX    (8U)
#endif

/**
 * @brief Network interface descriptor.
 *
 * @note All network interfaces should inherit from this structure.
 */
typedef struct __netif netif_t;

struct __netif {
    netif_t *next;  /**< Pointer to the next interface */
};

/**
 * @brief   Iterator for the interfaces
 *
 * Returns interface after @p last. To start use `last == NULL`.
 *
 * @param[in] last  The previous interface. Use `NULL` to start
 *                  iteration.
 *
 * @return next network interface.
 * @return @ref NULL, if there is no interface after @p last
 */
netif_t *netif_iter(netif_t *last);

/**
 * @brief   Gets name of an interface
 *
 * @pre `name != NULL`
 * @pre name holds at least @ref NETIF_NAMELENMAX characters
 *
 * @note    Supposed to be implemented by the networking module. `name` must be
 *          zero-terminated in the result!
 *
 * @param[in] netif A network interface.
 * @param[out] name The name of the interface. Must not be `NULL`. Must at least
 *                  hold @ref NETIF_NAMELENMAX bytes.
 *
 * @return  length of @p name on success
 */

int netif_get_name(netif_t *netif, char *name);

/**
 * @brief   Gets interface by name
 *
 * @pre `name != NULL`
 *
 *
 * @param[in] name  The name of an interface as a zero-terminated. Must not be
 *                  `NULL`.
 *
 * @return  The identifier of the interface on success.
 * @return  @ref NULL if no interface is named @p name.
 */
netif_t *netif_get_by_name(const char *name);

/**
 * @brief   Gets option from an interface
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in]   netif   A network interface.
 * @param[in]   opt     Option type.
 * @param[in]   context (Optional) context to the given option
 * @param[out]  value   Pointer to store the option's value in.
 * @param[in]   max_len Maximal amount of byte that fit into @p value.
 *
 * @return  Number of bytes written to @p value.
 * @return  `< 0` on error, 0 on success.
 */
int netif_get_opt(netif_t *netif, netopt_t opt, uint16_t context,
                  void *value, size_t max_len);

/**
 * @brief   Sets option to an interface
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] netif     A network interface.
 * @param[in] opt       Option type.
 * @param[in] context   (Optional) context to the given option
 * @param[in] value     Pointer to store the option's value in.
 * @param[in] value_len The length of @p value.
 *
 * @return Number of bytes used from @p value.
 * @return `< 0` on error, 0 on success.
 */
int netif_set_opt(netif_t *netif, netopt_t opt, uint16_t context,
                  void *value, size_t value_len);


/**
 * @brief   Registers a network interface in the global interface list.
 *
 * @note    This functions should be called when initializing an interface.
 *
 * @param[in] netif     Interface to be registered
 *
 * @return  0 on success
 * @return  -EINVAL if @p netif is NULL.
 */
int netif_register(netif_t *netif);

/**
 * @brief Sends a packet to a given interface
 *
 * @param[in] netif     A network interface
 * @param[in] msdu       MSDU packet to be sent (full L3 frame)
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int netif_send(netif_t *netif, iolist_t *msdu);

/**
 * @brief   Does link layer processing
 *
 *          This functions should be called right after the packet is fetched
 *          from the network device.
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] netif     A network interface
 * @param[in] psdu      Pointer to the PSDU (full L2 frame)
 * @param[in] psdu_len  Size of the PSDU
 * @param[in] rx_data   Data associated to the packet reception (LQI, RSSI, etc).
 *                      Can be NULL if the network device doesn't report data.
 */
void netif_recv(netif_t *netif, uint8_t *msdu, size_t msdu_len, void *rx_data);

/**
 * @brief   Rx done event
 *
 *          This function should be called from the MAC layer when
 *          the receive procedure is done. It can be used to dispatch packets,
 *          release buffers and tell upper layers about the reception
 *          of a packet.
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] netif     A network interface
 * @param[in] msdu      MSDU (full L3 frame) of the processed packet. If NULL,
 *                      there's no MSDU to dispatch.
 * @param[in] msdu_len  Length of the MSDU
 * @param[in] rx_data   Data associated to the packet reception (LQI, RSSI, etc).
 *                      Can be NULL if not data is passed to the stack.
 */
void netif_rx_done(netif_t *netif, uint8_t *msdu, size_t msdu_len, void *rx_data);

/**
 * @brief   Tx done event
 *
 *          This function should be called from the MAC layer when the
 *          transmission procedure is done. It can be used to release buffers
 *          and notify the upper layer about the transmission of a packet.
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] netif     A network interface
 * @param[in] msdu      MSDU of the sent packet
 * @param[in] status    Status of the transmission
 */
void netif_tx_done(netif_t *netif, iolist_t *msdu, int status);

/**
 * @brief   Allocates a buffer for the RX packet.
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] netif     A network interface
 * @param[in] size      Size of the packet to be allocated
 *
 * @return  pointer to the allocated buffer
 * @return  NULL if the stack couldn't allocate the packet
 */
void *netif_rx_pkt_alloc(netif_t *netif, size_t size);

/**
 * @brief   Called by the network interface when the interface is up
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] netif     A network interface
 */
void netif_link_up(netif_t *netif);

/**
 * @brief   Called by the network interface when the interface is down
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] netif     A network interface
 */
void netif_link_down(netif_t *netif);

#ifdef __cplusplus
}
#endif

#endif /* NET_NETIF_H */
/** @} */
