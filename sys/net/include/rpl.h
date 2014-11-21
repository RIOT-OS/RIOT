/*
 * Copyright (C) 2013, 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rpl RPL
 * @ingroup     net
 * @brief       Routing Protocol for Low power and Lossy Networks
 *
 * Header which includes all core RPL-functions. Normally it shouldn't be necessary to
 * modify this file.
 *
 * @{
 *
 * @file        rpl.h
 * @brief       RPL header. Declaration of global variables and functions needed for
 * core functionality of RPL.
 *
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 */

#ifndef __RPL_H
#define __RPL_H

#include <string.h>
#include <stdint.h>
#include <vtimer.h>
#include <mutex.h>
#include <transceiver.h>
#include "ipv6.h"
#include "rpl/rpl_dodag.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef CC1100_RADIO_MODE
#define CC1100_RADIO_MODE CC1100_MODE_WOR

#define RPL_PKT_RECV_BUF_SIZE 16
#define RPL_PROCESS_STACKSIZE (1024+512)

/* global variables */
extern rpl_of_t *rpl_objective_functions[NUMBER_IMPLEMENTED_OFS];
extern rpl_routing_entry_t rpl_routing_table[RPL_MAX_ROUTING_ENTRIES];
extern kernel_pid_t rpl_process_pid;

/* needed for receiving messages with ICMP-code 155. Received via IPC from ipv6.c */
extern mutex_t rpl_recv_mutex;

/* needed for sending RPL-messages */
extern mutex_t rpl_send_mutex;

extern msg_t rpl_msg_queue[RPL_PKT_RECV_BUF_SIZE];
extern char rpl_process_buf[RPL_PROCESS_STACKSIZE];
extern uint8_t rpl_buffer[BUFFER_SIZE - LL_HDR_LEN];

/**
 * @brief Initialization of RPL.
 *
 * This function initializes all basic RPL resources such as mutex for send/receive,
 * corresponding objective functions and sixlowpan (including own address). Calls
 * initialization for mode as specified by PL_DEFAULT_MOP in rpl_structs.h.
 *
 * @param[in] if_id             ID of the interface, which correspond to the network under RPL-control
 *
 * @return 1 if initialization was successful
 * @return 0 if initialization was not successful
 *
 */
uint8_t rpl_init(int if_id);

/**
 * @brief Get entry point for default objective function.
 *
 * This function is obsolete in rpl.h and will be moved shortly.
 *
 * @param[in] ocp               Objective code point for desired objective function
 *
 * @return Implementation of objective function
 *
 * */
rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp);

/**
 * @brief Initialization of RPL-root.
 *
 * This function initializes all RPL resources to act as a root node.
 * Because the root has different features in different modes, the core
 * initialization just calls the root-initialization of the chosen mode
 *
 */
void rpl_init_root(void);

/**
 * @brief Sends a DIO-message to a given destination
 *
 * This function sends a DIO message to a given destination. Because nodes can act
 * differently in different modes, this function just sets the mutex and call the DIO
 * sending function of the chosen mode.
 *
 * @param[in] destination       IPv6-address of the destination of the DIO. Should be a direct neighbor.
 *
 */
void send_DIO(ipv6_addr_t *destination);

/**
 * @brief Sends a DAO-message to a given destination
 *
 * This function sends a DAO message to a given destination. Because nodes can act
 * differently in different modes, this function just sets the mutex and call the DAO
 * sending function of the chosen mode.
 *
 * @param[in] destination       IPv6-address of the destination of the DAO. Should be the preferred parent.
 * @param[in] lifetime          Lifetime of the node. Reflect the estimated time of presence in the network.
 * @param[in] default_lifetime  If true, param lifetime is ignored and lifetime is dodag default-lifetime
 * @param[in] start_index       Describes whether a DAO must be split because of too many routing entries.
 *
 */
void send_DAO(ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime, uint8_t start_index);

/**
 * @brief Sends a DIS-message to a given destination
 *
 * This function sends a DIS message to a given destination or multicast-address. Because nodes can act
 * differently in different modes, this function just sets the mutex and call the DIS
 * sending function of the chosen mode.
 *
 * @param[in] destination       IPv6-address of the destination of the DIS. Should be a direct neighbor or multicast-address.
 *
 */
void send_DIS(ipv6_addr_t *destination);

/**
 * @brief Sends a DAO acknowledgment-message to a given destination
 *
 * This function sends a DAO_ACK message to a given destination. Because nodes can act
 * differently in different modes, this function just sets the mutex and call the DAO_ACK
 * sending function of the chosen mode.
 *
 * @param[in] destination       IPv6-address of the destination of the DAO_ACK. Should be a direct neighbor.
 *
 */
void send_DAO_ACK(ipv6_addr_t *destination);

/**
 * @brief Receives a DIO message
 *
 * This function handles receiving a DIO message. Because nodes can act differently in different modes,
 * this function just calls the receiving function of the chosen mode.
 *
 */
void recv_rpl_DIO(void);

/**
 * @brief Receives a DAO message
 *
 * This function handles receiving a DAO message. Because nodes can act differently in different modes,
 * this function just calls the receiving function of the chosen mode.
 *
 */
void recv_rpl_DAO(void);

/**
 * @brief Receives a DIS message
 *
 * This function handles receiving a DIS message. Because nodes can act differently in different modes,
 * this function just calls the receiving function of the chosen mode.
 *
 */
void recv_rpl_DIS(void);

/**
 * @brief Receives a DAO acknowledgment message
 *
 * This function handles receiving a DAO_ACK message. Because nodes can act differently in different modes,
 * this function just calls the receiving function of the chosen mode.
 *
 */
void recv_rpl_DAO_ACK(void);

/**
 * @brief Initialization of RPl-root.
 *
 * This function initializes all RPL resources especially for root purposes.
 * corresponding objective functions and sixlowpan (including own address). It also register mutexes for
 * sending and receiving RPL-based messages. Both are necessary because of parallel access from different
 * layers/modules of RIOT. May change with future structure changes.
 *
 * @param arg ignored
 * @returns nothing
 */
void *rpl_process(void *arg);

/**
 * @brief Returns next hop from routing table.
 *
 * @deprecated This function is obsolete and will be removed shortly. This will be replaced with a
 * common routing information base.
 *
 * @param[in] addr                  Destination address
 *
 * @return Next hop address
 *
 * */
ipv6_addr_t *rpl_get_next_hop(ipv6_addr_t *addr);

/**
 * @brief Adds routing entry to routing table
 *
 * @deprecated This function is obsolete and will be removed shortly. This will be replaced with a
 * common routing information base.
 *
 * @param[in] addr                  Destination address
 * @param[in] next_hop              Next hop address
 * @param[in] lifetime              Lifetime of the entry
 *
 * */
void rpl_add_routing_entry(ipv6_addr_t *addr, ipv6_addr_t *next_hop, uint16_t lifetime);

/**
 * @brief Deletes routing entry to routing table
 *
 * @deprecated This function is obsolete and will be removed shortly. This will be replaced with a
 * common routing information base.
 *
 * @param[in] addr                  Destination address
 *
 * */
void rpl_del_routing_entry(ipv6_addr_t *addr);

/**
 * @brief Finds routing entry for a given destination.
 *
 * @deprecated This function is obsolete and will be removed shortly. This will be replaced with a
 * common routing information base.
 *
 * @param[in] addr                  Destination address
 *
 * @return Routing entry address
 *
 * */
rpl_routing_entry_t *rpl_find_routing_entry(ipv6_addr_t *addr);

/**
 * @brief Clears routing table.
 *
 * @deprecated This function is obsolete and will be removed shortly. This will be replaced with a
 * common routing information base.
 *
 * */
void rpl_clear_routing_table(void);

/**
 * @brief Returns routing table
 *
 * @deprecated This function is obsolete and will be removed shortly. This will be replaced with a
 * common routing information base.
 *
 * @return Routing table
 *
 * */
rpl_routing_entry_t *rpl_get_routing_table(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __RPL_H */
