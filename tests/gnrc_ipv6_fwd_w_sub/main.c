/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Regression test to test subscribing to IPv6 packets while
 *              forwarding
 *
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "msg.h"
#include "net/ethernet/hdr.h"
#include "net/ipv6/addr.h"
#include "net/udp.h"
#include "net/gnrc.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/netdev_test.h"
#include "od.h"
#include "sched.h"
#include "shell.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "xtimer.h"

#include "common.h"

#define DUMPER_QUEUE_SIZE   (16)
#define NBR_MAC             { 0x57, 0x44, 0x33, 0x22, 0x11, 0x00, }
#define NBR_LINK_LOCAL      { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                              0x55, 0x44, 0x33, 0xff, 0xfe, 0x22, 0x11, 0x00, }
#define DST                 { 0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0xab, 0xcd, \
                              0x55, 0x44, 0x33, 0xff, 0xfe, 0x22, 0x11, 0x00, }
#define DST_PFX_LEN         (64U)
/* IPv6 header + payload:     version+TC  FL: 0       plen: 16    NH:17 HL:64 */
#define L2_PAYLOAD          { 0x60, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x40, \
                              /* source: random address */                    \
                              0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0xef, 0x01, \
                              0x02, 0xca, 0x4b, 0xef, 0xf4, 0xc2, 0xde, 0x01, \
                              /* destination: DST */                          \
                              0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0xab, 0xcd, \
                              0x55, 0x44, 0x33, 0xff, 0xfe, 0x22, 0x11, 0x00, \
                              /* random payload of length 16 */               \
                              0x54, 0xb8, 0x59, 0xaf, 0x3a, 0xb4, 0x5c, 0x85, \
                              0x1e, 0xce, 0xe2, 0xeb, 0x05, 0x4e, 0xa3, 0x85, }

static const uint8_t _nbr_mac[] = NBR_MAC;
static const ipv6_addr_t _nbr_link_local = { .u8 = NBR_LINK_LOCAL };
static const ipv6_addr_t _dst = { .u8 = DST };
static const uint8_t _l2_payload[] = L2_PAYLOAD;
static gnrc_netreg_entry_t _dumper;
static msg_t _dumper_queue[DUMPER_QUEUE_SIZE];
static char _dumper_stack[THREAD_STACKSIZE_MAIN];

static int _run_test(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "run_test", "runs the regression test", _run_test },
    { NULL, NULL, NULL }
};

static void *_dumper_thread(void *arg)
{
    (void)arg;
    msg_init_queue(_dumper_queue, DUMPER_QUEUE_SIZE);

    while (1) {
        msg_t msg;

        msg_receive(&msg);
        if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
            gnrc_pktsnip_t *pkt = msg.content.ptr;

            /* wait a bit to give IPv6 time to handle the packet */
            xtimer_usleep(500);
            /* dump pkt. Should be equal to _l2_payloa*/
            puts("I got a subscription!");
            od_hex_dump(pkt->data, pkt->size, OD_WIDTH_DEFAULT);
            gnrc_pktbuf_release(pkt);
        }
        else if (msg.type == GNRC_NETAPI_MSG_TYPE_SND) {
            /* we are not interested in sent packets from the node itself;
             * just release it */
            gnrc_pktbuf_release(msg.content.ptr);
        }
    }

    return NULL;
}

static int _dump_etherframe(netdev_t *dev, const iolist_t *iolist)
{
    static uint8_t outbuf[sizeof(ethernet_hdr_t) + sizeof(_l2_payload)];
    size_t outbuf_len = 0U;

    (void)dev;
    while (iolist) {
        if ((outbuf_len + iolist->iol_len) > sizeof(outbuf)) {
            printf("Ignoring packet: %u > %u\n",
                  (unsigned)(outbuf_len + iolist->iol_len),
                  (unsigned)sizeof(outbuf));
            /* ignore larger packets */
            return outbuf_len;
        }
        memcpy(&outbuf[outbuf_len], iolist->iol_base, iolist->iol_len);
        outbuf_len += iolist->iol_len;
        iolist = iolist->iol_next;
    }

    puts("Forwarded Ethernet frame:");
    od_hex_dump(outbuf, outbuf_len, OD_WIDTH_DEFAULT);
    return outbuf_len;
}

static gnrc_pktsnip_t *_build_recvd_pkt(void)
{
    gnrc_pktsnip_t *netif;
    gnrc_pktsnip_t *pkt;

    netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    expect(netif);
    gnrc_netif_hdr_set_netif(netif->data, _mock_netif);
    pkt = gnrc_pktbuf_add(netif, _l2_payload, sizeof(_l2_payload),
                          GNRC_NETTYPE_IPV6);
    expect(pkt);
    return pkt;
}

static int _run_test(int argc, char **argv)
{
    int subscribers;
    (void)argc;
    (void)argv;
    if (_dumper.target.pid <= KERNEL_PID_UNDEF) {
        gnrc_netreg_entry_init_pid(&_dumper, GNRC_NETREG_DEMUX_CTX_ALL,
                                   thread_create(_dumper_stack,
                                                 sizeof(_dumper_stack),
                                                 THREAD_PRIORITY_MAIN - 1, 0,
                                                 _dumper_thread, NULL,
                                                 "dumper"));
        expect(_dumper.target.pid > KERNEL_PID_UNDEF);
        /* give dumper thread time to run */
        xtimer_usleep(200);
    }
    /* activate dumping of sent ethernet frames */
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154,
                                              netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _dump_etherframe);
    /* first, test forwarding without subscription */
    subscribers = gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6,
                                               GNRC_NETREG_DEMUX_CTX_ALL,
                                               _build_recvd_pkt());
    /* only IPv6 should be subscribed at the moment */
    expect(subscribers == 1);
    /* subscribe dumper thread for any IPv6 packets */
    gnrc_netreg_register(GNRC_NETTYPE_IPV6, &_dumper);
    /* now test forwarding with subscription */
    subscribers = gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6,
                                               GNRC_NETREG_DEMUX_CTX_ALL,
                                               _build_recvd_pkt());
    /* expect 2 subscribers: IPv6 and gnrc_pktdump as registered above */
    expect(subscribers == 2);
    return 0;
}

int main(void)
{
    int res;

    /* initialize mock interface */
    _tests_init();
    /* define neighbor to forward to */
    res = gnrc_ipv6_nib_nc_set(&_nbr_link_local, _mock_netif->pid,
                               _nbr_mac, sizeof(_nbr_mac));
    expect(res == 0);
    /* set route to neighbor */
    res = gnrc_ipv6_nib_ft_add(&_dst, DST_PFX_LEN, &_nbr_link_local,
                               _mock_netif->pid, 0);
    expect(res == 0);
    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
