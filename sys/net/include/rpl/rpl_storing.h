/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net
 * @brief       Routing Protocol for Low power and Lossy Networks
 * @{
 *
 * @file        rpl_mode.h
 * @brief       RPL storing-mode header
 *
 * Header which includes all mode related RPL-functions. All functions are mandatory for any
 * RPL-mode. Describes receiving and sending of all RPL-related messages and special initialization behavior.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 */

#ifndef __RPL_SM_H
#define __RPL_SM_H

#include "rpl_structs.h"
#include "rpl_config.h"
#include "rpl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialization of RPL-root.
 *
 * This function initializes all RPL resources especially for root purposes. Initializes a new DODAG and sets
 * itself as root. Starts trickle-timer so sending DIOs starts and other can join the DODAG.
 *
 */
void rpl_init_root_mode(void);

/**
 * @brief Initialization of RPL storing mode.
 *
 * This function initializes all basic RPL mode resources. For this mode this includes only acquiring the own
 * address.
 *
 * @param[in] my_ipv6_legacy_address       Own IPv6 address as assigned by RPL core-initialization.
 *
 */
void rpl_init_mode(ipv6_legacy_addr_t *my_ipv6_legacy_address);

/**
 * @brief Sends a DIO-message to a given destination
 *
 * This function sends a DIO message to a given destination. This is triggered by the trickle-timer.
 *
 * @param[in] destination           IPv6-address of the destination of the DIO. Should be a direct neighbor or multicast address.
 *
 */
void rpl_send_DIO_mode(ipv6_legacy_addr_t *destination);

/**
 * @brief Returns whether a node is root or not
 *
 * This function initializes all basic RPL mode resources. For this mode this includes only acquiring the own
 * address.
 *
 * @return 1 if node is root
 * @return 0 if node is not root
 *
 */
uint8_t rpl_is_root_mode(void);

/**
 * @brief Sends a DAO-message to a given destination
 *
 * This function sends a DAO message to a given destination.
 *
 * @param[in] destination           IPv6-address of the destination of the DAO. Should be the proffered parent.
 * @param[in] lifetime              Lifetime of the node. Reflect the estimated time of presence in the network.
 * @param[in] default_lifetime      If true, param lifetime is ignored and lifetime is DODAG default-lifetime
 * @param[in] start_index           Describes whether a DAO must be split because of too many routing entries.
 *
 */
void rpl_send_DAO_mode(ipv6_legacy_addr_t *destination, uint8_t lifetime, bool default_lifetime, uint8_t start_index);

/**
 * @brief Sends a DIS-message to a given destination
 *
 * This function sends a DIS message to a given destination.
 *
 * @param[in] destination           IPv6-address of the destination of the DIS. Should be a direct neighbor.
 *
 */
void rpl_send_DIS_mode(ipv6_legacy_addr_t *destination);

/**
 * @brief Sends a DAO acknowledgment-message to a given destination
 *
 * This function sends a DAO_ACK message to a given destination.
 *
 * @param[in] destination           IPv6-address of the destination of the DAO_ACK. Should be a direct neighbor.
 *
 */
void rpl_send_DAO_ACK_mode(ipv6_legacy_addr_t *destination);

/**
 * @brief Receives a DIO message
 *
 * This function handles receiving a DIO message in any mode .
 *
 */
void rpl_recv_DIO_mode(void);

/**
 * @brief Receives a DAO message
 *
 * This function handles receiving a DAO message in any mode.
 *
 */
void rpl_recv_DAO_mode(void);

/**
 * @brief Receives a DIS message
 *
 * This function handles receiving a DIS message in any mode.
 *
 */
void rpl_recv_DIS_mode(void);

/**
 * @brief Receives a DAO acknowledgment message
 *
 * This function handles receiving a DAO_ACK message in any mode.
 *
 */
void rpl_recv_dao_ack_mode(void);

/**
 * @brief Sends a RPL message to a given destination
 *
 * This function sends any RPl related messages to a given destination. This implementation should be equal
 * for all modes and therefore should not be altered. Every mode related RPL-sending function calls this for
 * relaying it in lower layers to sixlowpan_legacy. Because send-functions are wrapped by a mutex in rpl.c, the same
 * mutex applies here.
 *
 * @param[in] destination           IPv6-address of the destination of the message.
 * @param[in] payload               Payload of the message.
 * @param[in] len                   Length of the message
 * @param[in] next_header           Index to next header in message.
 *
 */
void rpl_send(ipv6_legacy_addr_t *destination, uint8_t *payload, uint16_t p_len, uint8_t next_header);

#ifdef __cplusplus
}
#endif

#endif /* __RPL_SM_H */
/** @} */
