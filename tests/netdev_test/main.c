/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Show case application for netdev_test
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "net/ethernet.h"
#include "net/gnrc.h"
#include "net/gnrc/netif/ethernet.h"
#include "net/netdev_test.h"
#include "od.h"
#include "thread.h"
#include "utlist.h"

#define _EXP_LENGTH     (64)

#define _MAC_STACKSIZE  (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#define _MAC_PRIO       (THREAD_PRIORITY_MAIN - 4)

#define _MAIN_MSG_QUEUE_SIZE (2)

#define _TEST_PAYLOAD1  "gO3Xt,fP)6* MR161Auk?W^mTb\"LmY^Qc5w1h:C<+n(*/@4k("
#define _TEST_PAYLOAD2  "*b/'XKkraEBexaU\\O-X&<Bl'n%35Ll+nDy,jQ+[Oe4:9( 4cI"

#define EXECUTE(test) \
    puts("Executing " # test "()"); \
    if (!test()) { \
        puts(" + failed."); \
        return 1; \
    } \
    else { \
        puts(" + succeeded."); \
    }

static uint8_t _dev_addr[] = { 0x6c, 0x5d, 0xff, 0x73, 0x84, 0x6f };
static const uint8_t _test_dst[] = { 0xf5, 0x19, 0x9a, 0x1d, 0xd8, 0x8f };
static const uint8_t _test_src[] = { 0x41, 0x9b, 0x9f, 0x56, 0x36, 0x46 };

static char _mac_stack[_MAC_STACKSIZE];
static netdev_test_t _dev;
static msg_t _main_msg_queue[_MAIN_MSG_QUEUE_SIZE];
static uint8_t _tmp[_EXP_LENGTH];
static kernel_pid_t _mac_pid;
static uint8_t _tmp_len = 0;

static void _dev_isr(netdev_t *dev);
static int _dev_recv(netdev_t *dev, char *buf, int len, void *info);
static int _dev_send(netdev_t *dev, const iolist_t *iolist);
static int _dev_get_addr(netdev_t *dev, void *value, size_t max_len);
static int _dev_set_addr(netdev_t *dev, const void *value, size_t max_len);

/* tests getter */
static int test_get_addr(void)
{
    uint8_t tmp[sizeof(_dev_addr)];

    if (gnrc_netapi_get(_mac_pid, NETOPT_ADDRESS, 0, tmp, sizeof(tmp)) != sizeof(_dev_addr)) {
        puts("Error getting device address");
        return 0;
    }
    else if (memcmp(tmp, _dev_addr, sizeof(_dev_addr)) != 0) {
        puts("Got wrong device address");
        return 0;
    }
    return 1;
}

/* tests sending */
static int test_send(void)
{
    ethernet_hdr_t *exp_mac = (ethernet_hdr_t *)_tmp;
    uint8_t *exp_payload = _tmp + sizeof(ethernet_hdr_t);
    gnrc_pktsnip_t *pkt, *hdr;
    msg_t msg;

    /* prepare packet for sending */
    pkt = gnrc_pktbuf_add(NULL, _TEST_PAYLOAD1, sizeof(_TEST_PAYLOAD1) - 1,
                          GNRC_NETTYPE_UNDEF);
    if (pkt == NULL) {
        puts("Could not allocate send payload");
        return 0;
    }
    hdr = gnrc_netif_hdr_build(NULL, 0, (uint8_t *)_test_dst, sizeof(_test_dst));
    if (hdr == NULL) {
        gnrc_pktbuf_release(pkt);
        puts("Could not allocate send header");
        return 0;
    }
    LL_PREPEND(pkt, hdr);
    /* prepare expected data */
    memcpy(exp_mac->dst, _test_dst, sizeof(_test_dst));
    memcpy(exp_mac->src, _dev_addr, sizeof(_dev_addr));
    exp_mac->type = byteorder_htons(ETHERTYPE_UNKNOWN);
    memcpy(exp_payload, _TEST_PAYLOAD1, sizeof(_TEST_PAYLOAD1) - 1);
    _tmp_len = sizeof(_TEST_PAYLOAD1) + sizeof(ethernet_hdr_t) - 1;
    /* register for returned packet status */
    if (gnrc_neterr_reg(pkt) != 0) {
        puts("Can not register for error reporting");
        return 0;
    }
    /* send packet to MAC layer */
    gnrc_netapi_send(_mac_pid, pkt);
    /* wait for packet status and check */
    msg_receive(&msg);
    if ((msg.type != GNRC_NETERR_MSG_TYPE) ||
        (msg.content.value != GNRC_NETERR_SUCCESS)) {
        puts("Error sending packet");
        return 0;
    }
    return 1;
}

/* tests receiving */
static int test_receive(void)
{
    ethernet_hdr_t *rcv_mac = (ethernet_hdr_t *)_tmp;
    uint8_t *rcv_payload = _tmp + sizeof(ethernet_hdr_t);
    gnrc_pktsnip_t *pkt, *hdr;
    gnrc_netreg_entry_t me = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                        sched_active_pid);
    msg_t msg;

    if (_dev.netdev.event_callback == NULL) {
        puts("Device's event_callback not set");
        return 0;
    }
    /* prepare receive buffer */
    memcpy(rcv_mac->dst, _dev_addr, sizeof(_dev_addr));
    memcpy(rcv_mac->src, _test_src, sizeof(_test_src));
    /* no gnrc_ipv6 in compile unit => ETHERTYPE_IPV6 translates to
     * GNRC_NETTYPE_UNDEF */
    rcv_mac->type = byteorder_htons(ETHERTYPE_IPV6);
    memcpy(rcv_payload, _TEST_PAYLOAD2, sizeof(_TEST_PAYLOAD2) - 1);
    _tmp_len = sizeof(_TEST_PAYLOAD2) + sizeof(ethernet_hdr_t) - 1;

    /* register for GNRC_NETTYPE_UNDEF */
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &me);
    /* fire ISR event */
    _dev.netdev.event_callback((netdev_t *)&_dev.netdev, NETDEV_EVENT_ISR);
    /* wait for packet from MAC layer*/
    msg_receive(&msg);
    /* check message */
    if (msg.sender_pid != _mac_pid) {
        puts("Unexpected sender of netapi receive message");
        return 0;
    }
    if (msg.type != GNRC_NETAPI_MSG_TYPE_RCV) {
        puts("Expected netapi receive message");
        return 0;
    }
    pkt = msg.content.ptr;
    /* check payload */
    if (pkt->size != _tmp_len - sizeof(ethernet_hdr_t)) {
        puts("Payload of unexpected size");
    }
    if ((pkt->type != GNRC_NETTYPE_UNDEF) ||
        (memcmp(pkt->data, _TEST_PAYLOAD2, pkt->size) != 0)) {
        puts("Unexpected payload");
        puts("===========================================================");
        puts("expected");
        puts("===========================================================");
        od_hex_dump(_TEST_PAYLOAD2, pkt->size, OD_WIDTH_DEFAULT);
        puts("===========================================================");
        puts("send data");
        puts("===========================================================");
        od_hex_dump(pkt->data, pkt->size, OD_WIDTH_DEFAULT);
        return 0;
    }
    hdr = pkt->next;
    /* check netif header */
    if ((hdr->type != GNRC_NETTYPE_NETIF) || (hdr->next != NULL) ||
        (hdr->size) != (sizeof(gnrc_netif_hdr_t) + (2 * ETHERNET_ADDR_LEN))) {
        puts("Malformed header received");
        return 0;
    }
    if (memcmp(gnrc_netif_hdr_get_src_addr(hdr->data), _test_src,
               ETHERNET_ADDR_LEN) != 0) {
        char addr_str[ETHERNET_ADDR_LEN * 3];
        puts("Unexpected source received");
        puts("=================");
        puts("expected");
        puts("=================");
        puts(gnrc_netif_addr_to_str(_test_src, ETHERNET_ADDR_LEN, addr_str));
        puts("=================");
        puts("received source");
        puts("=================");
        puts(gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(hdr->data),
                                    ETHERNET_ADDR_LEN, addr_str));
        return 0;
    }
    if (memcmp(gnrc_netif_hdr_get_dst_addr(hdr->data), _dev_addr,
               ETHERNET_ADDR_LEN) != 0) {
        char addr_str[ETHERNET_ADDR_LEN * 3];
        puts("Unexpected destination received");
        puts("=================");
        puts("expected");
        puts("=================");
        puts(gnrc_netif_addr_to_str(_dev_addr, ETHERNET_ADDR_LEN, addr_str));
        puts("====================");
        puts("received destination");
        puts("====================");
        puts(gnrc_netif_addr_to_str(gnrc_netif_hdr_get_dst_addr(hdr->data),
                                    ETHERNET_ADDR_LEN, addr_str));
        return 0;
    }

    gnrc_pktbuf_release(pkt);
    gnrc_netreg_unregister(GNRC_NETTYPE_UNDEF, &me);
    return 1;
}

static int test_set_addr(void)
{
    static const uint8_t new_addr[] = { 0x71, 0x29, 0x5b, 0xc8, 0x52, 0x65 };
    uint8_t tmp[sizeof(new_addr)];

    if (gnrc_netapi_set(_mac_pid, NETOPT_ADDRESS, 0, (void *)new_addr,
                        sizeof(new_addr)) != sizeof(new_addr)) {
        puts("Error setting device address");
        return 0;
    }
    if (gnrc_netapi_get(_mac_pid, NETOPT_ADDRESS, 0, tmp,
                        sizeof(tmp)) != sizeof(tmp)) {
        puts("Error setting device address");
        return 0;
    }
    else if (memcmp(tmp, new_addr, sizeof(new_addr)) != 0) {
        puts("Set to wrong device address");
        return 0;
    }
    return 1;
}

int main(void)
{
    /* initialization */
    gnrc_pktbuf_init();
    msg_init_queue(_main_msg_queue, _MAIN_MSG_QUEUE_SIZE);
    netdev_test_setup(&_dev, NULL);
    netdev_test_set_isr_cb(&_dev, _dev_isr);
    netdev_test_set_recv_cb(&_dev, _dev_recv);
    netdev_test_set_send_cb(&_dev, _dev_send);
    netdev_test_set_get_cb(&_dev, NETOPT_ADDRESS, _dev_get_addr);
    netdev_test_set_set_cb(&_dev, NETOPT_ADDRESS, _dev_set_addr);
    _mac_pid = gnrc_netif_ethernet_create(_mac_stack, _MAC_STACKSIZE, _MAC_PRIO,
                                          "netdev_test", (netdev_t *)&_dev)->pid;

    /* test execution */
    EXECUTE(test_get_addr);
    EXECUTE(test_send);
    EXECUTE(test_receive);
    EXECUTE(test_set_addr);
    puts("ALL TESTS SUCCESSFUL");

    return 0;
}

/* netdev_test callbacks */
static void _dev_isr(netdev_t *dev)
{
    (void)dev;
    if (dev->event_callback) {
        dev->event_callback(dev, NETDEV_EVENT_RX_COMPLETE);
    }
}

static int _dev_recv(netdev_t *dev, char *buf, int len, void *info)
{
    (void)dev;
    (void)info;
    if (buf == NULL) {
        return _tmp_len;
    }
    else if (len < _tmp_len) {
        return -ENOBUFS;
    }
    else {
        memcpy(buf, _tmp, _tmp_len);
        return _tmp_len;
    }
}

static int _dev_send(netdev_t *dev, const iolist_t *iolist)
{
    int idx = 0;

    (void)dev;
    /* check packet content with expected data */
    for (; iolist; iolist = iolist->iol_next) {
        if (memcmp(&(_tmp[idx]), iolist->iol_base, iolist->iol_len) != 0) {
            puts("Unexpected send data:");
            puts("===========================================================");
            puts("expected");
            puts("===========================================================");
            od_hex_dump(&_tmp[idx], iolist->iol_len, OD_WIDTH_DEFAULT);
            puts("===========================================================");
            puts("send data");
            puts("===========================================================");
            od_hex_dump(iolist->iol_base, iolist->iol_len, OD_WIDTH_DEFAULT);
            return -EINVAL;
        }
        idx += iolist->iol_len;
    }
    if (idx != _tmp_len) {
        printf("Unexpected send length: %d (expected: %d)\n", idx, _tmp_len);
        return -EINVAL;
    }

    return idx;
}

static int _dev_get_addr(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    if (max_len < sizeof(_dev_addr)) {
        return -ENOBUFS;
    }
    memcpy(value, _dev_addr, sizeof(_dev_addr));
    return sizeof(_dev_addr);
}

static int _dev_set_addr(netdev_t *dev, const void *value, size_t value_len)
{
    (void)dev;
    if (value_len != sizeof(_dev_addr)) {
        return -EOVERFLOW;
    }
    memcpy(_dev_addr, value, sizeof(_dev_addr));
    return sizeof(_dev_addr);
}
