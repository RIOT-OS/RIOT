/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "msg.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/pktdump.h"
#include "timex.h"
#include "utlist.h"
#include "xtimer.h"
#include "optparse.h"

#define SERVER_MSG_QUEUE_SIZE   (8U)
#define SERVER_PRIO             (THREAD_PRIORITY_MAIN - 1)
#define SERVER_STACKSIZE        (THREAD_STACKSIZE_MAIN)
#define SERVER_RESET            (0x8fae)

static gnrc_netreg_entry_t server = GNRC_NETREG_ENTRY_INIT_PID(0, KERNEL_PID_UNDEF);

static char server_stack[SERVER_STACKSIZE];
static msg_t server_queue[SERVER_MSG_QUEUE_SIZE];
static kernel_pid_t server_pid = KERNEL_PID_UNDEF;
static uint8_t send_count = 0;

static void *_eventloop(void *arg)
{
    (void)arg;
    msg_t msg, reply;
    unsigned int rcv_count = 0;

    /* setup the message queue */
    msg_init_queue(server_queue, SERVER_MSG_QUEUE_SIZE);

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                printf("Packets received: %u\n", ++rcv_count);
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            case SERVER_RESET:
                rcv_count = 0;
                break;
            default:
                break;
        }
    }

    /* never reached */
    return NULL;
}

static int parse_portn(union opt_key key, const char *value,
                union opt_data *dest,
                const char **message)
{
    char *end_of_conv;

    (void)key;

    dest->d_uint = value == NULL ? 0 : strtoul(value, &end_of_conv, 0);

    if (value != NULL &&
        (*end_of_conv != '\0' || dest->d_uint <= 0
        || dest->d_uint > UINT16_MAX)) {
        *message = "Port number (p) : p ∈ ℕ ∧ p < 2¹⁶";
        return -OPTPARSE_BADSYNTAX;
    }
    else {
        return -OPTPARSE_OK;
    }
}

typedef struct {
    ipv6_addr_t addr;
    int iface;
} ipv6_addr_if_t;

/* Note: this conversion function expects dest.data to point to a location
 * capable of holding a ipv6_addr_t.
 */
static int parse_ip6addr(union opt_key key, const char *value,
                         union opt_data *dest,
                         const char **message)
{
    ipv6_addr_if_t *addr_if = dest->data;
    (void)key;

    if (value == NULL) {
        return -OPTPARSE_OK;
    }

    addr_if->iface = 0;// ipv6_addr_split_iface(value);

    if (ipv6_addr_from_str(&addr_if->addr, value) == NULL) {
        *message = "Error: unable to parse destination address";
        return -OPTPARSE_BADSYNTAX;
    }

    return -OPTPARSE_OK;
}

int udp_client_send(int argc, const char *argv[])
{
    enum UDP_OPTS {UDP_ADDR, UDP_PORT, UDP_BYTES, UDP_NUM, UDP_DELAY,
                     UDP_HELP, UDP_N_OPTS};

    static const opt_rule_t rules[] = {
        [UDP_ADDR] = OPTPARSE_P(CUSTOM_ACTION, "addr", "Destination addrress", parse_ip6addr),
        [UDP_PORT] = OPTPARSE_P(CUSTOM_ACTION, "port", "Destination port", parse_portn),
        [UDP_BYTES] = OPTPARSE_P_OPT(UINT, "bytes", "Packet size", 0),
        [UDP_NUM] = OPTPARSE_O(UINT, 'n', "num", "Number of packets", 1),
        [UDP_HELP] = OPTPARSE_O(DO_HELP, 'h', "help", "Show this help", 0),
        [UDP_DELAY] = OPTPARSE_O(UINT, 'd', "delay", "delay in us", 1)
    };
    static const opt_conf_t cfg = {.helpstr="Send data over UDP",
                                    .rules=rules,
                                    .n_rules=UDP_N_OPTS,
                                    .tune=OPTPARSE_IGNORE_ARGV0
    };

    ipv6_addr_if_t addr_if;
    opt_data_t args[UDP_N_OPTS];

    args[UDP_ADDR].data = &addr_if;

    if (optparse_cmd(&cfg, args, argc, argv) < 0) {
        return 1;
    }

    if ((addr_if.iface < 0) && (gnrc_netif_numof() == 1)) {
        addr_if.iface = gnrc_netif_iter(NULL)->pid;
    }

    for (unsigned int i = 0; i < args[UDP_NUM].d_uint; i++) {
        gnrc_pktsnip_t *payload, *udp, *ip;
        /* allocate payload */
        payload = gnrc_pktbuf_add(NULL, NULL, args[UDP_BYTES].d_uint,
                                  GNRC_NETTYPE_UNDEF);
        if (payload == NULL) {
            puts("Error: unable to copy data to packet buffer");
            return 2;
        }
        memset(payload->data, send_count++, args[UDP_BYTES].d_uint);
        /* allocate UDP header, set source port := destination port */
        udp = gnrc_udp_hdr_build(payload, args[UDP_PORT].d_uint,
                                          args[UDP_PORT].d_uint);
        if (udp == NULL) {
            puts("Error: unable to allocate UDP header");
            gnrc_pktbuf_release(payload);
            return 2;
        }
        /* allocate IPv6 header */
        ip = gnrc_ipv6_hdr_build(udp, NULL, &addr_if.addr);
        if (ip == NULL) {
            puts("Error: unable to allocate IPv6 header");
            gnrc_pktbuf_release(udp);
            return 2;
        }
        /* add netif header, if interface was given */
        if (addr_if.iface > 0) {
            gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);

            ((gnrc_netif_hdr_t *)netif->data)->if_pid = (kernel_pid_t)addr_if.iface;
            LL_PREPEND(ip, netif);
        }
        /* send packet */
        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
            puts("Error: unable to locate UDP thread");
            gnrc_pktbuf_release(ip);
            return 2;
        }
        /* access to `payload` was implicitly given up with the send operation above
         * => use original variable for output */
        printf("Success: send %u byte to [%s]:%u\n", args[UDP_BYTES].d_uint,
                                                     argv[1], /* oops, hacky!*/
                                                     args[UDP_PORT].d_uint);
        xtimer_usleep(args[UDP_DELAY].d_uint);
    }

    return 0;
}

int udp_server_start(int argc, const char *argv[])
{
    static const opt_rule_t rules[] = {
        OPTPARSE_P(CUSTOM_ACTION, "port", "Listen on this port number", parse_portn)
    };
    static const opt_conf_t cfg = {.helpstr="Listen on UDP ports",
                                    .rules=rules, .n_rules=1,
                                    .tune=OPTPARSE_IGNORE_ARGV0};

    opt_data_t port;

    if (optparse_cmd(&cfg, &port, argc, argv) < 0) {
        return 1;
    }

    /* check if server is already running */
    if (server.target.pid != KERNEL_PID_UNDEF) {
        printf("Error: server already running on port %" PRIu32 "\n",
               server.demux_ctx);
        return 2;
    }

    if (server_pid <= KERNEL_PID_UNDEF) {
        /* start server */
        server_pid = thread_create(server_stack, sizeof(server_stack), SERVER_PRIO,
                                   THREAD_CREATE_STACKTEST, _eventloop, NULL, "UDP server");
        if (server_pid <= KERNEL_PID_UNDEF) {
            puts("Error: can not start server thread");
            return 2;
        }
    }
    /* register server to receive messages from given port */
    gnrc_netreg_entry_init_pid(&server, port.d_uint, server_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &server);
    printf("Success: started UDP server on port %" PRIu16 "\n", port.d_uint);

    return 0;
}

int udp_server_stop(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    msg_t msg = { .type = SERVER_RESET };
    /* check if server is running at all */
    if (server.target.pid == KERNEL_PID_UNDEF) {
        printf("Error: server was not running\n");
        return 1;
    }
    /* reset server state */
    msg_send(&msg, server.target.pid);
    /* stop server */
    gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &server);
    gnrc_netreg_entry_init_pid(&server, 0, KERNEL_PID_UNDEF);
    puts("Success: stopped UDP server");

    return 0;
}

int udp_server_reset(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (server_pid > KERNEL_PID_UNDEF) {
            msg_t msg = { .type = SERVER_RESET };
            msg_send(&msg, server_pid);
            send_count = (uint8_t)0;
    }

    return 0;
}
