/*
 * Copyright (C) 2014-20 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <kernel_defines.h>

#include "bitfield.h"
#include "event.h"
#include "net/ethernet.h"
#include "net/ipv6.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6.h"
#if IS_USED(MODULE_GNRC_NETIF_PKTQ)
#include "net/gnrc/netif/pktq.h"
#endif /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
#include "net/gnrc/sixlowpan/ctx.h"
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR)
#include "net/gnrc/sixlowpan/frag/sfr.h"
#endif /* IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) */
#include "net/netstats.h"
#include "net/netstats/neighbor.h"
#include "fmt.h"
#include "log.h"
#include "sched.h"
#if IS_USED(MODULE_ZTIMER)
#include "ztimer.h"
#endif

#include "net/gnrc/netif.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/tx_sync.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _update_l2addr_from_dev(gnrc_netif_t *netif);
static void _check_netdev_capabilities(netdev_t *dev, bool legacy);
static void *_gnrc_netif_thread(void *args);
static void _event_cb(netdev_t *dev, netdev_event_t event);

typedef struct {
    gnrc_netif_t *netif;
    mutex_t init_done;
    int result;
} _netif_ctx_t;

int gnrc_netif_create(gnrc_netif_t *netif, char *stack, int stacksize,
                      char priority, const char *name, netdev_t *netdev,
                      const gnrc_netif_ops_t *ops)
{
    int res;
    _netif_ctx_t ctx;

    if (IS_ACTIVE(DEVELHELP) && gnrc_netif_highlander() && netif_iter(NULL)) {
        LOG_WARNING("gnrc_netif: gnrc_netif_highlander() returned true but "
                    "more than one interface is being registered.\n");
        assert(netif_iter(NULL) == NULL);
    }
#ifdef MODULE_GNRC_NETIF_BUS
    for (int i = 0; i < GNRC_NETIF_BUS_NUMOF; ++i) {
        msg_bus_init(&netif->bus[i]);
    }
#endif
    rmutex_init(&netif->mutex);
    netif->ops = ops;
    assert(netif->dev == NULL);
    netif->dev = netdev;

#ifdef MODULE_NETSTATS_NEIGHBOR
    netstats_nb_init(&netif->netif);
#endif

    /* prepare thread context */
    ctx.netif = netif;
    mutex_init(&ctx.init_done);
    mutex_lock(&ctx.init_done);

    res = thread_create(stack, stacksize, priority, 0,
                        _gnrc_netif_thread, &ctx, name);
    assert(res > 0);
    (void)res;

    /* wait for result of driver init */
    mutex_lock(&ctx.init_done);

    return ctx.result;
}

bool gnrc_netif_dev_is_6lo(const gnrc_netif_t *netif)
{
    switch (netif->device_type) {
#ifdef MODULE_GNRC_SIXLOENC
        case NETDEV_TYPE_ETHERNET:
            return (netif->flags & GNRC_NETIF_FLAGS_6LO);
#endif
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_CC110X:
        case NETDEV_TYPE_BLE:
        case NETDEV_TYPE_NRFMIN:
        case NETDEV_TYPE_NRF24L01P_NG:
        case NETDEV_TYPE_ESP_NOW:
            return true;
        default:
            return false;
    }
}

unsigned gnrc_netif_numof(void)
{
    gnrc_netif_t *netif = NULL;
    unsigned res = 0;

    while ((netif = gnrc_netif_iter(netif))) {
        if (netif->ops != NULL) {
            res++;
        }
    }
    return res;
}

gnrc_netif_t *gnrc_netif_iter(const gnrc_netif_t *prev)
{
    netif_t *result = netif_iter((prev) ? &prev->netif : NULL);
    return (result) ? container_of(result, gnrc_netif_t, netif) : NULL;
}

gnrc_netif_t *gnrc_netif_get_by_type(netdev_type_t type, uint8_t index)
{
    gnrc_netif_t *netif = NULL;
    while ((netif = gnrc_netif_iter(netif))) {

#ifdef MODULE_NETDEV_REGISTER
        if (netif->dev->type != type && type != NETDEV_ANY) {
            continue;
        }

        if (netif->dev->index != index && index != NETDEV_INDEX_ANY) {
            continue;
        }
#else
        (void)type;
        (void)index;
        assert(index == 0);
#endif

        return netif;
    }

    return NULL;
}

int gnrc_netif_get_from_netdev(gnrc_netif_t *netif, gnrc_netapi_opt_t *opt)
{
    int res = -ENOTSUP;

    gnrc_netif_acquire(netif);
    switch (opt->opt) {
        case NETOPT_6LO:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) =
                    (netopt_enable_t)gnrc_netif_is_6lo(netif);
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_HOP_LIMIT:
            assert(opt->data_len == sizeof(uint8_t));
            *((uint8_t *)opt->data) = netif->cur_hl;
            res = sizeof(uint8_t);
            break;
        case NETOPT_STATS:
            switch ((int16_t)opt->context) {
#if IS_USED(MODULE_NETSTATS_IPV6) && IS_USED(MODULE_GNRC_NETIF_IPV6)
            case NETSTATS_IPV6:
                {
                    assert(opt->data_len == sizeof(netstats_t));
                    /* IPv6 thread is updating this, to prevent data
                     * corruptions, we have to guarantee mutually exclusive
                     * access */
                    unsigned irq_state = irq_disable();
                    memcpy(opt->data, &netif->ipv6.stats,
                           sizeof(netif->ipv6.stats));
                    irq_restore(irq_state);
                    res = sizeof(netif->ipv6.stats);
                }
                break;
#endif
#ifdef MODULE_NETSTATS_L2
            case NETSTATS_LAYER2:
                assert(opt->data_len == sizeof(netstats_t));
                /* this is only accesses from the netif thread (us), so no need
                 * to lock this */
                memcpy(opt->data, &netif->stats,
                       sizeof(netif->stats));
                res = sizeof(netif->stats);
                break;
#endif
            default:
                /* take from device */
                break;
            }
            break;
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
        case NETOPT_IPV6_ADDR: {
                assert(opt->data_len >= sizeof(ipv6_addr_t));
                ipv6_addr_t *tgt = opt->data;

                res = 0;
                for (unsigned i = 0;
                     (res < (int)opt->data_len) &&
                     (i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF);
                     i++) {
                    if (netif->ipv6.addrs_flags[i] != 0) {
                        memcpy(tgt, &netif->ipv6.addrs[i], sizeof(ipv6_addr_t));
                        res += sizeof(ipv6_addr_t);
                        tgt++;
                    }
                }
            }
            break;
        case NETOPT_IPV6_ADDR_FLAGS: {
                assert(opt->data_len >= sizeof(uint8_t));
                uint8_t *tgt = opt->data;

                res = 0;
                for (unsigned i = 0;
                     (res < (int)opt->data_len) &&
                     (i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF);
                     i++) {
                    if (netif->ipv6.addrs_flags[i] != 0) {
                        *tgt = netif->ipv6.addrs_flags[i];
                        res += sizeof(uint8_t);
                        tgt++;
                    }
                }
            }
            break;
        case NETOPT_IPV6_GROUP: {
                assert(opt->data_len >= sizeof(ipv6_addr_t));
                ipv6_addr_t *tgt = opt->data;

                res = 0;
                for (unsigned i = 0;
                     (res < (int)opt->data_len) &&
                     (i < GNRC_NETIF_IPV6_GROUPS_NUMOF);
                     i++) {
                    if (!ipv6_addr_is_unspecified(&netif->ipv6.groups[i])) {
                        memcpy(tgt, &netif->ipv6.groups[i],
                               sizeof(ipv6_addr_t));
                        res += sizeof(ipv6_addr_t);
                        tgt++;
                    }
                }
            }
            break;
        case NETOPT_IPV6_IID:
            assert(opt->data_len >= sizeof(eui64_t));
            res = gnrc_netif_ipv6_get_iid(netif, opt->data);
            break;
        case NETOPT_MAX_PDU_SIZE:
            if (opt->context == GNRC_NETTYPE_IPV6) {
                assert(opt->data_len == sizeof(uint16_t));
                *((uint16_t *)opt->data) = netif->ipv6.mtu;
                res = sizeof(uint16_t);
            }
            /* else ask device */
            break;
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
        case NETOPT_IPV6_FORWARDING:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) = (gnrc_netif_is_rtr(netif)) ?
                                              NETOPT_ENABLE : NETOPT_DISABLE;
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_IPV6_SND_RTR_ADV:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) = (gnrc_netif_is_rtr_adv(netif)) ?
                                              NETOPT_ENABLE : NETOPT_DISABLE;
            res = sizeof(netopt_enable_t);
            break;
#endif  /* CONFIG_GNRC_IPV6_NIB_ROUTER */
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
        case NETOPT_6LO_IPHC:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) = (netif->flags &
                                               GNRC_NETIF_FLAGS_6LO_HC)
                                            ? NETOPT_ENABLE : NETOPT_DISABLE;
            res = sizeof(netopt_enable_t);
            break;
#endif  /* MODULE_GNRC_SIXLOWPAN_IPHC */
        default:
            break;
    }
    if (res == -ENOTSUP) {
        res = netif->dev->driver->get(netif->dev, opt->opt, opt->data,
                                      opt->data_len);
    }
    gnrc_netif_release(netif);
    return res;
}

int gnrc_netif_set_from_netdev(gnrc_netif_t *netif,
                               const gnrc_netapi_opt_t *opt)
{
    int res = -ENOTSUP;

    gnrc_netif_acquire(netif);
    switch (opt->opt) {
        case NETOPT_HOP_LIMIT:
            assert(opt->data_len == sizeof(uint8_t));
            netif->cur_hl = *((uint8_t *)opt->data);
            res = sizeof(uint8_t);
            break;
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
        case NETOPT_IPV6_ADDR: {
                assert(opt->data_len == sizeof(ipv6_addr_t));
                /* always assume manually added */
                uint8_t flags = ((((uint8_t)opt->context & 0xff) &
                                  ~GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK) |
                                 GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);
                uint8_t pfx_len = (uint8_t)(opt->context >> 8U);
                /* acquire locks a recursive mutex so we are safe calling this
                 * public function */
                res = gnrc_netif_ipv6_addr_add_internal(netif, opt->data,
                                                        pfx_len, flags);
                if (res >= 0) {
                    res = sizeof(ipv6_addr_t);
                }
            }
            break;
        case NETOPT_IPV6_ADDR_REMOVE:
            assert(opt->data_len == sizeof(ipv6_addr_t));
            /* acquire locks a recursive mutex so we are safe calling this
             * public function */
            gnrc_netif_ipv6_addr_remove_internal(netif, opt->data);
            res = sizeof(ipv6_addr_t);
            break;
        case NETOPT_IPV6_GROUP:
            assert(opt->data_len == sizeof(ipv6_addr_t));
            /* acquire locks a recursive mutex so we are safe calling this
             * public function */
            res = gnrc_netif_ipv6_group_join_internal(netif, opt->data);
            if (res >= 0) {
                res = sizeof(ipv6_addr_t);
            }
            break;
        case NETOPT_IPV6_GROUP_LEAVE:
            assert(opt->data_len == sizeof(ipv6_addr_t));
            /* acquire locks a recursive mutex so we are safe calling this
             * public function */
            gnrc_netif_ipv6_group_leave_internal(netif, opt->data);
            res = sizeof(ipv6_addr_t);
            break;
        case NETOPT_MAX_PDU_SIZE:
            if (opt->context == GNRC_NETTYPE_IPV6) {
                assert(opt->data_len == sizeof(uint16_t));
                netif->ipv6.mtu = *((uint16_t *)opt->data);
                res = sizeof(uint16_t);
            }
            /* else set device */
            break;
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
        case NETOPT_IPV6_FORWARDING:
            assert(opt->data_len == sizeof(netopt_enable_t));
            if (*(((netopt_enable_t *)opt->data)) == NETOPT_ENABLE) {
                netif->flags |= GNRC_NETIF_FLAGS_IPV6_FORWARDING;
            }
            else {
                if (gnrc_netif_is_rtr_adv(netif)) {
                    gnrc_ipv6_nib_change_rtr_adv_iface(netif, false);
                }
                netif->flags &= ~GNRC_NETIF_FLAGS_IPV6_FORWARDING;
            }
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_IPV6_SND_RTR_ADV:
            assert(opt->data_len == sizeof(netopt_enable_t));
            gnrc_ipv6_nib_change_rtr_adv_iface(netif,
                    (*(((netopt_enable_t *)opt->data)) == NETOPT_ENABLE));
            res = sizeof(netopt_enable_t);
            break;
#endif  /* CONFIG_GNRC_IPV6_NIB_ROUTER */
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
        case NETOPT_6LO_IPHC:
            assert(opt->data_len == sizeof(netopt_enable_t));
            if (*(((netopt_enable_t *)opt->data)) == NETOPT_ENABLE) {
                netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
            }
            else {
                netif->flags &= ~GNRC_NETIF_FLAGS_6LO_HC;
            }
            res = sizeof(netopt_enable_t);
            break;
#endif  /* MODULE_GNRC_SIXLOWPAN_IPHC */
        case NETOPT_RAWMODE:
            if (*(((netopt_enable_t *)opt->data)) == NETOPT_ENABLE) {
                netif->flags |= GNRC_NETIF_FLAGS_RAWMODE;
            }
            else {
                netif->flags &= ~GNRC_NETIF_FLAGS_RAWMODE;
            }
            /* Also propagate to the netdev device */
            netif->dev->driver->set(netif->dev, NETOPT_RAWMODE, opt->data,
                                      opt->data_len);
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_STATS:
            switch ((int16_t)opt->context) {
#if IS_USED(MODULE_NETSTATS_IPV6) && IS_USED(MODULE_GNRC_NETIF_IPV6)
            case NETSTATS_IPV6:
                {
                    /* IPv6 thread is updating this, to prevent data
                     * corruptions, we have to guarantee mutually exclusive
                     * access */
                    unsigned irq_state = irq_disable();
                    memset(&netif->ipv6.stats, 0, sizeof(netif->ipv6.stats));
                    irq_restore(irq_state);
                    res = 0;
                }
                break;
#endif
#ifdef MODULE_NETSTATS_L2
            case NETSTATS_LAYER2:
                /* this is only accesses from the netif thread (us), so no need
                 * to lock this */
                memset(&netif->stats, 0, sizeof(netif->stats));
                res = 0;
                break;
#endif
            default:
                /* take from device */
                break;
            }
        default:
            break;
    }
    if (res == -ENOTSUP) {
        res = netif->dev->driver->set(netif->dev, opt->opt, opt->data,
                                      opt->data_len);
        if (res > 0) {
            switch (opt->opt) {
                case NETOPT_ADDRESS:
                case NETOPT_ADDRESS_LONG:
                case NETOPT_ADDR_LEN:
                case NETOPT_SRC_LEN:
                    _update_l2addr_from_dev(netif);
                    break;
                case NETOPT_IEEE802154_PHY:
                    gnrc_netif_ipv6_init_mtu(netif);
                    break;
                default:
                    break;
            }
        }
    }
    gnrc_netif_release(netif);
    return res;
}

gnrc_netif_t *gnrc_netif_get_by_pid(kernel_pid_t pid)
{
    gnrc_netif_t *netif = NULL;

    while ((netif = gnrc_netif_iter(netif))) {
        if (netif->pid == pid) {
            return netif;
        }
    }
    return NULL;
}

void gnrc_netif_acquire(gnrc_netif_t *netif)
{
    if (netif && (netif->ops)) {
        rmutex_lock(&netif->mutex);
    }
}

void gnrc_netif_release(gnrc_netif_t *netif)
{
    if (netif && (netif->ops)) {
        rmutex_unlock(&netif->mutex);
    }
}

#if IS_USED(MODULE_GNRC_NETIF_IPV6)
static int _addr_idx(const gnrc_netif_t *netif, const ipv6_addr_t *addr);
static int _group_idx(const gnrc_netif_t *netif, const ipv6_addr_t *addr);

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

/**
 * @brief   Matches an address by prefix to an address on the interface and
 *          return length of the best match
 *
 * @param[in] netif     the network interface
 * @param[in] addr      the address to match
 *
 * @return  bits up to which the best match matches @p addr
 * @return  0, if no match was found
 *
 * @pre `netif != NULL` and `addr != NULL`
 */
static unsigned _match_to_len(const gnrc_netif_t *netif,
                              const ipv6_addr_t *addr);

/**
 * @brief   Matches an address by prefix to an address on the interface and
 *          return index of the best match
 *
 * @param[in] netif     the network interface
 * @param[in] addr      the address to match
 *
 * @return  index of the best match for @p addr
 * @return  -1 if no match was found
 *
 * @pre `netif != NULL` and `addr != NULL`
 */
static int _match_to_idx(const gnrc_netif_t *netif,
                        const ipv6_addr_t *addr);
/**
 * @brief Determines the scope of the given address.
 *
 * @param[in] addr              The IPv6 address to check.
 *
 * @return The scope of the address.
 *
 * @pre address is not loopback or unspecified.
 * see http://tools.ietf.org/html/rfc6724#section-4
 */
static uint8_t _get_scope(const ipv6_addr_t *addr);
static inline unsigned _get_state(const gnrc_netif_t *netif, unsigned idx);

/**
 * @brief selects potential source address candidates
 * @see <a href="http://tools.ietf.org/html/rfc6724#section-4">
 *      RFC6724, section 4
 *      </a>
 * @param[in]  netif            the interface used for sending
 * @param[in]  dst              the destination address
 * @param[in]  ll_only          only consider link-local addresses
 * @param[out] candidate_set    a bitfield representing all addresses
 *                              configured to @p netif, potential candidates
 *                              will be marked as 1
 *
 * @return -1 if no candidates were found
 * @return the index of the first candidate otherwise
 *
 * @pre the interface entry and its set of addresses must not be changed during
 *      runtime of this function
 */
static int _create_candidate_set(const gnrc_netif_t *netif,
                                 const ipv6_addr_t *dst, bool ll_only,
                                 uint8_t *candidate_set);

/** @brief Find the best candidate among the configured addresses
 *          for a certain destination address according to the 8 rules
 *          specified in RFC 6734, section 5.
 * @see <a href="http://tools.ietf.org/html/rfc6724#section-5">
 *      RFC6724, section 5
 *      </a>
 *
 * @param[in] netif              The interface for sending.
 * @param[in] dst                The destination IPv6 address.
 * @param[in, out] candidate_set The preselected set of candidate addresses as
 *                               a bitfield.
 *
 * @pre @p dst is not unspecified.
 *
 * @return The best matching candidate found on @p netif, may be NULL if none
 *         is found.
 */
static ipv6_addr_t *_src_addr_selection(gnrc_netif_t *netif,
                                        const ipv6_addr_t *dst,
                                        uint8_t *candidate_set);

int gnrc_netif_ipv6_addr_add_internal(gnrc_netif_t *netif,
                                      const ipv6_addr_t *addr,
                                      unsigned pfx_len, uint8_t flags)
{
    unsigned idx = UINT_MAX;

    assert((netif != NULL) && (addr != NULL));
    assert(!(ipv6_addr_is_multicast(addr) || ipv6_addr_is_unspecified(addr) ||
             ipv6_addr_is_loopback(addr)));
    assert((pfx_len > 0) && (pfx_len <= 128));
    gnrc_netif_acquire(netif);
    if ((flags & GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK) ==
        GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE) {
        /* set to first retransmission */
        flags &= ~GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE;
        flags |= 0x1;
    }
    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        if (ipv6_addr_equal(&netif->ipv6.addrs[i], addr)) {
            gnrc_netif_release(netif);
            return i;
        }
        if ((idx == UINT_MAX) && (netif->ipv6.addrs_flags[i] == 0)) {
            idx = i;
        }
    }
    if (idx == UINT_MAX) {
        gnrc_netif_release(netif);
        return -ENOMEM;
    }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
    ipv6_addr_t sol_nodes;
    int res;

    /* TODO: SHOULD delay join between 0 and MAX_RTR_SOLICITATION_DELAY
     * for SLAAC */
    ipv6_addr_set_solicited_nodes(&sol_nodes, addr);
    res = gnrc_netif_ipv6_group_join_internal(netif, &sol_nodes);
    if (res < 0) {
        DEBUG("gnrc_netif: Can't join solicited-nodes of %s on interface %"
              PRIkernel_pid "\n",
              ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
              netif->pid);
        gnrc_netif_release(netif);
        return res;
    }
#else  /* CONFIG_GNRC_IPV6_NIB_ARSM */
    if (!gnrc_netif_is_6ln(netif)) {
        LOG_WARNING("Address-resolution state-machine not activated. Neighbors "
                    "from interface %u\nwill not be able to resolve address "
                    "%s\n"
                    "    Use CONFIG_GNRC_IPV6_NIB_ARSM=1 to activate.\n",
                    netif->pid, ipv6_addr_to_str(addr_str, addr,
                                                 sizeof(addr_str)));
    }
#endif /* CONFIG_GNRC_IPV6_NIB_ARSM */
    netif->ipv6.addrs_flags[idx] = flags;
    memcpy(&netif->ipv6.addrs[idx], addr, sizeof(netif->ipv6.addrs[idx]));
#ifdef MODULE_GNRC_IPV6_NIB
    if (_get_state(netif, idx) == GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID) {
        void *state = NULL;
        gnrc_ipv6_nib_pl_t ple;
        bool in_pl = false;

        while (gnrc_ipv6_nib_pl_iter(netif->pid, &state, &ple)) {
            if (ipv6_addr_match_prefix(&ple.pfx, addr) >= pfx_len) {
                in_pl = true;
            }
        }
        if (!in_pl) {
            gnrc_ipv6_nib_pl_set(netif->pid, addr, pfx_len,
                                 UINT32_MAX, UINT32_MAX);
        }

        gnrc_netif_ipv6_bus_post(netif, GNRC_IPV6_EVENT_ADDR_VALID, &netif->ipv6.addrs[idx]);
    }
    else if (IS_USED(MODULE_GNRC_IPV6) &&
             IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC) &&
             !gnrc_netif_is_6ln(netif)) {
        /* cast to remove const qualifier (will still be used NIB internally as
         * const) */
        msg_t msg = { .type = GNRC_IPV6_NIB_DAD,
                      .content = { .ptr = &netif->ipv6.addrs[idx] } };

        msg_send(&msg, gnrc_ipv6_pid);
    }
#else
    (void)pfx_len;
#endif
    gnrc_netif_release(netif);
    return idx;
}

void gnrc_netif_ipv6_addr_remove_internal(gnrc_netif_t *netif,
                                          const ipv6_addr_t *addr)
{
    bool remove_sol_nodes = true;
    ipv6_addr_t sol_nodes;

    assert((netif != NULL) && (addr != NULL));
    ipv6_addr_set_solicited_nodes(&sol_nodes, addr);
    gnrc_netif_acquire(netif);
    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        if (ipv6_addr_equal(&netif->ipv6.addrs[i], addr)) {
            netif->ipv6.addrs_flags[i] = 0;
            ipv6_addr_set_unspecified(&netif->ipv6.addrs[i]);
        }
        else {
            ipv6_addr_t tmp;

            ipv6_addr_set_solicited_nodes(&tmp, &netif->ipv6.addrs[i]);
            /* there is still an address on the interface with the same
             * solicited nodes address */
            if (ipv6_addr_equal(&tmp, &sol_nodes)) {
                remove_sol_nodes = false;
            }
        }
    }
    if (remove_sol_nodes) {
        gnrc_netif_ipv6_group_leave_internal(netif, &sol_nodes);
    }
    gnrc_netif_release(netif);
}

int gnrc_netif_ipv6_addr_idx(gnrc_netif_t *netif,
                             const ipv6_addr_t *addr)
{
    int idx;

    assert((netif != NULL) && (addr != NULL));
    DEBUG("gnrc_netif: get index of %s from interface %" PRIkernel_pid "\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
          netif->pid);
    gnrc_netif_acquire(netif);
    idx = _addr_idx(netif, addr);
    gnrc_netif_release(netif);
    return idx;
}

int gnrc_netif_ipv6_addr_match(gnrc_netif_t *netif,
                               const ipv6_addr_t *addr)
{
    assert((netif != NULL) && (addr != NULL));
    gnrc_netif_acquire(netif);
    int idx = _match_to_idx(netif, addr);
    gnrc_netif_release(netif);
    return idx;
}

ipv6_addr_t *gnrc_netif_ipv6_addr_best_src(gnrc_netif_t *netif,
                                           const ipv6_addr_t *dst,
                                           bool ll_only)
{
    ipv6_addr_t *best_src = NULL;

    BITFIELD(candidate_set, CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF);

    assert((netif != NULL) && (dst != NULL));
    DEBUG("gnrc_netif: get best source address for %s\n",
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
    memset(candidate_set, 0, sizeof(candidate_set));
    gnrc_netif_acquire(netif);
    int first_candidate = _create_candidate_set(netif, dst, ll_only,
                                                candidate_set);
    if (first_candidate >= 0) {
        best_src = _src_addr_selection(netif, dst, candidate_set);
        if (best_src == NULL) {
            best_src = &(netif->ipv6.addrs[first_candidate]);
        }
    }
    gnrc_netif_release(netif);
    return best_src;
}

gnrc_netif_t *gnrc_netif_get_by_ipv6_addr(const ipv6_addr_t *addr)
{
    gnrc_netif_t *netif = NULL;

    DEBUG("gnrc_netif: get interface by IPv6 address %s\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
    while ((netif = gnrc_netif_iter(netif))) {
        if (_addr_idx(netif, addr) >= 0) {
            break;
        }
        if (_group_idx(netif, addr) >= 0) {
            break;
        }
    }
    return netif;
}

gnrc_netif_t *gnrc_netif_get_by_prefix(const ipv6_addr_t *prefix)
{
    gnrc_netif_t *netif = NULL, *best_netif = NULL;
    unsigned best_match = 0;

    while ((netif = gnrc_netif_iter(netif))) {
        unsigned match;

        if (((match = _match_to_len(netif, prefix)) > 0) &&
            (match > best_match)) {
            best_match = match;
            best_netif = netif;
        }
    }
    return best_netif;
}

static int _netif_ops_set_helper(gnrc_netif_t *netif, netopt_t opt,
                                 void *data, uint16_t data_len)
{
    gnrc_netapi_opt_t netapi_opt = {
        .opt = opt,
        .data = data,
        .data_len = data_len,
    };
    return netif->ops->set(netif, &netapi_opt);
}

int gnrc_netif_ipv6_group_join_internal(gnrc_netif_t *netif,
                                        const ipv6_addr_t *addr)
{
    uint8_t l2_group_data[GNRC_NETIF_L2ADDR_MAXLEN];
    unsigned idx = UINT_MAX;
    int l2_group_len;

    /* can be called out of lock */
    l2_group_len = gnrc_netif_ipv6_group_to_l2_group(netif, addr,
                                                     l2_group_data);
    gnrc_netif_acquire(netif);
    if (l2_group_len > 0) {
        int res = _netif_ops_set_helper(netif, NETOPT_L2_GROUP,
                                        l2_group_data, (uint16_t)l2_group_len);
        /* link layer does not support multicast, but we can still use
         * broadcast */
        if ((res != -ENOTSUP) && (res < 0)) {
            gnrc_netif_release(netif);
            return res;
        }
    }
    /* link layer does not support multicast, but we can still use
     * broadcast */
    else if (l2_group_len != -ENOTSUP) {
        gnrc_netif_release(netif);
        return l2_group_len;
    }
    for (unsigned i = 0; i < GNRC_NETIF_IPV6_GROUPS_NUMOF; i++) {
        if (ipv6_addr_equal(&netif->ipv6.groups[i], addr)) {
            gnrc_netif_release(netif);
            return i;
        }
        if ((idx == UINT_MAX) &&
            (ipv6_addr_is_unspecified(&netif->ipv6.groups[i]))) {
            idx = i;
        }
    }
    if (idx == UINT_MAX) {
        gnrc_netif_release(netif);
        return -ENOMEM;
    }
    memcpy(&netif->ipv6.groups[idx], addr, sizeof(netif->ipv6.groups[idx]));
    /* TODO:
     *  - MLD action
     */
    gnrc_netif_release(netif);
    return idx;
}

void gnrc_netif_ipv6_group_leave_internal(gnrc_netif_t *netif,
                                          const ipv6_addr_t *addr)
{
    uint8_t l2_group_data[GNRC_NETIF_L2ADDR_MAXLEN];
    int idx = -1, l2_group_len;
    /* IPv6 addresses that correspond to the same L2 address */
    unsigned l2_groups = 0;

    assert((netif != NULL) && (addr != NULL));
    /* can be called out of lock */
    l2_group_len = gnrc_netif_ipv6_group_to_l2_group(netif, addr,
                                                     l2_group_data);
    /* link layer does not support multicast, but might still have used
     * broadcast */
    if ((l2_group_len < 0) && (l2_group_len != -ENOTSUP)) {
        return;
    }
    gnrc_netif_acquire(netif);
    for (unsigned i = 0; i < GNRC_NETIF_IPV6_GROUPS_NUMOF; i++) {
        if (l2_group_len > 0) {
            uint8_t tmp[GNRC_NETIF_L2ADDR_MAXLEN];
            if (!ipv6_addr_is_unspecified(&netif->ipv6.groups[i]) &&
                (gnrc_netif_ipv6_group_to_l2_group(netif,
                                                   &netif->ipv6.groups[i],
                                                   tmp) == l2_group_len)) {
                if (memcmp(tmp, l2_group_data, l2_group_len) == 0) {
                    l2_groups++;
                }
            }
        }
        if (ipv6_addr_equal(&netif->ipv6.groups[i], addr)) {
            idx = i;
        }
    }
    if (idx < 0) {
        gnrc_netif_release(netif);
        return;
    }
    /* we need to have found at least one corresponding group for the IPv6
     * group we want to leave when link layer supports multicast */
    assert((l2_group_len == -ENOTSUP) || (l2_groups > 0));
    /* we only found exactly IPv6 multicast address that corresponds to
     * `l2_group_data`, so we can remove it, if there would be more, we need
     * to stay in the group */
    if (l2_groups == 1) {
        int res = _netif_ops_set_helper(netif, NETOPT_L2_GROUP_LEAVE,
                                        l2_group_data, (uint16_t)l2_group_len);
        /* link layer does not support multicast, but might still have used
         * broadcast */
        if ((res != -ENOTSUP) && (res < 0)) {
            DEBUG("gnrc_netif: error leaving link layer group\n");
        }
    }
    if (idx >= 0) {
        ipv6_addr_set_unspecified(&netif->ipv6.groups[idx]);
        /* TODO:
         *  - MLD action */
    }
    gnrc_netif_release(netif);
}

int gnrc_netif_ipv6_group_idx(gnrc_netif_t *netif, const ipv6_addr_t *addr)
{
    int idx;

    assert((netif != NULL) && (addr != NULL));
    gnrc_netif_acquire(netif);
    idx = _group_idx(netif, addr);
    gnrc_netif_release(netif);
    return idx;
}

static int _idx(const gnrc_netif_t *netif, const ipv6_addr_t *addr, bool mcast)
{
    if (!ipv6_addr_is_unspecified(addr)) {
        const ipv6_addr_t *iplist = (mcast) ? netif->ipv6.groups :
                                              netif->ipv6.addrs;
        unsigned ipmax = (mcast) ? GNRC_NETIF_IPV6_GROUPS_NUMOF :
                                   CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF;
        for (unsigned i = 0; i < ipmax; i++) {
            if (ipv6_addr_equal(&iplist[i], addr)) {
                return i;
            }
        }
    }
    return -1;
}

static inline int _addr_idx(const gnrc_netif_t *netif, const ipv6_addr_t *addr)
{
    return _idx(netif, addr, false);
}

static inline int _group_idx(const gnrc_netif_t *netif, const ipv6_addr_t *addr)
{
    return _idx(netif, addr, true);
}

static unsigned _match_to_len(const gnrc_netif_t *netif,
                              const ipv6_addr_t *addr)
{
    assert((netif != NULL) && (addr != NULL));

    int n = _match_to_idx(netif, addr);
    return (n >= 0) ? ipv6_addr_match_prefix(&(netif->ipv6.addrs[n]), addr) : 0;
}

static int _match_to_idx(const gnrc_netif_t *netif,
                         const ipv6_addr_t *addr)
{
    assert((netif != NULL) && (addr != NULL));

    int idx = -1;
    unsigned best_match = 0;
    for (int i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        unsigned match;

        if (netif->ipv6.addrs_flags[i] == 0) {
            continue;
        }
        match = ipv6_addr_match_prefix(&(netif->ipv6.addrs[i]), addr);
        if (match > best_match) {
            idx = i;
            best_match = match;
        }
    }
    if (idx != -1) {
        DEBUG("gnrc_netif: Found %s on interface %" PRIkernel_pid " matching ",
              ipv6_addr_to_str(addr_str, &netif->ipv6.addrs[idx],
                               sizeof(addr_str)),
              netif->pid);
        DEBUG("%s by %u bits\n",
              ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
              best_match);
    }
    else {
        DEBUG("gnrc_netif: Did not found any address on interface %"
              PRIkernel_pid
              " matching %s\n",
              netif->pid,
              ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
    }
    return idx;
}

static uint8_t _get_scope(const ipv6_addr_t *addr)
{
    if (ipv6_addr_is_multicast(addr)) {
        /* return multicast scope as is, see
         * https://tools.ietf.org/html/rfc4291#section-2.7*/
        return addr->u8[1] & 0x0f;
    }
    else if (ipv6_addr_is_link_local(addr)) {
        return IPV6_ADDR_MCAST_SCP_LINK_LOCAL;
    }
    else if (ipv6_addr_is_site_local(addr)) {
        return IPV6_ADDR_MCAST_SCP_SITE_LOCAL;
    }
    else {
        return IPV6_ADDR_MCAST_SCP_GLOBAL;
    }
}

static inline unsigned _get_state(const gnrc_netif_t *netif, unsigned idx)
{
    return (netif->ipv6.addrs_flags[idx] &
            GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK);
}

/**
 * @brief selects potential source address candidates
 * @see <a href="http://tools.ietf.org/html/rfc6724#section-4">
 *      RFC6724, section 4
 *      </a>
 * @param[in]  netif            the interface used for sending
 * @param[in]  dst              the destination address
 * @param[in]  ll_only          only consider link-local addresses
 * @param[out] candidate_set    a bitfield representing all addresses
 *                              configured to @p netif, potential candidates
 *                              will be marked as 1
 *
 * @return -1 if no candidates were found
 * @return the index of the first candidate otherwise
 *
 * @pre the interface entry and its set of addresses must not be changed during
 *      runtime of this function
 */
static int _create_candidate_set(const gnrc_netif_t *netif,
                                 const ipv6_addr_t *dst, bool ll_only,
                                 uint8_t *candidate_set)
{
    int res = -1;

    DEBUG("gathering source address candidates\n");
    /* currently this implementation supports only addresses as source address
     * candidates assigned to this interface. Thus we assume all addresses to be
     * on interface @p netif */
    (void) dst;
    for (int i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        const ipv6_addr_t *tmp = &(netif->ipv6.addrs[i]);

        DEBUG("Checking address: %s\n",
              ipv6_addr_to_str(addr_str, tmp, sizeof(addr_str)));
        /* "In any case, multicast addresses and the unspecified address MUST
         * NOT be included in a candidate set."
         *
         * flags are set if not unspecfied and multicast addresses are in
         * `netif->ipv6.groups` so not considered here.
         */
        if ((netif->ipv6.addrs_flags[i] == 0) ||
            /* https://tools.ietf.org/html/rfc4862#section-2:
             *  A tentative address is not considered assigned to an interface
             *  in the usual sense.  An interface discards received packets
             *  addressed to a tentative address, but accepts Neighbor Discovery
             *  packets related to Duplicate Address Detection for the tentative
             *  address.
             *  (so don't consider tentative addresses for source address
             *  selection) */
            gnrc_netif_ipv6_addr_dad_trans(netif, i)) {
            continue;
        }
        /* Check if we only want link local addresses */
        if (ll_only && !ipv6_addr_is_link_local(tmp)) {
            continue;
        }
        /* "For all multicast and link-local destination addresses, the set of
         *  candidate source addresses MUST only include addresses assigned to
         *  interfaces belonging to the same link as the outgoing interface."
         *
         * "For site-local unicast destination addresses, the set of candidate
         *  source addresses MUST only include addresses assigned to interfaces
         *  belonging to the same site as the outgoing interface."
         *  -> we should also be fine, since we're only iterating addresses of
         *     the sending interface
         */
        /* put all other addresses into the candidate set */
        DEBUG("add to candidate set\n");
        bf_set(candidate_set, i);
        if (res < 0) {
            res = i;
        }
    }
    return res;
}

/* number of "points" assigned to an source address candidate with equal scope
 * than destination address */
#define RULE_2_PTS          (IPV6_ADDR_MCAST_SCP_GLOBAL + 1)
/* number of "points" assigned to an source address candidate in preferred
 * state */
#define RULE_3_PTS          (1)

/**
 * @brief   Caps the match at a source addresses prefix length
 *
 * @see https://tools.ietf.org/html/rfc6724#section-2.2
 *
 * @param[in] netif The network interface @p src was selected from
 * @param[in] src   A potential source address
 * @param[in] match The number of bits matching of @p src with another address
 *
 * @return  @p match or a number lesser than @p match, if @p src has a shorter
 *          prefix.
 */
static unsigned _cap_match(const gnrc_netif_t *netif, const ipv6_addr_t *src,
                           unsigned match)
{
    unsigned best_prefix = 0;

    if (ipv6_addr_is_link_local(src)) {
        best_prefix = 64U;  /* Link-local prefix is always of length 64 */
    }
#ifdef MODULE_GNRC_IPV6_NIB
    else {
        void *state = NULL;
        gnrc_ipv6_nib_pl_t ple;

        while (gnrc_ipv6_nib_pl_iter(netif->pid, &state, &ple)) {
            if ((ipv6_addr_match_prefix(&ple.pfx, src) > best_prefix)) {
                best_prefix = ple.pfx_len;
            }
        }
    }
#endif /* MODULE_GNRC_IPV6_NIB */
    return ((best_prefix > 0) && (best_prefix < match)) ? best_prefix : match;
}

static ipv6_addr_t *_src_addr_selection(gnrc_netif_t *netif,
                                        const ipv6_addr_t *dst,
                                        uint8_t *candidate_set)
{
    int idx = -1;
    /* create temporary set for assigning "points" to candidates winning in the
     * corresponding rules.
     */
    uint8_t winner_set[CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF];

    memset(winner_set, 0, CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF);
    uint8_t max_pts = 0;
    /* _create_candidate_set() assures that `dst` is not unspecified and if
     * `dst` is loopback rule 1 will fire anyway.  */
    uint8_t dst_scope = _get_scope(dst);

    DEBUG("finding the best match within the source address candidates\n");
    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        ipv6_addr_t *ptr = &(netif->ipv6.addrs[i]);

        DEBUG("Checking address: %s\n",
              ipv6_addr_to_str(addr_str, ptr, sizeof(addr_str)));
        /* entries which are not part of the candidate set can be ignored */
        if (!(bf_isset(candidate_set, i))) {
            DEBUG("Not part of the candidate set - skipping\n");
            continue;
        }
        /* Rule 1: if we have an address configured that equals the destination
         * use this one as source */
        if (ipv6_addr_equal(ptr, dst)) {
            DEBUG("Ease one - rule 1\n");
            return ptr;
        }
        /* Rule 2: Prefer appropriate scope. */
        uint8_t candidate_scope = _get_scope(ptr);
        if (candidate_scope == dst_scope) {
            DEBUG("winner for rule 2 (same scope) found\n");
            winner_set[i] += (dst_scope + RULE_2_PTS);
        }
        else if (candidate_scope > dst_scope) {
            DEBUG("winner for rule 2 (larger scope) found\n");
            /* From https://tools.ietf.org/html/rfc6724#section-5:
             * >  Rule 2: Prefer appropriate scope.
             * >  If Scope(SA) < Scope(SB): If Scope(SA) < Scope(D), then prefer
             * >  SB and otherwise prefer SA.
             * Meaning give address with larger scope than `dst` but closest to
             * `dst` precedence.
             * As the if above already ensures that the scope is larger than
             * the scope of the destination address we give the address with the
             * smallest scope that lands here the highest score */
            winner_set[i] += (dst_scope + (RULE_2_PTS - candidate_scope));
        }
        else {
            DEBUG("winner for rule 2 (smaller scope) found\n");
            /* don't add `dst_scope` here to keep it smaller than larger and
             * equal scope */
            winner_set[i] += candidate_scope;
        }

        /* Rule 3: Avoid deprecated addresses. */
        if (_get_state(netif, i) !=
            GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_DEPRECATED) {
            DEBUG("winner for rule 3 found\n");
            winner_set[i] += RULE_3_PTS;
        }

        /* Rule 4: Prefer home addresses.
         * Does not apply, gnrc does not support Mobile IP.
         * TODO: update as soon as gnrc supports Mobile IP
         */

        /* Rule 5: Prefer outgoing interface.
         * RFC 6724 says:
         * "It is RECOMMENDED that the candidate source addresses be the set of
         *  unicast addresses assigned to the interface that will be used to
         *  send to the destination (the "outgoing" interface).  On routers,
         *  the candidate set MAY include unicast addresses assigned to any
         *  interface that forwards packets, subject to the restrictions
         *  described below."
         *  Currently this implementation uses ALWAYS source addresses assigned
         *  to the outgoing interface. Hence, Rule 5 is always fulfilled.
         */

        /* Rule 6: Prefer matching label.
         * Flow labels are currently not supported by gnrc.
         * TODO: update as soon as gnrc supports flow labels
         */

        /* Rule 7: Prefer temporary addresses.
         * Temporary addresses are currently not supported by gnrc.
         * TODO: update as soon as gnrc supports temporary addresses
         */

        if (winner_set[i] > max_pts) {
            idx = i;
            max_pts = winner_set[i];
        }
    }
    /* check if we have a clear winner, otherwise
     * rule 8: Use longest matching prefix.*/
    uint8_t best_match = 0;
    /* collect candidates with maximum points */
    for (int i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        if (winner_set[i] == max_pts) {
            const ipv6_addr_t *addr = &netif->ipv6.addrs[i];
            unsigned match = ipv6_addr_match_prefix(addr, dst);

            match = _cap_match(netif, addr, match);
            /* if match == 0 for all case, it takes above selected idx */
            if (match > best_match) {
                idx = i;
                best_match = match;
            }
        }
    }
    if (idx < 0) {
        DEBUG("No winner found\n");
        return NULL;
    }
    else {
        DEBUG("Winner is: %s\n", ipv6_addr_to_str(addr_str,
                                                  &netif->ipv6.addrs[idx],
                                                  sizeof(addr_str)));
        return &netif->ipv6.addrs[idx];
    }
}

int gnrc_netif_ipv6_add_prefix(gnrc_netif_t *netif,
                               const ipv6_addr_t *pfx, uint8_t pfx_len,
                               uint32_t valid, uint32_t pref)
{
    int res;
    eui64_t iid;
    ipv6_addr_t addr = {0};

    assert(netif != NULL);
    DEBUG("gnrc_netif: (re-)configure prefix %s/%d\n",
          ipv6_addr_to_str(addr_str, pfx, sizeof(addr_str)), pfx_len);
    if (gnrc_netapi_get(netif->pid, NETOPT_IPV6_IID, 0, &iid,
                        sizeof(eui64_t)) >= 0) {
        ipv6_addr_set_aiid(&addr, iid.uint8);
    }
    else {
        LOG_WARNING("gnrc_netif: cannot get IID of netif %u\n", netif->pid);
        return -ENODEV;
    }
    ipv6_addr_init_prefix(&addr, pfx, pfx_len);

    /* add address as valid */
    res = gnrc_netif_ipv6_addr_add_internal(netif, &addr, pfx_len,
                                           GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);
    if (res < 0) {
        goto out;
    }

    /* update lifetime */
    if (valid < UINT32_MAX) { /* UINT32_MAX means infinite lifetime */
        /* the valid lifetime is given in seconds, but the NIB's timers work
         * in microseconds, so we have to scale down to the smallest
         * possible value (UINT32_MAX - 1). */
        valid = (valid > (UINT32_MAX / MS_PER_SEC))
              ? (UINT32_MAX - 1) : valid * MS_PER_SEC;
    }
    if (pref < UINT32_MAX) { /* UINT32_MAX means infinite lifetime */
        /* same treatment for pref */
        pref = (pref > (UINT32_MAX / MS_PER_SEC))
             ? (UINT32_MAX - 1) : pref * MS_PER_SEC;
    }
    gnrc_ipv6_nib_pl_set(netif->pid, pfx, pfx_len, valid, pref);

    /* configure 6LoWPAN specific options */
    if (IS_USED(MODULE_GNRC_IPV6_NIB) &&
        IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR) &&
        IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C) &&
        gnrc_netif_is_6ln(netif)) {

        if (IS_ACTIVE(CONFIG_GNRC_NETIF_IPV6_BR_AUTO_6CTX)) {
            /* configure compression context */
            if (gnrc_sixlowpan_ctx_update_6ctx(pfx, pfx_len, valid)) {
                DEBUG("gnrc_netif: add compression context for prefix %s/%u\n",
                       ipv6_addr_to_str(addr_str, pfx, sizeof(addr_str)), pfx_len);
            }
        }

        (void)gnrc_ipv6_nib_abr_add(&addr);
    }

out:
    return res;
}

#if IS_USED(MODULE_GNRC_NETIF_BUS)
static bool _has_global_addr(gnrc_netif_t *netif)
{
    bool has_global = false;

    gnrc_netif_acquire(netif);

    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        if (ipv6_addr_is_unspecified(&netif->ipv6.addrs[i])) {
            continue;
        }
        if (!ipv6_addr_is_link_local(&netif->ipv6.addrs[i])) {
            has_global = true;
            break;
        }
    }

    gnrc_netif_release(netif);
    return has_global;
}

static void _netif_bus_attach_and_subscribe_addr_valid(gnrc_netif_t *netif,
                                                       msg_bus_entry_t *sub)
{
    msg_bus_t *bus = gnrc_netif_get_bus(netif, GNRC_NETIF_BUS_IPV6);
    msg_bus_attach(bus, sub);
    msg_bus_subscribe(sub, GNRC_IPV6_EVENT_ADDR_VALID);
}

static void _netif_bus_detach(gnrc_netif_t *netif, msg_bus_entry_t *sub)
{
    msg_bus_t *bus = gnrc_netif_get_bus(netif, GNRC_NETIF_BUS_IPV6);
    msg_bus_detach(bus, sub);
}

static void _netif_bus_msg_global(gnrc_netif_t *netif, msg_t* m, bool *has_global)
{
    msg_bus_t* bus = gnrc_netif_get_bus(netif, GNRC_NETIF_BUS_IPV6);
    if (msg_is_from_bus(bus, m) && ipv6_addr_is_global(m->content.ptr)) {
        DEBUG_PUTS("gnrc_netif: got global address");
        *has_global = true;
    }
}

static void _unset_arg(void *arg)
{
    ztimer_t *timer = arg;
    if (timer) {
        timer->arg = NULL;
    }
}

bool gnrc_netif_ipv6_wait_for_global_address(gnrc_netif_t *netif,
                                             uint32_t timeout_ms)
{
    unsigned netif_numof = gnrc_netif_numof();

    /* no interfaces */
    if (netif_numof == 0) {
        return false;
    }

    msg_bus_entry_t subs[netif_numof];
    bool has_global = false;

    if (netif != NULL) {
        if (_has_global_addr(netif)) {
            DEBUG("gnrc_netif: %u already has a global address\n", netif->pid);
            return true;
        }

        _netif_bus_attach_and_subscribe_addr_valid(netif, &subs[0]);
    } else {
        /* subscribe to all interfaces */
        for (unsigned count = 0;
             (netif = gnrc_netif_iter(netif));
             count++) {
            if (_has_global_addr(netif)) {
                DEBUG("gnrc_netif: %u already has a global address\n", netif->pid);
                has_global = true;
            }

            _netif_bus_attach_and_subscribe_addr_valid(netif, &subs[count]);
        }
    }

    /* wait for global address */
    msg_t m;
    ztimer_t _timeout = { .callback = _unset_arg, .arg = &_timeout };
    if (timeout_ms != UINT32_MAX) {
        ztimer_set(ZTIMER_MSEC, &_timeout, timeout_ms);
    }

    while (!has_global) {
        /* stop if timeout reached */
        if (!_timeout.arg) {
            DEBUG_PUTS("gnrc_netif: timeout reached");
            break;
        }

        /* waiting for any message */
        if (timeout_ms == UINT32_MAX) {
            msg_receive(&m);
        } else if (ztimer_msg_receive_timeout(ZTIMER_MSEC, &m, timeout_ms) < 0) {
            DEBUG_PUTS("gnrc_netif: timeout waiting for prefix");
            break;
        }

        if (netif != NULL) {
            _netif_bus_msg_global(netif, &m, &has_global);
        } else {
            /* scan all interfaces */
            gnrc_netif_t *_netif = NULL;
            while ((_netif = gnrc_netif_iter(_netif))) {
                _netif_bus_msg_global(_netif, &m, &has_global);
            }
        }
    }

    ztimer_remove(ZTIMER_MSEC, &_timeout);

    /* called with a given interface */
    if (netif != NULL) {
        _netif_bus_detach(netif, &subs[0]);
    } else {
        /* unsubscribe all */
        for (unsigned count = 0;
             (netif = gnrc_netif_iter(netif));
             count++) {
            _netif_bus_detach(netif, &subs[count]);
        }
    }

    return has_global;
}
#endif  /* IS_USED(MODULE_GNRC_NETIF_BUS) */
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */

static void _update_l2addr_from_dev(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    int res;
    netopt_t opt = gnrc_netif_get_l2addr_opt(netif);

    res = dev->driver->get(dev, opt, netif->l2addr,
                           sizeof(netif->l2addr));
    if (res != -ENOTSUP) {
        netif->flags |= GNRC_NETIF_FLAGS_HAS_L2ADDR;
    }
    else {
        /* If no address is provided but still an address length given above,
         * we are in an invalid state */
        assert(netif->l2addr_len == 0);
    }
    if (res > 0) {
        netif->l2addr_len = res;
    }
}

static void _init_from_device(gnrc_netif_t *netif)
{
    int res;
    netdev_t *dev = netif->dev;
    uint16_t tmp;

    res = dev->driver->get(dev, NETOPT_DEVICE_TYPE, &tmp, sizeof(tmp));
    (void)res;
    assert(res == sizeof(tmp));
    netif->device_type = (uint8_t)tmp;
    gnrc_netif_ipv6_init_mtu(netif);
    _update_l2addr_from_dev(netif);
}

static void _check_netdev_capabilities(netdev_t *dev, bool legacy)
{
    /* Check whether RX- and TX-complete interrupts are supported by the driver */
    if (IS_ACTIVE(DEVELHELP)) {
        if (IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ)) {
            if (!legacy) {
                /* new API implies TX end event */
                return;
            }
            netopt_enable_t enable = NETOPT_ENABLE;
            int res = dev->driver->get(dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));
            if ((res != sizeof(enable)) || (enable != NETOPT_ENABLE)) {
                LOG_WARNING("NETOPT_TX_END_IRQ not implemented by driver\n");
            }
        }
    }
}

#ifdef DEVELHELP
static bool options_tested = false;

/* checks if a device supports all required options and functions */
static void _test_options(gnrc_netif_t *netif)
{
    uint8_t dummy_addr[GNRC_NETIF_L2ADDR_MAXLEN] = { 0 };
    ndp_opt_t dummy_opt = { .len = 1U };
    uint64_t tmp64 = 0ULL;

    (void)dummy_addr;
    (void)dummy_opt;
    (void)tmp64;
#if (GNRC_NETIF_L2ADDR_MAXLEN > 0)
    /* check if address was set in _update_l2addr_from_dev()
     * (NETOPT_DEVICE_TYPE already tested in _configure_netdev()) and
     * if MTU and max. fragment size was set properly by
     * gnrc_netif_ipv6_init_mtu()
     * all checked types below have link-layer addresses so we don't need to
     * check `GNRC_NETIF_FLAGS_HAS_L2ADDR` */
    switch (netif->device_type) {
#ifdef TEST_SUITES
        case NETDEV_TYPE_TEST:
            /* make no assumptions about test devices */
            break;
#endif
        case NETDEV_TYPE_BLE:
        case NETDEV_TYPE_ETHERNET:
        case NETDEV_TYPE_ESP_NOW:
            assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
            assert(ETHERNET_ADDR_LEN == netif->l2addr_len);
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
            switch (netif->device_type) {
                case NETDEV_TYPE_BLE:
#if IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU)
                    assert(netif->ipv6.mtu >= IPV6_MIN_MTU);
#else
                    assert(netif->ipv6.mtu == IPV6_MIN_MTU);
#endif /* IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU) */
                    break;
                case NETDEV_TYPE_ETHERNET:
                    assert(netif->ipv6.mtu == ETHERNET_DATA_LEN);
                    break;
                case NETDEV_TYPE_ESP_NOW:
                    assert(netif->ipv6.mtu <= ETHERNET_DATA_LEN);
            }
#endif  /* IS_USED(MODULE GNRC_NETIF_IPV6) */
            break;
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_NRFMIN: {
            gnrc_nettype_t tmp;

            /* in case assert() evaluates to NOP */
            (void)tmp;
            assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
            assert((IEEE802154_SHORT_ADDRESS_LEN == netif->l2addr_len) ||
                   (IEEE802154_LONG_ADDRESS_LEN == netif->l2addr_len));
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
#if IS_USED(MODULE_GNRC_NETIF_6LO)
#if IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU)
            assert(netif->ipv6.mtu >= IPV6_MIN_MTU);
#else /* IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU) */
            assert(netif->ipv6.mtu == IPV6_MIN_MTU);
#endif /* IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU) */
            assert(-ENOTSUP != netif->dev->driver->get(netif->dev, NETOPT_PROTO,
                                                       &tmp, sizeof(tmp)));
#else   /* IS_USED(MODULE_GNRC_NETIF_6LO) */
            assert(netif->ipv6.mtu < UINT16_MAX);
#endif  /* IS_USED(MODULE_GNRC_NETIF_6LO) */
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
#ifdef MODULE_GNRC_SIXLOWPAN_ND
            assert((netif->device_type != NETDEV_TYPE_IEEE802154) ||
                   (-ENOTSUP != netif->dev->driver->get(netif->dev,
                                                        NETOPT_ADDRESS_LONG,
                                                        &dummy_addr,
                                                        sizeof(dummy_addr))));
#endif  /* MODULE_GNRC_SIXLOWPAN_ND */
            break;
        }
        case NETDEV_TYPE_CC110X:
            assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
            assert(1U == netif->l2addr_len);
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
            assert(netif->ipv6.mtu < UINT16_MAX);
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
            break;
        case NETDEV_TYPE_NRF24L01P_NG:
            assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
            assert(netif->l2addr_len >= 3U && netif->l2addr_len <= 5U);
            break;
        case NETDEV_TYPE_SLIP:
#if IS_USED(MODULE_SLIPDEV_L2ADDR)
            assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
            assert(8U == netif->l2addr_len);
            break;
#endif /* IS_USED(MODULE_SLIPDEV_L2ADDR) */
        case NETDEV_TYPE_LORA: /* LoRa doesn't provide L2 ADDR */
            assert(!(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR));
            assert(0U == netif->l2addr_len);
            /* don't check MTU here for now since I'm not sure the current
             * one is correct ^^" "*/
            break;
        default:
            /* device type not supported yet, please amend case above when
             * porting new device type */
            assert(false);
    }
    /* These functions only apply to network devices having link-layers */
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
        assert(-ENOTSUP != gnrc_netif_ipv6_get_iid(netif, (eui64_t *)&tmp64));
        assert(-ENOTSUP != gnrc_netif_ndp_addr_len_from_l2ao(netif,
                                                             &dummy_opt));
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
        assert(-ENOTSUP != gnrc_netif_ipv6_iid_to_addr(netif, (eui64_t *)&tmp64,
                                                       dummy_addr));
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
    }
#endif /* (GNRC_NETIF_L2ADDR_MAXLEN > 0) */
    options_tested = true;
}
#endif /* DEVELHELP */

int gnrc_netif_default_init(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    /* register the event callback with the device driver */
    dev->event_callback = _event_cb;
    dev->context = netif;
    int res = dev->driver->init(dev);
    if (res < 0) {
        return res;
    }
    netif_register(&netif->netif);
    _check_netdev_capabilities(dev, gnrc_netif_netdev_legacy_api(netif));
    _init_from_device(netif);
#ifdef DEVELHELP
    _test_options(netif);
#endif
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR)
    gnrc_sixlowpan_frag_sfr_init_iface(netif);
#endif
    netif->cur_hl = CONFIG_GNRC_NETIF_DEFAULT_HL;
#ifdef MODULE_GNRC_IPV6_NIB
    gnrc_ipv6_nib_init_iface(netif);
#endif
#if DEVELHELP
    assert(options_tested);
#endif
    return 0;
}

static void _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt, bool push_back);

/**
 * @brief   Call the ISR handler from an event
 *
 * @param[in]   evp     pointer to the event
 */
static void _event_handler_isr(event_t *evp)
{
    gnrc_netif_t *netif = container_of(evp, gnrc_netif_t, event_isr);
    netif->dev->driver->isr(netif->dev);
}

static void _process_receive_stats(gnrc_netif_t *netdev, gnrc_pktsnip_t *pkt)
{
    if (!IS_USED(MODULE_NETSTATS_NEIGHBOR)) {
        return;
    }

    gnrc_netif_hdr_t *hdr;
    const uint8_t *src = NULL;
    gnrc_pktsnip_t *netif = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);

    if (netif == NULL) {
        return;
    }

    size_t src_len;
    hdr = netif->data;
    src = gnrc_netif_hdr_get_src_addr(hdr);
    src_len = hdr->src_l2addr_len;
    netstats_nb_update_rx(&netdev->netif, src, src_len, hdr->rssi, hdr->lqi);
}

static event_t *_gnrc_netif_fetch_event(gnrc_netif_t *netif)
{
    event_t *ev;
    /* Iterate from highest priority to lowest priority */
    for (int i = 0; i < GNRC_NETIF_EVQ_NUMOF; i++) {
        if ((ev = event_get(&netif->evq[i]))) {
            return ev;
        }
    }
    return NULL;
}

/**
 * @brief   Process any pending events and wait for IPC messages
 *
 * This function will block until an IPC message is received. Events posted to
 * the event queue will be processed while waiting for messages.
 *
 * @param[in]   netif   gnrc_netif instance to operate on
 * @param[out]  msg     pointer to message buffer to write the first received message
 *
 * @return >0 if msg contains a new message
 */
static void _process_events_await_msg(gnrc_netif_t *netif, msg_t *msg)
{
    while (1) {
        /* Using messages for external IPC, and events for internal events */

        /* First drain the queues before blocking the thread */
        /* Events will be handled before messages */
        DEBUG("gnrc_netif: handling events\n");
        event_t *evp;
        /* We can not use event_loop() or event_wait() because then we would not
         * wake up when a message arrives */
        while ((evp = _gnrc_netif_fetch_event(netif))) {
            DEBUG("gnrc_netif: event %p\n", (void *)evp);
            if (evp->handler) {
                evp->handler(evp);
            }
        }
        /* non-blocking msg check */
        int msg_waiting = msg_try_receive(msg);
        if (msg_waiting > 0) {
            return;
        }
        DEBUG("gnrc_netif: waiting for events\n");
        /* Block the thread until something interesting happens */
        thread_flags_wait_any(THREAD_FLAG_MSG_WAITING | THREAD_FLAG_EVENT);
    }
}

static void _send_queued_pkt(gnrc_netif_t *netif)
{
    (void)netif;
#if IS_USED(MODULE_GNRC_NETIF_PKTQ)
    gnrc_pktsnip_t *pkt;

    if ((pkt = gnrc_netif_pktq_get(netif)) != NULL) {
        _send(netif, pkt, true);
        gnrc_netif_pktq_sched_get(netif);
    }
#endif /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
}

static void _tx_done(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt,
                     gnrc_pktsnip_t *tx_sync, int res, bool push_back)
{
    (void)push_back; /* only used with IS_USED(MODULE_GNRC_NETIF_PKTQ) */

    uint32_t err = (res < 0) ? -res : GNRC_NETERR_SUCCESS;

    if (tx_sync != NULL) {
        gnrc_pktbuf_release_error(tx_sync, err);
    }

    /* no frame was transmitted */
    if (res < 0) {
        DEBUG("gnrc_netif: error sending packet %p (code: %i)\n",
              (void *)pkt, res);

        if (IS_USED(MODULE_NETSTATS_NEIGHBOR)) {
            netstats_nb_update_tx(&netif->netif, NETSTATS_NB_BUSY, 0);
        }
    }
#ifdef MODULE_NETSTATS_L2
    else {
        netif->stats.tx_bytes += res;
    }
#endif
#if IS_USED(MODULE_GNRC_NETIF_PKTQ)
    if (res == -EBUSY) {
        int put_res;

        /* Lower layer was busy.
         * Since "busy" could also mean that the lower layer is currently
         * receiving, trying to wait for the device not being busy any more
         * could run into the risk of overriding the received packet on send
         * Rather, queue the packet within the netif now and try to send them
         * again after the device completed its busy state. */
        if (push_back) {
            put_res = gnrc_netif_pktq_push_back(netif, pkt);
        }
        else {
            put_res = gnrc_netif_pktq_put(netif, pkt);
            gnrc_netif_pktq_sched_get(netif);
        }
        if (put_res == 0) {
            DEBUG("gnrc_netif: (re-)queued pkt %p\n", (void *)pkt);
            return; /* early return to not release */
        }
        else {
            LOG_ERROR("gnrc_netif: can't queue packet for sending, drop it\n");
            /* If we got here, it means the device was busy and the pkt queue
             * was full. The packet should be dropped here anyway */
            gnrc_pktbuf_release_error(pkt, ENOMEM);
        }
        return;
    }
    else if (gnrc_netif_netdev_legacy_api(netif)) {
        /* remove previously held packet */
        gnrc_pktbuf_release(pkt);
        return;
    }
#endif /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */

    if (gnrc_netif_netdev_new_api(netif)) {
        /* with new netdev (with confirm_send), the netif remains owner of the
         * pkt and is in charge of releasing it once TX is completed */
        gnrc_pktbuf_release_error(pkt, err);
    }
}

#if IS_USED(MODULE_NETDEV_NEW_API)
/**
 * @brief   Call the confirm_send handler from an event
 *
 * @param[in]   evp     pointer to the event
 */
static void _event_handler_tx_done(event_t *evp)
{
    gnrc_netif_t *netif = container_of(evp, gnrc_netif_t, event_tx_done);
    int res = netif->dev->driver->confirm_send(netif->dev, NULL);
    /* after confirm_send() is called, the device is ready to send the next
     * frame. So clear netif->tx_pkt to signal readiness */
    gnrc_pktsnip_t *pkt = netif->tx_pkt;
    netif->tx_pkt = NULL;
    bool push_back = netif->flags & GNRC_NETIF_FLAGS_TX_FROM_PKTQUEUE;
    netif->flags &= ~GNRC_NETIF_FLAGS_TX_FROM_PKTQUEUE;
    _tx_done(netif, pkt, NULL, res, push_back);
}
#endif

static void _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt, bool push_back)
{
#if IS_USED(MODULE_NETDEV_NEW_API)
    if (netif->tx_pkt != NULL) {
        /* Upper layer is handing out frames faster than hardware can transmit.
         * Note that not only doesn't it make sense to bother the driver if it
         * is still busy, but overwriting netif->tx_pkt would leak the memory
         * of the current sent frame.
         *
         * Also note: If gnrc_netif_pktq is used, it will (try to) queue the
         * outgoing frame to send it later on.
         * */
        _tx_done(netif, pkt, NULL, -EBUSY, push_back);
        return;
    }
#endif

#if IS_USED(MODULE_GNRC_NETIF_PKTQ)
    /* send queued packets first to keep order */
    if (!push_back && !gnrc_netif_pktq_empty(netif)) {
        int put_res;

        /* try to send pkt from queue first. At least with the legacy blocking
         * API, this may make room in the pktqueue */
        _send_queued_pkt(netif);

        put_res = gnrc_netif_pktq_put(netif, pkt);
        if (put_res == 0) {
            DEBUG("gnrc_netif: (re-)queued pkt %p\n", (void *)pkt);
            return;
        }
        else {
            LOG_WARNING("gnrc_netif: can't queue packet for sending, try sending\n");
            /* try to send anyway */
        }
    }
    /* hold in case device was busy to not having to rewrite *all* the link
     * layer implementations in case `gnrc_netif_pktq` is included */
    if (gnrc_netif_netdev_legacy_api(netif)) {
        gnrc_pktbuf_hold(pkt, 1);
    }
#endif /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */

    /* Record send in neighbor statistics if destination is unicast */
    if (IS_USED(MODULE_NETSTATS_NEIGHBOR)) {
        gnrc_netif_hdr_t *netif_hdr = pkt->data;
        if (netif_hdr->flags &
            (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
            DEBUG("l2 stats: Destination is multicast or unicast, NULL recorded\n");
            netstats_nb_record(&netif->netif, NULL, 0);
        } else {
            DEBUG("l2 stats: recording transmission\n");
            netstats_nb_record(&netif->netif,
                               gnrc_netif_hdr_get_dst_addr(netif_hdr),
                               netif_hdr->dst_l2addr_len);
        }
    }

    /* Split off the TX sync snip */
    gnrc_pktsnip_t *tx_sync = IS_USED(MODULE_GNRC_TX_SYNC)
                            ? gnrc_tx_sync_split(pkt) : NULL;
    int res = netif->ops->send(netif, pkt);

    /* For legacy netdevs (no confirm_send) TX is blocking, thus it is always
     * completed. For new netdevs (with confirm_send), TX is async. It is only
     * done if TX failed right away (res < 0).
     */
    if (gnrc_netif_netdev_legacy_api(netif) || (res != 0)) {
        _tx_done(netif, pkt, tx_sync, res, push_back);
    }
#if IS_USED(MODULE_NETDEV_NEW_API)
    else {
        /* new API *and* send() was a success --> block netif and memorize
         * frame to free memory later */
        netif->tx_pkt = pkt;
    }

    gnrc_pkt_append(pkt, tx_sync);
    if (IS_USED(MODULE_GNRC_NETIF_PKTQ) && push_back) {
        netif->flags |= GNRC_NETIF_FLAGS_TX_FROM_PKTQUEUE;
    }
#endif
}

static void *_gnrc_netif_thread(void *args)
{
    _netif_ctx_t *ctx = args;
    gnrc_netapi_opt_t *opt;
    gnrc_netif_t *netif;
    int res;
    msg_t reply = { .type = GNRC_NETAPI_MSG_TYPE_ACK };

    DEBUG("gnrc_netif: starting thread %i\n", thread_getpid());
    netif = ctx->netif;
    gnrc_netif_acquire(netif);
    netif->pid = thread_getpid();

    netif->event_isr.handler = _event_handler_isr;
#if IS_USED(MODULE_NETDEV_NEW_API)
    netif->event_tx_done.handler = _event_handler_tx_done;
#endif
    /* set up the event queue */
    event_queues_init(netif->evq, GNRC_NETIF_EVQ_NUMOF);

    /* setup the link-layer's message queue */
    msg_init_queue(netif->msg_queue, ARRAY_SIZE(netif->msg_queue));
    /* initialize low-level driver */
    ctx->result = netif->ops->init(netif);
    /* signal that driver init is done */
    mutex_unlock(&ctx->init_done);
    if (ctx->result < 0) {
        LOG_ERROR("gnrc_netif: init %u failed: %d\n", thread_getpid(), ctx->result);
        return NULL;
    }
#ifdef MODULE_NETSTATS_L2
    memset(&netif->stats, 0, sizeof(netstats_t));
#endif
    /* now let rest of GNRC use the interface */
    gnrc_netif_release(netif);
#if (CONFIG_GNRC_NETIF_MIN_WAIT_AFTER_SEND_US > 0U)
    uint32_t last_wakeup = ztimer_now(ZTIMER_USEC);
#endif

    while (1) {
        msg_t msg;
        /* msg will be filled by _process_events_await_msg.
         * The function will not return until a message has been received. */
        _process_events_await_msg(netif, &msg);

        /* dispatch netdev, MAC and gnrc_netapi messages */
        DEBUG("gnrc_netif: message %u\n", (unsigned)msg.type);
        switch (msg.type) {
#if IS_USED(MODULE_GNRC_NETIF_PKTQ)
            case GNRC_NETIF_PKTQ_DEQUEUE_MSG:
                DEBUG("gnrc_netif: send from packet send queue\n");
                _send_queued_pkt(netif);
                break;
#endif  /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("gnrc_netif: GNRC_NETDEV_MSG_TYPE_SND received\n");
                _send(netif, msg.content.ptr, false);
#if (CONFIG_GNRC_NETIF_MIN_WAIT_AFTER_SEND_US > 0U)
                ztimer_periodic_wakeup(
                        ZTIMER_USEC,
                        &last_wakeup,
                        CONFIG_GNRC_NETIF_MIN_WAIT_AFTER_SEND_US
                    );
                /* override last_wakeup in case last_wakeup +
                 * CONFIG_GNRC_NETIF_MIN_WAIT_AFTER_SEND_US was in the past */
                last_wakeup = ztimer_now(ZTIMER_USEC);
#endif
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                opt = msg.content.ptr;
#ifdef MODULE_NETOPT
                DEBUG("gnrc_netif: GNRC_NETAPI_MSG_TYPE_SET received. opt=%s\n",
                      netopt2str(opt->opt));
#else
                DEBUG("gnrc_netif: GNRC_NETAPI_MSG_TYPE_SET received. opt=%d\n",
                      opt->opt);
#endif
                /* set option for device driver */
                res = netif->ops->set(netif, opt);
                DEBUG("gnrc_netif: response of netif->ops->set(): %i\n", res);
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                opt = msg.content.ptr;
#ifdef MODULE_NETOPT
                DEBUG("gnrc_netif: GNRC_NETAPI_MSG_TYPE_GET received. opt=%s\n",
                      netopt2str(opt->opt));
#else
                DEBUG("gnrc_netif: GNRC_NETAPI_MSG_TYPE_GET received. opt=%d\n",
                      opt->opt);
#endif
                /* get option from device driver */
                res = netif->ops->get(netif, opt);
                DEBUG("gnrc_netif: response of netif->ops->get(): %i\n", res);
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            default:
                if (netif->ops->msg_handler) {
                    DEBUG("gnrc_netif: delegate message of type 0x%04x to "
                          "netif->ops->msg_handler()\n", msg.type);
                    netif->ops->msg_handler(netif, &msg);
                }
                else {
                    DEBUG("gnrc_netif: unknown message type 0x%04x"
                          "(no message handler defined)\n", msg.type);
                }
                break;
        }
    }
    /* never reached */
    return NULL;
}

static void _pass_on_packet(gnrc_pktsnip_t *pkt)
{
    /* throw away packet if no one is interested */
    if (!gnrc_netapi_dispatch_receive(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL,
                                      pkt)) {
        DEBUG("gnrc_netif: unable to forward packet of type %i\n", pkt->type);
        gnrc_pktbuf_release(pkt);
        return;
    }
}

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    gnrc_netif_t *netif = (gnrc_netif_t *)dev->context;

    if (event == NETDEV_EVENT_ISR) {
        event_post(&netif->evq[GNRC_NETIF_EVQ_INDEX_PRIO_LOW], &netif->event_isr);
    }
#if IS_USED(MODULE_NETDEV_NEW_API)
    else if (gnrc_netif_netdev_new_api(netif)
             && (event == NETDEV_EVENT_TX_COMPLETE)) {
        event_post(&netif->evq[GNRC_NETIF_EVQ_INDEX_PRIO_LOW],
                   &netif->event_tx_done);
    }
#endif
    else {
        DEBUG("gnrc_netif: event triggered -> %i\n", event);
        gnrc_pktsnip_t *pkt = NULL;
        switch (event) {
            case NETDEV_EVENT_LINK_UP:
                if (IS_USED(MODULE_GNRC_IPV6)) {
                    msg_t msg = { .type = GNRC_IPV6_NIB_IFACE_UP, .content = { .ptr = netif } };

                    msg_send(&msg, gnrc_ipv6_pid);
                }
                break;
            case NETDEV_EVENT_LINK_DOWN:
                if (IS_USED(MODULE_GNRC_IPV6)) {
                    msg_t msg = { .type = GNRC_IPV6_NIB_IFACE_DOWN, .content = { .ptr = netif } };

                    msg_send(&msg, gnrc_ipv6_pid);
                }
                break;
            case NETDEV_EVENT_RX_COMPLETE:
                pkt = netif->ops->recv(netif);
                /* send packet previously queued within netif due to the lower
                 * layer being busy.
                 * Further packets will be sent on later TX_COMPLETE */
                _send_queued_pkt(netif);
                if (pkt) {
                    _process_receive_stats(netif, pkt);
                    _pass_on_packet(pkt);
                }
                break;
#if IS_USED(MODULE_NETDEV_LEGACY_API)
#  if IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ)
            case NETDEV_EVENT_TX_COMPLETE:
            case NETDEV_EVENT_TX_COMPLETE_DATA_PENDING:
                /* send packet previously queued within netif due to the lower
                 * layer being busy.
                 * Further packets will be sent on later TX_COMPLETE or
                 * TX_MEDIUM_BUSY */
                _send_queued_pkt(netif);
#    if IS_USED(MODULE_NETSTATS_L2)
                /* we are the only ones supposed to touch this variable,
                 * so no acquire necessary */
                netif->stats.tx_success++;
#    endif  /* IS_USED(MODULE_NETSTATS_L2) */
                if (IS_USED(MODULE_NETSTATS_NEIGHBOR)) {
                    int8_t retries = -1;
                    dev->driver->get(dev, NETOPT_TX_RETRIES_NEEDED, &retries, sizeof(retries));
                    netstats_nb_update_tx(&netif->netif, NETSTATS_NB_SUCCESS, retries + 1);
                }
                break;
#  endif  /* IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ) */
#  if IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ) || \
      IS_USED(MODULE_NETSTATS_NEIGHBOR)
            case NETDEV_EVENT_TX_MEDIUM_BUSY:
            case NETDEV_EVENT_TX_NOACK:
                /* update neighbor statistics */
                if (IS_USED(MODULE_NETSTATS_NEIGHBOR)) {
                    int8_t retries = -1;
                    netstats_nb_result_t result;
                    if (event == NETDEV_EVENT_TX_NOACK) {
                        result = NETSTATS_NB_NOACK;
                        dev->driver->get(dev, NETOPT_TX_RETRIES_NEEDED, &retries, sizeof(retries));
                    } else {
                        result = NETSTATS_NB_BUSY;
                    }
                    netstats_nb_update_tx(&netif->netif, result, retries + 1);
                }
                /* send packet previously queued within netif due to the lower
                 * layer being busy.
                 * Further packets will be sent on later TX_COMPLETE or
                 * TX_MEDIUM_BUSY */
                _send_queued_pkt(netif);
#    if IS_USED(MODULE_NETSTATS_L2)
                /* we are the only ones supposed to touch this variable,
                 * so no acquire necessary */
                netif->stats.tx_failed++;
#    endif  /* IS_USED(MODULE_NETSTATS_L2) */
                break;
#  endif  /* IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ) */
#endif /* IS_USED(MODULE_NETDEV_LEGACY_API) */
            default:
                DEBUG("gnrc_netif: warning: unhandled event %u.\n", event);
        }
    }
}
/** @} */
