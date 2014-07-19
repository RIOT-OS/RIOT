/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    basic_mac Dummy MAC
 * @ingroup     net
 * @brief       Basic link layer protocol to speak with any transceiver driver
 *              in some manner.
 * @{
 *
 * @file        basic_mac.h
 * @brief       Basic link layer protocol to speak with any transceiver driver
 *              in some manner.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __BASIC_MAC_H_
#define __BASIC_MAC_H_

#include <stdlib.h>

#include "netdev/base.h"
#include "netapi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASIC_MAC_REGISTRY_SIZE
/**
 * @brief   The size of dummy MAC's recipant registry.
 */
#define BASIC_MAC_REGISTRY_SIZE (1)
#endif

/**
 * @brief   The minimal required stack size for a
 *          dummy MAC thread
 * TODO: determine real minimal size based on variable used in C file
 */
#define BASIC_MAC_MINIMAL_STACK_SIZE    (32)

/**
 * @brief   Basic configuration types
 *
 * @extends netapi_conf_type_t
 */
typedef enum {
    BASIC_MAC_PROTO = NETAPI_CONF_PROTO,            /**< Set or get protocol */
    BASIC_MAC_CHANNEL = NETAPI_CONF_CARRIER,        /**< Set or get channel */
    BASIC_MAC_ADDRESS = NETAPI_CONF_ADDRESS,        /**< Set or get address */
    BASIC_MAC_NID = NETAPI_CONF_SUBNETS,            /**< Set or get network id
                                                     *   (e.g. PAN ID in 802.15.4)*/
    BASIC_MAC_MAX_PACKET_SIZE = NETAPI_CONF_MAX_PACKET_SIZE,    /**< Set or get maximum
                                                                 *   packet size */
    BASIC_MAC_SRC_LEN = NETAPI_CONF_SRC_LEN,        /**< Set or get default source length */
    BASIC_MAC_REGISTRY = NETAPI_CONF_REGISTRY,      /**< get registered threads */
    BASIC_MAC_ADDRESS2 = NETDEV_OPT_ADDRESS_LONG,   /**< Set or get alternative address
                                                     *   format (e.g EUI-64 in 802.15.4) */
} basic_mac_conf_type_t;

/**
 * @brief   Initializes the module
 */
void basic_mac_init_module(void);

/**
 * @brief   Initialize new dummy MAC layer.
 *
 * @param[in] stack     Stack for the control thread
 * @param[in] stacksize Size of *stack*
 * @param[in] priority  Priority for the control thread
 * @param[in] flags     Flags for the control thread
 * @param[in] name      Name for the control thread
 * @param[in] dev       An *initialized* network device to use with this MAC
 *                      layer
 *
 * @see @ref thread_create
 *
 * @return  PID of dummy MAC control thread on success
 * @return  -EINVAL if priority is invalid
 * @return  -EOVERFLOW if no slot for the new thread is available
 */
kernel_pid_t basic_mac_init(char *stack, int stacksize, char priority, int flags,
                            const char *name, netdev_t *dev);

#ifdef MODULE_NETDEV_DUMMY
/**
 * @brief   Re-updates the receive callback of a network device for testing
 *          purposes
 *
 * @param[in] dev   A network device
 */
void basic_mac_update_callback(netdev_t *dev);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BASIC_MAC_H_ */
