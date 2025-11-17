/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "net/gnrc/pkt.h"
#include "test_utils/expect.h"
#include "xtimer.h"

#define NETIF_STACKSIZE     THREAD_STACKSIZE_DEFAULT
#define NETIF_PRIO          (THREAD_PRIORITY_MAIN - 4)
#define MAIN_QUEUE_SIZE     (8)

static char netif_stack[NETIF_STACKSIZE];
static msg_t main_msg_queue[MAIN_QUEUE_SIZE];

static atomic_int sends_completed = ATOMIC_VAR_INIT(0);
static gnrc_netif_t netif;
static netdev_test_t netdev_test;
static netdev_t *netdev = &netdev_test.netdev.netdev;

/* With 6LoWPAN, This test message needs exactly two fragments to be transmitted
 * due to the maximum L2 PDU of 96 bytes */
static const char test_msg[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU"
                               "VWXYZ0123456789.,:;!?@#$%^&*()[]{}-_=+/<>`~\'\""
                               "\\";
static unsigned pld_pos = 0;

static bool carries_test_message(const iolist_t *iol)
{
    /* Dark magic: We just assume that the test message will be placed at the
     * end of one single iolist chunk and that fragments (if applicable) are
     * send in order. We also assume that no more than four bytes in a row will
     * never match the test message payload by chance.
     *
     * Note that this intentionally ignores retransmissions, which can occur in
     * the SFR case.
     */

    while (iol) {
        if ((iol->iol_base != NULL) && (iol->iol_len > 0)) {
            const char *pos = iol->iol_base;
            const char *end = pos + iol->iol_len;
            while (pos < end) {
                if (*pos == test_msg[pld_pos]) {
                    size_t len = (size_t)end - (size_t)pos;
                    if ((len > 4) &&
                            (pld_pos + len <= sizeof(test_msg)) &&
                            !memcmp(pos, &test_msg[pld_pos], len)) {
                        /* data matches next chunk of test message */
                        pld_pos += len;
                        return true;
                    }
                }
                pos++;
            }
        }
        iol = iol->iol_next;
    }
    return false;
}

static int netdev_send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev;
    if (carries_test_message(iolist)) {
        /* sending part of UDP datagram */
        xtimer_msleep(100);
        atomic_fetch_add(&sends_completed, 1);
    }
    return iolist_size(iolist);
}

static int netdev_get_device_type(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    const uint16_t type_ipv6 = NETDEV_TYPE_ETHERNET;
    const uint16_t type_6lo = NETDEV_TYPE_IEEE802154;
    expect(max_len == sizeof(uint16_t));
    if (IS_USED(MODULE_NETDEV_IEEE802154)) {
        memcpy(value, &type_6lo, sizeof(type_6lo));
    }
    else {
        memcpy(value, &type_ipv6, sizeof(type_ipv6));
    }
    return sizeof(uint16_t);
}

static int netdev_get_max_pdu_size(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    const uint16_t pdu_size_ethernet = 1500;
    const uint16_t pdu_size_6lo = 96;
    expect(max_len == sizeof(uint16_t));
    if (IS_USED(MODULE_NETDEV_IEEE802154)) {
        memcpy(value, &pdu_size_6lo, sizeof(pdu_size_6lo));
    }
    else {
        memcpy(value, &pdu_size_ethernet, sizeof(pdu_size_ethernet));
    }
    return sizeof(uint16_t);
}

static int netdev_get_proto(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    const gnrc_nettype_t proto =
#if IS_USED(MODULE_NETDEV_IEEE802154)
        GNRC_NETTYPE_SIXLOWPAN;
#else
        GNRC_NETTYPE_IPV6;
#endif
    expect(max_len == sizeof(proto));
    memcpy(value, &proto, sizeof(proto));
    return sizeof(proto);
}

static int netdev_get_address(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    const uint8_t addr[] = {
#if IS_USED(MODULE_NETDEV_IEEE802154)
        0x13, 0x37
#else
        0x13, 0x37, 0xac, 0xdc, 0xbe, 0xef
#endif
    };
    expect(max_len >= sizeof(addr));
    memcpy(value, addr, sizeof(addr));
    return sizeof(addr);
}

static int netdev_get_address_long(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    const uint8_t addr[] = {0x13, 0x37, 0x13, 0x37, 0x13, 0x37, 0x13, 0x37};
    if (!IS_USED(MODULE_NETDEV_IEEE802154)) {
        return -ENOTSUP;
    }
    expect(max_len >= sizeof(addr));
    memcpy(value, addr, sizeof(addr));
    return sizeof(addr);
}

static int netdev_get_src_len(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    const uint16_t src_len = 2;
    if (!IS_USED(MODULE_NETDEV_IEEE802154)) {
        return -ENOTSUP;
    }
    expect(max_len == sizeof(src_len));
    memcpy(value, &src_len, sizeof(src_len));
    return sizeof(src_len);
}

int main(void)
{
    puts(
        "Test application for gnrc_tx_sync\n"
        "=================================\n"
        "\n"
        "This application sends a single UDP datagram over a virtual Ethernet\n"
        "or 802.15.4 device. (Set TEST_6LO to 1 for 802.15.4 and to 0 for\n"
        "Ethernet.) The simulated network devices blocks for 100ms - more than\n"
        "enough time for sock_upd_send() to return if no synchronization is done.\n"
        "If an 802.15.4 device is simulated, the test datagram will be transmitted\n"
        "in two fragments. In that case sock_udp_send() must only return after both\n"
        "fragments are out. In the Ethernet case, only a single frame is send.\n"
    );

    if (IS_USED(MODULE_NETDEV_IEEE802154)) {
        printf("IEEE 802.15.4 mode (TEST_6LO=1, TEST_FRAG_SFR=%u), "
               "sending 2 6LoWPAN fragments",
               (unsigned)IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR));
    }
    else {
        puts("Ethernet mode (TEST_6LO=0), sending 1 IPv6 packet");
    }
    msg_init_queue(main_msg_queue, MAIN_QUEUE_SIZE);
    netdev_test_setup(&netdev_test, NULL);
    netdev_test_set_send_cb(&netdev_test, netdev_send);
    netdev_test_set_get_cb(&netdev_test, NETOPT_DEVICE_TYPE, netdev_get_device_type);
    netdev_test_set_get_cb(&netdev_test, NETOPT_MAX_PDU_SIZE, netdev_get_max_pdu_size);
    netdev_test_set_get_cb(&netdev_test, NETOPT_PROTO, netdev_get_proto);
    netdev_test_set_get_cb(&netdev_test, NETOPT_ADDRESS, netdev_get_address);
    netdev_test_set_get_cb(&netdev_test, NETOPT_ADDRESS_LONG, netdev_get_address_long);
    netdev_test_set_get_cb(&netdev_test, NETOPT_SRC_LEN, netdev_get_src_len);
    gnrc_netif_raw_create(&netif, netif_stack, sizeof(netif_stack), NETIF_PRIO,
                          "netdev_test", netdev);

    sock_udp_t sock;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = { .family = AF_INET6 };
    remote.port = 12345;
    ipv6_addr_set_all_nodes_multicast((ipv6_addr_t *)&remote.addr.ipv6,
                                      IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
    expect(sock_udp_create(&sock, &local, NULL, 0) == 0);
    /* with gnrc_tx_sync, we expect sock_udp_send() to block until transmission is done */
    expect(sock_udp_send(&sock, test_msg, sizeof(test_msg), &remote) > 0);
    /* the virtual netdev device increments sends_completed for each frame carrying the test
     * payload. If this value has not reached the value of 1 (or 2 for 6LoWPAN fragmentation),
     * the test has failed. */
    int sends = atomic_load(&sends_completed);
    int sends_expected = (IS_USED(MODULE_NETDEV_IEEE802154)) ? 2 : 1;
    printf("transmissions expected = %d, transmissions completed = %d\n",
           sends_expected, sends);
    printf("sent %u out of %" PRIuSIZE " bytes\n", pld_pos, sizeof(test_msg));
    expect(sends == sends_expected);
    expect(pld_pos == sizeof(test_msg));

    puts("TEST PASSED");

    return 0;
}
