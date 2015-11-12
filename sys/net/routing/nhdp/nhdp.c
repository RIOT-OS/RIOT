/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     nhdp
 * @{
 *
 * @file
 * @brief       Implementation of NHDP's core functionality
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "net/gnrc/conn.h"
#include "net/conn/udp.h"
#include "msg.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "thread.h"
#include "utlist.h"
#include "mutex.h"
#include "net/ipv6/addr.h"

#include "rfc5444/rfc5444_writer.h"

#include "lib_table.h"
#include "iib_table.h"
#include "nib_table.h"
#include "nhdp.h"
#include "nhdp_address.h"
#include "nhdp_writer.h"
#include "nhdp_reader.h"

#ifndef MODULE_CONN_UDP
#error "nhdp needs a conn_udp implementation to work"
#endif

char nhdp_stack[NHDP_STACK_SIZE];
char nhdp_rcv_stack[NHDP_STACK_SIZE];

/* Internal variables */
static kernel_pid_t nhdp_pid = KERNEL_PID_UNDEF;
static kernel_pid_t nhdp_rcv_pid = KERNEL_PID_UNDEF;
static kernel_pid_t helper_pid = KERNEL_PID_UNDEF;
static nhdp_if_entry_t nhdp_if_table[GNRC_NETIF_NUMOF];
static mutex_t send_rcv_mutex = MUTEX_INIT;
static conn_udp_t conn;

#if (NHDP_METRIC_NEEDS_TIMER)
static vtimer_t metric_timer;
static timex_t metric_interval;
#endif

/* Internal function prototypes */
static void *_nhdp_runner(void *arg __attribute__((unused)));
static void *_nhdp_receiver(void *arg __attribute__((unused)));
static void write_packet(struct rfc5444_writer *wr __attribute__((unused)),
                         struct rfc5444_writer_target *iface __attribute__((unused)),
                         void *buffer, size_t length);

/*---------------------------------------------------------------------------*
 *                            NHDP Core API                                  *
 *---------------------------------------------------------------------------*/

void nhdp_init(void)
{
    if (nhdp_pid != KERNEL_PID_UNDEF) {
        /* do not initialize twice */
        return;
    }

    /* Prepare interface table */
    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        nhdp_if_table[i].if_pid = KERNEL_PID_UNDEF;
        memset(&nhdp_if_table[i].wr_target, 0, sizeof(struct rfc5444_writer_target));
    }

    /* Initialize reader and writer */
    nhdp_writer_init();
    nhdp_reader_init();
}

kernel_pid_t nhdp_start(void)
{
    if (nhdp_pid == KERNEL_PID_UNDEF) {
        /* Init destination address for NHDP's packets */

        /* Start the NHDP thread */
        nhdp_pid = thread_create(nhdp_stack, sizeof(nhdp_stack), THREAD_PRIORITY_MAIN - 1,
                                 CREATE_STACKTEST, _nhdp_runner, NULL, "NHDP");

#if (NHDP_METRIC_NEEDS_TIMER)
        /* Configure periodic timer message to refresh metric values */
        if (nhdp_pid != KERNEL_PID_UNDEF) {
            metric_interval = timex_from_uint64(DAT_REFRESH_INTERVAL * SEC_IN_USEC);
            vtimer_set_msg(&metric_timer, metric_interval, nhdp_pid, NHDP_METRIC_TIMER, NULL);
        }
#endif
    }

    return nhdp_pid;
}

int nhdp_register_if_default(kernel_pid_t if_pid, uint8_t *addr, size_t addr_size,
                             uint8_t addr_type, uint16_t max_pl_size)
{
    return nhdp_register_if(if_pid, addr, addr_size, addr_type, max_pl_size,
                            NHDP_DEFAULT_HELLO_INT_MS, NHDP_DEFAULT_HOLD_TIME_MS);
}

int nhdp_register_if(kernel_pid_t if_pid, uint8_t *addr, size_t addr_size, uint8_t addr_type,
                     uint16_t max_pl_size, uint16_t hello_int_ms, uint16_t val_time_ms)
{
    nhdp_if_entry_t *if_entry = NULL;
    nhdp_addr_t *nhdp_addr;
    msg_t signal_msg;

    if (nhdp_rcv_pid != KERNEL_PID_UNDEF) {
        return -2;
    }

    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        if (nhdp_if_table[i].if_pid == KERNEL_PID_UNDEF) {
            if_entry = &nhdp_if_table[i];
            break;
        }
    }

    if (!if_entry) {
        /* Maximum number of registerable interfaces reached */
        return -2;
    }

    uint16_t payload_size = max_pl_size > NHDP_MAX_RFC5444_PACKET_SZ
                            ? NHDP_MAX_RFC5444_PACKET_SZ : max_pl_size;
    if_entry->wr_target.packet_buffer = (uint8_t *) calloc(payload_size, sizeof(uint8_t));

    if (!if_entry->wr_target.packet_buffer) {
        /* Insufficient memory */
        return -1;
    }

    if_entry->wr_target.packet_size = payload_size;
    if_entry->wr_target.sendPacket = write_packet;

    /* Get NHDP address entry for the given address */
    nhdp_addr = nhdp_addr_db_get_address(addr, addr_size, addr_type);

    if (!nhdp_addr) {
        /* Insufficient memory */
        free(if_entry->wr_target.packet_buffer);
        return -1;
    }

    /* Add the interface to the LIB */
    if (lib_add_if_addr(if_pid, nhdp_addr) != 0) {
        free(if_entry->wr_target.packet_buffer);
        nhdp_decrement_addr_usage(nhdp_addr);
        return -1;
    }

    /* Create new IIB for the interface */
    if (iib_register_if(if_pid) != 0) {
        /* TODO: Cleanup lib entry */
        free(if_entry->wr_target.packet_buffer);
        nhdp_decrement_addr_usage(nhdp_addr);
        return -1;
    }

    /* Set Interface's PID */
    if_entry->if_pid = if_pid;
    /* Set HELLO_INTERVAL and H_HOLD_TIME (validity time) */
    if_entry->hello_interval.seconds = 0;
    if_entry->hello_interval.microseconds = MS_IN_USEC * hello_int_ms;
    if_entry->validity_time.seconds = 0;
    if_entry->validity_time.microseconds = MS_IN_USEC * val_time_ms;
    timex_normalize(&if_entry->hello_interval);
    timex_normalize(&if_entry->validity_time);
    /* Reset sequence number */
    if_entry->seq_no = 0;

    /* Everything went well */
    nhdp_decrement_addr_usage(nhdp_addr);
    nhdp_writer_register_if(&if_entry->wr_target);
    helper_pid = if_pid;

    /* Start the receiving thread */
    nhdp_rcv_pid = thread_create(nhdp_rcv_stack, sizeof(nhdp_rcv_stack), THREAD_PRIORITY_MAIN - 1,
                                 CREATE_STACKTEST, _nhdp_receiver, NULL, "nhdp_rcv_thread");

    /* Start sending periodic HELLO */
    signal_msg.type = MSG_TIMER;
    signal_msg.content.ptr = (char *) if_entry;
    /* TODO: msg_send or msg_try_send? */
    msg_try_send(&signal_msg, nhdp_pid);

    return 0;
}

int nhdp_register_non_manet_if(kernel_pid_t if_pid, uint8_t *addr, size_t addr_size,
                               uint8_t addr_type)
{
    return nhdp_add_address(if_pid, addr, addr_size, addr_type);
}

int nhdp_add_address(kernel_pid_t if_pid, uint8_t *addr, size_t addr_size, uint8_t addr_type)
{
    int result;

    /* Get NHDP address entry for the given address */
    nhdp_addr_t *nhdp_addr = nhdp_addr_db_get_address(addr, addr_size, addr_type);

    if (!nhdp_addr) {
        /* Insufficient memory */
        return -1;
    }

    result = lib_add_if_addr(if_pid, nhdp_addr);
    nhdp_decrement_addr_usage(nhdp_addr);

    return result;
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

/**
 * Function executed by NHDP thread receiving messages in an endless loop
 */
static void *_nhdp_runner(void *arg)
{
    nhdp_if_entry_t *if_entry;
    msg_t msg_rcvd, msg_queue[NHDP_MSG_QUEUE_SIZE];

    (void)arg;
    msg_init_queue(msg_queue, NHDP_MSG_QUEUE_SIZE);

    while (1) {
        msg_receive(&msg_rcvd);

        switch (msg_rcvd.type) {
            case MSG_TIMER:
                mutex_lock(&send_rcv_mutex);
                if_entry = (nhdp_if_entry_t *) msg_rcvd.content.ptr;

                nhdp_writer_send_hello(if_entry);

                /* TODO: Add jitter */

                /* Schedule next sending */
                vtimer_set_msg(&if_entry->if_timer, if_entry->hello_interval,
                               thread_getpid(), MSG_TIMER, (void *) if_entry);
                mutex_unlock(&send_rcv_mutex);
                break;

#if (NHDP_METRIC_NEEDS_TIMER)
            case NHDP_METRIC_TIMER:
                mutex_lock(&send_rcv_mutex);
                /* Process necessary metric computations */
                iib_process_metric_refresh();

                /* Schedule next sending */
                vtimer_set_msg(&metric_timer, metric_interval,
                               thread_getpid(), NHDP_METRIC_TIMER, NULL);
                mutex_unlock(&send_rcv_mutex);
                break;
#endif
            default:
                break;
        }
    }

    return 0;
}

/**
 * Receive HELLOs over the configured socket and handle them
 */
static void *_nhdp_receiver(void *arg __attribute__((unused)))
{
    uint32_t fromlen;
    char nhdp_rcv_buf[NHDP_MAX_RFC5444_PACKET_SZ];
    msg_t msg_q[NHDP_MSG_QUEUE_SIZE];

    msg_init_queue(msg_q, NHDP_MSG_QUEUE_SIZE);

    /* Configure socket address for the manet port 269 */
    ipv6_addr_t unspec = IPV6_ADDR_UNSPECIFIED;

    /* Bind UDP socket to socket address */
    if (conn_udp_create(&conn, &unspec, sizeof(unspec), AF_INET6, MANET_PORT) == -1) {
        /* Failed creating the connection */
        return 0;
    }

    while (1) {
        ipv6_addr_t rcv_addr;
        uint16_t rcv_port;
        int32_t rcv_size = conn_udp_recvfrom(&conn, (void *)nhdp_rcv_buf,
                                             NHDP_MAX_RFC5444_PACKET_SZ, &rcv_addr,
                                             sizeof(rcv_addr), &rcv_port);

        if (rcv_size > 0) {
            /* Packet received, let the reader handle it */
            mutex_lock(&send_rcv_mutex);
            nhdp_reader_handle_packet(helper_pid, (void *)nhdp_rcv_buf, rcv_size);
            mutex_unlock(&send_rcv_mutex);
        }
    }

    gnrc_udp_close(&conn);
    return 0;
}

/**
 * Send packet for the registered interface
 * Called by oonf_api to send packet over the configured socket
 */
static void write_packet(struct rfc5444_writer *wr __attribute__((unused)),
                         struct rfc5444_writer_target *iface __attribute__((unused)),
                         void *buffer, size_t length)
{
    ipv6_addr_t src, dst = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    uint16_t sport, dport = MANET_PORT;
    conn_udp_getlocaladdr(&conn, &src, &sport);
    conn_udp_sendto(buffer, length, &src, sizeof(src), &dst, sizeof(dst), AF_INET, sport, dport);
}
