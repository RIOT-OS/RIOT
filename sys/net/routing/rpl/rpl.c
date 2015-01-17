/*
 * Copyright (C) 2013, 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     rpl
 * @{
 *
 * @file        rpl.c
 * @brief       Implementation of the RPL-core.
 *
 * Implementation of core RPL-functions. Normally it shouldn't be necessary to
 * modify this file.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 */

#include <string.h>
#include "vtimer.h"
#include "thread.h"
#include "mutex.h"

#include "msg.h"
#include "rpl.h"
#include "etx_beaconing.h"
#include "of0.h"
#include "of_mrhof.h"
#include "trickle.h"

#include "sixlowpan.h"
#include "net_help.h"
#if RPL_DEFAULT_MOP == RPL_STORING_MODE_NO_MC
#include "rpl/rpl_storing.h"
#elif RPL_DEFAULT_MOP == RPL_NON_STORING_MODE
#include "rpl/rpl_nonstoring.h"
#else
#include "rpl/rpl_storing.h"
#endif

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#undef TRICKLE_TIMER_STACKSIZE
#define TRICKLE_TIMER_STACKSIZE (KERNEL_CONF_STACKSIZE_MAIN)
char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif
#include "debug.h"

/* global variables */
kernel_pid_t rpl_process_pid = KERNEL_PID_UNDEF;
mutex_t rpl_send_mutex = MUTEX_INIT;
msg_t rpl_msg_queue[RPL_PKT_RECV_BUF_SIZE];
char rpl_process_buf[RPL_PROCESS_STACKSIZE];
uint8_t rpl_buffer[BUFFER_SIZE - LL_HDR_LEN];

#if RPL_DEFAULT_MOP == RPL_NON_STORING_MODE
uint8_t srh_buffer[BUFFER_SIZE];
uint8_t srh_send_buffer[BUFFER_SIZE];
ipv6_addr_t *down_next_hop;
ipv6_srh_t *srh_header;
msg_t srh_m_send, srh_m_recv;
#endif

rpl_routing_entry_t rpl_routing_table[RPL_MAX_ROUTING_ENTRIES];

uint8_t rpl_max_routing_entries;
ipv6_addr_t my_address;

/* IPv6 message buffer */
ipv6_hdr_t *ipv6_buf;
icmpv6_hdr_t *icmp_buf;

uint8_t rpl_init(int if_id)
{
    rpl_instances_init();

    /* initialize routing table */
    rpl_max_routing_entries = RPL_MAX_ROUTING_ENTRIES;
    rpl_clear_routing_table();

    if (rpl_routing_table == NULL) {
        DEBUGF("Routing table init failed!\n");
        return SIXLOWERROR_NULLPTR;
    }
    else {
        DEBUGF("Routing table init finished!\n");
    }

    init_trickle();
    //thread_print_all();

    rpl_process_pid = thread_create(rpl_process_buf, RPL_PROCESS_STACKSIZE,
                                    PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                    rpl_process, NULL, "rpl_process");

    sixlowpan_lowpan_init_interface(if_id);
    DEBUG("sixlowpan_lowpan_init_interface \n");
    
    /* need link local prefix to query _our_ corresponding address  */
    ipv6_addr_t ll_address;
    ipv6_addr_set_link_local_prefix(&ll_address);
    ipv6_net_if_get_best_src_addr(&my_address, &ll_address);
    ipv6_register_rpl_handler(rpl_process_pid);
    ipv6_iface_set_srh_indicator(rpl_is_root);
    ipv6_iface_set_routing_provider(rpl_get_next_hop);
    DEBUGF("All addresses set!\n");

    /* initialize objective function manager */
    rpl_of_manager_init(&my_address);
    rpl_init_mode(&my_address);
    return SIXLOWERROR_SUCCESS;
}

void rpl_init_root(void)
{
#if (RPL_DEFAULT_MOP == RPL_NON_STORING_MODE)
#ifndef RPL_NODE_IS_ROOT
puts("\n############################## ERROR ###############################");
puts("This configuration has NO ROUTING TABLE available for the root node!");
puts("The root will NOT be INITIALIZED.");
puts("Please build the binary for root in non-storing MOP with:");
puts("\t\t'make RPL_NODE_IS_ROOT=1'");
puts("############################## ERROR ###############################\n");
return;
#endif
#endif
    rpl_init_root_mode();
}

uint8_t rpl_is_root(void)
{
    return rpl_is_root_mode();
}

#if RPL_DEFAULT_MOP == RPL_NON_STORING_MODE
void internal_srh_process(ipv6_srh_t *srh_header)
{
    /* modify it accordingly - the number of entries is not depending on padding, because there is none. */
    uint8_t n = srh_header->hdrextlen /  sizeof(ipv6_addr_t);

    if (srh_header->segments_left > n) {
        memset(&down_next_hop, 0, sizeof(ipv6_addr_t));
        DEBUGF("[Error] source-routing header is invalid.\n");
    }
    else {
        uint8_t segs = srh_header->segments_left;
        DEBUGF("SRH-length: %d\n", srh_header->hdrextlen);
        down_next_hop = &srh_header->route[n - segs];
        srh_header->segments_left = segs - 1;
        DEBUGF("Segments left after reduction: %d\n", srh_header->segments_left);
        DEBUGF("Next hop is: %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, down_next_hop));
    }
}
#endif

void *rpl_process(void *arg)
{
    (void) arg;

    msg_t m_recv;
    msg_init_queue(rpl_msg_queue, RPL_PKT_RECV_BUF_SIZE);

    while (1) {
        msg_receive(&m_recv);

        /* differentiate packet types */
        ipv6_buf = ((ipv6_hdr_t *)m_recv.content.ptr);
        memcpy(&rpl_buffer, ipv6_buf, NTOHS(ipv6_buf->length) + IPV6_HDR_LEN);

        /* This is an RPL-related message. */
        if (ipv6_buf->nextheader == IPV6_PROTO_NUM_ICMPV6) {
            icmp_buf = ((icmpv6_hdr_t *)(m_recv.content.ptr + IPV6_HDR_LEN));

            /* get code for message-interpretation and process message */
            DEBUGF("Received RPL information of type %04X and length %u\n", icmp_buf->code, NTOHS(ipv6_buf->length));

            switch (icmp_buf->code) {
                case (ICMP_CODE_DIS): {
                    rpl_recv_DIS();
                    break;
                }

                case (ICMP_CODE_DIO): {
                    rpl_recv_DIO();
                    break;
                }

                case (ICMP_CODE_DAO): {
                    rpl_recv_DAO();
                    break;
                }

                case (ICMP_CODE_DAO_ACK): {
                    rpl_recv_DAO_ACK();
                    break;
                }

                default:
                    break;
            }
        }

#if RPL_DEFAULT_MOP == RPL_NON_STORING_MODE
        /* If the message is not RPL-type, it relates to non-storing mode */
        else if (RPL_DEFAULT_MOP == RPL_NON_STORING_MODE) {

            if (ipv6_buf->nextheader == IPV6_PROTO_NUM_SRH) {
                srh_header = ((ipv6_srh_t *)(m_recv.content.ptr + IPV6_HDR_LEN));

                /* if there are no segments left, the routing is finished */
                if (srh_header->segments_left == 0) {
                    DEBUGF("Source routing finished with next header: %02X.\n", srh_header->nextheader);
                    DEBUGF("Size of srh: %d\n", srh_header->hdrextlen);
                    uint8_t *payload = ((uint8_t *)(m_recv.content.ptr + IPV6_HDR_LEN + sizeof(ipv6_srh_t)+srh_header->hdrextlen));
                    rpl_remove_srh_header(ipv6_buf, payload, srh_header->nextheader);
                }
                else {
                    internal_srh_process(srh_header);
                    if (down_next_hop != NULL) {
                        uint8_t *payload = ((uint8_t *)(m_recv.content.ptr + IPV6_HDR_LEN));
                        rpl_srh_sendto(payload, NTOHS(ipv6_buf->length), &ipv6_buf->srcaddr, down_next_hop, srh_header, 0);
                    }
                }
            }
            else  {
                srh_header = rpl_get_srh_header(ipv6_buf);

                if (srh_header != NULL) {
                    uint8_t *payload = ((uint8_t *)(m_recv.content.ptr + IPV6_HDR_LEN));
                    rpl_srh_sendto(payload, NTOHS(ipv6_buf->length), &ipv6_buf->srcaddr, &ipv6_buf->destaddr, srh_header, srh_header->hdrextlen + sizeof(ipv6_srh_t));
                }
            }

        }

#endif
    }
}

void rpl_send_DIO(ipv6_addr_t *destination)
{
    if (destination) {
        DEBUGF("Send DIO to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }

    mutex_lock(&rpl_send_mutex);
    rpl_send_DIO_mode(destination);
    mutex_unlock(&rpl_send_mutex);
}

void rpl_send_DAO(ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime, uint8_t start_index)
{
    if (destination) {
        DEBUGF("Send DAO to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }

    mutex_lock(&rpl_send_mutex);
    rpl_send_DAO_mode(destination, lifetime, default_lifetime, start_index);
    mutex_unlock(&rpl_send_mutex);
}

void rpl_send_DIS(ipv6_addr_t *destination)
{
    if (destination) {
        DEBUGF("Send DIS to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }

    mutex_lock(&rpl_send_mutex);
    rpl_send_DIS_mode(destination);
    mutex_unlock(&rpl_send_mutex);
}

void rpl_send_DAO_ACK(ipv6_addr_t *destination)
{
    if (destination) {
        DEBUGF("Send DAO ACK to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }

    mutex_lock(&rpl_send_mutex);
    rpl_send_DAO_ACK_mode(destination);
    mutex_unlock(&rpl_send_mutex);
}

void rpl_recv_DIO(void)
{
    DEBUGF("DIO received\n");

    rpl_recv_DIO_mode();
}

void rpl_recv_DAO(void)
{
    DEBUGF("DAO received\n");

    rpl_recv_DAO_mode();
}

void rpl_recv_DIS(void)
{
    DEBUGF("DIS received\n");

    rpl_recv_DIS_mode();

}

void rpl_recv_DAO_ACK(void)
{
    DEBUGF("DAO ACK received\n");

    rpl_recv_dao_ack_mode();
}

ipv6_addr_t *rpl_get_next_hop(ipv6_addr_t *addr)
{

    DEBUGF("Looking up the next hop to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, addr));

    for (uint8_t i = 0; i < rpl_max_routing_entries; i++) {
        if (rpl_routing_table[i].used) {
            DEBUGF("checking %d: %s\n", i, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &rpl_routing_table[i].address));
        }

        if ((RPL_DEFAULT_MOP == RPL_NON_STORING_MODE) && rpl_is_root()) {
            if (rpl_routing_table[i].used && rpl_equal_id(&rpl_routing_table[i].address, addr)) {
                DEBUGF("found %d: %s\n", i, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &rpl_routing_table[i].address));
                return &rpl_routing_table[i].address;
            }
        }
        else {
            if (rpl_routing_table[i].used && rpl_equal_id(&rpl_routing_table[i].address, addr)) {
                DEBUGF("found %d: %s\n", i, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &rpl_routing_table[i].next_hop));
                return &rpl_routing_table[i].next_hop;
            }
        }
    }

    return (rpl_get_my_preferred_parent());
}

void rpl_add_routing_entry(ipv6_addr_t *addr, ipv6_addr_t *next_hop, uint16_t lifetime)
{
    rpl_routing_entry_t *entry = rpl_find_routing_entry(addr);

    if (entry != NULL) {
        entry->lifetime = lifetime;
        return;
    }

    DEBUGF("Adding routing entry %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, addr));

    for (uint8_t i = 0; i < rpl_max_routing_entries; i++) {
        if (!rpl_routing_table[i].used) {
            memcpy(&rpl_routing_table[i].address, addr, sizeof(ipv6_addr_t));
            memcpy(&rpl_routing_table[i].next_hop, next_hop, sizeof(ipv6_addr_t));
            rpl_routing_table[i].lifetime = lifetime;
            rpl_routing_table[i].used = 1;
            break;
        }
    }
}

void rpl_del_routing_entry(ipv6_addr_t *addr)
{

    DEBUGF("Deleting routing entry %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, addr));

    for (uint8_t i = 0; i < rpl_max_routing_entries; i++) {
        if (rpl_routing_table[i].used && rpl_equal_id(&rpl_routing_table[i].address, addr)) {
            memset(&rpl_routing_table[i], 0, sizeof(rpl_routing_table[i]));
            return;
        }
    }
}

rpl_routing_entry_t *rpl_find_routing_entry(ipv6_addr_t *addr)
{

    DEBUGF("Finding routing entry %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, addr));

    for (uint8_t i = 0; i < rpl_max_routing_entries; i++) {
        if (rpl_routing_table[i].used && rpl_equal_id(&rpl_routing_table[i].address, addr)) {
            return &rpl_routing_table[i];
        }
    }

    return NULL;
}

void rpl_clear_routing_table(void)
{

    for (uint8_t i = 0; i < rpl_max_routing_entries; i++) {
        memset(&rpl_routing_table[i], 0, sizeof(rpl_routing_table[i]));
    }
}

rpl_routing_entry_t *rpl_get_routing_table(void)
{
    return rpl_routing_table;
}

#if RPL_DEFAULT_MOP == RPL_NON_STORING_MODE
/* everything from here on is non-storing mode related */

void rpl_add_srh_entry(ipv6_addr_t *child, ipv6_addr_t *parent, uint16_t lifetime)
{

    rpl_routing_entry_t *entry = rpl_find_routing_entry(child);

    /* If we already have this entry and the parent from parent/child is the same as already
     * registered, we only update the lifetime. If only the parent of the child changes, we
     * delete the previous entry and add it below.
     */
    if (entry != NULL) {
        if (ipv6_addr_is_equal(parent, &entry->next_hop)) {
            entry->lifetime = lifetime;
            return;
        }
        else {
            rpl_del_routing_entry(child);
        }
    }

    /* This maybe a bit confusing since the root also using the standard routing table, but in this case
     * the code stays cleaner - especially for rt_over_timer from trickle.c. Just keep in mind that
     * address is now child (unique, iteration variable) and parent is now next_hop. The whole routing table
     * transforms to a list of children and their parents, so that route aggregation can be done properly.
     */
    DEBUGF("Adding source-routing entry child: %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, child));
    DEBUGF("Adding source-routing entry parent: %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, parent));

    for (uint8_t i = 0; i < rpl_max_routing_entries; i++) {
        if (!rpl_routing_table[i].used) {
            memcpy(&rpl_routing_table[i].address, child, sizeof(ipv6_addr_t));
            memcpy(&rpl_routing_table[i].next_hop, parent, sizeof(ipv6_addr_t));
            rpl_routing_table[i].lifetime = lifetime;
            rpl_routing_table[i].used = 1;
            break;
        }
    }
}

/**
 * @brief   Checks if two IPv6 host suffixes are equal.
 *
 * @param[in] a     An IPv6 address.
 * @param[in] b     Another IPv6 address.
 *
 * @return  1 if *a* and *b* are equal for host suffixes, 0 otherwise.
 */
int ipv6_suffix_is_equal(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    return (a->uint32[2] == b->uint32[2]) &&
           (a->uint32[3] == b->uint32[3]);
}

ipv6_srh_t *rpl_get_srh_header(ipv6_hdr_t *act_ipv6_hdr)
{
    uint8_t route_length = RPL_MAX_SRH_PATH_LENGTH;
    ipv6_addr_t rev_route[route_length];
    ipv6_addr_t *actual_node;
    uint8_t counter = 0;
    uint8_t traceable;
    ipv6_srh_t *srh_header = (ipv6_srh_t *)(&srh_buffer);

    /* init source route with child */
    actual_node = &(act_ipv6_hdr->destaddr);
    DEBUGF("DESTINATION NODE: %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, actual_node));

    while (!(rpl_equal_id(actual_node, &my_address))) {
        /* set check variable - this is reversed, if a child/parent-relation is found in one iteration of the routing table */
        traceable = 0;

        for (uint8_t i = 0; i < rpl_max_routing_entries; i++) {
            if (rpl_routing_table[i].used && ipv6_suffix_is_equal(&rpl_routing_table[i].address, actual_node)) {
                DEBUGF("[INFO] Found parent-child relation with P: %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &rpl_routing_table[i].next_hop));
                DEBUGF(" and C: %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, actual_node));
                memcpy(&rev_route[counter], actual_node, sizeof(ipv6_addr_t));

                actual_node = &rpl_routing_table[i].next_hop;
                counter++;
                traceable = 1;

                if (counter > route_length) {
                    DEBUGF("[INFO] Both lengths are %d and %d\n", counter, route_length);
                    DEBUGF("Error with computing source routing header.\n");
                    return NULL;
                }
                break;
            }
        }

        if (traceable == 0) {
            DEBUGF("No route to destination.\n");
            return NULL;
        }
    }

    /* build real route based on reversed route. After building it starts with the node next to destination */
    if (counter > 1) {
        for (uint8_t i = 0; i < counter-1; i++) {
            memcpy(&srh_header->route[i], &rev_route[counter-i-2], sizeof(ipv6_addr_t));
        }
        srh_header->hdrextlen = sizeof(ipv6_addr_t)*(counter-1);
        memcpy(&(act_ipv6_hdr->destaddr), &rev_route[counter-1], sizeof(ipv6_addr_t));
        DEBUGF("Route size: %d\n", srh_header->hdrextlen);
    }
    else {
        srh_header->hdrextlen = 0;
    }

    /* actually build SRH-header */
    memcpy(&srh_header->nextheader, &(act_ipv6_hdr->nextheader), sizeof(uint8_t));
    srh_header->routing_type = 3;
    srh_header->segments_left = counter - 1;
    /* cmpri & cmpre are both 0, because by now there is only support for full addresses */
    srh_header->cmpri = 0;
    srh_header->cmpre = 0;
    /* since the route has exactly the length of its intermediate nodes, padding is zero. */
    srh_header->pad = 0;
    srh_header->reserved = 0;
    /* set the destination-address in ipv6-buf->destaddr, which is the pointer of child */
    return srh_header;
}

void rpl_remove_srh_header(ipv6_hdr_t *ipv6_header, const void *buf, uint8_t nextheader)
{
    ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&srh_send_buffer));
    uint8_t *payload = &srh_send_buffer[IPV6_HDR_LEN];
    memcpy(temp_ipv6_header, ipv6_header, sizeof(ipv6_hdr_t));
    int msg_length = NTOHS(ipv6_header->length) - sizeof(ipv6_srh_t);
    temp_ipv6_header->length = HTONS(msg_length);
    temp_ipv6_header->nextheader = nextheader;
    memcpy(payload, buf, msg_length);
    DEBUGF("Source routing header extraction finished.\n");
    DEBUGF("Dest is now: %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &temp_ipv6_header->destaddr));
    srh_m_send.content.ptr = (char *) srh_send_buffer;
    DEBUGF("Return from relay: %d\n", msg_send_receive(&srh_m_send, &srh_m_recv, ip_process_pid));
}

int rpl_srh_sendto(const void *buf, uint16_t len, ipv6_addr_t *src, ipv6_addr_t *dest, ipv6_srh_t *srh_header, uint8_t srh_length)
{
    ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&srh_send_buffer));
    ipv6_srh_t *current_packet = ((ipv6_srh_t *)(&srh_send_buffer[IPV6_HDR_LEN]));
    uint8_t *payload = &srh_send_buffer[IPV6_HDR_LEN + srh_length];
    memcpy(&(temp_ipv6_header->destaddr), dest, sizeof(ipv6_addr_t));
    memcpy(&(temp_ipv6_header->srcaddr), src, sizeof(ipv6_addr_t));
    memcpy(current_packet, srh_header, srh_length);
    memcpy(payload, buf, len);
    uint16_t plength = srh_length + len;

    DEBUGF("Build SRH package finished. Going to send it.\n");
    DEBUGF("SRH-length: %d\n", current_packet->hdrextlen);
    DEBUGF("My payload length: %d\n", plength);

    return ipv6_sendto(&temp_ipv6_header->destaddr, IPV6_PROTO_NUM_SRH, (uint8_t *)current_packet, plength, &temp_ipv6_header->destaddr);
}
#endif
