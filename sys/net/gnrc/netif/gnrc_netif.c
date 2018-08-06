/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
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

#include "bitfield.h"
#include "net/ethernet.h"
#include "net/ipv6.h"
#include "net/gnrc.h"
#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6.h"
#endif /* MODULE_GNRC_IPV6_NIB */
#ifdef MODULE_NETSTATS_IPV6
#include "net/netstats.h"
#endif
#include "log.h"
#include "sched.h"

#include "net/gnrc/netif.h"
#include "net/gnrc/netif/internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _NETIF_NETAPI_MSG_QUEUE_SIZE    (8)

static gnrc_netif_t _netifs[GNRC_NETIF_NUMOF];

static void _update_l2addr_from_dev(gnrc_netif_t *netif);
static void _configure_netdev(netdev_t *dev);
static void *_gnrc_netif_thread(void *args);
static void _event_cb(netdev_t *dev, netdev_event_t event);

gnrc_netif_t *gnrc_netif_create(char *stack, int stacksize, char priority,
                                const char *name, netdev_t *netdev,
                                const gnrc_netif_ops_t *ops)
{
    gnrc_netif_t *netif = NULL;
    int res;

    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        if (_netifs[i].dev == netdev) {
            return &_netifs[i];
        }
        if ((netif == NULL) && (_netifs[i].ops == NULL)) {
            netif = &_netifs[i];
        }
    }
    assert(netif != NULL);
    rmutex_init(&netif->mutex);
    netif->ops = ops;
    assert(netif->dev == NULL);
    netif->dev = netdev;
    res = thread_create(stack, stacksize, priority, THREAD_CREATE_STACKTEST,
                        _gnrc_netif_thread, (void *)netif, name);
    (void)res;
    assert(res > 0);
    return netif;
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
    assert((prev == NULL) || (prev >= _netifs));
    for (const gnrc_netif_t *netif = (prev == NULL) ? _netifs : (prev + 1);
         netif < (_netifs + GNRC_NETIF_NUMOF); netif++) {
        if (netif->ops != NULL) {
            /* we don't care about external modification */
            return (gnrc_netif_t *)netif;
        }
    }
    return NULL;
}

int gnrc_netif_get_from_netdev(gnrc_netif_t *netif, gnrc_netapi_opt_t *opt)
{
    int res = -ENOTSUP;

    gnrc_netif_acquire(netif);
    switch (opt->opt) {
        case NETOPT_HOP_LIMIT:
            assert(opt->data_len == sizeof(uint8_t));
            *((uint8_t *)opt->data) = netif->cur_hl;
            res = sizeof(uint8_t);
            break;
        case NETOPT_STATS:
            /* XXX discussed this with Oleg, it's supposed to be a pointer */
            switch ((int16_t)opt->context) {
#if defined(MODULE_NETSTATS_IPV6) && defined(MODULE_GNRC_IPV6)
                case NETSTATS_IPV6:
                    assert(opt->data_len == sizeof(netstats_t *));
                    *((netstats_t **)opt->data) = &netif->ipv6.stats;
                    res = sizeof(&netif->ipv6.stats);
                    break;
#endif
                default:
                    /* take from device */
                    break;
            }
            break;
#ifdef MODULE_GNRC_IPV6
        case NETOPT_IPV6_ADDR: {
                assert(opt->data_len >= sizeof(ipv6_addr_t));
                ipv6_addr_t *tgt = opt->data;

                res = 0;
                for (unsigned i = 0;
                     (res < (int)opt->data_len) && (i < GNRC_NETIF_IPV6_ADDRS_NUMOF);
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
                     (res < (int)opt->data_len) && (i < GNRC_NETIF_IPV6_ADDRS_NUMOF);
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
                     (res < (int)opt->data_len) && (i < GNRC_NETIF_IPV6_GROUPS_NUMOF);
                     i++) {
                    if (!ipv6_addr_is_unspecified(&netif->ipv6.groups[i])) {
                        memcpy(tgt, &netif->ipv6.groups[i], sizeof(ipv6_addr_t));
                        res += sizeof(ipv6_addr_t);
                        tgt++;
                    }
                }
            }
            break;
        case NETOPT_IPV6_IID:
            assert(opt->data_len >= sizeof(eui64_t));
            if (gnrc_netif_ipv6_get_iid(netif, opt->data) == 0) {
                res = sizeof(eui64_t);
            }
            break;
        case NETOPT_MAX_PACKET_SIZE:
            if (opt->context == GNRC_NETTYPE_IPV6) {
                assert(opt->data_len == sizeof(uint16_t));
                *((uint16_t *)opt->data) = netif->ipv6.mtu;
                res = sizeof(uint16_t);
            }
            /* else ask device */
            break;
#if GNRC_IPV6_NIB_CONF_ROUTER
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
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#endif  /* MODULE_GNRC_IPV6 */
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
        case NETOPT_6LO_IPHC:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) = (netif->flags & GNRC_NETIF_FLAGS_6LO_HC) ?
                                              NETOPT_ENABLE : NETOPT_DISABLE;
            res = sizeof(netopt_enable_t);
            break;
#endif  /* MODULE_GNRC_SIXLOWPAN_IPHC */
        default:
            break;
    }
    if (res == -ENOTSUP) {
        res = netif->dev->driver->get(netif->dev, opt->opt, opt->data, opt->data_len);
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
#ifdef MODULE_GNRC_IPV6
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
        case NETOPT_MAX_PACKET_SIZE:
            if (opt->context == GNRC_NETTYPE_IPV6) {
                assert(opt->data_len == sizeof(uint16_t));
                netif->ipv6.mtu = *((uint16_t *)opt->data);
                res = sizeof(uint16_t);
            }
            /* else set device */
            break;
#if GNRC_IPV6_NIB_CONF_ROUTER
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
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#endif  /* MODULE_GNRC_IPV6 */
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
                case NETOPT_STATE:
                    if (*((netopt_state_t *)opt->data) == NETOPT_STATE_RESET) {
                        _configure_netdev(netif->dev);
                    }
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

static inline char _half_byte_to_char(uint8_t half_byte)
{
    return (half_byte < 10) ? ('0' + half_byte) : ('a' + (half_byte - 10));
}

char *gnrc_netif_addr_to_str(const uint8_t *addr, size_t addr_len, char *out)
{
    char *res = out;

    assert((out != NULL) && ((addr != NULL) || (addr_len == 0U)));
    out[0] = '\0';
    for (size_t i = 0; i < addr_len; i++) {
        *(out++) = _half_byte_to_char(*(addr) >> 4);
        *(out++) = _half_byte_to_char(*(addr++) & 0xf);
        *(out++) = (i == (addr_len - 1)) ? '\0' : ':';
    }
    return res;
}

static inline int _dehex(char c, int default_)
{
    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    }
    else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    else {
        return default_;
    }
}

size_t gnrc_netif_addr_from_str(const char *str, uint8_t *out)
{
    /* Walk over str from the end. */
    /* Take two chars a time as one hex value (%hhx). */
    /* Leading zeros can be omitted. */
    /* Every non-hexadimal character is a delimiter. */
    /* Leading, tailing and adjacent delimiters are forbidden. */
    const char *end_str = str;
    uint8_t *out_end = out;
    size_t count = 0;
    int assert_cell = 1;

    assert(out != NULL);
    if ((str == NULL) || (str[0] == '\0')) {
        return 0;
    }
    /* find end of string */
    while (end_str[1]) {
        ++end_str;
    }
    while (end_str >= str) {
        int a = 0, b = _dehex(*end_str--, -1);

        if (b < 0) {
            if (assert_cell) {
                return 0;
            }
            else {
                assert_cell = 1;
                continue;
            }
        }
        assert_cell = 0;
        if (end_str >= str) {
            a = _dehex(*end_str--, 0);
        }
        count++;
        *out_end++ = (a << 4) | b;
    }
    if (assert_cell) {
        return 0;
    }
    /* out is reversed */
    while (out < --out_end) {
        uint8_t tmp = *out_end;
        *out_end = *out;
        *out++ = tmp;
    }
    return count;
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

#ifdef MODULE_GNRC_IPV6
static inline bool _addr_anycast(const gnrc_netif_t *netif, unsigned idx);
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
 * @param[in] filter    a bitfield with the bits at the position equal to the
 *                      indexes of the addresses you want to include in the
 *                      search set to one. NULL for all addresses
 *
 * @return  index of the best match for @p addr
 * @return  -1 if no match was found
 *
 * @pre `netif != NULL` and `addr != NULL`
 */
static int _match_to_idx(const gnrc_netif_t *netif,
                        const ipv6_addr_t *addr,
                        const uint8_t *filter);
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
    for (unsigned i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
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
#if GNRC_IPV6_NIB_CONF_ARSM
    ipv6_addr_t sol_nodes;
    int res;

    /* TODO: SHOULD delay join between 0 and MAX_RTR_SOLICITATION_DELAY
     * for SLAAC */
    ipv6_addr_set_solicited_nodes(&sol_nodes, addr);
    res = gnrc_netif_ipv6_group_join_internal(netif, &sol_nodes);
    if (res < 0) {
        DEBUG("gnrc_netif: Can't join solicited-nodes of %s on interface %" PRIkernel_pid "\n",
              ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
              netif->pid);
        return res;
    }
#endif /* GNRC_IPV6_NIB_CONF_ARSM */
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
    }
#if GNRC_IPV6_NIB_CONF_SLAAC
    else if (!gnrc_netif_is_6ln(netif)) {
        /* cast to remove const qualifier (will still be used NIB internally as
         * const) */
        msg_t msg = { .type = GNRC_IPV6_NIB_DAD,
                      .content = { .ptr = &netif->ipv6.addrs[idx] } };

        msg_send(&msg, gnrc_ipv6_pid);
    }
#endif
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
    for (unsigned i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
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
    int idx = _match_to_idx(netif, addr, NULL);
    gnrc_netif_release(netif);
    return idx;
}

ipv6_addr_t *gnrc_netif_ipv6_addr_best_src(gnrc_netif_t *netif,
                                           const ipv6_addr_t *dst,
                                           bool ll_only)
{
    ipv6_addr_t *best_src = NULL;

    BITFIELD(candidate_set, GNRC_NETIF_IPV6_ADDRS_NUMOF);

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

int gnrc_netif_ipv6_group_join_internal(gnrc_netif_t *netif,
                                        const ipv6_addr_t *addr)
{
    unsigned idx = UINT_MAX;

    gnrc_netif_acquire(netif);
    for (unsigned i = 0; i < GNRC_NETIF_IPV6_GROUPS_NUMOF; i++) {
        if (ipv6_addr_equal(&netif->ipv6.groups[i], addr)) {
            gnrc_netif_release(netif);
            return i;
        }
        if ((idx == UINT_MAX) && (ipv6_addr_is_unspecified(&netif->ipv6.groups[i]))) {
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
    int idx;

    assert((netif != NULL) && (addr != NULL));
    gnrc_netif_acquire(netif);
    idx = _group_idx(netif, addr);
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

#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_CC110X) || \
    defined(MODULE_NRFMIN) || defined(MODULE_XBEE)
static void _create_iid_from_short(const gnrc_netif_t *netif, eui64_t *eui64)
{
    const unsigned offset = sizeof(eui64_t) - netif->l2addr_len;

    assert(netif->l2addr_len <= 3);
    memset(eui64->uint8, 0, sizeof(eui64->uint8));
    eui64->uint8[3] = 0xff;
    eui64->uint8[4] = 0xfe;
    memcpy(&eui64->uint8[offset], netif->l2addr, netif->l2addr_len);
}
#endif /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_CC110X) ||
        * defined(MODULE_NRFMIN) || defined(MODULE_XBEE) */

int gnrc_netif_ipv6_get_iid(gnrc_netif_t *netif, eui64_t *eui64)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
        switch (netif->device_type) {
#ifdef MODULE_NETDEV_ETH
            case NETDEV_TYPE_ETHERNET:
                assert(netif->l2addr_len == ETHERNET_ADDR_LEN);
                eui64->uint8[0] = netif->l2addr[0] ^ 0x02;
                eui64->uint8[1] = netif->l2addr[1];
                eui64->uint8[2] = netif->l2addr[2];
                eui64->uint8[3] = 0xff;
                eui64->uint8[4] = 0xfe;
                eui64->uint8[5] = netif->l2addr[3];
                eui64->uint8[6] = netif->l2addr[4];
                eui64->uint8[7] = netif->l2addr[5];
                return 0;
#endif
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
            case NETDEV_TYPE_IEEE802154:
                switch (netif->l2addr_len) {
                    case IEEE802154_SHORT_ADDRESS_LEN:
                        _create_iid_from_short(netif, eui64);
                        return 0;
                    case IEEE802154_LONG_ADDRESS_LEN:
                        memcpy(eui64, netif->l2addr, sizeof(eui64_t));
                        eui64->uint8[0] ^= 0x02;
                        return 0;
                    default:
                        /* this should not happen */
                        assert(false);
                        break;
                }
                break;
#endif
#ifdef MODULE_NORDIC_SOFTDEVICE_BLE
            case NETDEV_TYPE_BLE:
                assert(netif->l2addr_len == sizeof(eui64_t));
                memcpy(eui64, netif->l2addr, sizeof(eui64_t));
                eui64->uint8[0] ^= 0x02;
                return 0;
#endif
#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
            case NETDEV_TYPE_CC110X:
            case NETDEV_TYPE_NRFMIN:
                _create_iid_from_short(netif, eui64);
                return 0;
#endif
            default:
                (void)eui64;
                break;
        }
    }
#endif
    return -ENOTSUP;
}

static inline bool _addr_anycast(const gnrc_netif_t *netif, unsigned idx)
{
    return (netif->ipv6.addrs_flags[idx] & GNRC_NETIF_IPV6_ADDRS_FLAGS_ANYCAST);
}

static int _idx(const gnrc_netif_t *netif, const ipv6_addr_t *addr, bool mcast)
{
    if (!ipv6_addr_is_unspecified(addr)) {
        const ipv6_addr_t *iplist = (mcast) ? netif->ipv6.groups : netif->ipv6.addrs;
        unsigned ipmax = (mcast) ? GNRC_NETIF_IPV6_GROUPS_NUMOF : GNRC_NETIF_IPV6_ADDRS_NUMOF;
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

    int n = _match_to_idx(netif, addr, NULL);
    return (n >= 0) ? ipv6_addr_match_prefix(&(netif->ipv6.addrs[n]), addr) : 0;
}

static int _match_to_idx(const gnrc_netif_t *netif,
                         const ipv6_addr_t *addr,
                         const uint8_t *filter)
{
    assert((netif != NULL) && (addr != NULL));

    int idx = -1;
    unsigned best_match = 0;
    for (int i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        unsigned match;

        if ((netif->ipv6.addrs_flags[i] == 0) ||
            ((filter != NULL) && _addr_anycast(netif, i)) ||
            /* discard const intentionally */
            ((filter != NULL) && !(bf_isset((uint8_t *)filter, i)))) {
            continue;
        }
        match = ipv6_addr_match_prefix(&(netif->ipv6.addrs[i]), addr);
        if (((match > 64U) || !ipv6_addr_is_link_local(&(netif->ipv6.addrs[i]))) &&
            (match >= best_match)) {
            idx = i;
            best_match = match;
        }
    }
    if (idx != -1) {
        DEBUG("gnrc_netif: Found %s on interface %" PRIkernel_pid " matching ",
              ipv6_addr_to_str(addr_str, &netif->ipv6.addrs[idx],
                               sizeof(addr_str)),
              netif->pid);
        DEBUG("%s by %u bits (used as source address = %s)\n",
              ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
              best_match,
              (filter != NULL) ? "true" : "false");
    }
    else {
        DEBUG("gnrc_netif: Did not found any address on interface %" PRIkernel_pid
              " matching %s (used as source address = %s)\n",
              netif->pid,
              ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
              (filter != NULL) ? "true" : "false");
    }
    return idx;
}

static uint8_t _get_scope(const ipv6_addr_t *addr)
{
    if (ipv6_addr_is_link_local(addr)) {
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
    for (int i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        const ipv6_addr_t *tmp = &(netif->ipv6.addrs[i]);

        DEBUG("Checking address: %s\n",
              ipv6_addr_to_str(addr_str, tmp, sizeof(addr_str)));
        /* "In any case, multicast addresses and the unspecified address MUST NOT
         *  be included in a candidate set."
         */
        if ((netif->ipv6.addrs_flags[i] == 0) ||
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
#define RULE_2A_PTS         (4)
/* number of "points" assigned to an source address candidate with smaller scope
 * than destination address */
#define RULE_2B_PTS         (2)
/* number of "points" assigned to an source address candidate in preferred state */
#define RULE_3_PTS          (1)

static ipv6_addr_t *_src_addr_selection(gnrc_netif_t *netif,
                                        const ipv6_addr_t *dst,
                                        uint8_t *candidate_set)
{
    /* create temporary set for assigning "points" to candidates winning in the
     * corresponding rules.
     */
    uint8_t winner_set[GNRC_NETIF_IPV6_ADDRS_NUMOF];

    memset(winner_set, 0, GNRC_NETIF_IPV6_ADDRS_NUMOF);
    uint8_t max_pts = 0;
    /* _create_candidate_set() assures that `dst` is not unspecified and if
     * `dst` is loopback rule 1 will fire anyway.  */
    uint8_t dst_scope = _get_scope(dst);

    DEBUG("finding the best match within the source address candidates\n");
    for (unsigned i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        ipv6_addr_t *ptr = &(netif->ipv6.addrs[i]);

        DEBUG("Checking address: %s\n",
              ipv6_addr_to_str(addr_str, ptr, sizeof(addr_str)));
        /* entries which are not  part of the candidate set can be ignored */
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
        /* both link local */
        uint8_t candidate_scope = _get_scope(ptr);
        if (candidate_scope == dst_scope) {
            DEBUG("winner for rule 2 (same scope) found\n");
            winner_set[i] += RULE_2A_PTS;
            if (winner_set[i] > max_pts) {
                max_pts = RULE_2A_PTS;
            }
        }
        else if (candidate_scope < dst_scope) {
            DEBUG("winner for rule 2 (smaller scope) found\n");
            winner_set[i] += RULE_2B_PTS;
            if (winner_set[i] > max_pts) {
                max_pts = winner_set[i];
            }
        }
        /* Rule 3: Avoid deprecated addresses. */
        if (_get_state(netif, i) == GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_DEPRECATED) {
            DEBUG("winner for rule 3 found\n");
            winner_set[i] += RULE_3_PTS;
            if (winner_set[i] > max_pts) {
                max_pts = winner_set[i];
            }
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
    }
    /* reset candidate set to mark winners */
    memset(candidate_set, 0, (GNRC_NETIF_IPV6_ADDRS_NUMOF + 7) / 8);
    /* check if we have a clear winner */
    /* collect candidates with maximum points */
    for (int i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        if (winner_set[i] == max_pts) {
            bf_set(candidate_set, i);
        }
    }
    /* otherwise apply rule 8: Use longest matching prefix. */
    int idx = _match_to_idx(netif, dst, candidate_set);
    return (idx < 0) ? NULL : &netif->ipv6.addrs[idx];
}
#endif  /* MODULE_GNRC_IPV6 */

#if (GNRC_NETIF_NUMOF > 1) || !defined(MODULE_GNRC_SIXLOWPAN)
bool gnrc_netif_is_6ln(const gnrc_netif_t *netif)
{
    switch (netif->device_type) {
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_CC110X:
        case NETDEV_TYPE_BLE:
        case NETDEV_TYPE_NRFMIN:
            return true;
        default:
            return false;
    }
}
#endif  /* (GNRC_NETIF_NUMOF > 1) || !defined(MODULE_GNRC_SIXLOWPAN) */

static void _update_l2addr_from_dev(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    int res;
    netopt_t opt = NETOPT_ADDRESS;

    switch (netif->device_type) {
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) \
    || defined(MODULE_NORDIC_SOFTDEVICE_BLE)
        case NETDEV_TYPE_BLE:
        case NETDEV_TYPE_IEEE802154: {
                uint16_t tmp;

                res = dev->driver->get(dev, NETOPT_SRC_LEN, &tmp, sizeof(tmp));
                assert(res == sizeof(tmp));
                netif->l2addr_len = (uint8_t)tmp;
                if (tmp == IEEE802154_LONG_ADDRESS_LEN) {
                    opt = NETOPT_ADDRESS_LONG;
                }
            }
            break;
#endif
        default:
            break;
    }
    res = dev->driver->get(dev, opt, netif->l2addr,
                           sizeof(netif->l2addr));
    if (res != -ENOTSUP) {
        netif->flags |= GNRC_NETIF_FLAGS_HAS_L2ADDR;
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
    switch (netif->device_type) {
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_NRFMIN) || defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_NRFMIN:
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
#endif
#ifdef MODULE_GNRC_IPV6
            res = dev->driver->get(dev, NETOPT_MAX_PACKET_SIZE, &tmp, sizeof(tmp));
            assert(res == sizeof(tmp));
#ifdef MODULE_GNRC_SIXLOWPAN
            netif->ipv6.mtu = IPV6_MIN_MTU;
            netif->sixlo.max_frag_size = tmp;
#else
            netif->ipv6.mtu = tmp;
#endif
#endif
            break;
#endif  /* MODULE_NETDEV_IEEE802154 */
#ifdef MODULE_NETDEV_ETH
        case NETDEV_TYPE_ETHERNET:
#ifdef MODULE_GNRC_IPV6
            netif->ipv6.mtu = ETHERNET_DATA_LEN;
#endif
            break;
#endif
#ifdef MODULE_NORDIC_SOFTDEVICE_BLE
        case NETDEV_TYPE_BLE:
            netif->ipv6.mtu = IPV6_MIN_MTU;
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
#endif
            break;
#endif
        default:
#ifdef MODULE_GNRC_IPV6
            res = dev->driver->get(dev, NETOPT_MAX_PACKET_SIZE, &tmp, sizeof(tmp));
            if (res < 0) {
                /* assume maximum possible transition unit */
                netif->ipv6.mtu = UINT16_MAX;
            }
            else {
                netif->ipv6.mtu = tmp;
            }
#endif
            break;
    }
    _update_l2addr_from_dev(netif);
}

static void _configure_netdev(netdev_t *dev)
{
    /* Enable RX- and TX-complete interrupts */
    static const netopt_enable_t enable = NETOPT_ENABLE;
    int res = dev->driver->set(dev, NETOPT_RX_END_IRQ, &enable, sizeof(enable));
    if (res < 0) {
        DEBUG("gnrc_netif: enable NETOPT_RX_END_IRQ failed: %d\n", res);
    }
#ifdef MODULE_NETSTATS_L2
    res = dev->driver->set(dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));
    if (res < 0) {
        DEBUG("gnrc_netif: enable NETOPT_TX_END_IRQ failed: %d\n", res);
    }
#endif
}

static void *_gnrc_netif_thread(void *args)
{
    gnrc_netapi_opt_t *opt;
    gnrc_netif_t *netif;
    netdev_t *dev;
    int res;
    msg_t reply = { .type = GNRC_NETAPI_MSG_TYPE_ACK };
    msg_t msg, msg_queue[_NETIF_NETAPI_MSG_QUEUE_SIZE];

    DEBUG("gnrc_netif: starting thread %i\n", sched_active_pid);
    netif = args;
    gnrc_netif_acquire(netif);
    dev = netif->dev;
    netif->pid = sched_active_pid;
    /* setup the link-layer's message queue */
    msg_init_queue(msg_queue, _NETIF_NETAPI_MSG_QUEUE_SIZE);
    /* register the event callback with the device driver */
    dev->event_callback = _event_cb;
    dev->context = netif;
    /* initialize low-level driver */
    dev->driver->init(dev);
    _configure_netdev(dev);
    _init_from_device(netif);
    netif->cur_hl = GNRC_NETIF_DEFAULT_HL;
#ifdef MODULE_GNRC_IPV6_NIB
    gnrc_ipv6_nib_init_iface(netif);
#endif
    if (netif->ops->init) {
        netif->ops->init(netif);
    }
    /* now let rest of GNRC use the interface */
    gnrc_netif_release(netif);

    while (1) {
        DEBUG("gnrc_netif: waiting for incoming messages\n");
        msg_receive(&msg);
        /* dispatch netdev, MAC and gnrc_netapi messages */
        switch (msg.type) {
            case NETDEV_MSG_TYPE_EVENT:
                DEBUG("gnrc_netif: GNRC_NETDEV_MSG_TYPE_EVENT received\n");
                dev->driver->isr(dev);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("gnrc_netif: GNRC_NETDEV_MSG_TYPE_SND received\n");
                res = netif->ops->send(netif, msg.content.ptr);
                if (res < 0) {
                    DEBUG("gnrc_netif: error sending packet %p (code: %u)\n",
                          msg.content.ptr, res);
                }
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
    if (!gnrc_netapi_dispatch_receive(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("gnrc_netif: unable to forward packet of type %i\n", pkt->type);
        gnrc_pktbuf_release(pkt);
        return;
    }
}

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    gnrc_netif_t *netif = (gnrc_netif_t *) dev->context;

    if (event == NETDEV_EVENT_ISR) {
        msg_t msg = { .type = NETDEV_MSG_TYPE_EVENT,
                      .content = { .ptr = netif } };

        if (msg_send(&msg, netif->pid) <= 0) {
            puts("gnrc_netif: possibly lost interrupt.");
        }
    }
    else {
        DEBUG("gnrc_netif: event triggered -> %i\n", event);
        switch (event) {
            case NETDEV_EVENT_RX_COMPLETE: {
                    gnrc_pktsnip_t *pkt = netif->ops->recv(netif);

                    if (pkt) {
                        _pass_on_packet(pkt);
                    }
                }
                break;
#ifdef MODULE_NETSTATS_L2
            case NETDEV_EVENT_TX_MEDIUM_BUSY:
                /* we are the only ones supposed to touch this variable,
                 * so no acquire necessary */
                dev->stats.tx_failed++;
                break;
            case NETDEV_EVENT_TX_COMPLETE:
                /* we are the only ones supposed to touch this variable,
                 * so no acquire necessary */
                dev->stats.tx_success++;
                break;
#endif
            default:
                DEBUG("gnrc_netif: warning: unhandled event %u.\n", event);
        }
    }
}
/** @} */
