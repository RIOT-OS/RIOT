/**
 * RPL implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup rpl
 * @{
 * @file    rpl.c
 * @brief   basic RPL functions
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
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

/* You can only run Storing Mode by now. Other unsupported modes lead to default (Storing Mode) */
#if RPL_DEFAULT_MOP == RPL_STORING_MODE_NO_MC
#include "rpl_storing.h"
#else
#include "rpl_storing.h"
#endif

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#undef TRICKLE_TIMER_STACKSIZE
#define TRICKLE_TIMER_STACKSIZE (KERNEL_CONF_STACKSIZE_MAIN)
char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif
#include "debug.h"

/* global variables */
rpl_of_t *rpl_objective_functions[NUMBER_IMPLEMENTED_OFS];
rpl_routing_entry_t rpl_routing_table[RPL_MAX_ROUTING_ENTRIES];
kernel_pid_t rpl_process_pid;
mutex_t rpl_recv_mutex = MUTEX_INIT;
mutex_t rpl_send_mutex = MUTEX_INIT;
msg_t rpl_msg_queue[RPL_PKT_RECV_BUF_SIZE];
char rpl_process_buf[RPL_PROCESS_STACKSIZE];
uint8_t rpl_buffer[BUFFER_SIZE - LL_HDR_LEN];

/* IPv6 message buffer */
ipv6_hdr_t *ipv6_buf;

/* find implemented OF via objective code point */
rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp)
{
    for (uint16_t i = 0; i < NUMBER_IMPLEMENTED_OFS; i++) {
        if (ocp == rpl_objective_functions[i]->ocp) {
            return rpl_objective_functions[i];
        }
    }

    return NULL;
}

uint8_t rpl_init(int if_id)
{
    rpl_instances_init();

    /* initialize routing table */
    rpl_clear_routing_table();
    init_trickle();
    rpl_process_pid = thread_create(rpl_process_buf, RPL_PROCESS_STACKSIZE,
                                    PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                    rpl_process, NULL, "rpl_process");

    /* INSERT NEW OBJECTIVE FUNCTIONS HERE */
    rpl_objective_functions[0] = rpl_get_of0();
    rpl_objective_functions[1] = rpl_get_of_mrhof();

    sixlowpan_lowpan_init_interface(if_id);
    /* need link local prefix to query _our_ corresponding address  */
    ipv6_addr_t my_address;
    ipv6_addr_t ll_address;
    ipv6_addr_set_link_local_prefix(&ll_address);
    ipv6_net_if_get_best_src_addr(&my_address, &ll_address);
    ipv6_register_rpl_handler(rpl_process_pid);

    /* initialize ETX-calculation if needed */
    if (RPL_DEFAULT_OCP == 1) {
        DEBUGF("INIT ETX BEACONING\n");
        etx_init_beaconing(&my_address);
    }

    rpl_init_mode(&my_address);

    return SIXLOWERROR_SUCCESS;
}

void rpl_init_root(void)
{
    rpl_init_root_mode();
}

void *rpl_process(void *arg)
{
    (void) arg;

    msg_t m_recv;
    msg_init_queue(rpl_msg_queue, RPL_PKT_RECV_BUF_SIZE);

    while (1) {
        msg_receive(&m_recv);
        mutex_lock(&rpl_recv_mutex);
        uint8_t *code;
        code = ((uint8_t *)m_recv.content.ptr);
        /* differentiate packet types */
        ipv6_buf = ipv6_get_buf();
        memcpy(&rpl_buffer, ipv6_buf, NTOHS(ipv6_buf->length) + IPV6_HDR_LEN);
        DEBUGF("Received RPL information of type %04X and length %u\n", *code, NTOHS(ipv6_buf->length));

        switch (*code) {
            case (ICMP_CODE_DIS): {
                recv_rpl_DIS();
                mutex_unlock(&rpl_recv_mutex);
                break;
            }

            case (ICMP_CODE_DIO): {
                recv_rpl_DIO();
                mutex_unlock(&rpl_recv_mutex);
                break;
            }

            case (ICMP_CODE_DAO): {
                recv_rpl_DAO();
                mutex_unlock(&rpl_recv_mutex);
                break;
            }

            case (ICMP_CODE_DAO_ACK): {
                recv_rpl_DAO_ACK();
                mutex_unlock(&rpl_recv_mutex);
                break;
            }

            default:
                mutex_unlock(&rpl_recv_mutex);
                break;
        }
    }
}

/**************************************************************/
/* General RPL-send & -receive functions. Call mode-functions */
/**************************************************************/

void send_DIO(ipv6_addr_t *destination)
{
    DEBUGF("Send DIO to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));

    mutex_lock(&rpl_send_mutex);
    send_DIO_mode(destination);
    mutex_unlock(&rpl_send_mutex);
}

void send_DAO(ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime, uint8_t start_index)
{
    DEBUG("Send DAO to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));

    mutex_lock(&rpl_send_mutex);
    send_DAO_mode(destination, lifetime, default_lifetime, start_index);
    mutex_unlock(&rpl_send_mutex);
}

void send_DIS(ipv6_addr_t *destination)
{
    DEBUGF("Send DIS to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));

    mutex_lock(&rpl_send_mutex);
    send_DIS_mode(destination);
    mutex_unlock(&rpl_send_mutex);
}

void send_DAO_ACK(ipv6_addr_t *destination)
{
    DEBUGF("Send DAO ACK to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));

    mutex_lock(&rpl_send_mutex);
    send_DAO_ACK_mode(destination);
    mutex_unlock(&rpl_send_mutex);
}

void recv_rpl_DIO(void)
{
    DEBUGF("DIO received\n");

    recv_rpl_DIO_mode();
}

void recv_rpl_DAO(void)
{
    DEBUGF("DAO received\n");

    recv_rpl_DAO_mode();
}

void recv_rpl_DIS(void)
{
    DEBUGF("DIS received\n");

    recv_rpl_DIS_mode();

}

void recv_rpl_DAO_ACK(void)
{
    DEBUGF("DAO ACK received\n");

    recv_rpl_dao_ack_mode();
}

/**************************************************************/
/**************************************************************/

/******************************************************************************/
/* Routing related functions are obsolete and will be replaced in near future */
/******************************************************************************/

ipv6_addr_t *rpl_get_next_hop(ipv6_addr_t *addr)
{
    DEBUGF("looking up the next hop to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, addr));
    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (rpl_routing_table[i].used) {
            DEBUGF("checking %d: %s\n", i, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &rpl_routing_table[i].address));
        }
        if (rpl_routing_table[i].used && rpl_equal_id(&rpl_routing_table[i].address, addr)) {
            DEBUGF("found %d: %s\n", i, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &rpl_routing_table[i].next_hop));
            return &rpl_routing_table[i].next_hop;
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

    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
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
    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (rpl_routing_table[i].used && rpl_equal_id(&rpl_routing_table[i].address, addr)) {
            memset(&rpl_routing_table[i], 0, sizeof(rpl_routing_table[i]));
            return;
        }
    }
}

rpl_routing_entry_t *rpl_find_routing_entry(ipv6_addr_t *addr)
{
    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (rpl_routing_table[i].used && rpl_equal_id(&rpl_routing_table[i].address, addr)) {
            return &rpl_routing_table[i];
        }
    }

    return NULL;
}

void rpl_clear_routing_table(void)
{
    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        memset(&rpl_routing_table[i], 0, sizeof(rpl_routing_table[i]));
    }

}

rpl_routing_entry_t *rpl_get_routing_table(void)
{
    return rpl_routing_table;
}

/******************************************************************************/
/******************************************************************************/
