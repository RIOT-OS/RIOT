/*
 * Copyright (C) 2018 Freie Universität Berlin
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
 */

#include <kernel_defines.h>
#include <stdbool.h>

#include "log.h"
#include "luid.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif/internal.h"

#include "_nib-6ln.h"
#include "_nib-arsm.h"
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC_TEMPORARY_ADDRESSES)
#include "_nib-slaac.h"
#include "evtimer.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN) || IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC)
static char addr_str[IPV6_ADDR_MAX_STR_LEN];

void _auto_configure_addr(gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                          uint8_t pfx_len)
{
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;
    int idx;
    uint8_t flags = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE;

#if !IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC)
    if (!gnrc_netif_is_6ln(netif)) {
        LOG_WARNING("SLAAC not activated; will not auto-configure IPv6 address "
                         "for interface %u.\n"
                    "    Use CONFIG_GNRC_IPV6_NIB_SLAAC=1 to activate.\n",
                    netif->pid);
        return;
    }
#endif
    if (!(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR)) {
        DEBUG("nib: interface %i has no link-layer addresses\n", netif->pid);
        return;
    }
    DEBUG("nib: add address based on %s/%u automatically to interface %u\n",
          ipv6_addr_to_str(addr_str, pfx, sizeof(addr_str)),
          pfx_len, netif->pid);
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    bool new_address = false;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
    if (gnrc_netif_ipv6_get_iid(netif, (eui64_t *)&addr.u64[1]) < 0) {
        DEBUG("nib: Can't get IID on interface %u\n", netif->pid);
        return;
    }
    ipv6_addr_init_prefix(&addr, pfx, pfx_len);
    if ((idx = gnrc_netif_ipv6_addr_idx(netif, &addr)) < 0) {
        if ((idx = gnrc_netif_ipv6_addr_add_internal(netif, &addr, pfx_len,
                                                     flags)) < 0) {
            DEBUG("nib: Can't add link-local address on interface %u\n",
                  netif->pid);
            return;
        }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
        new_address = true;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
    }

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    /* mark link-local addresses as valid on 6LN */
    if (gnrc_netif_is_6ln(netif) && ipv6_addr_is_link_local(pfx)) {
        /* don't do this beforehand or risk a deadlock:
         *  - gnrc_netif_ipv6_addr_add_internal() adds VALID (i.e. manually configured
         *    addresses to the prefix list locking the NIB's mutex which is already
         *    locked here) */
        netif->ipv6.addrs_flags[idx] &= ~GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK;
        netif->ipv6.addrs_flags[idx] |= GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID;
        gnrc_netif_ipv6_bus_post(netif, GNRC_IPV6_EVENT_ADDR_VALID, &netif->ipv6.addrs[idx]);
    }
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    if (new_address && gnrc_netif_is_6ln(netif) &&
        !gnrc_netif_is_6lbr(netif)) {
        _handle_rereg_address(&netif->ipv6.addrs[idx]);
    }
#else   /* CONFIG_GNRC_IPV6_NIB_6LN */
    (void)idx;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
}
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN || CONFIG_GNRC_IPV6_NIB_SLAAC */

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC_TEMPORARY_ADDRESSES)
int32_t _generate_temporary_addr(gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                                 const uint32_t pfx_pref_ltime,
                                 const uint8_t retries,
                                 int *idx)
{
    DEBUG("nib: add temporary address based on %s/%u automatically to interface %u\n",
          ipv6_addr_to_str(addr_str, pfx, sizeof(addr_str)),
          64, netif->pid);

    if (pfx_pref_ltime <= _get_netif_regen_advance(netif)) {
        /* While RIOT's implementation design differs from the RFC implementation design,
         * this check is here to fulfill the constraints imposed by
         * https://www.rfc-editor.org/rfc/rfc8981.html#section-3.4-3.4.2.2
         * in combination with the next bullet point ("5.")
         *
         * It also covers/fulfills the requirement
         * "Note that if a temporary address becomes deprecated as result of
         * processing a Prefix Information option with a zero preferred lifetime,
         * then a new temporary address MUST NOT be generated"
         */
        LOG_WARNING("nib: Abort adding temporary address "
              "because prefix's preferred lifetime too short (%"PRIu32" <= %"PRIu32")\n",
              pfx_pref_ltime, _get_netif_regen_advance(netif));
        return -1;
    }
    const uint32_t ta_max_pref_lft = TEMP_PREFERRED_LIFETIME
            - random_uint32_range(0, MAX_DESYNC_FACTOR + 1);
    if (ta_max_pref_lft <= _get_netif_regen_advance(netif)) {
        /* https://www.rfc-editor.org/rfc/rfc8981.html#section-3.4-3.5 */
        LOG_ERROR("nib: Abort adding temporary address because configured "
              "TEMP_PREFERRED_LIFETIME (%lu) too short or MAX_DESYNC_FACTOR too high (%lu)\n",
              TEMP_PREFERRED_LIFETIME, MAX_DESYNC_FACTOR);

        /* in other words, as per
         * https://www.rfc-editor.org/rfc/rfc8981.html#section-3.8-7.2 */
        assert(MAX_DESYNC_FACTOR < TEMP_PREFERRED_LIFETIME - _get_netif_regen_advance(netif));

        return -1;
    }

    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_init_prefix(&addr, pfx, SLAAC_PREFIX_LENGTH);
    do {
        _ipv6_get_random_iid((eui64_t *) &addr.u64[1]);
    } while (gnrc_netif_ipv6_addr_idx(netif, &addr) >= 0);

    if (gnrc_ipv6_nib_pl_set(netif->pid, &addr,
                             IPV6_ADDR_BIT_LEN,
                             TEMP_VALID_LIFETIME,
                             ta_max_pref_lft) != 0) {
        LOG_WARNING("nib: Abort adding temporary address because prefix list full\n");
        return -1;
    }
    int index;
    uint8_t flags = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE
            | (retries << GNRC_NETIF_IPV6_ADDRS_FLAGS_IDGEN_RETRIES_POS);
    if ((index = gnrc_netif_ipv6_addr_add_internal(netif,
                                                   &addr,
                                                   IPV6_ADDR_BIT_LEN,
                                                   flags)) < 0) {
        LOG_WARNING("nib: Abort adding temporary address, adding address failed\n");
        //remove the just created prefix again
        gnrc_ipv6_nib_pl_del(netif->pid, &addr, IPV6_ADDR_BIT_LEN);
        return -1;
    }
    if (idx != NULL) {
        *idx = index;
    }

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    if (gnrc_netif_is_6ln(netif) &&
        !gnrc_netif_is_6lbr(netif)) {
        _handle_rereg_address(&netif->ipv6.addrs[index]);
    }
#endif

    return ta_max_pref_lft;
}

bool is_temporary_addr(const gnrc_netif_t *netif, const ipv6_addr_t *addr) {
    return gnrc_ipv6_nib_pl_has_prefix(netif->pid, addr, IPV6_ADDR_BIT_LEN);
}

bool _iid_is_iana_reserved(const eui64_t *iid)
{
    return (iid->uint64.u64 == htonll(0))
    || (iid->uint32[0].u32 == htonl(0x02005eff) && iid->uint8[4] == 0xfe)
    || (iid->uint32[0].u32 == htonl(0xfdffffff)
        && iid->uint16[2].u16 == htons(0xffff)
        && iid->uint8[6] == 0xff && (iid->uint8[7] & 0x80));
}

void _ipv6_get_random_iid(eui64_t *iid)
{
    do {
        random_bytes(iid->uint8, 8);
    } while (_iid_is_iana_reserved(iid));
}

uint32_t _get_netif_regen_advance(const gnrc_netif_t *netif)
{
    return 2 + (TEMP_IDGEN_RETRIES *
    (gnrc_netif_ipv6_dad_transmits(netif) * (netif->ipv6.retrans_time / 1000))
    );
}

bool _get_slaac_prefix_pref_until(const gnrc_netif_t *netif,
                                  const ipv6_addr_t *addr,
                                  uint32_t *slaac_prefix_pref_until) {
    void *state = NULL;
    gnrc_ipv6_nib_pl_t ple;
    while (gnrc_ipv6_nib_pl_iter(netif->pid, &state, &ple)) {
        if (ple.pfx_len == SLAAC_PREFIX_LENGTH
            && ipv6_addr_match_prefix(&ple.pfx, addr) >= ple.pfx_len) {
            *slaac_prefix_pref_until = ple.pref_until;
            return true;
        }
    }
    return false;
}

bool _iter_slaac_prefix_to_temp_addr(const gnrc_netif_t *netif,
                                     const ipv6_addr_t *slaac_pfx, void *state,
                                     ipv6_addr_t *next_temp_addr) {
    gnrc_ipv6_nib_pl_t ple;
    while (gnrc_ipv6_nib_pl_iter(netif->pid, &state, &ple)) {
        if (ple.pfx_len == IPV6_ADDR_BIT_LEN /* is temp addr. prefix */
            && ipv6_addr_match_prefix(&ple.pfx, slaac_pfx) >= SLAAC_PREFIX_LENGTH) {
            *next_temp_addr = ple.pfx;
            return true;
        }
    }
    return false;
}
#endif

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC)
static bool _try_l2addr_reconfiguration(gnrc_netif_t *netif)
{
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    uint16_t hwaddr_len;

    if (gnrc_netapi_get(netif->pid, NETOPT_SRC_LEN, 0, &hwaddr_len,
                        sizeof(hwaddr_len)) < 0) {
        return false;
    }
    luid_get(hwaddr, hwaddr_len);
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    if (hwaddr_len == IEEE802154_LONG_ADDRESS_LEN) {
        if (gnrc_netapi_set(netif->pid, NETOPT_ADDRESS_LONG, 0, hwaddr,
                            hwaddr_len) < 0) {
            return false;
        }
    }
    else
#endif
    if (gnrc_netapi_set(netif->pid, NETOPT_ADDRESS, 0, hwaddr,
                        hwaddr_len) < 0) {
        return false;
    }
    return true;
}

static bool _try_addr_reconfiguration(gnrc_netif_t *netif)
{
    eui64_t orig_iid;
    bool remove_old = false, hwaddr_reconf;

    if (gnrc_netif_ipv6_get_iid(netif, &orig_iid) > 0) {
        remove_old = true;
    }
    /* seize netif to netif thread since _try_l2addr_reconfiguration uses
     * gnrc_netapi_get()/gnrc_netapi_set(). Since these are synchronous this is
     * safe */
    gnrc_netif_release(netif);
    /* reacquire netif for IPv6 address reconfiguraton */
    hwaddr_reconf = _try_l2addr_reconfiguration(netif);
    gnrc_netif_acquire(netif);
    if (hwaddr_reconf) {
        if (remove_old) {
            for (unsigned i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
                ipv6_addr_t *addr = &netif->ipv6.addrs[i];
                if (addr->u64[1].u64 == orig_iid.uint64.u64) {
                    gnrc_netif_ipv6_addr_remove_internal(netif, addr);
                }
            }
        }
        DEBUG("nib: Changed hardware address, due to DAD\n");
        _auto_configure_addr(netif, &ipv6_addr_link_local_prefix, 64U);
    }
    return hwaddr_reconf;
}

void _remove_tentative_addr(gnrc_netif_t *netif, const ipv6_addr_t *addr)
{
    DEBUG("nib: other node has TENTATIVE address %s assigned "
          "=> removing that address\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC_TEMPORARY_ADDRESSES)
    bool is_temp_addr = is_temporary_addr(netif, addr);
    //need to determine before addr removal,
    //because it deletes the prefix which is used to determine whether it's a temp addr
    uint8_t retries;
    if (is_temp_addr) {
        int idx = gnrc_netif_ipv6_addr_idx(netif, addr);
        assert(idx >= 0);
        retries = gnrc_netif_ipv6_addr_gen_retries(netif, idx);
    }
#endif
    gnrc_netif_ipv6_addr_remove_internal(netif, addr);

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC_TEMPORARY_ADDRESSES)
    if (is_temp_addr) {
        if (retries >= TEMP_IDGEN_RETRIES) {
            //https://www.rfc-editor.org/rfc/rfc8981.html#section-3.4-3.7
            LOG_ERROR("nib: Not regenerating temporary address, retried often enough.\n");
            if (!gnrc_ipv6_nib_pl_reschedule_regen(netif->pid, addr, 0)) {
                LOG_WARNING("nib: Removing regen event timer failed\n");
                assert(false);
            }
            return;
        }
        retries++;
        DEBUG("Trying regeneration of temporary address. (%u/%u)\n", retries, TEMP_IDGEN_RETRIES);

        //find associated prefix
        uint32_t slaac_prefix_pref_until;
        if (!_get_slaac_prefix_pref_until(netif, addr, &slaac_prefix_pref_until)) {
            // at least one match is expected,
            LOG_WARNING("nib: The temporary address smh outlived the SLAAC prefix valid lft.");
            assert(false);
            return;
        }

        uint32_t now = evtimer_now_msec();
        assert(now < slaac_prefix_pref_until);
        // else the temporary address smh outlived the SLAAC prefix preferred lft
        int idx;
        int32_t ta_max_pref_lft;
        ta_max_pref_lft = _generate_temporary_addr(netif, addr,
                                                   slaac_prefix_pref_until - now,
                                                   retries,
                                                   &idx);
        if (ta_max_pref_lft < 0) {
            LOG_WARNING("nib: Temporary address regeneration failed after DAD failure.\n");
            return;
        }

        //re-schedule regen event (incl. deleting old one)
        if (!gnrc_ipv6_nib_pl_reschedule_regen(netif->pid, addr, ta_max_pref_lft -
                _get_netif_regen_advance(netif))) {
            LOG_WARNING("nib: Temporary address regeneration failed after DAD failure, "
                  "SLAAC prefix was not found to reschedule address regeneration timer.\n");
            //delete just created temp addr
            gnrc_netif_ipv6_addr_remove_internal(netif, &netif->ipv6.addrs[idx]);
            assert(false);
            return;
        }

        return;
    }
#endif

    if (!ipv6_addr_is_link_local(addr) ||
        !_try_addr_reconfiguration(netif)) {
        /* Cannot use target address as personal address and can
         * not change hardware address to retry SLAAC => use purely
         * DHCPv6 instead */
        if (IS_USED(MODULE_DHCPV6_CLIENT_IA_NA)) {
            netif->ipv6.aac_mode &= ~GNRC_NETIF_AAC_AUTO;
            netif->ipv6.aac_mode |= GNRC_NETIF_AAC_DHCP;
            dhcpv6_client_req_ia_na(netif->pid);
        }
        else {
            DEBUG("nib: would set interface %i to DHCPv6, "
                  "but DHCPv6 is not provided", netif->pid);
        }
    }
}

static int _get_netif_state(gnrc_netif_t **netif, const ipv6_addr_t *addr)
{
    *netif = gnrc_netif_get_by_ipv6_addr(addr);
    if (*netif != NULL) {
        int idx;

        gnrc_netif_acquire(*netif);
        idx = gnrc_netif_ipv6_addr_idx(*netif, addr);
        return ((idx >= 0) && gnrc_netif_ipv6_addr_dad_trans(*netif, idx)) ?
               idx : -1;
    }
    return -1;
}

void _handle_dad(const ipv6_addr_t *addr)
{
    ipv6_addr_t sol_nodes;
    gnrc_netif_t *netif = NULL;
    int idx = _get_netif_state(&netif, addr);
    if (idx >= 0) {
        ipv6_addr_set_solicited_nodes(&sol_nodes, addr);
        _snd_ns(addr, netif, &ipv6_addr_unspecified, &sol_nodes);
        _evtimer_add((void *)&netif->ipv6.addrs[idx],
                     GNRC_IPV6_NIB_VALID_ADDR,
                     &netif->ipv6.addrs_timers[idx],
                     netif->ipv6.retrans_time);
    }
    if (netif != NULL) {
        /* was acquired in `_get_netif_state()` */
        gnrc_netif_release(netif);
    }
}

void _handle_valid_addr(const ipv6_addr_t *addr)
{
    gnrc_netif_t *netif = NULL;
    int idx = _get_netif_state(&netif, addr);

    DEBUG("nib: validating address %s (idx: %d, netif: %d)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), idx,
          (netif != NULL) ? netif->pid : 0);
    if (idx >= 0) {
        netif->ipv6.addrs_flags[idx] &= ~GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK;
        netif->ipv6.addrs_flags[idx] |= GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID;
        gnrc_netif_ipv6_bus_post(netif, GNRC_IPV6_EVENT_ADDR_VALID, &netif->ipv6.addrs[idx]);
    }
    if (netif != NULL) {
        /* was acquired in `_get_netif_state()` */
        gnrc_netif_release(netif);
    }
}
#else  /* CONFIG_GNRC_IPV6_NIB_SLAAC */
typedef int dont_be_pedantic;
#endif /* CONFIG_GNRC_IPV6_NIB_SLAAC */

/** @} */
