/*
 * micromesh_routing.c - Exchanges the keys with other nodes in the network
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        micromesh_routing.c
 * @brief       Micro Mesh Routing
 *
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        09.09.2013 13:11:55
 */

/*---------------------------------------------------------------------------*/
//                                      Included Files
/*---------------------------------------------------------------------------*/

/*  Radio includes */
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"
#include "cc1100_phy.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kernel.h"
#include "thread.h"
#include "msg.h"
#include "hwtimer.h"
#include "vtimer.h"

/* Project Includes */
#include "secure_routing_globals.h"
#include "network_nodes.h"
#include "microeval.h"
#include "packet_security.h"
#include "packet_mac.h"
#include "replay_protection.h"
#include "micromesh_routing.h"
#include "secure_routing.h"


/*---------------------------------------------------------------------------*/
//                                      Variables
/*---------------------------------------------------------------------------*/
char micromesh_thread_stack_buffer[4500];
// Queue for packets
static packet_queue_entry_t packet_queue[MESSAGE_QUEUE_SIZE];
// Timer handle
static struct vtimer_t vt;
// RREQ timeout active bit
static volatile bool rreq_to_active = false;
// Name of the time-out thread
static const char *rreq_timeout_process_name = "mmrd";
// The PID of the timeout thread
static int rreq_timeout_process_pid = -1;
// Stores all the micro-mesh statistics
static mmr_stat_t mmr_stats;
// Says if a  timer is to be stopped
static int stop_pending_timer = 0;
// Queue of messages intended for timer
static msg_t timer_rreq_msg_queue[16];

/*---------------------------------------------------------------------------*/
//                                      Functions
/*---------------------------------------------------------------------------*/
static void rreq_timeout_process(void);
static void post_next_rreq_timeout(void);


/**
 * Returns time of RTC in seconds.
 *
 * @return Time of RTC in seconds
 */
static uint32_t rtc_now(void)
{
    return HWTIMER_TICKS_TO_US(hwtimer_now()) / 1000000;
}

/*---------------------------------------------------------------------------*/
//                        Routing table management
/*---------------------------------------------------------------------------*/

/**
 * @brief       Extract route information and store them in route table.
 *
 * @param       local_addr Local network address of this node
 * @param       length Length of address list
 * @param       list Address list with route information
 */
static void rt_extract_routes(uint16_t local_addr, uint8_t length,
                                      uint16_t *list)
{
    ulog_info("Extracting routes...");

    int i = 0;

    while (i < length && list[i] != local_addr) {
        i++;
    }

    if (i == length) {
        ulog_error("Local address not found in routes list");
        return;
    }

    int pos = i;
    int leftNeighbour = -1;
    int rightNeighbour = -1;

    if (pos > 0) {
        leftNeighbour = list[pos - 1];
    }

    if (pos + 1 < length) {
        rightNeighbour = list[pos + 1];
    }

    // 'pos' is the position of the local node
    // following chunk of code will find routes to
    // the left of pos, then to the right of pos
    i = 0;

    while (i < length) {
        uint16_t next = list[i];

        ulog("> Item %d = addr:%lu", i, next);

        if (local_addr != next) {
            int distance = pos - i;
            int router = leftNeighbour;

            if (distance < 0) {
                router = rightNeighbour;
                distance *= -1;
            }
            network_nodes_add_route((uint8_t)next, (uint8_t)router,
                                        (uint8_t)distance, list, length);
        }
        i++;
    }
}

/*---------------------------------------------------------------------------*/
//                                      Message queue management
/*---------------------------------------------------------------------------*/

/**
 * @brief       Add a message to the message queue.
 *
 * @param       msg The packet to add to the queue
 *
 * @return      A pointer to a message queue entry or NULL if
 *              message queue is full.
 */
static packet_queue_entry_t *pq_add(secure_packet_t *packet)
{
    ulog_info("Called pq_add");

    // Holds eventually first active RREQ to same destination
    packet_queue_entry_t *pFirstFoundDup = NULL;

    // Find the first active RREQ to this destination
    int i;

    for (i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if (packet_queue[i].timestamp != 0 &&
            packet_queue[i].packet.destination == packet->destination &&
            packet_queue[i].retry_count != RREQ_NONE) {
            ulog("FOUND Duplicated Request to %u.%u in route req queue",
                 (0xFF00 & packet->destination) >> 8,
                 (0xFF & packet->destination));

            /*
             * Save the first found entry to modify later if insertion was
             * successful
             */
            pFirstFoundDup = &packet_queue[i];
            break;
        }
    }

    // If RREQ for same destination found then reset values
    // even if the new message will get dropped later on because of
    // limited queue space. Route to this destination gets queried
    // again for sure so make new RREQ as soon as possible...
    if (pFirstFoundDup != NULL) {
        pFirstFoundDup->retry_count = 0;
        pFirstFoundDup->timestamp = 1;
        mmr_stats.rreq_duplicated++;
    }

    // Find free position to insert new message
    for (i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if (packet_queue[i].timestamp == 0) {
            // Free position found, add entry
            memcpy((void *) & (packet_queue[i].packet),
                   packet, sizeof(secure_packet_t));

            if (pFirstFoundDup != NULL) {
                /* There is already a RREQ for this destination, so don't
                 *  generate a new one
                 */
                packet_queue[i].retry_count = RREQ_NONE;
            }
            else {
                // Set initial RREQ retry counter to zero
                packet_queue[i].retry_count = 0;
            }

            packet_queue[i].timestamp = 1;
            ulog("> Added packet to routing queue at pos %d", i);
            return &packet_queue[i];
        }
    }

    return NULL;
}

/**
 * @brief       Count messages for given destination.
 *
 * @param       dst Destination address
 *
 * @return      The number of messages for the destination.
 */
static int pq_msgs_for_destination(uint16_t dst)
{
    int i, dst_count = 0;

    for (i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if (packet_queue[i].timestamp != 0 &&
                packet_queue[i].packet.destination == dst) {
            dst_count++;
        }
    }

    return dst_count;
}

/**
 * @brief       Remove all messages for given destination out of message queue.
 *
 * @param       dst Destination address
 */
static void pq_remove_msgs_for_destination(uint16_t dst)
{

    int i;

    for (i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if (packet_queue[i].timestamp != 0 &&
                packet_queue[i].packet.destination == dst) {
            packet_queue[i].timestamp = 0;
        }
    }
}

/**
 * @brief       Send all queued messages for given destination.
 *
 * @param       dst Destination address
 */
static void pq_dequeue_and_send(uint16_t dst)
{
    int i;
    ulog_info("> pq_dequeue_and_send");

    // Stop any pending RREQ-Timeout, it's possibly handled now
    rreq_to_active = false;
    vtimer_remove(&vt);
    stop_pending_timer = 1;

    // Prioritize packets for given destination, route entry should exist
    network_node_t *node = network_nodes_find(dst);

    if (node != NULL) {
        ulog("> Found node for dst=%d. Gateway=%d.",
                 (uint8_t)dst, node->gateway);

        if (node->gateway == 0) {
            ulog("> Will not send to gateway 0");
        }

        for (i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
            if (packet_queue[i].timestamp != 0 &&
                packet_queue[i].packet.destination == dst &&
                node->gateway != 0) {
                // Set send function here
                ulog("> Sending packet via gateway %d", node->gateway);

                secure_packet_t *packet = &(packet_queue[i].packet);

                set_physical_source(packet);

                // Encrypt, mac the packet. Will be done in-place
                if (secure_packet(packet,
                                      get_this_node_address()) >= PacketSecurity_OK) {
                    int res = enqueue_for_send(node->gateway,
                                  SECURE_ROUTING_PROTOCOL,
                                  PRIORITY_DATA,
                                  (uint8_t *)packet,
                                  sizeof(secure_packet_t),
                                  1,
                                  RADIO_SEND_RETRY,
                                  0);

                    if (res == SecureRouting_OK) {
                        packet_queue[i].timestamp = 0;
                        ulog_info("> Reset queue pos %d to 0", i);
                    }
                }
                else {
                    ulog("Did not send packet because securing it failed");
                }
            }
        }
    }

    // Now all other packets
    for (i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if (packet_queue[i].timestamp != 0 &&
                packet_queue[i].packet.destination != dst) {
            network_node_t *node = network_nodes_find(packet_queue[i].
                                                          packet.destination);

            if (node != NULL) {
                ulog("> Sending packet via gateway %d", node->gateway);

                if (node->gateway == 0) {
                    ulog("> Will not send to gateway 0");
                    continue;
                }

                secure_packet_t *packet = &(packet_queue[i].packet);

                set_physical_source(packet);

                // Encrypt, mac the packet. Will be done in-place
                if (secure_packet(packet,
                                       get_this_node_address()) >= PacketSecurity_OK) {
                    int res = enqueue_for_send(node->gateway,
                                                   SECURE_ROUTING_PROTOCOL,
                                                   PRIORITY_DATA,
                                                   (uint8_t *)packet,
                                                   sizeof(secure_packet_t),
                                                   1,
                                                   RADIO_SEND_RETRY,
                                                   0);

                    if (res == SecureRouting_OK) {
                        packet_queue[i].timestamp = 0;
                        ulog_info("> Reset queue pos %d to 0", i);
                    }
                }
                else {
                    ulog("Did not send packet because securing it failed");
                }
            }
        }
    }

    // This function was triggered either by RREP packet or RREQ-Timeout.
    // So update or set new RREQ-Timeout for other packets in queue.
    post_next_rreq_timeout();
}

/*---------------------------------------------------------------------------*/
//                                      Initialization of MMR layer
/*---------------------------------------------------------------------------*/

/**
 * @brief       Initialises the micromesh layer
 *
 */
void micromesh_init(void)
{
    ulog("> Initialising Micro-Mesh");
    // rt_init();
    memset(packet_queue, 0, sizeof(packet_queue_entry_t) * MESSAGE_QUEUE_SIZE);

    if (rreq_timeout_process_pid < 0) {
        rreq_timeout_process_pid = thread_create(micromesh_thread_stack_buffer,
                                                         4500,
                                                         PRIORITY_MAIN + 2,
                                                         CREATE_STACKTEST,
                                                 rreq_timeout_process,
                                                 rreq_timeout_process_name);
    }
    else {
        ulog("> MMR thread already exists. Pid=%d", rreq_timeout_process_pid);
    }
}

/*---------------------------------------------------------------------------*/
//                     Send & receive functions
/*---------------------------------------------------------------------------*/

/**
 * @brief       Generates a route reply message.
 *
 * @param       rreq_msg Corresponding route request message
 */
static void generate_route_reply_message(mmr_rreq_message_t *rreq_msg)
{
    ulog("> generate_route_reply_message");

    // Create RREP message
    mmr_rrep_message_t rrep_msg;
    rrep_msg.type = MMR_TYPE_RREP;
    rrep_msg.length = rreq_msg->length;
    rrep_msg.destination = rreq_msg->source;
    rrep_msg.source = rreq_msg->destination;
    memcpy(rrep_msg.address,
           rreq_msg->address,
           rreq_msg->length * sizeof(uint16_t));

    // Create MMR message containing the RREP message
    secure_packet_t packet;
    memset(&packet, 0, sizeof(secure_packet_t));

    set_physical_source_p(packet);

    packet.protocol = SECURE_ROUTING_PROTOCOL;
    packet.payload_len = sizeof(mmr_rrep_message_t);
    packet.source = rrep_msg.source;
    packet.destination = rrep_msg.destination;
    packet.flags = SECURE_FLAG(PeekHint_Peek, SecurityMode_EncryptAndMAC);
    packet.ttl = TTL_THRESHOLD;

    memcpy(packet.data, (void *)&rrep_msg, sizeof(mmr_rrep_message_t));

    secure_packet(&packet, get_this_node_address());

    // Source address must exist in route table to find correct
    // interface id and next hop (should be created by RREQ)
    network_node_t *node = network_nodes_find(packet.destination);

    if (node != NULL) {
        ulog("> Sending route-reply to %d via %d", packet.destination,
              node->gateway);

        // Send message to next hop
        mmr_stats.rrep_originated++;

        // Wait a little bit before sending it to allow the broadcast wave
        //to passs
        /*
        uint32_t jitter = (cc1100_get_avg_transmission_duration() * (5));
        uint32_t wait = (jitter * 1000) + 10; // +10 to avoid being on a
        time-out boundary
        timex_t t = timex_set(0, wait);

        ulog("> Waiting for. %lu ms before sending route-reply", jitter);
        vtimer_sleep(t);
        */

        enqueue_for_send(node->gateway,
                                 SECURE_ROUTING_PROTOCOL,
                                 PRIORITY_DATA, (uint8_t *)&packet,
                                 sizeof(secure_packet_t),
                                 1,
                                 RADIO_SEND_RETRY,
                                 0);
    }
    else {
        ulog_error("Node was not found in table");
    }
}

/**
 * @brief       Generates a route error message.
 *
 * @param       dst Destination address of RERR packet
 * @param       gateway Next hop network address of RERR packet
 * @param       intf Interface id of RERR packet
 * @param       type Error type of RERR packet
 * @param       type_data Type specific data of RERR packet
 */
static void generate_route_error_message(uint16_t dst,
                                                 uint16_t gateway,
                                                 uint8_t type,
                                                 uint16_t destination)
{
    ulog("> call generate_route_error_message");

    if (dst == GLOBAL_BROADCAST) {
        ulog_error("Route Error message would have been to global. Dropping");
        return;
    }

    // Define RERR message
    mmr_rerr_message_t rerr_msg;
    rerr_msg.type = MMR_TYPE_RERR;
    rerr_msg.error_type = type;
    rerr_msg.node_id = get_this_node_address();
    rerr_msg.next_hop_address = gateway;
    rerr_msg.destination = destination;

    // Wrap RERR message in secure packet
    secure_packet_t packet;
    memset(&packet, 0, sizeof(secure_packet_t));

    set_physical_source_p(packet);
    packet.protocol = SECURE_ROUTING_PROTOCOL;
    // Added +4 padding so that message long enough for encryption
    packet.payload_len = sizeof(mmr_rerr_message_t) + 4;
    packet.source = get_this_node_address();
    packet.ttl = TTL_THRESHOLD;
    packet.flags = SECURE_FLAG(PeekHint_Peek, SecurityMode_EncryptAndMAC);
    packet.destination = dst;

    memcpy(packet.data, (void *)&rerr_msg, sizeof(mmr_rerr_message_t));

    secure_packet(&packet, get_this_node_address());

    // Send message to next hop
    mmr_stats.rerr_originated++;

    network_node_t *node = network_nodes_find(packet.destination);

    if (node != NULL) {
        ulog("> Sending route-error to %d via %d", packet.destination,
                 node->gateway);

        if (node->gateway != 0) {
            enqueue_for_send(node->gateway,
                                 SECURE_ROUTING_PROTOCOL,
                                 PRIORITY_DATA, (uint8_t *)&packet,
                                 sizeof(secure_packet_t),
                                 1,
                                 1,
                                 0);
        }
        else {
            ulog_error("> Not sending route-error cause it would be through 0 \
                           gateway");
        }
    }
    else {
        ulog_error("Node was not found in table");
    }
}

/**
 * @brief       Receive a route request message.
 *
 * @param       msg         The route request packet
 * @param       packet_info Additional packet information
 */
static int receive_route_request_message(mmr_rreq_message_t *msg,
                                                 packet_info_t *packet_info)
{
    ulog("Received Route Request Message. Source=%lu, AddressCount=%d. \
          Dest=%lu", msg->source, msg->length, msg->destination);

    uint16_t my_addr = get_this_node_address();

    // If address list of RREQ message has enough space
    if (msg->length < ADDRESS_LIST_SIZE) {
        // append our node id to list
        msg->address[msg->length++] = my_addr;

        // add routes with overhearing
        rt_extract_routes(my_addr, msg->length, msg->address);
    }
    // Distance between sender and receiver is too long, discard packet
    else {
        // Drop RREQ packet => set TTL to zero
        packet_info->ttl = 0;
        ulog("> Dropped RREQ. Address list is full");

        // This returning 1 will make the rebroadcast end
        return 1;
    }

    // If RREQ message was send to us, then send RREP message
    if (msg->destination == my_addr) {
        ulog("> Route was meant for this node. Generating route reply");

        // Don't forward RREQ packet any further => set TTL to zero
        packet_info->ttl = 0;
        generate_route_reply_message(msg);

        return 1;
    }

    return 0;
}


/**
 * @brief       Tests if the net message contains a RERR.
 *
 * @param       packet The secure packet to test
 *
 * @return      true if the net message contains a RERR; false otherwise.
 */
static bool is_route_error(secure_packet_t *packet)
{
    if (packet->protocol == SECURE_ROUTING_PROTOCOL) {
        // First byte in {RREQ, RREP, RERR} is always type
        if (packet->data[0] == MMR_TYPE_RERR) {
            return true;
        }
    }

    return false;
}

/**
 * @brief       Examines the packet if it can be used to update the route
 *
 * @param       packet The packet to be examined
 *
 */
void micromesh_peek(secure_packet_t *packet)
{
    // Only look at micro mesh routing messages
    if (packet->protocol == SECURE_ROUTING_PROTOCOL) {
        uint16_t my_addr = get_this_node_address();

        uint8_t type = packet->data[0];

        if (type == MMR_TYPE_RREP) {
            ulog("> Peeked route-reply. Extracting routes");
            // Add routes to route table
            mmr_rrep_message_t *rrep_msg = (mmr_rrep_message_t *)packet->data;

            rt_extract_routes(my_addr, rrep_msg->length, rrep_msg->address);
        }
        else if (type == MMR_TYPE_RERR) {
            ulog("> Peeked route-error, marking nodes");

            /* If not destination of RERR, then remove route to unavailable
             * node in RERR packet
             */
            if (packet->destination != my_addr) {
                mmr_rerr_message_t *rerr_msg = (mmr_rerr_message_t *)
                                                       packet->data;

                ulog("> Route Error at node: %lu. Could not send to %lu. \
                         Intended destination was %lu", rerr_msg->node_id,
                                                        rerr_msg->next_hop_address,
                                                        rerr_msg->destination);

                if (rerr_msg->error_type == RERR_NODE_UNREACHABLE) {
                    network_nodes_set_node_unreachable(network_nodes_get_root(),
                                                           rerr_msg->destination);
                }
            }
        }
    }
}


/**
 * @brief       Receive a route reply message.
 *
 * @param       msg The route reply packet
 * @param       packet_info Additional packet information
 */
static void receive_route_reply_message(mmr_rrep_message_t *msg,
                                                packet_info_t *packet_info)
{
    ulog_info("> receive_route_reply_message.");

    ulog("> Extracting route. Items=%d", msg->length);
    uint16_t my_addr = get_this_node_address();
    rt_extract_routes(my_addr, msg->length, msg->address);

    ulog("> --> Sending items in queue");
    pq_dequeue_and_send(msg->source);
}

/**
 * @brief       Receive a route error message.
 *
 * @param       msg The route error packet
 * @param       packet_info Additional packet information
 */
static void receive_route_error_message(mmr_rerr_message_t *msg,
                                                packet_info_t *packet_info)
{
    switch (msg->error_type) {
        case RERR_NODE_UNREACHABLE:
            ulog("> Route Error at node: %lu. Could not send to %lu. \
                 Intended destination was %lu",
                 msg->node_id,
                 msg->next_hop_address,
                 msg->destination);
            network_nodes_set_node_unreachable(network_nodes_get_root(),
                                                   msg->destination);
            break;

        default:
            ulog_error(" RERR error type is unknown");
            break;
    }
}

/**
 * @brief       Computes the RREQ timeout period, given a
 *              TTL and destination address value.
 *
 * @param       ttl Time to live
 * @param       dst Network destination address
 *
 * @return      RREQ timeout period in seconds
 */
static int compute_rreq_timeout(int ttl, uint16_t dst)
{

    // 5 seconds as minimum time-out + 3seconds for each additional
    // Implying ttl 10 = 35 seconds time-out
    return RREQ_TIMEOUT_BASE + (RREQ_TIMEOUT_PER_TTL * ttl);
}

/**
 * @brief       Broadcast a RREQ message.
 *
 * A single route request can repeatedly broadcast RREQ messages,
 * with increasing TTL value, until a route has been found.
 *
 * @param       mq_entry Pointer to a message queue entry (the packet
 *              for which to find the route)
 */
static void rreq_broadcast(packet_queue_entry_t *pq_entry)
{
    ulog("> Started Route-Request. TTL=%d",
         pq_entry->retry_count == 0 ? TTL_START : TTL_THRESHOLD);

    if (pq_entry->retry_count == RREQ_NONE) {
        ulog("rreq duplicated do not send");
        return;
    }

    // Create RREQ message
    mmr_rreq_message_t rreq_msg;
    rreq_msg.type = MMR_TYPE_RREQ;
    rreq_msg.length = 1;
    rreq_msg.destination = pq_entry->packet.destination;
    rreq_msg.source = pq_entry->packet.source;
    rreq_msg.address[0] = pq_entry->packet.source;

    // ulog("Creating the secure packet");
    // Wrap RREQ message in net message
    secure_packet_t packet;
    memset(&packet, 0, sizeof(secure_packet_t));

    set_physical_source_p(packet);

    packet.protocol = SECURE_ROUTING_PROTOCOL;
    packet.payload_len = sizeof(mmr_rreq_message_t);
    packet.source = rreq_msg.source;
    packet.flags = SECURE_FLAG(PeekHint_Peek, SecurityMode_EncryptAndMAC);;
    packet.ttl = pq_entry->retry_count == 0 ? TTL_START : TTL_THRESHOLD;
    packet.destination = GLOBAL_BROADCAST;
    memcpy(packet.data, (void *)&rreq_msg, sizeof(mmr_rreq_message_t));

    secure_packet(&packet, get_this_node_address());

    // Broadcast the net message
    pq_entry->retry_count++;
    pq_entry->timestamp = rtc_now();

    ulog("> Executing Radio-Send");
    enqueue_for_send(GLOBAL_BROADCAST,
                         SECURE_ROUTING_PROTOCOL,
                         PRIORITY_DATA, (uint8_t *)&packet,
                         sizeof(secure_packet_t),
                         1,
                         1,
                         0);
}

/**
 * @brief       Find next RREQ to time out. Post event immediately or with
 *              vtimer.
 */
static void post_next_rreq_timeout(void)
{
    ulog_info("> Called Post-Next-RREQ-Timeout");

    int i, j = -1;
    uint32_t now, next = 0xffffffff;

    for (i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if (packet_queue[i].timestamp != 0 &&
            packet_queue[i].retry_count != RREQ_NONE) {
            int ttl = packet_queue[i].retry_count == 1 ?
                          TTL_START : TTL_THRESHOLD;
            int to = compute_rreq_timeout(ttl,
                                              packet_queue[i].packet.destination);

            ulog_info("> Packet found in queue. TS=%lu. Pack-Timeout=%d",
                       packet_queue[i].timestamp, to);

            if (packet_queue[i].timestamp + to < next) {
                next = packet_queue[i].timestamp + to;
                j = i;
            }
        }
    }

    if (j == -1) {
        ulog_info("> exit post_next_rreq_timeout j=-1");
        return;
    }

    // Stop any vtimer
    // ulog("> Removing vtimer");
    rreq_to_active = false;

    // If current time greater than RREQ timeout value
    now = rtc_now();

    ulog_info("> Now TS = %lu, next Q TS=%lu", now, next);

    if (now >= next) {
        ulog("> Q should be processed now");

        // Schedule RREQ-Timeout immediately
        msg_t m;
        m.type = MSG_TIMER;
        m.content.ptr = (char *)&packet_queue[j];
        rreq_to_active = true;

        if (msg_send(&m, rreq_timeout_process_pid, false) < 0) {
            /* If this fails, then it should not resend. Else an infinite loop
             *  happens.
             */
            ulog_error("Error: Could not send message to time-out thread");
        }

    }
    else {

        ulog("> Timeout required in %lus. Setting", next - now);

        // Set new vtimer with time difference
        rreq_to_active = true;
        stop_pending_timer = 0;

        vtimer_remove(&vt);
        stop_pending_timer = 1;

        timex_t t = timex_set(next - now, 0);
        vtimer_set_msg(&vt, t, rreq_timeout_process_pid, &packet_queue[j]);

    }

}

/**
 * This event is called periodically after a route request is originated,
 * until a route has been found.
 *
 * Each time it is called, it rebroadcasts the route request message with a
 * new rreq id and incremented TTL.
 */
static void rreq_timeout(packet_queue_entry_t *pqe)
{
    ulog("!rreq_timeout");

    // Test if valid entry passed
    if (pqe->timestamp == 0) {
        ulog("MMR [WARN]: invalid message queue entry for RREQ-Timeout");
        goto post_next_to;
    }

    // See if route to destination was found
    network_node_t *node = network_nodes_find(pqe->packet.destination);

    // If found and no messages in queue for destination: return (queued
    // packets are send on reception of RREP); If found but messages in
    // queue: trigger send immediately here!
    if (node != NULL) {
        ulog("Node found");

        int msg_count = pq_msgs_for_destination(pqe->packet.destination);

        if (msg_count > 0) {
            pq_dequeue_and_send(pqe->packet.destination);
            return;
        }
        else {
            // Added just for security but this case should never occur
#if (MMR_INFO_LEVEL >= LEVEL_WARN)
            puts("MMR [WARN]: RREQ-Timeout occurred, route is available but \
                  no messages for destination");
#endif
            // Anyway: jump to update next RREQ-Timeout
            goto post_next_to;
        }
    }

    ulog("node not found");

    /*
     * Otherwise send new RREQ if below threshold (means also retry count !=
     * RREQ_NONE)
     */
    if (pqe->retry_count < RREQ_THRESHOLD) {
        ulog("Rebroadcasting Retry %d", pqe->retry_count);

        // Broadcast new RREQ message (with incremented TTL)
        rreq_broadcast(pqe);
    }
    else {
        ulog("Removing all messages for destination");

        // Remove all messages for this destination
        mmr_stats.messages_no_route_found++;
        pq_remove_msgs_for_destination(pqe->packet.destination);
    }

    // Anyway: update or set next RREQ-Timeout
post_next_to:
    post_next_rreq_timeout();

}

/*
 * Runs the process for the timeout timer to posts its messages to
 *
 */
static void rreq_timeout_process(void)
{
    ulog("> Started Micro-Mesh Timeout Thread");

    int res = msg_init_queue(timer_rreq_msg_queue, 16);

    if (res < 0) {
        ulog_error("ERROR: RREQ Message queue could not be inited");
    }

    msg_t m;

    do {
        msg_receive(&m);

        if (m.type == MSG_TIMER && rreq_to_active) {
            vtimer_remove(&vt);

            ulog("!Time-Out Message");
            rreq_to_active = false;
            rreq_timeout((packet_queue_entry_t *)m.content.ptr);
        }
    }
    while (m.type != MSG_EXIT);
}

/**
 * Called to send the secure packet with routing. If the packet
 * cannot be immediately sent, it will be queued and a route built
 *
 * @param packet The secure packet to send
 *
 * @return True or False depending if it failed or succeeded
 */
bool micromesh_send(secure_packet_t *packet)
{
    ulog_info("> Starting micromesh send");

    // bool enqueue = true;
    if (IS_LOCAL_ADDRESS(packet->destination)) {
        ulog_error("> Micromesh message is already at destination %d, why is \
                    routing called?", packet->destination);
        return false;
    }

    if (packet->destination == 0 || packet->ttl == 1) {
        // If the ttl of a packet is 1, then it is a keep-alive packet. Or it
        // is supposed to be directly within range. So no route-request is
        // started for this packet.

        ulog_info("> Broadcast packet not routed");

        // Encrypt, mac the packet. Will be done in-place
        if (secure_packet(packet,
                                  get_this_node_address()) >= PacketSecurity_OK) {
            // broadcast, does not need routing
            if (enqueue_for_send(packet->destination,
                                     SECURE_ROUTING_PROTOCOL,
                                     PRIORITY_DATA, (uint8_t *)packet,
                                     sizeof(secure_packet_t),
                                     1,
                                     1,
                                     0) == SecureRouting_OK) {
                return 1;
            }
            else {
                ulog_error("> Enqueue function failed. Queue full?");
                return 0;
            }
        }
        else {
            ulog("Did not send packet because securing it failed");
            return 0;
        }

        return 0;
    }


    // Look if we have information about the gateway node
    network_node_t *node = network_nodes_find(packet->destination);

    // If next hop address found in routing table, forward message
    if (node != 0 && node->gateway != 0) {
        ulog("> Gateway %d found for address %d",
              node->gateway, packet->destination);

        // Encrypt, mac the packet. Will be done in-place
        if (secure_packet(packet,
                                  get_this_node_address()) >= PacketSecurity_OK) {
            if (enqueue_for_send(node->gateway,
                                 SECURE_ROUTING_PROTOCOL,
                                 PRIORITY_DATA, (uint8_t *)packet,
                                 sizeof(secure_packet_t),
                                 1, RADIO_SEND_RETRY, 0) == SecureRouting_OK) {
                return 1;
            }
        }
        else {
            ulog("Did not send packet because securing it failed");
            return 0;
        }

    }
    // Otherwise, save message in queue; broadcast RREQ message
    else {
        ulog("> No gateway for node, building route");

        ulog_info("> Adding message to packet queue");
        packet_queue_entry_t *pqe = pq_add(packet);

        if (pqe != NULL) {
            rreq_broadcast(pqe);
            post_next_rreq_timeout();
            mmr_stats.rreq_originated++;
            return true;
        }
    }

    return false;
}

/**
 * Called when a secure packet is dropped
 *
 * @param message The secure packet that was dropped
 * @param next_hop The gateway
 * @param error The error
 *
 */
void micromesh_packet_dropped(secure_packet_t *packet,
                                      uint16_t next_hop,
                                      int error)
{
    if (error == ROUTE_ERROR_BROKEN_ROUTE) {
        ulog("> Broken route detected (%lu). Removing nodes using it",
             next_hop);
        // Local failure detected - remove all routes through broken link
        network_nodes_set_node_unreachable(network_nodes_get_root(), next_hop);
        mmr_stats.messages_broken_link_on_forward++;
    }
    else if (error == ROUTE_ERROR_MISSING_ROUTE) {
        ulog("> Missing route detected");
        mmr_stats.messages_no_route_avail_on_forward++;
    }

    // If source != net_addr, send RERR to source of message
    if (!IS_LOCAL_ADDRESS(packet->source)) {
        ulog("> Generating route-error message to %d", packet->source);

        // Do not generate RERR if it is already a RERR message
        if (is_route_error(packet)) {
            ulog("> Broken route on route-error. No route-err sent");
            return;
        }

        if (packet->destination == 0) {
            ulog("> Broken route on broadcast. No route-err sent");
            return;
        }

        // Find next hop to source
        network_node_t *node = network_nodes_find(packet->source);

        if (node != NULL) {
            ulog("> Sending Route-error message via %d",  node->gateway);
            generate_route_error_message(packet->source,
                                             node->gateway,
                                         RERR_NODE_UNREACHABLE,
                                         packet->destination);
        }
        else {
            ulog_error("Cannot send RERR to source %d, no route!",
                           packet->source);
        }
    }
    else {
        ulog_error("Route-Error at local node");
    }

}

/**
 * Called when a secure packet arrived
 *
 * @param packet The secure packet that arrived
 * @param packet_info The full packet info
 *
 * @return One of the Routing_Result results, depending on what needs to be
 *         done
 */
int micromesh_receive(secure_packet_t *packet, packet_info_t *packet_info)
{
    int handled = 0;
    int packet_for_mm = 0;
    uint8_t type = packet->data[0];

    ulog_info("> Micromesh Receive called. Type=%d", type);

    if (type == MMR_TYPE_RREQ) {
        ulog("> Micromesh route request arrived");
        handled = receive_route_request_message((mmr_rreq_message_t *)
                                                         packet->data,
                                                         packet_info);
        mmr_stats.rreq_received++;
        ulog_info("> EXPERIMENT(RREQCoverage, RREQS=%lu",
                   mmr_stats.rreq_received);
        packet_for_mm = 1;

    }
    else if (type == MMR_TYPE_RREP) {
        ulog("> !!!Micromesh route reply arrived");
        receive_route_reply_message((mmr_rrep_message_t *)packet->data,
                                             packet_info);
        mmr_stats.rrep_received++;
        packet_for_mm = 1;
    }
    else if (type == MMR_TYPE_RERR) {
        ulog("> Micromesh route error arrived");
        receive_route_error_message((mmr_rerr_message_t *)packet->data,
                                             packet_info);
        mmr_stats.rerr_received++;
        packet_for_mm = 1;
    }

    if (packet_for_mm && handled == 1) {
        return Routing_PacketHandledNoRebroadcast;
    }

    if (packet_for_mm && handled == 0) {
        return Routing_PacketHandledRebroadcast;
    }

    return Routing_OK;
}

/**
 * Prints all the statistics about the routing layer
 *
 */
void micromesh_print_stats(void)
{
    ulog("ROUTING LAYER STATS");
    printf("-------------------\n");
    printf("Route requests originated: %lu\n", mmr_stats.rreq_originated);
    printf("Route requests duplicated: %lu\n", mmr_stats.rreq_duplicated);
    printf("Route replies originated:  %lu\n", mmr_stats.rrep_originated);
    printf("Route errors originated:   %lu\n", mmr_stats.rerr_originated);
    printf("Route requests received:   %lu\n", mmr_stats.rreq_received);
    printf("Route replies received:    %lu\n", mmr_stats.rrep_received);
    printf("Route errors received:     %lu\n", mmr_stats.rerr_received);
    printf("\n");
    printf("#Messages with no route found: %lu\n",
                mmr_stats.messages_no_route_found);
    printf("#Messages with broken link on forward: %lu\n",
                mmr_stats.messages_broken_link_on_forward);
    printf("#Messages with no route available on forward: %lu\n",
                mmr_stats.messages_no_route_avail_on_forward);
    printf("\n");

    for (int i = 0; i < MESSAGE_QUEUE_SIZE; i++) {
        if (packet_queue[i].timestamp > 0) {
            printf("Queue %d: Pack to %d\n",
                   i,
                   packet_queue[i].packet.destination);
        }
    }
}
