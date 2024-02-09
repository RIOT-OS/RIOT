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
#if IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY)
#include "_nib-slaac.h"
#include <hashes/sha256.h>
#include "ztimer.h"
#include "random.h"
#endif

#include "log.h"
#include "luid.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif/internal.h"

#include "_nib-6ln.h"
#include "_nib-arsm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN) || IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC)
static char addr_str[IPV6_ADDR_MAX_STR_LEN];

#if !IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY)
void _auto_configure_addr(gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                          uint8_t pfx_len)
#else
inline void _auto_configure_addr(gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                          uint8_t pfx_len)
{
    _auto_configure_addr_with_dad_ctr(netif, pfx, pfx_len, 0);
}

void _auto_configure_addr_with_dad_ctr(gnrc_netif_t *netif,
                                       const ipv6_addr_t *pfx, uint8_t pfx_len,
                                       uint8_t dad_ctr)
#endif
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
    DEBUG("nib: add address based on %s/%u automatically to interface %u\n",
          ipv6_addr_to_str(addr_str, pfx, sizeof(addr_str)),
          pfx_len, netif->pid);
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    bool new_address = false;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY)
    bool is_rfc7217 = !(gnrc_netif_is_6ln(netif) && ipv6_addr_is_link_local(pfx));
    if (is_rfc7217) {
        if (ipv6_get_rfc7217_iid((eui64_t *) &addr.u64[1], netif, pfx, &dad_ctr) < 0) {
            return;
        }
        flags |= (dad_ctr << GNRC_NETIF_IPV6_ADDRS_FLAGS_IDGEN_RETRIES_POS);
    } else
#endif
    {
        if (!(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR)) {
            DEBUG("nib: interface %i has no link-layer addresses\n", netif->pid);
            return;
        }
        if (gnrc_netif_ipv6_get_iid(netif, (eui64_t *)&addr.u64[1]) < 0) {
            DEBUG("nib: Can't get IID on interface %u\n", netif->pid);
            return;
        }
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
#if IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY)
    else if (is_rfc7217) {
        /*"the same network prefix" - https://datatracker.ietf.org/doc/html/rfc7217#section-5*/
        /*"unacceptable identifier" -> retry IDGEN (dad_ctr+1)*/
        if (!_stable_privacy_should_retry_idgen(&dad_ctr, "Generated address already exists")) {
            return;
        }

        _auto_configure_addr_with_dad_ctr(netif, pfx, pfx_len, dad_ctr);
        return;
    }
#endif

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

#if IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY)
bool _iid_is_iana_reserved(const eui64_t *iid)
{
    return (iid->uint64.u64 == htonll(0))
           || (iid->uint32[0].u32 == htonl(0x02005eff) && iid->uint8[4] == 0xfe)
           || (iid->uint32[0].u32 == htonl(0xfdffffff) && iid->uint16[2].u16 == htons(0xffff) && iid->uint8[6] == 0xff && (iid->uint8[7] & 0x80));
}

inline bool _stable_privacy_should_retry_idgen(uint8_t *dad_ctr, const char *reason) {
    if (*dad_ctr < STABLE_PRIVACY_IDGEN_RETRIES) { //within retry limit
        LOG_DEBUG("nib: %s", reason);
        *dad_ctr += 1;
        LOG_DEBUG(", retrying IDGEN. (%u/%u)\n", *dad_ctr, STABLE_PRIVACY_IDGEN_RETRIES);
        return true;
    }
    //retried often enough
    LOG_WARNING("nib: %s", reason);
    LOG_WARNING(", not retrying: IDGEN_RETRIES limit reached\n");
    return false;
}

int ipv6_get_rfc7217_iid(eui64_t *iid, const gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                         uint8_t *dad_ctr)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (!(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR))
#endif /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    {
        LOG_ERROR("nib: interface %i has no link-layer addresses\n", netif->pid);
        return -ENOTSUP;
    }

#ifndef STABLE_PRIVACY_SECRET_KEY
#error "Stable privacy requires a secret_key, this should have been configured by sys/net/gnrc/Makefile.dep"
#endif
    const uint8_t secret_key[16] = { STABLE_PRIVACY_SECRET_KEY };
    //SHOULD be of at least 128 bits - https://datatracker.ietf.org/doc/html/rfc7217

    uint8_t digest[SHA256_DIGEST_LENGTH];

    {
        sha256_context_t c;
        sha256_init(&c);
        sha256_update(&c, pfx, sizeof(*pfx));
        sha256_update(&c, &netif->l2addr, netif->l2addr_len);
        sha256_update(&c, dad_ctr, sizeof(*dad_ctr));
        sha256_update(&c, secret_key, sizeof(secret_key));
        sha256_final(&c, digest);
    }

    iid->uint64.u64 = 0;
    //uninitialized if called via gnrc_netapi_get (NETOPT_IPV6_IID_RFC7217)
    //needs to be all zeros as precondition for the following copy operation

    assert(sizeof(digest) >= sizeof(*iid)); //as bits: 256 >= 64 //digest is large enough

    //copy digest into IID
    //RFC 7217 says "starting from the least significant bit",
    //i.e. in reverse order
    for (uint8_t i = 0; i < sizeof(*iid); i++) { //for each of the 8 bytes
        for (int j = 0; j < 8; j++) { //for each of the 8 bits _within byte_
            if ((digest[(sizeof(digest)-1)-i])&(1<<j)) { //is 1 //reverse order
                iid->uint8[i] |= 1 << ((8-1)-j); //set 1 //precondition: iid->uint8[i] = 0
            }
        }
    }

    /* "The resulting Interface Identifier SHOULD be compared against
     * the reserved IPv6 Interface Identifiers"
     * - https://datatracker.ietf.org/doc/html/rfc7217#section-5 */
    if (_iid_is_iana_reserved(iid)) {
        if (!_stable_privacy_should_retry_idgen(dad_ctr,
                                                "IANA reserved IID generated")) {
            return -1;
        }
        iid->uint64.u64 = 0; /* @pre for method call */
        return ipv6_get_rfc7217_iid(iid, netif, pfx, dad_ctr);
    }

    return 0;
}
#endif

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC)
#if !IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY)
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
#endif

void _remove_tentative_addr(gnrc_netif_t *netif, const ipv6_addr_t *addr)
{
    DEBUG("nib: other node has TENTATIVE address %s assigned "
          "=> removing that address\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
#if IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY)
    int idx = gnrc_netif_ipv6_addr_idx(netif, addr);
    assert(idx >= 0);
    uint8_t dad_counter = gnrc_netif_ipv6_addr_gen_retries(netif, idx);
#endif
    gnrc_netif_ipv6_addr_remove_internal(netif, addr);

#if IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY)
    if (_stable_privacy_should_retry_idgen(&dad_counter, "Duplicate address detected")) {

        //> Hosts SHOULD introduce a random delay between 0 and IDGEN_DELAY seconds
        //- https://datatracker.ietf.org/doc/html/rfc7217#section-6
        uint32_t random_delay_ms = random_uint32_range(0, STABLE_PRIVACY_IDGEN_DELAY_MS);
        ztimer_sleep(ZTIMER_MSEC, random_delay_ms);

        _auto_configure_addr_with_dad_ctr(netif, addr, SLAAC_PREFIX_LENGTH, dad_counter);
    } else {
        //"hosts MUST NOT automatically fall back to employing other algorithms for generating Interface Identifiers"
        //- https://datatracker.ietf.org/doc/html/rfc7217#section-6

        //> If the address is a link-local address
        //> [...]
        //> not formed from an interface identifier based on the hardware address
        //> IP operation on the interface MAY be continued
        //- https://datatracker.ietf.org/doc/html/rfc4862#section-5.4.5
    }
#else
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
#endif
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
