/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @}
 */

#include "msg.h"
#include "net/ethernet.h"
#include "net/ipv6.h"
#include "net/l2util.h"
#include "net/netdev/eth.h"
#include "net/netdev_test.h"
#include "net/sock.h"
#include "net/udp.h"
#include "sched.h"
#include "test_utils/expect.h"
#include "ztimer.h"
#include "timex.h"

#include "lwip.h"
#include "lwip/ip4.h"
#include "lwip/inet_chksum.h"
#include "lwip/nd6.h"
#include "lwip/priv/nd6_priv.h"
#include "lwip/netif/compat.h"
#include "lwip/netif/netdev.h"
#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"

#include "constants.h"
#include "stack.h"

#define _MSG_QUEUE_SIZE         (4)
#define _SEND_DONE              (0x92d7)
#define _NETDEV_BUFFER_SIZE     (128)

static msg_t _msg_queue[_MSG_QUEUE_SIZE];
static uint8_t _netdev_buffer[_NETDEV_BUFFER_SIZE];
netdev_test_t netdev;
static lwip_netif_t lwip_netif;
static kernel_pid_t _check_pid = KERNEL_PID_UNDEF;
static mutex_t _netdev_buffer_mutex = MUTEX_INIT;
static uint8_t _netdev_buffer_size;

static int _get_max_pkt_size(netdev_t *dev, void *value, size_t max_len)
{
    return netdev_eth_get(dev, NETOPT_MAX_PDU_SIZE, value, max_len);
}

static int _get_src_len(netdev_t *dev, void *value, size_t max_len)
{
    uint16_t *v = value;

    (void)dev;
    if (max_len != sizeof(uint16_t)) {
        return -EOVERFLOW;
    }

    *v = sizeof(uint64_t);

    return sizeof(uint16_t);
}

static int _get_addr(netdev_t *dev, void *value, size_t max_len)
{
    static const uint8_t _local_ip[] = _TEST_ADDR6_LOCAL;

    (void)dev;
    expect(max_len >= ETHERNET_ADDR_LEN);
    return l2util_ipv6_iid_to_addr(NETDEV_TYPE_ETHERNET,
                                   (eui64_t *)&_local_ip[8],
                                   value);
}

static int _get_addr_len(netdev_t *dev, void *value, size_t max_len)
{
    return netdev_eth_get(dev, NETOPT_ADDR_LEN, value, max_len);
}

static int _get_device_type(netdev_t *dev, void *value, size_t max_len)
{
    return netdev_eth_get(dev, NETOPT_DEVICE_TYPE, value, max_len);
}

static void _netdev_isr(netdev_t *dev)
{
    dev->event_callback(dev, NETDEV_EVENT_RX_COMPLETE);
}

static int _netdev_recv(netdev_t *dev, char *buf, int len, void *info)
{
    int res;

    (void)dev;
    (void)info;
    mutex_lock(&_netdev_buffer_mutex);
    if (buf != NULL) {
        if ((unsigned)len < _netdev_buffer_size) {
            mutex_unlock(&_netdev_buffer_mutex);
            return -ENOBUFS;
        }
        memcpy(buf, _netdev_buffer, _netdev_buffer_size);
    }
    res = _netdev_buffer_size;
    mutex_unlock(&_netdev_buffer_mutex);
    return res;
}

static int _netdev_send(netdev_t *dev, const iolist_t *iolist)
{
    msg_t done = { .type = _SEND_DONE };
    unsigned offset = 0;

    (void)dev;
    mutex_lock(&_netdev_buffer_mutex);
    for (; iolist; iolist = iolist->iol_next) {
        memcpy(&_netdev_buffer[offset], iolist->iol_base, iolist->iol_len);
        offset += iolist->iol_len;
        if (offset > sizeof(_netdev_buffer)) {
            mutex_unlock(&_netdev_buffer_mutex);
            return -ENOBUFS;
        }
    }
    mutex_unlock(&_netdev_buffer_mutex);
    done.content.value = (uint32_t)offset - sizeof(ethernet_hdr_t) -
                         sizeof(udp_hdr_t);
    msg_send(&done, _check_pid);
    return offset;
}

void _net_init(void)
{
    struct netif *netif = &lwip_netif.lwip_netif;
    msg_init_queue(_msg_queue, _MSG_QUEUE_SIZE);
    _check_pid = thread_getpid();

    netdev_test_setup(&netdev, NULL);
    netdev_test_set_get_cb(&netdev, NETOPT_SRC_LEN, _get_src_len);
    netdev_test_set_get_cb(&netdev, NETOPT_MAX_PDU_SIZE,
                            _get_max_pkt_size);
    netdev_test_set_get_cb(&netdev, NETOPT_ADDRESS, _get_addr);
    netdev_test_set_get_cb(&netdev, NETOPT_ADDR_LEN,
                            _get_addr_len);
    netdev_test_set_get_cb(&netdev, NETOPT_SRC_LEN,
                            _get_addr_len);
    netdev_test_set_get_cb(&netdev, NETOPT_DEVICE_TYPE,
                            _get_device_type);
    netdev_test_set_recv_cb(&netdev, _netdev_recv);
    netdev_test_set_isr_cb(&netdev, _netdev_isr);
    /* netdev needs to be set-up */
    expect(netdev.netdev.netdev.driver);
    netdev.netdev.netdev.context = &lwip_netif;
#if LWIP_IPV4
    ip4_addr_t local4, mask4, gw4;
    local4.addr = _TEST_ADDR4_LOCAL;
    mask4.addr = _TEST_ADDR4_MASK;
    gw4.addr = _TEST_ADDR4_GW;
    netif_add(netif, &local4, &mask4, &gw4, &netdev, lwip_netdev_init, tcpip_input);
#else
    netif_add(netif, &netdev, lwip_netdev_init, tcpip_input);
#endif
#if LWIP_IPV6
    static const uint8_t local6_a[] = _TEST_ADDR6_LOCAL;
    /* XXX need to copy into a stack variable. Otherwise, when just using
     * `local6_a` this leads to weird alignment problems on some platforms with
     * netif_add_ip6_address() below */
    ip6_addr_t local6;
    s8_t idx;

    memcpy(&local6.addr, local6_a, sizeof(local6.addr));
    ip6_addr_clear_zone(&local6);
    netif_add_ip6_address(netif, &local6, &idx);
    for (int i = 0; i <= idx; i++) {
        netif->ip6_addr_state[i] |= IP6_ADDR_VALID;
    }
#endif
    netif_set_default(netif);
    lwip_bootstrap();
    ztimer_sleep(ZTIMER_MSEC, 3 * MS_PER_SEC);    /* Let the auto-configuration run warm */
}

void _prepare_send_checks(void)
{
    uint8_t remote6[] = _TEST_ADDR6_REMOTE;
    uint8_t mac[sizeof(uint64_t)];

    memcpy(mac, &remote6[8], sizeof(uint64_t));
    mac[0] ^= 0x2;
    mac[3] = mac[5];
    mac[4] = mac[6];
    mac[5] = mac[7];

    netdev_test_set_send_cb(&netdev, _netdev_send);
#if LWIP_ARP
    const ip4_addr_t remote4 = { .addr = _TEST_ADDR4_REMOTE };
    LOCK_TCPIP_CORE();
    expect(ERR_OK == etharp_add_static_entry(&remote4, (struct eth_addr *)mac));
    UNLOCK_TCPIP_CORE();
#endif
#if LWIP_IPV6
    memset(destination_cache, 0,
           LWIP_ND6_NUM_DESTINATIONS * sizeof(struct nd6_destination_cache_entry));
    memset(neighbor_cache, 0,
           LWIP_ND6_NUM_NEIGHBORS * sizeof(struct nd6_neighbor_cache_entry));
    for (int i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
        struct nd6_neighbor_cache_entry *nc = &neighbor_cache[i];
        struct netif *netif = &lwip_netif.lwip_netif;
        if (nc->state == ND6_NO_ENTRY) {
            nc->state = ND6_REACHABLE;
            memcpy(&nc->next_hop_address.addr, remote6, sizeof(nc->next_hop_address.addr));
            ip6_addr_assign_zone(&nc->next_hop_address,
                                 IP6_UNICAST, netif);
            memcpy(&nc->lladdr, mac, 6);
            nc->netif = netif;
            nc->counter.reachable_time = UINT32_MAX;
            break;
        }
    }
#endif
}

bool _inject_4packet(uint32_t src, uint32_t dst, uint16_t src_port,
                     uint16_t dst_port, void *data, size_t data_len,
                     uint16_t netif)
{
#if LWIP_IPV4
    mutex_lock(&_netdev_buffer_mutex);
    ethernet_hdr_t *eth_hdr = (ethernet_hdr_t *)_netdev_buffer;
    struct ip_hdr *ip_hdr = (struct ip_hdr *)(eth_hdr + 1);
    udp_hdr_t *udp_hdr = (udp_hdr_t *)(ip_hdr + 1);
    uint8_t *payload = (uint8_t *)(udp_hdr + 1);
    const uint16_t udp_len = (uint16_t)(sizeof(udp_hdr_t) + data_len);
    (void)netif;

    _get_addr(&netdev.netdev.netdev, &eth_hdr->dst, sizeof(eth_hdr->dst));
    eth_hdr->type = byteorder_htons(ETHERTYPE_IPV4);
    IPH_VHL_SET(ip_hdr, 4, 5);
    IPH_TOS_SET(ip_hdr, 0);
    IPH_LEN_SET(ip_hdr, htons(sizeof(struct ip_hdr) + udp_len));
    IPH_TTL_SET(ip_hdr, 64);
    IPH_PROTO_SET(ip_hdr, PROTNUM_UDP);
    ip_hdr->src.addr = src;
    ip_hdr->dest.addr = dst;
    IPH_CHKSUM_SET(ip_hdr, 0);
    IPH_CHKSUM_SET(ip_hdr, inet_chksum(ip_hdr, sizeof(struct ip_hdr)));

    udp_hdr->src_port = byteorder_htons(src_port);
    udp_hdr->dst_port = byteorder_htons(dst_port);
    udp_hdr->length.u16 = IPH_LEN(ip_hdr);
    udp_hdr->checksum.u16 = 0;

    memcpy(payload, data, data_len);

    _netdev_buffer_size = sizeof(ethernet_hdr_t) + sizeof(struct ip_hdr) +
                          sizeof(udp_hdr_t) + data_len;
    mutex_unlock(&_netdev_buffer_mutex);
    netdev_trigger_event_isr(&netdev.netdev.netdev);

    return true;
#else
    (void)src; (void)dst; (void)src_port; (void)dst_port; (void)netif;
    (void)data; (void)data_len;
    return false;
#endif
}

bool _inject_6packet(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                     uint16_t src_port, uint16_t dst_port,
                     void *data, size_t data_len, uint16_t netif)
{
#if LWIP_IPV6
    mutex_lock(&_netdev_buffer_mutex);
    ethernet_hdr_t *eth_hdr = (ethernet_hdr_t *)_netdev_buffer;
    ipv6_hdr_t *ipv6_hdr = (ipv6_hdr_t *)(eth_hdr + 1);
    udp_hdr_t *udp_hdr = (udp_hdr_t *)(ipv6_hdr + 1);
    uint8_t *payload = (uint8_t *)(udp_hdr + 1);
    const uint16_t udp_len = (uint16_t)(sizeof(udp_hdr_t) + data_len);
    uint16_t csum = 0;
    (void)netif;

    _get_addr(&netdev.netdev.netdev, &eth_hdr->dst, sizeof(eth_hdr->dst));
    eth_hdr->type = byteorder_htons(ETHERTYPE_IPV6);
    ipv6_hdr_set_version(ipv6_hdr);
    ipv6_hdr->len = byteorder_htons(udp_len);
    ipv6_hdr->nh = PROTNUM_UDP;
    ipv6_hdr->hl = 64;
    memcpy(&ipv6_hdr->src, src, sizeof(ipv6_hdr->src));
    memcpy(&ipv6_hdr->dst, dst, sizeof(ipv6_hdr->dst));
    csum = ipv6_hdr_inet_csum(csum, ipv6_hdr, ipv6_hdr->nh, udp_len);

    udp_hdr->src_port = byteorder_htons(src_port);
    udp_hdr->dst_port = byteorder_htons(dst_port);
    udp_hdr->length = ipv6_hdr->len;
    udp_hdr->checksum.u16 = 0;
    memcpy(payload, data, data_len);
    csum = inet_csum(csum, (uint8_t *)udp_hdr, udp_len);
    if (csum == UINT16_MAX) {
        udp_hdr->checksum = byteorder_htons(csum);
    }
    else {
        udp_hdr->checksum = byteorder_htons(~csum);
    }
    _netdev_buffer_size = sizeof(ethernet_hdr_t) + sizeof(ipv6_hdr_t) +
                          sizeof(udp_hdr_t) + data_len;
    mutex_unlock(&_netdev_buffer_mutex);
    netdev_trigger_event_isr(&netdev.netdev.netdev);

    return true;
#else
    (void)src; (void)dst; (void)src_port; (void)dst_port; (void)netif;
    (void)data; (void)data_len;
    return false;
#endif
}

bool _check_net(void)
{
    /* TODO maybe check packet buffer here too? */
    return true;
}

bool _check_4packet(uint32_t src, uint32_t dst, uint16_t src_port,
                    uint16_t dst_port, void *data, size_t data_len,
                    uint16_t netif, bool random_src_port)
{
#if LWIP_IPV4
    msg_t msg = { .content = { .value = 0 } };

    (void)netif;
    while (data_len != (msg.content.value - sizeof(struct ip_hdr))) {
        msg_receive(&msg);
    }
    mutex_lock(&_netdev_buffer_mutex);
    ethernet_hdr_t *eth_hdr = (ethernet_hdr_t *)_netdev_buffer;
    struct ip_hdr *ip_hdr = (struct ip_hdr *)(eth_hdr + 1);
    udp_hdr_t *udp_hdr = (udp_hdr_t *)(ip_hdr + 1);
    uint8_t *payload = (uint8_t *)(udp_hdr + 1);
    uint16_t payload_len = byteorder_ntohs(udp_hdr->length) - sizeof(udp_hdr_t);
    const bool ip_correct = ((src == 0) || (src = ip_hdr->src.addr)) &&
                            (dst = ip_hdr->dest.addr);
    const bool udp_correct = (random_src_port ||
                              (src_port == byteorder_ntohs(udp_hdr->src_port))) &&
                             (dst_port == byteorder_ntohs(udp_hdr->dst_port));
    const bool payload_correct = (data_len == payload_len) &&
                                 (memcmp(data, payload, data_len) == 0);
    mutex_unlock(&_netdev_buffer_mutex);
    return ip_correct && udp_correct && payload_correct;
#else
    (void)src; (void)dst; (void)src_port; (void)dst_port; (void)netif;
    (void)data; (void)data_len; (void)random_src_port;
    return false;
#endif
}

bool _check_6packet(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                    uint16_t src_port, uint16_t dst_port,
                    void *data, size_t data_len, uint16_t netif,
                    bool random_src_port)
{
#if LWIP_IPV6
    msg_t msg = { .content = { .value = 0 } };

    (void)netif;
    while (data_len != (msg.content.value - sizeof(ipv6_hdr_t))) {
        msg_receive(&msg);
    }
    mutex_lock(&_netdev_buffer_mutex);
    ethernet_hdr_t *eth_hdr = (ethernet_hdr_t *)_netdev_buffer;
    ipv6_hdr_t *ipv6_hdr = (ipv6_hdr_t *)(eth_hdr + 1);
    udp_hdr_t *udp_hdr = (udp_hdr_t *)(ipv6_hdr + 1);
    uint8_t *payload = (uint8_t *)(udp_hdr + 1);
    uint16_t payload_len = byteorder_ntohs(udp_hdr->length) - sizeof(udp_hdr_t);
    const bool ip_correct = (ipv6_addr_is_unspecified(src) ||
                             ipv6_addr_equal(src, &ipv6_hdr->src)) &&
                            ipv6_addr_equal(dst, &ipv6_hdr->dst);
    const bool udp_correct = (random_src_port ||
                              (src_port == byteorder_ntohs(udp_hdr->src_port))) &&
                             (dst_port == byteorder_ntohs(udp_hdr->dst_port));
    const bool payload_correct = (data_len == payload_len) &&
                                 (memcmp(data, payload, data_len) == 0);
    mutex_unlock(&_netdev_buffer_mutex);
    return ip_correct && udp_correct && payload_correct;
#else
    (void)src; (void)dst; (void)src_port; (void)dst_port; (void)netif;
    (void)data; (void)data_len; (void)random_src_port;
    return false;
#endif
}
