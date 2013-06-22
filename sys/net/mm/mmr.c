/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

/**
 * @file
 * @internal
 * @brief		Micro Mesh Routing
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note		$Id: mmr.c 3854 2011-12-06 15:27:01Z hwill $
 */

#include "configure.h"
#include "mmr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "net.h"

#include "clock.h"
#include "utimer.h"
#include "kernel.h"
#include "thread.h"
#include "msg.h"

#define LEVEL_INFO	2				///< All messages are printed
#define LEVEL_WARN	1				///< Only warnings and error messages are printed
#define LEVEL_ERROR	0				///< Only error messages are printed
#define MMR_INFO_LEVEL LEVEL_WARN	///< Current information level

#define DEBUG(...)
//#define DEBUG(...) printf(__VA_ARGS__)

#define CONSTANT_SECOND					 (1)

#define RREQ_ID_SEQUENCE_NUMBER_START	 (1)
#define RREQ_THRESHOLD					 (3)
#define RREQ_NONE						 (0xFF)	/* Send no RREQs for these messages, value */
/* must be greater than RREQ_THRESHOLD */

#define TTL_START						 (1)
#define TTL_THRESHOLD					(10)

#define RREQ_TIMEOUT_BASE		(2*CONSTANT_SECOND)
#define RREQ_TIMEOUT_PER_TTL	(1*CONSTANT_SECOND)

/*---------------------------------------------------------------------------*/
//					Message queue data structures
/*---------------------------------------------------------------------------*/

#define MESSAGE_QUEUE_SIZE	(20)

typedef struct {
    net_message_t message;
    volatile uint32_t timestamp;
    uint8_t retry_count;
} message_queue_entry_t;

static message_queue_entry_t message_queue[MESSAGE_QUEUE_SIZE];

/*---------------------------------------------------------------------------*/
//					RREQ-Timeout data structures
/*---------------------------------------------------------------------------*/

static struct utimer ut;
static volatile bool rreq_to_active = false;		/* RREQ timeout active bit */
static const char *rreq_timeout_process_name  = "mmrd";
static uint16_t rreq_timeout_process_pid;
static void rreq_timeout_process(void);
static void post_next_rreq_timeout(void);

/*---------------------------------------------------------------------------*/
//					Statistic data structures
/*---------------------------------------------------------------------------*/

typedef struct mmr_stat {
    uint32_t rreq_originated;
    uint32_t rrep_originated;
    uint32_t rerr_originated;
    uint32_t rreq_received;
    uint32_t rrep_received;
    uint32_t rerr_received;
    uint32_t messages_no_route_found;				/* RREQ found no route */
    uint32_t messages_no_route_avail_on_forward;	/* Forwarding: no route in route table */
    uint32_t messages_broken_link_on_forward;		/* Forwarding: broken link detected */
    uint32_t rreq_duplicated;
} mmr_stat_t;

static mmr_stat_t mmr_stats;

/*---------------------------------------------------------------------------*/

/**
 * Returns time of RTC in seconds.
 *
 * @return Time of RTC in seconds
 */
static uint32_t rtc_now(void)
{
    return (uint32_t)(clock_get_systemtime() / 1000);
}

/*---------------------------------------------------------------------------*/
//					Routing table management
/*---------------------------------------------------------------------------*/

/**
 * @brief	Extract route information and store them in route table.
 *
 * @param 	local_addr Local network address of this node
 * @param 	length Length of address list
 * @param 	list Address list with route information
 */
static void rt_extract_routes(uint16_t local_addr, uint8_t length, uint16_t *list)
{
    DEBUG("call [%u]: rt_extract_routes\n", fk_thread->pid);
    uint16_t net_id = NETWORK_ADDR_BC(list[0]); 		/* BC address of source of RREQ */
    route_table_entry_t *rte = rt_lookup_route(net_id);	/* Should exist (preconfigured) */

    if(rte == NULL) {
        DEBUG("exit [%u]: rt_extract_routes\n", fk_thread->pid);
        return;											/* else exit here */
    }

    int i = 0;

    while(i < length && list[i] != local_addr) {
        i++;
    }

    if(i == length) {
        DEBUG("exit [%u]: rt_extract_routes\n", fk_thread->pid);
        return;
    }

    int pos = i;
    int leftNeighbour = -1;
    int rightNeighbour = -1;

    if(pos > 0) {
        leftNeighbour = list[pos - 1];
    }

    if(pos + 1 != length) {
        rightNeighbour = list[pos + 1];
    }

    i = 0;

    while(i < length) {
        uint16_t next = list[i];

        if(local_addr != next) {
            int distance = pos - i;
            int router = leftNeighbour;

            if(distance < 0) {
                router = rightNeighbour;
                distance *= -1;
            }

            rt_add_route(next, (uint16_t)router, (uint8_t)distance, rte->interface_id);
        }

        i++;
    }

    DEBUG("exit [%u]: rt_extract_routes\n", fk_thread->pid);
}

/*---------------------------------------------------------------------------*/
//					Message queue management
/*---------------------------------------------------------------------------*/

/**
 * @brief	Add a message to the message queue.
 *
 * @param 	msg The packet to add to the queue
 *
 * @return	A pointer to a message queue entry or NULL if
 *          message queue is full.
 */
static message_queue_entry_t *mq_add(net_message_t *msg)
{
    DEBUG("call [%u]: mq_add\n", fk_thread->pid);

    /* Holds eventually first active RREQ to same destination */
    message_queue_entry_t *pFirstFoundDup = NULL;

    /* Find the first active RREQ to this destination */
    int i;

    for(i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if(message_queue[i].timestamp != 0 &&
           message_queue[i].message.destination == msg->destination &&
           message_queue[i].retry_count != RREQ_NONE) {
            DEBUG("%s FOUND Duplicated Request to %u.%u in route req queue\n",
                  __FUNCTION__, (0xFF00 & msg->destination) >> 8, (0xFF & msg->destination));

            /* Save the first found entry to modify later if insertion was successful */
            pFirstFoundDup = &message_queue[i];
            break;
        }
    }

    /* If RREQ for same destination found then reset values
     * even if the new message will get dropped later on because of
     * limited queue space. Route to this destination gets queried
     * again for sure so make new RREQ as soon as possible... */
    if(pFirstFoundDup != NULL) {
        pFirstFoundDup->retry_count = 0;
        pFirstFoundDup->timestamp = 1;
        mmr_stats.rreq_duplicated++;
    }

    /* Find free position to insert new message */
    for(i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if(message_queue[i].timestamp == 0) {
            /* Free position found, add entry */
            message_queue[i].message = *msg;

            if(pFirstFoundDup != NULL) {
                /* There is already a RREQ for this destination, so don't
                 * generate a new one */
                message_queue[i].retry_count = RREQ_NONE;
            }
            else {
                /* Set initial RREQ retry counter to zero */
                message_queue[i].retry_count = 0;
            }

            message_queue[i].timestamp = 1;
            DEBUG("exit [%u]: mq_add\n", fk_thread->pid);
            return &message_queue[i];
        }
    }

    DEBUG("exit [%u]: mq_add\n", fk_thread->pid);
    return NULL;
}

/**
 * @brief	Count messages for given destination.
 *
 * @param 	dst Destination address
 *
 * @return	The number of messages for the destination.
 */
static int mq_msgs_for_destination(uint16_t dst)
{
    DEBUG("call [%u]: mq_msgs_for_destination\n", fk_thread->pid);
    int i, dst_count = 0;

    for(i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if(message_queue[i].timestamp != 0 &&
           message_queue[i].message.destination == dst) {
            dst_count++;
        }
    }

    DEBUG("exit [%u]: mq_msgs_for_destination\n", fk_thread->pid);
    return dst_count;
}

/**
 * @brief	Remove all messages for given destination out of message queue.
 *
 * @param 	dst Destination address
 */
static void mq_remove_msgs_for_destination(uint16_t dst)
{
    DEBUG("call [%u]: mq_remove_msgs_for_destination\n", fk_thread->pid);
    int i;

    for(i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if(message_queue[i].timestamp != 0 &&
           message_queue[i].message.destination == dst) {
            message_queue[i].timestamp = 0;
        }
    }

    DEBUG("exit [%u]: mq_remove_msgs_for_destination\n", fk_thread->pid);
}

/**
 * @brief	Send all queued messages for given destination.
 *
 * @param 	dst Destination address
 */
static void mq_dequeue_and_send(uint16_t dst)
{
    int i;
    DEBUG("call [%u]: mq_dequeue_and_send\n", fk_thread->pid);

    /* Stop any pending RREQ-Timeout, it's possibly handled now */
    rreq_to_active = false;
    utimer_remove(&ut);

    /* Prioritize packets for given destination, route entry should exist */
    route_table_entry_t *rte = rt_lookup_route(dst);

    if(rte != NULL) {
        for(i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
            if(message_queue[i].timestamp != 0 &&
                    message_queue[i].message.destination == dst) {
                bool res = net_enqueue_for_transmission(&message_queue[i].message,
                                                        rte->interface_id,
                                                        rte->gateway, true);

                if(res) {
                    message_queue[i].timestamp = 0;
                }
            }
        }
    }

    /* Now all other packets */
    for(i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if(message_queue[i].timestamp != 0 &&
           message_queue[i].message.destination != dst) {
            route_table_entry_t *rte = rt_lookup_route(message_queue[i].message.destination);

            if(rte != NULL) {
                bool res = net_enqueue_for_transmission(&message_queue[i].message,
                                                        rte->interface_id,
                                                        rte->gateway, true);

                if(res) {
                    message_queue[i].timestamp = 0;
                }
            }
        }
    }

    /* This function was triggered either by RREP packet or RREQ-Timeout. */
    /* So update or set new RREQ-Timeout for other packets in queue. */
    post_next_rreq_timeout();
    DEBUG("exit [%u]: mq_dequeue_and_send\n", fk_thread->pid);
}

/*---------------------------------------------------------------------------*/
//					Initialization of MMR layer
/*---------------------------------------------------------------------------*/

void mmr_init(void)
{
    rt_init();
    memset(message_queue, 0, sizeof(message_queue_entry_t) * MESSAGE_QUEUE_SIZE);
    rreq_timeout_process_pid = thread_create(2500, PRIORITY_MMREQ, CREATE_STACKTEST,
                               rreq_timeout_process, rreq_timeout_process_name);
}

/*---------------------------------------------------------------------------*/
//						Send & receive functions
/*---------------------------------------------------------------------------*/

/**
 * @brief	Tests if the net message contains a RERR.
 *
 * @param 	msg The net message to test
 *
 * @return	true if the net message contains a RERR; false otherwise.
 */
static bool is_route_error(net_message_t *msg)
{
    if(msg->protocol == LAYER_2_PROTOCOL_MMR) {
        /* First byte in {RREQ, RREP, RERR} is always type */
        if(msg->payload[0] == MMR_TYPE_RERR) {
            return true;
        }
    }

    return false;
}

/**
 * @brief	Generates a route reply message.
 *
 * @param 	rreq_msg Corresponding route request message
 */
static void generate_route_reply_message(mmr_rreq_message_t *rreq_msg)
{
    DEBUG("call [%u]: generate_route_reply_message\n", fk_thread->pid);
    /* Create RREP message */
    mmr_rrep_message_t rrep_msg;
    rrep_msg.type = MMR_TYPE_RREP;
    rrep_msg.length = rreq_msg->length;
    rrep_msg.destination = rreq_msg->source;
    rrep_msg.source = rreq_msg->destination;
    memcpy(rrep_msg.address, rreq_msg->address, rreq_msg->length * sizeof(uint16_t));

    /* Create MMR message containing the RREP message */
    net_message_t net_msg;
    net_msg.protocol = LAYER_2_PROTOCOL_MMR;
    net_msg.flags_tos = PRIORITY_ALARM;
    net_msg.seq_clr_id = 0;
    net_msg.ttl = TTL_THRESHOLD;
    net_msg.source = rrep_msg.source;
    net_msg.destination = rrep_msg.destination;
    memcpy(net_msg.payload, (void *)&rrep_msg, sizeof(mmr_rrep_message_t));

    /* Source address must exist in route table to find correct */
    /* interface id and next hop (should be created by RREQ) */
    route_table_entry_t *rte = rt_lookup_route(net_msg.destination);

    if(rte != NULL) {
        /* Send message to next hop */
        mmr_stats.rrep_originated++;
        net_enqueue_for_transmission(&net_msg, rte->interface_id, rte->gateway, true);
    }

    DEBUG("exit [%u]: generate_route_reply_message\n", fk_thread->pid);
}

/**
 * @brief	Generates a route error message.
 *
 * @param 	dst Destination address of RERR packet
 * @param 	gateway Next hop network address of RERR packet
 * @param 	intf Interface id of RERR packet
 * @param 	type Error type of RERR packet
 * @param 	type_data Type specific data of RERR packet
 */
static void generate_route_error_message(uint16_t dst, uint16_t gateway, int intf, uint8_t type, uint16_t type_data)
{
    DEBUG("call [%u]: generate_route_error_message\n", fk_thread->pid);
    /* Define RERR message */
    mmr_rerr_message_t rerr_msg;
    rerr_msg.type = MMR_TYPE_RERR;
    rerr_msg.error_type = type;
    rerr_msg.type_specific_info = type_data;

    /* Wrap RERR message in net message */
    net_message_t net_msg;
    net_msg.protocol = LAYER_2_PROTOCOL_MMR;
    net_msg.flags_tos = PRIORITY_DATA;
    net_msg.seq_clr_id = 0;
    net_msg.ttl = TTL_THRESHOLD;
    net_msg.source = net_get_address_in_subnet(dst);
    net_msg.destination = dst;
    memcpy(net_msg.payload, (void *)&rerr_msg, sizeof(mmr_rerr_message_t));

    /* Send message to next hop */
    mmr_stats.rerr_originated++;
    net_enqueue_for_transmission(&net_msg, intf, gateway, true);
    DEBUG("exit [%u]: generate_route_error_message\n", fk_thread->pid);
}

/**
 * @brief	Receive a route request message.
 *
 * @param 	msg The route request packet
 * @param 	packet_info Additional packet information
 */
static void receive_route_request_message(mmr_rreq_message_t *msg,
                                          packet_info_t *packet_info)
{
    DEBUG("call [%u]: receive_route_request_message\n", fk_thread->pid);
    uint16_t my_addr = net_get_address_in_subnet(msg->source);
#if (MMR_INFO_LEVEL >= LEVEL_WARN)

    if(my_addr == 0) {
        puts("MMR [WARN]: received RREQ with unknown network part of source address");
        puts("MMR [WARN]: => can't find own net address in sub net!");
    }

#endif

    /* If address list of RREQ message has enough space */
    if(msg->length < ADDRESS_LIST_SIZE) {
        /* append our node id to list */
        msg->address[msg->length++] = my_addr;
        /* add routes with overhearing */
        rt_extract_routes(my_addr, msg->length, msg->address);
    }
    /* Distance between sender and receiver is too long, discard packet */
    else {
        /* Drop RREQ packet => set TTL to zero */
        *packet_info->ttl_ptr = 0;
        DEBUG("exit [%u]: receive_route_request_message\n", fk_thread->pid);
        return;
    }

    /* If RREQ message was send to us, then send RREP message */
    if(msg->destination == my_addr) {
        /* Don't forward RREQ packet any further => set TTL to zero */
        *packet_info->ttl_ptr = 0;
        generate_route_reply_message(msg);
    }

    DEBUG("exit [%u]: receive_route_request_message\n", fk_thread->pid);
}

/**
 * @brief	Receive a route reply message.
 *
 * @param 	msg The route reply packet
 * @param 	packet_info Additional packet information
 */
static void receive_route_reply_message(mmr_rrep_message_t *msg,
                                        packet_info_t *packet_info)
{
    DEBUG("call [%u]: receive_route_reply_message\n", fk_thread->pid);
    /* RREP received: Send out queued packets for which routes are now known */
    mq_dequeue_and_send(msg->source);
    DEBUG("exit [%u]: receive_route_reply_message\n", fk_thread->pid);
}

/**
 * @brief	Receive a route error message.
 *
 * @param 	msg The route error packet
 * @param 	packet_info Additional packet information
 */
static void receive_route_error_message(mmr_rerr_message_t *msg,
                                        packet_info_t *packet_info)
{
    DEBUG("call [%u]: receive_route_error_message\n", fk_thread->pid);

    switch(msg->error_type) {
        case RERR_NODE_UNREACHABLE:
            rt_remove_route(msg->type_specific_info);
            break;

        default:
#if (MMR_INFO_LEVEL >= LEVEL_INFO)
            puts("MMR [INFO]: RERR error type is unknown");
#endif
            break;
    }

    DEBUG("exit [%u]: receive_route_error_message\n", fk_thread->pid);
}

/**
 * @brief	Computes the RREQ timeout period, given a
 *          TTL and destination address value.
 *
 * @param 	ttl Time to live
 * @param 	dst Network destination address
 *
 * @return	RREQ timeout period in seconds
 */
static int compute_rreq_timeout(int ttl, uint16_t dst)
{
    int t_hop = net_get_interface_transmission_duration(dst);

    if(t_hop == -1) {
        t_hop = RREQ_TIMEOUT_PER_TTL * ttl;
    }
    else {
        t_hop = (t_hop * ttl + 999) / 1000;
    }

    return RREQ_TIMEOUT_BASE + 2 * t_hop;
}

/**
 * @brief	Broadcast a RREQ message.
 *
 * A single route request can repeatedly broadcast RREQ messages,
 * with increasing TTL value, until a route has been found.
 *
 * @param 	mq_entry Pointer to a message queue entry (the packet
 *                   for which to find the route)
 */
static void rreq_broadcast(message_queue_entry_t *mq_entry)
{
    DEBUG("call [%u]: rreq_broadcast\n", fk_thread->pid);

    if(mq_entry->retry_count == RREQ_NONE) {
        DEBUG("call [%u]: rreq duplicated do not send\n", fk_thread->pid);
        return;
    }

    /* Create RREQ message */
    mmr_rreq_message_t rreq_msg;
    rreq_msg.type = MMR_TYPE_RREQ;
    rreq_msg.length = 1;
    rreq_msg.destination = mq_entry->message.destination;
    rreq_msg.source = mq_entry->message.source;
    rreq_msg.address[0] = mq_entry->message.source;

    /* Wrap RREQ message in net message */
    net_message_t net_msg;
    net_msg.protocol = LAYER_2_PROTOCOL_MMR;
    net_msg.flags_tos = PRIORITY_DATA;
    net_msg.seq_clr_id = 0;
    net_msg.ttl = mq_entry->retry_count == 0 ? TTL_START : TTL_THRESHOLD;
    net_msg.source = rreq_msg.source;
    net_msg.destination = NETWORK_ADDR_BC(rreq_msg.destination);
    memcpy(net_msg.payload, (void *)&rreq_msg, sizeof(mmr_rreq_message_t));

    /* Broadcast the net message */
    mq_entry->retry_count++;
    mq_entry->timestamp = rtc_now();
    /* Find the broadcast route table entry */
    route_table_entry_t *rte = rt_lookup_route(net_msg.destination);

    if(rte != NULL) {
        /* Next hop address is broadcast address of lower layer */
        net_enqueue_for_transmission(&net_msg, rte->interface_id,
                                     rte->gateway, true);
    }

    DEBUG("exit [%u]: rreq_broadcast\n", fk_thread->pid);
}

/**
 * @brief	Find next RREQ to time out. Post event immediately or
 *          with utimer.
 */
static void post_next_rreq_timeout(void)
{
    DEBUG("call [%u]: post_next_rreq_timeout\n", fk_thread->pid);
    int i, j = -1;
    uint32_t now, next = 0xffffffff;

    for(i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if((message_queue[i].timestamp != 0) && (message_queue[i].retry_count !=
                                              RREQ_NONE)) {
            int ttl = message_queue[i].retry_count == 1 ? TTL_START : TTL_THRESHOLD;
            int to = compute_rreq_timeout(ttl,
                                          message_queue[i].message.destination);

            if(message_queue[i].timestamp + to < next) {
                next = message_queue[i].timestamp + to;
                j = i;
            }
        }
    }

    if(j == -1) {
        DEBUG("exit [%u]: post_next_rreq_timeout\n", fk_thread->pid);
        return;
    }

    /* Stop any utimer */
    rreq_to_active = false;
    utimer_remove(&ut);
    /* If current time greater than RREQ timeout value */
    now = rtc_now();

    if(now >= next) {
        /* Schedule RREQ-Timeout immediately */
        msg m;
        m.type = MSG_TIMER;
        m.content.ptr = (char *)&message_queue[j];
        rreq_to_active = true;

        if(msg_send(&m, rreq_timeout_process_pid, false) != 1) {
            /* Message could not be send (receiver not waiting), schedule
             * timer with minimum delay */
#if (MMR_INFO_LEVEL >= LEVEL_WARN)
            puts("MMR [WARN]: Immediate schedule of RREQ-Timeout failed, process not waiting!");
#endif
            utimer_set_wpid(&ut, 1, rreq_timeout_process_pid, &message_queue[j]);
        }
    }
    else {
        /* Set new utimer with time difference */
        rreq_to_active = true;
        utimer_set_wpid(&ut, next - now, rreq_timeout_process_pid,
                        &message_queue[j]);
    }

    DEBUG("exit [%u]: post_next_rreq_timeout\n", fk_thread->pid);
}

/**
 * This event is called periodically after a route request is originated,
 * until a route has been found.
 *
 * Each time it is called, it rebroadcasts the route request message with a
 * new rreq id and incremented TTL.
 */
static void rreq_timeout(message_queue_entry_t *mqe)
{
    DEBUG("call [%u]: rreq_timeout\n", fk_thread->pid);

    /* Test if valid entry passed */
    if(mqe->timestamp == 0) {
#if (MMR_INFO_LEVEL >= LEVEL_WARN)
        puts("MMR [WARN]: invalid message queue entry for RREQ-Timeout");
#endif
        goto post_next_to;
    }

    /* See if route to destination was found */
    route_table_entry_t *rte = rt_lookup_route(mqe->message.destination);

    /* If found and no messages in queue for destination: return (queued
     * packets are send on reception of RREP); If found but messages in queue:
     * trigger send immediately here! */
    if(rte != NULL) {
        int msg_count = mq_msgs_for_destination(mqe->message.destination);

        if(msg_count > 0) {
            mq_dequeue_and_send(mqe->message.destination);
            DEBUG("exit [%u]: rreq_timeout\n", fk_thread->pid);
            return;
        }
        else {
            /* Added just for security but this case should never occur */
#if (MMR_INFO_LEVEL >= LEVEL_WARN)
            puts("MMR [WARN]: RREQ-Timeout occurred, route is available but no messages for destination");
#endif
            /* Anyway: jump to update next RREQ-Timeout */
            goto post_next_to;
        }
    }

    /* Otherwise send new RREQ if below threshold (means also retry count !=
     * RREQ_NONE) */
    if(mqe->retry_count < RREQ_THRESHOLD) {
        /* Broadcast new RREQ message (with incremented TTL) */
        rreq_broadcast(mqe);
    }
    else {
        /* Remove all messages for this destination */
        mmr_stats.messages_no_route_found++;
        mq_remove_msgs_for_destination(mqe->message.destination);
    }

    /* Anyway: update or set next RREQ-Timeout */
post_next_to:
    post_next_rreq_timeout();
    DEBUG("exit [%u]: rreq_timeout\n", fk_thread->pid);
}

static void rreq_timeout_process(void)
{
    msg m;

    do {
        msg_receive(&m);

        if(m.type == MSG_TIMER && rreq_to_active) {
            rreq_to_active = false;
            rreq_timeout((message_queue_entry_t *)m.content.ptr);
        }
    }
    while(m.type != MSG_EXIT);
}

void mmr_peek(net_message_t *message, packet_info_t *packet_info)
{
    DEBUG("call [%u]: mmr_peek\n", fk_thread->pid);

    /* Only look at micro mesh routing messages */
    if(message->protocol == LAYER_2_PROTOCOL_MMR) {
        uint8_t type = message->payload[0];
        uint16_t my_addr = net_get_address_in_subnet(message->source);

        if(type == MMR_TYPE_RREP) {
            /* Add routes to route table */
            mmr_rrep_message_t *rrep_msg = (mmr_rrep_message_t *)message->payload;
#if (MMR_INFO_LEVEL >= LEVEL_WARN)

            if(my_addr == 0) {
                puts("MMR [WARN]: received RREP with unknown network part of source address");
                puts("MMR [WARN]: => can't find own net address in sub net!");
            }

#endif
            rt_extract_routes(my_addr, rrep_msg->length, rrep_msg->address);
        }
        else if(type == MMR_TYPE_RERR) {
#if (MMR_INFO_LEVEL >= LEVEL_WARN)

            if(my_addr == 0) {
                puts("MMR [WARN]: received RERR with unknown network part of source address");
                puts("MMR [WARN]: => can't find own net address in sub net!");
            }

#endif

            /* If not destination of RERR, then remove route to unavailable
             * node in RERR packet */
            if(message->destination != my_addr) {
                mmr_rerr_message_t *rerr_msg =
                    (mmr_rerr_message_t *)message->payload;

                if(rerr_msg->error_type == RERR_NODE_UNREACHABLE) {
                    rt_remove_route(rerr_msg->type_specific_info);
                }
            }
        }
    }

    DEBUG("exit [%u]: mmr_peek\n", fk_thread->pid);
}

bool mmr_send(net_message_t *message)
{
    DEBUG("call [%u]: mmr_send\n", fk_thread->pid);
    bool enqueue = true;

    if(message->destination == net_get_address_in_subnet(message->destination)) {
#if (MMR_INFO_LEVEL >= LEVEL_WARN)
        puts("MMR [WARN]: message is already at destination, why is routing called?");
#endif
        DEBUG("exit [%u]: mmr_send\n", fk_thread->pid);
        return false;
    }

    if(NETWORK_ADDR_NET(message->destination) == 0) {
#if (MMR_INFO_LEVEL >= LEVEL_WARN)
        puts("MMR [WARN]: NET part of address cannot be 0!");
#endif
        DEBUG("exit [%u]: mmr_send\n", fk_thread->pid);
        return false;
    }

    if(NETWORK_ADDR_HOST(message->destination) == 0) {
#if (MMR_INFO_LEVEL >= LEVEL_INFO)
        puts("MMR [INFO]: broadcast destination, why is routing called? A route entry should exist!");
#endif
        enqueue = false;
    }

    /* Look up next hop address for this destination in routing table */
    route_table_entry_t *rte = rt_lookup_route(message->destination);

    /* If next hop address found in routing table, forward message */
    if(rte != NULL) {
        DEBUG("exit [%u]: mmr_send\n", fk_thread->pid);
        return net_enqueue_for_transmission(message, rte->interface_id, rte->gateway, true);
    }
    /* Otherwise, save message in queue; broadcast RREQ message */
    else {
        if(!enqueue) {
            /* Don't enqueue broadcast destinations */
            DEBUG("exit [%u]: mmr_send\n", fk_thread->pid);
            return false;
        }

        message_queue_entry_t *mqe = mq_add(message);

        if(mqe != NULL) {
            rreq_broadcast(mqe);
            post_next_rreq_timeout();
            mmr_stats.rreq_originated++;
            DEBUG("exit [%u]: mmr_send\n", fk_thread->pid);
            return true;
        }
    }

    DEBUG("exit [%u]: mmr_send\n", fk_thread->pid);
    return false;
}

void mmr_packet_dropped(net_message_t *message, uint16_t next_hop, int error)
{
    DEBUG("call [%u]: mmr_packet_dropped\n", fk_thread->pid);

    if(error == ROUTE_ERROR_BROKEN_ROUTE) {
        /* Local failure detected - remove all routes through broken link */
        rt_remove_gateway_routes(next_hop);
        mmr_stats.messages_broken_link_on_forward++;
    }
    else if(error == ROUTE_ERROR_MISSING_ROUTE) {
        mmr_stats.messages_no_route_avail_on_forward++;
    }

    /* If source != net_addr, send RERR to source of message */
    if(message->source != net_get_address_in_subnet(message->source)) {
        /* Do not generate RERR if it is already a RERR message */
        if(is_route_error(message)) {
            DEBUG("exit [%u]: mmr_packet_dropped\n", fk_thread->pid);
            return;
        }

        /* Find next hop to source */
        route_table_entry_t *rte = rt_lookup_route(message->source);

        if(rte != NULL) {
            generate_route_error_message(message->source, rte->gateway,
                                         rte->interface_id,
                                         RERR_NODE_UNREACHABLE,
                                         message->destination);
        }

#if (MMR_INFO_LEVEL >= LEVEL_WARN)
        else {
            printf("MMR [WARN]: cannot send RERR to source #%u, no route found!\n",
                    message->source);
        }

#endif
    }

    DEBUG("exit [%u]: mmr_packet_dropped\n", fk_thread->pid);
}

void mmr_receive(void *msg, int msg_size, packet_info_t *packet_info)
{
    DEBUG("call [%u]: mmr_receive\n", fk_thread->pid);
    uint8_t *p = (uint8_t *) msg;
    uint8_t type = p[0];

    if(type == MMR_TYPE_RREQ) {
        receive_route_request_message((mmr_rreq_message_t *)msg, packet_info);
        mmr_stats.rreq_received++;
    }
    else if(type == MMR_TYPE_RREP) {
        receive_route_reply_message((mmr_rrep_message_t *)msg, packet_info);
        mmr_stats.rrep_received++;
    }
    else if(type == MMR_TYPE_RERR) {
        receive_route_error_message((mmr_rerr_message_t *)msg, packet_info);
        mmr_stats.rerr_received++;
    }

#if (MMR_INFO_LEVEL >= LEVEL_INFO)
    else {
        printf("MMR [INFO]: can't handle message of type %u\n", type);
    }

#endif
    DEBUG("exit [%u]: mmr_receive\n", fk_thread->pid);
}

void mmr_print_stats(void)
{
    printf("ROUTING LAYER STATS\r\n");
    printf("-------------------\r\n");
    printf("Route requests originated: %lu\r\n", mmr_stats.rreq_originated);
    printf("Route requests duplicated: %lu\r\n", mmr_stats.rreq_duplicated);
    printf("Route replies originated:  %lu\r\n", mmr_stats.rrep_originated);
    printf("Route errors originated:   %lu\r\n", mmr_stats.rerr_originated);
    printf("Route requests received:   %lu\r\n", mmr_stats.rreq_received);
    printf("Route replies received:    %lu\r\n", mmr_stats.rrep_received);
    printf("Route errors received:     %lu\r\n", mmr_stats.rerr_received);
    printf("\r\n");
    printf("#Messages with no route found:                %lu\r\n", mmr_stats.messages_no_route_found);
    printf("#Messages with broken link on forward:        %lu\r\n", mmr_stats.messages_broken_link_on_forward);
    printf("#Messages with no route available on forward: %lu\r\n", mmr_stats.messages_no_route_avail_on_forward);
    printf("\r\n");
}
