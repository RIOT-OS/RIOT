/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Text application for gnrc_tx_sync
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

/* keep include of stdint.h before stdatomic.h for compatibility with broken
 * toolchains */
#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>

#include "net/af.h"
#include "net/gnrc/netif/raw.h"
#include "net/ipv6/addr.h"
#include "net/netdev_test.h"
#include "net/sock/udp.h"
#include "test_utils/expect.h"
#include "xtimer.h"

#define NETIF_STACKSIZE     THREAD_STACKSIZE_DEFAULT
#define NETIF_PRIO          (THREAD_PRIORITY_MAIN - 4)
#define MAIN_QUEUE_SIZE     (8)

static char netif_stack[NETIF_STACKSIZE];
static msg_t main_msg_queue[MAIN_QUEUE_SIZE];

static atomic_int send_completed = ATOMIC_VAR_INIT(0);
static gnrc_netif_t netif;
static netdev_test_t netdev;

static int netdev_send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev; (void)iolist;
    xtimer_msleep(100);
    atomic_store(&send_completed, 1);
    return 0;
}

static int netdev_get_device_type(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev; (void)max_len;
    const uint16_t type = NETDEV_TYPE_TEST;
    memcpy(value, &type, sizeof(type));
    return sizeof(type);
}

int main(void)
{
    msg_init_queue(main_msg_queue, MAIN_QUEUE_SIZE);
    netdev_test_setup(&netdev, NULL);
    netdev_test_set_send_cb(&netdev, netdev_send);
    netdev_test_set_get_cb(&netdev, NETOPT_DEVICE_TYPE, netdev_get_device_type);
    gnrc_netif_raw_create(&netif, netif_stack, sizeof(netif_stack), NETIF_PRIO,
                          "netdev_test", &netdev.netdev);

    sock_udp_t sock;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = { .family = AF_INET6 };
    remote.port = 12345;
    ipv6_addr_set_all_nodes_multicast((ipv6_addr_t *)&remote.addr.ipv6,
                                      IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
    expect(sock_udp_create(&sock, &local, NULL, 0) == 0);
    expect(sock_udp_send(&sock, "Test", sizeof("Test"), &remote) > 0);
    expect(atomic_load(&send_completed));

    puts("TEST PASSED");

    return 0;
}
