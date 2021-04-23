/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Collecting connection information to all nodes in the network
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc/rpl.h"
#include "timex.h"
#include "random.h"
#include "utlist.h"
#include "xtimer.h"

#define SERVER_MSG_QUEUE_SIZE   (8U)
#define SERVER_PRIO             (THREAD_PRIORITY_MAIN - 1)
#define SERVER_STACKSIZE        (THREAD_STACKSIZE_MAIN)

#define SERVER_PORT             (1234)

#define RPL_PREFIX              (0xdeadbeef00000000)
#define RPL_IID                 (0x1)
#define RPL_PREFIX_LEN          (64U)

static char server_stack[SERVER_STACKSIZE];
static msg_t server_queue[SERVER_MSG_QUEUE_SIZE];

static kernel_pid_t server_pid = KERNEL_PID_UNDEF;

static gnrc_netreg_entry_t server = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                               KERNEL_PID_UNDEF);

static mutex_t sender_mutex;
static bool sender_running;

static struct node {
    ipv6_addr_t addr;
    uint16_t count;
    uint8_t hops_left;
    int8_t rssi;
    uint8_t lqi;
} all_nodes[CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF];

static ipv6_addr_t _get_addr(void)
{
    ipv6_addr_t addr;
    addr.u64[0] = byteorder_htonll(RPL_PREFIX);
    addr.u64[1] = byteorder_htonll(RPL_IID);

    return addr;
}

static struct node *_get_entry(const ipv6_addr_t *addr)
{
    for (unsigned i = 0; i < ARRAY_SIZE(all_nodes); ++i) {
        if (all_nodes[i].hops_left == 0) {
            memcpy(&all_nodes[i].addr, addr, sizeof(*addr));
            return &all_nodes[i];
        }

        if (memcmp(&all_nodes[i].addr, addr, sizeof(*addr)) == 0) {
            return &all_nodes[i];
        }
    }

    return NULL;
}

static unsigned _print_stats(void)
{
    printf("node address\t\t\tcount\tTTL\tRSSI\tLQI\n");

    unsigned total = 0;
    unsigned node_count = 0;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    for (unsigned i = 0; i < ARRAY_SIZE(all_nodes); ++i) {
        if (all_nodes[i].hops_left == 0) {
            break;
        }

        node_count++;
        total += all_nodes[i].count;

        ipv6_addr_to_str(addr_str, & all_nodes[i].addr, sizeof(addr_str));

        printf("%s\t%u\t%u\t%d\t%u\n", addr_str, all_nodes[i].count,
                                       all_nodes[i].hops_left,
                                       all_nodes[i].rssi, all_nodes[i].lqi);
    }

    printf("nodes: %u\n", node_count);

    return total;
}

static void _process(gnrc_pktsnip_t *snip)
{
    struct node *n = NULL;

     for (; snip != NULL; snip = snip->next) {
        switch (snip->type) {
        case GNRC_NETTYPE_NETIF:
            {
                gnrc_netif_hdr_t *hdr = snip->data;

                /* store information about last hop */
                if (n) {
                    n->rssi = (n->rssi + hdr->rssi) / 2;
                    n->lqi  = (n->lqi + hdr->lqi) / 2;
                }
            }
            break;
        case GNRC_NETTYPE_IPV6:
            {
                const network_uint64_t prefix = byteorder_htonll(RPL_PREFIX);
                ipv6_hdr_t *hdr = snip->data;

                if (memcmp(&hdr->src, &prefix, sizeof(prefix))) {
                    puts("prefix mismatch");
                    return;
                }

                n = _get_entry(&hdr->src);

                if (n == NULL) {
                    puts("node table full");
                    return;
                }

                n->hops_left = hdr->hl;
            }
            break;
        default:
            break;
        }
    }

    if (n) {
        n->count++;
    }
}

static void *_sendloop(void *arg)
{
    ipv6_addr_t addr = _get_addr();
    const char data[] = "Hello";

    mutex_lock(&sender_mutex);
    sender_running = true;

    /* don't let all nodes send at once */
    xtimer_usleep(random_uint32() >> 12);

    while (sender_running) {
        gnrc_pktsnip_t *payload = gnrc_pktbuf_add(NULL, data, sizeof(data), GNRC_NETTYPE_UNDEF);
        gnrc_pktsnip_t *udp = gnrc_udp_hdr_build(payload, SERVER_PORT, SERVER_PORT);
        gnrc_pktsnip_t *ip = gnrc_ipv6_hdr_build(udp, NULL, &addr);

        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
            puts("Error: unable to locate UDP thread");
            gnrc_pktbuf_release(ip);
        }

        xtimer_sleep(1);
    }

    mutex_unlock(&sender_mutex);

    return arg;
}

static void *_eventloop(void *arg)
{
    (void)arg;
    msg_t msg, reply;

    /* setup the message queue */
    msg_init_queue(server_queue, SERVER_MSG_QUEUE_SIZE);

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                _process(msg.content.ptr);
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            default:
                break;
        }
    }

    /* never reached */
    return NULL;
}

static void _add_addr(void)
{
    gnrc_netif_t *iface = gnrc_netif_iter(NULL);
    ipv6_addr_t addr = _get_addr();
    uint16_t flags = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID
                   | (RPL_PREFIX_LEN << 8);
    netif_set_opt(&iface->netif, NETOPT_IPV6_ADDR, flags, &addr, sizeof(addr));
}

static void _rpl_root(void)
{
    ipv6_addr_t dodag_id = _get_addr();
    gnrc_rpl_root_init(1, &dodag_id, false, false);
}

static void start_server(void)
{
    if (server_pid != KERNEL_PID_UNDEF) {
        return;
    }

    sender_running = false;
    mutex_lock(&sender_mutex);

    _add_addr();
    _rpl_root();

    /* start server */
    server_pid = thread_create(server_stack, sizeof(server_stack), SERVER_PRIO,
                               THREAD_CREATE_STACKTEST, _eventloop, NULL, "stats server");

    /* register server to receive messages from given port */
    gnrc_netreg_entry_init_pid(&server, SERVER_PORT, server_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &server);
    printf("Success: started UDP server on port %" PRIu16 "\n", SERVER_PORT);
}

void start_sender(void)
{
    /* start server */
    thread_create(server_stack, sizeof(server_stack), SERVER_PRIO,
                  THREAD_CREATE_STACKTEST, _sendloop, NULL, "probe sender");
}

int stats_cmd(int argc, char **argv)
{
    static unsigned start_time;

    if (argc < 2) {
        printf("usage: %s [start|info]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "start") == 0) {
        start_server();
        start_time = xtimer_now_usec64() / 1000000UL;
    }
    else if (strcmp(argv[1], "info") == 0) {
        unsigned total = _print_stats();
        unsigned diff  = (xtimer_now_usec64() / 1000000UL) - start_time;
        printf("running for %u s\n", diff);
        printf("%u received (%u / s)\n", total, total / diff);
    }
    else {
        puts("error: invalid command");
    }
    return 0;
}
