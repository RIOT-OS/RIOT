/**
 * 6lowpan border router implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowborder.c
 * @brief   constraint node implementation for a 6lowpan border router
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

/* TODO: Put this in its own module */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "mutex.h"
#include "thread.h"
#include "msg.h"
#include "posix_io.h"
#include "board_uart0.h"

#include "ieee802154_frame.h"
#include "sixlowpan/error.h"
#include "bordermultiplex.h"
#include "flowcontrol.h"
#include "border.h"
#include "ip.h"
#include "icmp.h"
#include "serialnumber.h"

#include "net_help.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define READER_STACK_SIZE   (KERNEL_CONF_STACKSIZE_DEFAULT)

char serial_reader_stack[READER_STACK_SIZE];
kernel_pid_t serial_reader_pid = KERNEL_PID_UNDEF;

uint8_t serial_out_buf[BORDER_BUFFER_SIZE];
uint8_t serial_in_buf[BORDER_BUFFER_SIZE];

ipv6_addr_t *abr_addr;

uint8_t *get_serial_out_buffer(int offset)
{
    if (offset > BUFFER_SIZE) {
        return NULL;
    }

    return &(serial_out_buf[offset]);
}

uint8_t *get_serial_in_buffer(int offset)
{
    if (offset > BUFFER_SIZE) {
        return NULL;
    }

    return &(serial_in_buf[offset]);
}

kernel_pid_t border_get_serial_reader()
{
    return serial_reader_pid;
}

void serial_reader_f(void)
{
    kernel_pid_t main_pid;
    msg_t m;
    border_packet_t *uart_buf;

    msg_receive(&m);
    main_pid = m.sender_pid;

    while (1) {
        int bytes = readpacket(get_serial_in_buffer(0), BORDER_BUFFER_SIZE);

        if (bytes < 0) {
            switch (bytes) {
                case (-SIXLOWERROR_ARRAYFULL): {
                    printf("ERROR: Array was full\n");
                    break;
                }

                default: {
                    printf("ERROR: unknown\n");
                    break;
                }
            }

            continue;
        }

        uart_buf = (border_packet_t *)get_serial_in_buffer(0);

        if (uart_buf->empty == 0) {
            if (uart_buf->type == BORDER_PACKET_CONF_TYPE) {
                border_conf_header_t *conf_packet = (border_conf_header_t *)uart_buf;

                if (conf_packet->conftype == BORDER_CONF_SYN) {
                    m.content.ptr = (char *)conf_packet;
                    msg_send(&m, main_pid, 1);
                    continue;
                }
            }

            flowcontrol_deliver_from_uart(uart_buf, bytes);
        }
    }
}

int sixlowpan_lowpan_border_init(int if_id)
{
    ipv6_net_if_addr_t *addr = NULL;
    uint8_t abr_addr_initialized = 0;

    serial_reader_pid = thread_create(
                            serial_reader_stack, READER_STACK_SIZE,
                            PRIORITY_MAIN - 1, CREATE_STACKTEST,
                            serial_reader_f, "serial_reader");
    ip_process_pid = thread_create(ip_process_buf, IP_PROCESS_STACKSIZE,
                                   PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                   border_process_lowpan,
                                   "border_process_lowpan");

    if (ip_process_pid != KERNEL_PID_UNDEF) {
        return 0;
    }

    if (!sixlowpan_lowpan_init_interface(if_id)) {
        return 0;
    }

    while (net_if_iter_addresses(if_id, (net_if_addr_t **) &addr)) {
        if (!ipv6_addr_is_multicast(addr->addr_data) &&
            !ipv6_addr_is_link_local(addr->addr_data) &&
            !ipv6_addr_is_loopback(addr->addr_data) &&
            !ipv6_addr_is_unique_local_unicast(addr->addr_data)) {
            abr_addr_initialized = 1;
            abr_addr = addr->addr_data;
            break;
        }
    }

    if (!abr_addr_initialized) {
        DEBUG("sixlowpan_lowpan_border_init(): A prefix must be initialized to"
              "interface %d first", if_id);
        return 0;
    }

    ipv6_init_as_router();

    return 1;
}

void border_process_lowpan(void)
{
    msg_t m;

    while (1) {
        msg_receive(&m);
        ipv6_hdr_t *ipv6_buf = (ipv6_hdr_t *)m.content.ptr;

        if (ipv6_buf->nextheader == IPV6_PROTO_NUM_ICMPV6) {
            icmpv6_hdr_t *icmp_buf = (icmpv6_hdr_t *)(((uint8_t *)ipv6_buf) + IPV6_HDR_LEN);

            if (icmp_buf->type == ICMPV6_TYPE_REDIRECT) {
                continue;
            }

            if (icmpv6_demultiplex(icmp_buf) == 0) {
                continue;
            }

            /* Here, other ICMPv6 message types for ND may follow. */
        }

        /* TODO: Bei ICMPv6-Paketen entsprechende LoWPAN-Optionen verarbeiten und entfernen */
        multiplex_send_ipv6_over_uart(ipv6_buf);
    }
}
