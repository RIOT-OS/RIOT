/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_auto_subnets Simple-Subnet auto-configuration
 * @ingroup     net_gnrc
 * @brief       Automatic configuration for cascading subnets
 *
 * About
 * =====
 *
 * This module provides an automatic configuration for networks with a (simple)
 * tree topology.
 *
 * If a sufficiently large IPv6 subnet (prefix length < /64) is provided via Router
 * Advertisements, a routing node with this module will automatically configure
 * subnets from it by dividing it into sub-prefixes for each downstream interface.
 *
 * When using the `gnrc_ipv6_auto_subnets_simple` module, there can only be a single
 * routing node on each level of the network but an arbitrary number of leaf nodes and
 * downstream interfaces.
 *
 * ![Example Topology with only one router on each level](gnrc_ipv6_auto_subnets_simple.svg)
 *
 * If there are multiple routing nodes on the same link, coordination between the
 * routers is required.
 * For this the `gnrc_ipv6_auto_subnets` implements a simple UDP based synchronisation
 * protocol where each router announces the number of subnets they want to create.
 *
 * ![Synchronisation Algorithm](gnrc_ipv6_auto_subnets-flow.svg)
 *
 * The layer 2 address of the sender then determines the order in which the prefixes
 * are assigned.
 *
 * ![Example Topology with multiple routers](gnrc_ipv6_auto_subnets.svg)
 *
 * The downstream network(s) receive the sub-prefix via Router Advertisements
 * and the process repeats until the bits of the prefix are exhausted.
 * The smallest subnet must still have a /64 prefix.
 *
 * The new subnet must no longer be considered on-link by the hosts in the
 * parent network.
 * Therefore the downstream router will send a router advertisement, which only
 * contains a Route Information Option, to the upstream network.
 * The Route Information Option contains the prefix of the downstream network
 * so that upstream routers will no longer consider hosts in this subnet on-link,
 * but instead will use the downstream router to route to the new subnet.
 *
 * The need for a Route Information Option
 * ---------------------------------------
 *
 * All nodes that want to communicate with hosts in a downstream subnet must implement
 * parsing of the Route Information Option. For routing RIOT nodes this is enabled by
 * default, non-routing nodes need to enable the `gnrc_ipv6_nib_rio` module.
 *
 * This is because all addresses in the subnet are also within the original network, so
 * without further information hosts would consider those addresses on-link and perform
 * neighbor solicitation to communicate with them.
 *
 * E.g. if host Ⓒ  (`2001:db8:0:8:5075:35ff:fefa:30bc`) sends an ICMPv6 Echo request to
 * Ⓑ  (`2001:db8:0:0:a7a2:12e0:48bc:7487`), it would not get a response:
 *
 * ![Auto-Subnets without RIO](gnrc_ipv6_auto_subnets-without_rio.svg)
 *
 * To solve this, the routing node Ⓐ  also sends a Router Advertisement to the <i>upstream</i>
 * network that only contains a Route Information Option for each downstream network created
 * by that router.
 * This way hosts in the upstream network will prefer the route via Ⓐ  over link-local
 * transmission as it is a stronger match than the upstream prefix:
 *
 * ![Auto-Subnets with RIO](gnrc_ipv6_auto_subnets-with_rio.svg)
 *
 * Usage
 * =====
 *
 * Simply add the `gnrc_ipv6_auto_subnets` or `gnrc_ipv6_auto_subnets_simple` module
 * to the nodes that should act as routers in the cascading network.
 * The upstream network will be automatically chosen as the one that first
 * receives a router advertisement.
 *
 * If only a single level of downstream routers exists and a sufficiently small
 * upstream prefix is provided, we can skip the synchronisation and instead derive
 * the *prefix* from the EUI of the downstream interface.
 *
 * e.g. given a prefix `fd12::/16` a router with a downstream interface with the
 * layer 2 address `12:84:0C:87:1F:B7` would create the prefix `fd12:1284:c87:1fb7::/64`
 * for the downstream network.
 *
 * To enable this behavior, chose the `gnrc_ipv6_auto_subnets_eui` module.
 *
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "compiler_hints.h"
#include "macros/utils.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/rpl.h"
#include "random.h"
#include "xtimer.h"

/**
 * @brief Port for the custom UDP sync protocol
 */
#ifndef CONFIG_GNRC_IPV6_AUTO_SUBNETS_PORT
#define CONFIG_GNRC_IPV6_AUTO_SUBNETS_PORT          (16179)
#endif

/**
 * @brief Max number of other routers on the same link
 */
#ifndef CONFIG_GNRC_IPV6_AUTO_SUBNETS_PEERS_MAX
#define CONFIG_GNRC_IPV6_AUTO_SUBNETS_PEERS_MAX     (4)
#endif

/**
 * @brief How often the number subnets should be announced by the routers
 */
#ifndef CONFIG_GNRC_IPV6_AUTO_SUBNETS_TX_PER_PERIOD
#define CONFIG_GNRC_IPV6_AUTO_SUBNETS_TX_PER_PERIOD (3)
#endif

/**
 * @brief How long to wait for other routers announcements before resending
 *        or creating subnets when the retry counter is exhausted
 */
#ifndef CONFIG_GNRC_IPV6_AUTO_SUBNETS_TIMEOUT_MS
#define CONFIG_GNRC_IPV6_AUTO_SUBNETS_TIMEOUT_MS    (50)
#endif

/**
 * @brief How many bits of a new prefix have to match the old prefix
 *        for it to be considered for replacement.
 *
 *        Set this if you want to join multiple upstream subnets at the same time.
 *
 *        If you use `gnrc_ipv6_auto_subnets` instead of `gnrc_ipv6_auto_subnets_simple`
 *        make sure to also set CONFIG_GNRC_IPV6_AUTO_SUBNETS_NUMOF accordingly.
 */
#ifndef CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_FIX_LEN
#define CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_FIX_LEN (0)
#endif

/**
 * @brief   Minimal length of a new prefix.
 *          e.g. Linux will only accept /64 prefixes for SLAAC
 */
#ifndef CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_MIN_LEN
#define CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_MIN_LEN (0)
#endif

/**
 * @brief   If the prefix changes, maximal difference in bits to find the
 *          previous prefix and remove it.
 */
#ifndef CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_DIFF_MAX
#define CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_DIFF_MAX (4)
#endif

/**
 * @brief Number of subnets that can be configured.
 *
 *        This is not needed when using the `gnrc_ipv6_auto_subnets_simple` module
 *        or if only a single upstream subnet is to be partitioned.
 *
 *        If this is set to any number higher than 1, make sure to also configure
 *        CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_FIX_LEN to suit your setup.
 */
#ifndef CONFIG_GNRC_IPV6_AUTO_SUBNETS_NUMOF
#define CONFIG_GNRC_IPV6_AUTO_SUBNETS_NUMOF         (1)
#endif

/**
 * @brief Enable this if you have a static network that might experience
 *        high packet loss under certain conditions.
 *        If enabled, this option causes the module to always assume the highest
 *        number of downstream routers it has ever seen.
 *        This prevents different/conflicting downstream subnets from being configured
 *        if multiple sync packets got lost.
 */
#ifndef CONFIG_GNRC_IPV6_AUTO_SUBNETS_STATIC
#define CONFIG_GNRC_IPV6_AUTO_SUBNETS_STATIC        (0)
#endif

/* Code below should not be included by Doxygen */
#ifndef DOXYGEN

#define SERVER_THREAD_STACKSIZE                     (THREAD_STACKSIZE_DEFAULT)
#define SERVER_MSG_QUEUE_SIZE                       (CONFIG_GNRC_IPV6_AUTO_SUBNETS_PEERS_MAX)
#define SERVER_MSG_TYPE_TIMEOUT                     (0x8fae)

#define ENABLE_DEBUG 0
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

#if !IS_USED(MODULE_GNRC_IPV6_AUTO_SUBNETS_SIMPLE)

/**
 * @brief Custom UDP sync protocol
 */
typedef struct __attribute__((packed)) {
    uint8_t version;        /**< version number, must be 0 */
    uint8_t num_subnets;    /**< number of subnets a host wants to create */
} _auto_subnets_request_v0_t;

/* keep a copy of PIO information in memory */
static gnrc_netif_t *_upstream;
static ndp_opt_pi_t _pio_cache[CONFIG_GNRC_IPV6_AUTO_SUBNETS_NUMOF];
static mutex_t _pio_cache_lock;

static char auto_subnets_stack[SERVER_THREAD_STACKSIZE];
static msg_t server_queue[SERVER_MSG_QUEUE_SIZE];

/* store neighbor routers l2 address to ignore duplicate packets */
static uint8_t l2addrs[CONFIG_GNRC_IPV6_AUTO_SUBNETS_PEERS_MAX]
                      [CONFIG_GNRC_IPV6_NIB_L2ADDR_MAX_LEN];

/* PID of the event thread */
static kernel_pid_t _server_pid;

static bool _store_pio(const ndp_opt_pi_t *pio)
{
    mutex_lock(&_pio_cache_lock);

    for (unsigned i = 0; i < ARRAY_SIZE(_pio_cache); ++i) {
        if (_pio_cache[i].len == 0) {
            _pio_cache[i] = *pio;

            mutex_unlock(&_pio_cache_lock);
            return true;
        }
    }

    mutex_unlock(&_pio_cache_lock);
    return false;
}

static int _send_udp(gnrc_netif_t *netif, const ipv6_addr_t *addr,
                     uint16_t port, const void *data, size_t len)
{
    gnrc_pktsnip_t *payload, *udp, *ip;

    /* allocate payload */
    payload = gnrc_pktbuf_add(NULL, data, len, GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        DEBUG("auto_subnets: unable to copy data to packet buffer\n");
        return -ENOBUFS;
    }

    /* allocate UDP header, set source port := destination port */
    udp = gnrc_udp_hdr_build(payload, port, port);
    if (udp == NULL) {
        DEBUG("auto_subnets: unable to allocate UDP header\n");
        gnrc_pktbuf_release(payload);
        return -ENOBUFS;
    }

    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, addr);
    if (ip == NULL) {
        DEBUG("auto_subnets: unable to allocate IPv6 header\n");
        gnrc_pktbuf_release(udp);
        return -ENOBUFS;
    }

    /* add netif header, if interface was given */
    if (netif != NULL) {
        gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
        if (netif_hdr == NULL) {
            DEBUG("auto_subnets: unable to allocate netif header\n");
            gnrc_pktbuf_release(ip);
            return -ENOBUFS;
        }

        gnrc_netif_hdr_set_netif(netif_hdr->data, netif);
        ip = gnrc_pkt_prepend(ip, netif_hdr);
    }

    /* send packet */
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP,
                                   GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
        DEBUG("auto_subnets: unable to locate UDP thread\n");
        gnrc_pktbuf_release(ip);
        return -ENETUNREACH;
    }

    return 0;
}
#endif /* !IS_USED(MODULE_GNRC_IPV6_AUTO_SUBNETS_SIMPLE) */

static void _init_sub_prefix(ipv6_addr_t *out,
                             const ipv6_addr_t *prefix, uint8_t bits,
                             uint8_t idx, uint8_t idx_bits)
{
    uint8_t bytes = bits / 8;
    uint8_t rem   = bits % 8;
    int8_t shift  = 8 - rem - idx_bits;

    /* first copy old prefix */
    memset(out, 0, sizeof(*out));
    ipv6_addr_init_prefix(out, prefix, bits);

    /* if new bits are between bytes, first copy over the most significant bits */
    if (shift < 0) {
        out->u8[bytes] |= idx >> -shift;
        out->u8[++bytes] = 0;
        shift += 8;
    }

    /* shift remaining bits at the end of the prefix */
    out->u8[bytes] |= idx << shift;
}

static uint8_t _init_sub_prefix_eui(ipv6_addr_t *out,
                                    const ipv6_addr_t *prefix, uint8_t bits,
                                    const uint8_t *eui, uint8_t eui_len)
{
    assert(eui_len <= sizeof(uint64_t));

    /* If the EUI is too large, discard most significant bits as
       those are typically manufacturer ID */
    uint64_t mask = UINT64_MAX >> bits;

    union {
        uint64_t u64;
        uint8_t u8[8];
    } eui64 = {};
    uint64_t pfx = byteorder_ntohll(prefix->u64[0]);

    /* If EUI is small, we want to preserve leftover unused bits at the end */
    uint8_t bits_total = bits + 8 * eui_len;
    uint8_t shift = bits_total < 64
                  ? 64 - bits_total
                  : 0;

    /* treat EUI as a EUI-64 with unused bytes set to 0 */
    memcpy(&eui64.u8[sizeof(uint64_t) - eui_len], eui, eui_len);
    eui64.u64 = ntohll(eui64.u64) & mask;

    /* create downstream prefix from upstream prefix + masked EUI64 */
    out->u64[0] = byteorder_htonll(pfx | (eui64.u64 << shift));

    /* we don't create prefixes that longer than 64 bits */
    return MIN(64, bits_total);
}

static inline uint8_t _absdiff(uint8_t a, uint8_t b)
{
    return a > b ? a - b
                 : b - a;
}

/* returns true if a new prefix was added, false if nothing changed */
static bool _remove_old_prefix(gnrc_netif_t *netif,
                               const ipv6_addr_t *pfx, uint8_t pfx_len,
                               gnrc_pktsnip_t **ext_opts)
{
    gnrc_ipv6_nib_pl_t entry;
    gnrc_pktsnip_t *tmp;
    void *state = NULL;
    ipv6_addr_t old_pfx;
    uint8_t old_pfx_len = 0;

    /* iterate prefix list to see if the prefix already exists */
    while (gnrc_ipv6_nib_pl_iter(netif->pid, &state, &entry)) {
        uint8_t match_len = ipv6_addr_match_prefix(&entry.pfx, pfx);

        /* The prefix did not change - nothing to do here */
        if (match_len >= pfx_len && pfx_len == entry.pfx_len) {
            return false;
        }

        /* find prefix that is closest to the new prefix */
        if (match_len > old_pfx_len &&
            _absdiff(pfx_len, entry.pfx_len) <= CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_DIFF_MAX) {
            old_pfx_len = entry.pfx_len;
            old_pfx = entry.pfx;
        }
    }

    /* no prefix found */
    if (old_pfx_len == 0) {
        return true;
    }

    DEBUG("auto_subnets: remove old prefix %s/%u\n",
          ipv6_addr_to_str(addr_str, &old_pfx, sizeof(addr_str)), old_pfx_len);

    /* invalidate old prefix in RIO */
    tmp = gnrc_ndp_opt_ri_build(&old_pfx, old_pfx_len, 0,
                                NDP_OPT_RI_FLAGS_PRF_ZERO, *ext_opts);
    if (tmp) {
        *ext_opts = tmp;
    }

    /* remove the prefix */
    gnrc_ipv6_nib_pl_del(netif->pid, &old_pfx, old_pfx_len);

    return true;
}

static void _configure_subnets(uint8_t subnets, uint8_t start_idx, gnrc_netif_t *upstream,
                               const ndp_opt_pi_t *pio, const ipv6_addr_t *src)
{
    gnrc_netif_t *downstream = NULL;
    gnrc_pktsnip_t *ext_opts = NULL;
    const ipv6_addr_t *prefix = &pio->prefix;
    uint32_t valid_ltime = byteorder_ntohl(pio->valid_ltime);
    uint32_t pref_ltime = byteorder_ntohl(pio->pref_ltime);
    const uint8_t prefix_len = pio->prefix_len;
    uint8_t new_prefix_len, subnet_len;

    DEBUG("auto_subnets: create %u subnets, start with %u\n", subnets, start_idx);

    /* Calculate remaining prefix length.
     * For n subnets we consume floor(log_2 n) + 1 bits.
     * To calculate floor(log_2 n) quickly, find the position of the
     * most significant set bit by counting leading zeros.
     */
    subnet_len = 32 - __builtin_clz(subnets);
    new_prefix_len = prefix_len + subnet_len;

    if (new_prefix_len > 64) {
        DEBUG("auto_subnets: can't split /%u into %u subnets\n", prefix_len, subnets);
        return;
    }

    if (new_prefix_len < may_be_zero(CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_MIN_LEN)) {
        new_prefix_len = CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_MIN_LEN;
    }

    while ((downstream = gnrc_netif_iter(downstream))) {
        gnrc_pktsnip_t *tmp;
        ipv6_addr_t new_prefix;
        int idx;

        if (downstream == upstream) {
            continue;
        }

        /* create subnet from upstream prefix */
        if (IS_USED(MODULE_GNRC_IPV6_AUTO_SUBNETS_EUI)) {
            uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
            int hwaddr_len = netif_get_opt(&downstream->netif, NETOPT_ADDRESS, 0,
                                           hwaddr, sizeof(hwaddr));
            if (hwaddr_len <= 0) {
                DEBUG("auto_subnets: can't get l2 address from netif %u\n", downstream->pid);
                continue;
            }
            new_prefix_len = _init_sub_prefix_eui(&new_prefix, prefix, prefix_len, hwaddr, hwaddr_len);
            new_prefix_len = MAX(new_prefix_len, CONFIG_GNRC_IPV6_AUTO_SUBNETS_PREFIX_MIN_LEN);
        } else {
            _init_sub_prefix(&new_prefix, prefix, prefix_len, ++start_idx, subnet_len);
        }

        DEBUG("auto_subnets: configure prefix %s/%u on %u\n",
              ipv6_addr_to_str(addr_str, &new_prefix, sizeof(addr_str)),
              new_prefix_len, downstream->pid);

        /* first remove old prefix if the prefix changed */
        if (_remove_old_prefix(downstream, &new_prefix, new_prefix_len, &ext_opts)) {

            /* configure subnet on downstream interface */
            idx = gnrc_netif_ipv6_add_prefix(downstream, &new_prefix, new_prefix_len,
                                         valid_ltime, pref_ltime);
            if (idx < 0) {
                DEBUG("auto_subnets: adding prefix to %u failed\n", downstream->pid);
                continue;
            }

            /* start advertising subnet */
            gnrc_ipv6_nib_change_rtr_adv_iface(downstream, true);

            /* configure RPL root if applicable */
            gnrc_rpl_configure_root(downstream, &downstream->ipv6.addrs[idx]);
        }

        /* add route information option with new subnet */
        tmp = gnrc_ndp_opt_ri_build(&new_prefix, new_prefix_len, valid_ltime,
                                    NDP_OPT_RI_FLAGS_PRF_ZERO, ext_opts);
        if (tmp == NULL) {
            DEBUG("auto_subnets: No space left in packet buffer. Not adding RIO\n");
        } else {
            ext_opts = tmp;
        }
    }

    /* immediately send an RA with RIO */
    if (ext_opts) {
        gnrc_ndp_rtr_adv_send(upstream, NULL, src, true, ext_opts);
    } else {
        DEBUG("auto_subnets: Options empty, not sending RA\n");
    }
}

void gnrc_ipv6_nib_rtr_adv_pio_cb(gnrc_netif_t *upstream, const ndp_opt_pi_t *pio,
                                  const ipv6_addr_t *src)
{
    /* create a subnet for each downstream interface */
    unsigned subnets = gnrc_netif_numof() - 1;

    if (subnets == 0) {
        return;
    }

    if (pio->valid_ltime.u32 == 0) {
        return;
    }

    /* only consider prefix meant for autonomous address configuration */
    if (!(pio->flags & NDP_OPT_PI_FLAGS_A)) {
        return;
    }

#if IS_USED(MODULE_GNRC_IPV6_AUTO_SUBNETS_SIMPLE)
    /* 'don't broadcast RA if we are a 6lo node - unicast allows l2 retransmissions */
    if (!gnrc_netif_is_6ln(upstream)) {
        src = NULL;
    }
    /* if we are the only router on this bus, we can directly choose a prefix */
    _configure_subnets(subnets, 0, upstream, pio, src);
#else
    (void)src;

    /* store PIO information for later use */
    if (!_store_pio(pio)) {
        DEBUG("auto_subnets: no space left in PIO cache, increase CONFIG_GNRC_IPV6_AUTO_SUBNETS_NUMOF\n");
        return;
    }
    _upstream  = upstream;

    /* start advertising by sending timeout message to the server thread */
    msg_t m = {
        .type = SERVER_MSG_TYPE_TIMEOUT
    };

    msg_send(&m, _server_pid);
#endif /* !IS_USED(MODULE_GNRC_IPV6_AUTO_SUBNETS_SIMPLE) */
}

#if !IS_USED(MODULE_GNRC_IPV6_AUTO_SUBNETS_SIMPLE)
/**
 * @brief Check if memory region is set to 0
 *
 * @param[in]   The memory array to check
 * @param[in]   The size of the memory array
 *
 * @return  true if all bytes are set to 0
 */
static bool _all_zero(const uint8_t *addr, size_t len)
{
    for (const uint8_t *end = addr + len; addr != end; ++addr) {
        if (*addr) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Allocates a l2 address in the `l2addrs` array
 *
 * @param[in]   addr    The l2 address to insert
 * @param[in]   len     l2 address length
 *
 * @return  1 if the address was added to the `l2addrs` array
 *          0 if the address was already in the array
 *         -1 if there was no more space in the `l2addrs` array
 */
static int _alloc_l2addr_entry(const void *addr, size_t len)
{
    int empty = -1;
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_AUTO_SUBNETS_PEERS_MAX; ++i) {
        if (_all_zero(l2addrs[i], len)) {
            empty = i;
            continue;
        }
        if (memcmp(addr, l2addrs[i], len) == 0) {
            return 0;
        }
    }

    if (empty < 0) {
        return -1;
    }

    memcpy(l2addrs[empty], addr, len);
    return 1;
}

/**
 * @brief Compare the l2 address of the received packet with the l2 address of the
 *        interface it was received on.
 *
 *        Only the first packet from a host generates a comparison, all subsequent
 *        packets will be ignored until the `l2addrs` array is reset.
 *
 * @param[in] upstream interface, ignore if the source does not match
 * @param[in] pkt   a received packet
 *
 * @return  1 if the sender l2 address is in order before the local l2 address
 * @return  0 if the order could not be determined or a packet from the sender
 *            was already processed
 * @return -1 if the sender l2 address is in order behind the local l2 address
 */
static int _get_my_l2addr_rank(gnrc_netif_t *iface, gnrc_pktsnip_t *pkt)
{
    const void *src_addr;
    gnrc_pktsnip_t *netif_hdr;
    gnrc_netif_hdr_t *hdr;

    if (iface == NULL) {
        return 0;
    }

    netif_hdr = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
    if (netif_hdr == NULL) {
        return 0;
    }

    /* ignore packet if it was received on the wrong interface */
    hdr = netif_hdr->data;
    if (iface->pid != hdr->if_pid) {
        return 0;
    }

    /* ignore packets without source address */
    src_addr = gnrc_netif_hdr_get_src_addr(hdr);
    if (src_addr == NULL) {
        return 0;
    }

    /* check if we have seen the host before */
    if (_alloc_l2addr_entry(src_addr, iface->l2addr_len) == 0) {
        return 0;
    }

    return memcmp(iface->l2addr, src_addr, iface->l2addr_len);
}

static void _receive_announce(gnrc_pktsnip_t *pkt, uint8_t *subnets, uint8_t *idx_start)
{
    _auto_subnets_request_v0_t *request = pkt->data;

    /* Check if we already got an announcement from that host, */
    /* in this case, res will be 0. */
    int res = _get_my_l2addr_rank(_upstream, pkt);
    if (res) {
        /* calculate total number of subnets */
        *subnets += request->num_subnets;

        DEBUG("auto_subnets: %u new remote subnets, total %u\n",
              request->num_subnets, *subnets);

        /* If other host is before us in order of MAC addresses, add
         * their subnets to our offset */
        if (res > 0) {
            *idx_start += request->num_subnets;
        }
    }

    gnrc_pktbuf_release(pkt);
}

static void _send_announce(uint8_t local_subnets, xtimer_t *timer, msg_t *msg)
{
    uint32_t timeout_us;
    _auto_subnets_request_v0_t request = {
        .num_subnets = local_subnets,
    };

    /* broadcast the number of subnets we want to create */
    _send_udp(_upstream, &ipv6_addr_all_routers_link_local,
              CONFIG_GNRC_IPV6_AUTO_SUBNETS_PORT,
              &request, sizeof(request));

    /* configure timeout for resend */
    timeout_us = random_uint32_range(
                    CONFIG_GNRC_IPV6_AUTO_SUBNETS_TIMEOUT_MS * US_PER_MS / 2,
                    CONFIG_GNRC_IPV6_AUTO_SUBNETS_TIMEOUT_MS * US_PER_MS);
    xtimer_set_msg(timer, timeout_us, msg, _server_pid);
    DEBUG("auto_subnets: announce sent, next timeout in %" PRIu32 " µs\n", timeout_us);
}

static void _process_pio_cache(uint8_t subnets, uint8_t idx_start, gnrc_netif_t *upstream)
{
    mutex_lock(&_pio_cache_lock);

    for (unsigned i = 0; i < ARRAY_SIZE(_pio_cache); ++i) {
        if (!_pio_cache[i].len) {
            continue;
        }

        /* use PIO for prefix configuration */
        _configure_subnets(subnets, idx_start, upstream, &_pio_cache[i], NULL);

        /* invalidate entry */
        _pio_cache[i].len = 0;
    }

    mutex_unlock(&_pio_cache_lock);
}

static void *_eventloop(void *arg)
{
    (void)arg;

    xtimer_t timeout_timer;
    msg_t msg, timeout_msg = { .type = SERVER_MSG_TYPE_TIMEOUT };
    gnrc_netreg_entry_t server = GNRC_NETREG_ENTRY_INIT_PID(0, KERNEL_PID_UNDEF);
    const uint8_t local_subnets = gnrc_netif_numof() - 1;
    uint8_t idx_start = 0;
    uint8_t subnets = local_subnets;
    uint8_t tx_period = CONFIG_GNRC_IPV6_AUTO_SUBNETS_TX_PER_PERIOD;

    /* only used with CONFIG_GNRC_IPV6_AUTO_SUBNETS_STATIC set */
    uint8_t idx_old = 0;
    uint8_t subnets_old = 0;

    DEBUG("auto_subnets: %u local subnets\n", subnets);

    if (subnets == 0) {
        return NULL;
    }

    /* setup the message queue */
    msg_init_queue(server_queue, SERVER_MSG_QUEUE_SIZE);

    /* register server to receive messages from given port */
    gnrc_netreg_entry_init_pid(&server, CONFIG_GNRC_IPV6_AUTO_SUBNETS_PORT, thread_getpid());
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &server);

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
        case GNRC_NETAPI_MSG_TYPE_RCV:
            _receive_announce(msg.content.ptr, &subnets, &idx_start);
            break;
        case SERVER_MSG_TYPE_TIMEOUT:
            if (tx_period--) {
                /* send subnet announcement */
                _send_announce(local_subnets, &timeout_timer, &timeout_msg);
            } else {

                /* don't re-enumerate subnets of a downstream router goes down */
                if (CONFIG_GNRC_IPV6_AUTO_SUBNETS_STATIC) {
                    /* If we got less subnets than before, use the old value */
                    if (subnets < subnets_old) {
                        subnets = subnets_old;
                        idx_start = idx_old;
                    }
                    /* Store subnet high water mark for later use */
                    else {
                        subnets_old = subnets;
                        idx_old = idx_start;
                    }
                }

                /* config round done, configure subnets */
                _process_pio_cache(subnets, idx_start, _upstream);

                /* start a new round of counting */
                tx_period = CONFIG_GNRC_IPV6_AUTO_SUBNETS_TX_PER_PERIOD;
                memset(l2addrs, 0, sizeof(l2addrs));
                idx_start = 0;
                subnets = local_subnets;
            }
            break;
        }
    }

    /* never reached */
    return NULL;
}

void gnrc_ipv6_auto_subnets_init(void)
{
    /* initiate auto_subnets thread */
    _server_pid = thread_create(auto_subnets_stack, sizeof(auto_subnets_stack),
                                THREAD_PRIORITY_MAIN - 1, 0,
                                _eventloop, NULL, "auto_subnets");
}
#endif /* !IS_USED(MODULE_GNRC_IPV6_AUTO_SUBNETS_SIMPLE) */
#endif /* !DOXYGEN */
/** @} */
