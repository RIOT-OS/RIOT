/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief       Tests default configuration of GNRC's Network Information Base
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "cib.h"
#include "common.h"
#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "net/ethernet.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/netif/internal.h"
#include "net/ndp.h"
#include "sched.h"
#include "timex.h"

#define _BUFFER_SIZE    (128)
#define _CUR_HL         (155)
#define _RTR_LTIME      (6612U)
#define _REACH_TIME     (1210388825UL)
#define _RETRANS_TIMER  (3691140UL)
#define _LOC_GB_PFX_LEN (64U)
#define _REM_GB_PFX_LEN (37U)
#define _PIO_PFX_LTIME  (0x8476fedf)

static const uint8_t _loc_l2[] = { _LL0, _LL1, _LL2, _LL3, _LL4, _LL5 };
static const ipv6_addr_t _loc_ll = { {
                0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            _LL0 ^ 2, _LL1, _LL2, 0xff, 0xfe, _LL3, _LL4, _LL5
        } };
static const ipv6_addr_t _loc_gb = { {
                0x20, 0x01, 0x2b, 0x2e, 0x43, 0x80, 0x00, 0x00,
            _LL0 ^ 2, _LL1, _LL2, 0xff, 0xfe, _LL3, _LL4, _LL5
        } };
static const ipv6_addr_t _loc_sol_nodes = { {
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0xff, _LL3, _LL4, _LL5
        } };
#define _loc_iid    _loc_ll.u64[1].u8
static const uint8_t _rem_l2[] = { _LL0, _LL1, _LL2, _LL3, _LL4, _LL5 + 1 };
static const ipv6_addr_t _rem_ll = { {
                0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            _LL0 ^ 2, _LL1, _LL2, 0xff, 0xfe, _LL3, _LL4, _LL5 + 1
        } };
static const ipv6_addr_t _rem_gb = { {
                0x20, 0x01, 0x18, 0xc9, 0xf8, 0x00, 0x00, 0x00,
            _LL0 ^ 2, _LL1, _LL2, 0xff, 0xfe, _LL3, _LL4, _LL5 + 1
        } };
#define _rem_iid    _rem_ll.u64[1].u8
static uint8_t _buffer[_BUFFER_SIZE];
static ipv6_hdr_t *ipv6 = (ipv6_hdr_t *)&_buffer[0];
static icmpv6_hdr_t *icmpv6 = (icmpv6_hdr_t *)&_buffer[sizeof(ipv6_hdr_t)];

static inline size_t ceil8(size_t size);

static void _set_up(void)
{
    _common_set_up();
    gnrc_netif_acquire(_mock_netif);
    /* reset some fields not set by the nib interface initializer */
    _mock_netif->ipv6.mtu = ETHERNET_DATA_LEN;
    _mock_netif->cur_hl = CONFIG_GNRC_NETIF_DEFAULT_HL;
    gnrc_netif_ipv6_addr_remove_internal(_mock_netif, &_loc_gb);
    gnrc_netif_release(_mock_netif);
    memset(_buffer, 0, sizeof(_buffer));
    gnrc_pktbuf_init();
    /* remove messages */
    while (msg_avail()) {
        msg_t msg;
        msg_receive(&msg);
    }
}

static void test_get_next_hop_l2addr__EHOSTUNREACH(const ipv6_addr_t *dst,
                                                   gnrc_netif_t *netif,
                                                   bool on_link)
{
    ipv6_addr_t addr;
    msg_t msg;
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    gnrc_pktsnip_t *pkt;

    memcpy(&addr, &_rem_gb, sizeof(addr));
    addr.u8[15]--;
    if (ipv6_addr_is_link_local(dst)) {
        on_link = true;
    }
    else if (on_link) {
        /* add _rem_gb prefix as on-link prefix */
        TEST_ASSERT(gnrc_netif_ipv6_addr_add_internal(_mock_netif, &addr,
                            _REM_GB_PFX_LEN,
                            GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID) >= 0);
    }
    else {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(NULL, 0, &_rem_ll,
                                                      _mock_netif->pid, 0));
    }
    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH,
                          gnrc_ipv6_nib_get_next_hop_l2addr(dst, netif,
                                                            NULL, &nce));
    if (on_link && (netif != NULL)) {
        ndp_nbr_sol_t *nbr_sol;
        bool contains_sl2ao = false;

        TEST_ASSERT_MESSAGE(gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                            "Expected neighbor cache entry");
        TEST_ASSERT_MESSAGE(ipv6_addr_equal(dst, &nce.ipv6),
                            "dst != nce->ipv6");
        TEST_ASSERT_EQUAL_INT(0, nce.l2addr_len);
        TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE,
                              gnrc_ipv6_nib_nc_get_nud_state(&nce));
        TEST_ASSERT(!gnrc_ipv6_nib_nc_is_router(&nce));
        TEST_ASSERT_EQUAL_INT(netif->pid, gnrc_ipv6_nib_nc_get_iface(&nce));
        TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC,
                              gnrc_ipv6_nib_nc_get_ar_state(&nce));
        TEST_ASSERT_EQUAL_INT(1, msg_avail());
        msg_receive(&msg);
        TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
        pkt = msg.content.ptr;
        TEST_ASSERT_NOT_NULL(pkt->next);
        TEST_ASSERT_NOT_NULL(pkt->next->next);
        TEST_ASSERT_EQUAL_INT(sizeof(ndp_nbr_sol_t), pkt->next->next->size);
        nbr_sol = pkt->next->next->data;
        TEST_ASSERT_EQUAL_INT(ICMPV6_NBR_SOL, nbr_sol->type);
        TEST_ASSERT_NOT_NULL(pkt->next->next->next);
        for (gnrc_pktsnip_t *opt_snip = pkt->next->next->next; opt_snip != NULL;
             opt_snip = opt_snip->next) {
            ndp_opt_t *opt = opt_snip->data;
            if (opt->type == NDP_OPT_SL2A) {
                contains_sl2ao = true;
                TEST_ASSERT_EQUAL_INT(1U, opt->len);
                TEST_ASSERT_MESSAGE(memcmp(&_loc_l2, opt + 1,
                                           sizeof(_loc_l2)) == 0,
                                    "src_l2 != pkt->l2");
            }
        }
        TEST_ASSERT_MESSAGE(contains_sl2ao, "Sent NS does not contain SL2AO");
        gnrc_pktbuf_release(pkt);
        TEST_ASSERT(gnrc_pktbuf_is_empty());
    }
    gnrc_netif_ipv6_addr_remove_internal(_mock_netif, &addr);
}

static void test_get_next_hop_l2addr__link_local_EHOSTUNREACH_no_iface(void)
{
    test_get_next_hop_l2addr__EHOSTUNREACH(&_rem_ll, NULL, false);
}

static void test_get_next_hop_l2addr__link_local_EHOSTUNREACH_iface(void)
{
    test_get_next_hop_l2addr__EHOSTUNREACH(&_rem_ll, _mock_netif, false);
}

static void test_get_next_hop_l2addr__global_EHOSTUNREACH_no_iface_off_link(void)
{
    test_get_next_hop_l2addr__EHOSTUNREACH(&_rem_gb, NULL, false);
}

static void test_get_next_hop_l2addr__global_EHOSTUNREACH_iface_off_link(void)
{
    test_get_next_hop_l2addr__EHOSTUNREACH(&_rem_gb, _mock_netif, false);
}

static void test_get_next_hop_l2addr__global_EHOSTUNREACH_no_iface_on_link(void)
{
    test_get_next_hop_l2addr__EHOSTUNREACH(&_rem_gb, NULL, true);
}

static void test_get_next_hop_l2addr__global_EHOSTUNREACH_iface_on_link(void)
{
    test_get_next_hop_l2addr__EHOSTUNREACH(&_rem_gb, _mock_netif, true);
}

static void test_get_next_hop_l2addr__ENETUNREACH(void)
{
    gnrc_ipv6_nib_nc_t nce;

    TEST_ASSERT_EQUAL_INT(-ENETUNREACH,
                          gnrc_ipv6_nib_get_next_hop_l2addr(&_rem_gb, NULL,
                                                            NULL, &nce));
}

static void test_get_next_hop_l2addr__link_local_static_conf(void)
{
    gnrc_ipv6_nib_nc_t nce;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&_rem_ll, _mock_netif->pid,
                                                  _rem_l2, sizeof(_rem_l2)));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_get_next_hop_l2addr(&_rem_ll,
                                                               _mock_netif,
                                                               NULL, &nce));
    TEST_ASSERT_MESSAGE((memcmp(&_rem_ll, &nce.ipv6, sizeof(_rem_ll)) == 0),
                        "_rem_ll != nce.ipv6");
    TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), nce.l2addr_len);
    TEST_ASSERT_MESSAGE((memcmp(&_rem_l2, &nce.l2addr, nce.l2addr_len) == 0),
                        "_rem_l2 != nce.l2addr");
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT_EQUAL_INT(_mock_netif->pid, gnrc_ipv6_nib_nc_get_iface(&nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_is_router(&nce));
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL,
                          gnrc_ipv6_nib_nc_get_ar_state(&nce));
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

void _simulate_ndp_handshake(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                             uint8_t adv_flags)
{
    msg_t msg;
    gnrc_ipv6_nib_nc_t nce;
    ndp_nbr_adv_t *nbr_adv = (ndp_nbr_adv_t *)icmpv6;
    ndp_opt_t *tl2ao = (ndp_opt_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                             sizeof(ndp_nbr_adv_t)];

    /* trigger sending of neighbor discovery */
    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH,
                          gnrc_ipv6_nib_get_next_hop_l2addr(dst,
                                                            _mock_netif,
                                                            NULL, &nce));
    TEST_ASSERT_EQUAL_INT(1, msg_avail());
    /* clear message queue */
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    gnrc_pktbuf_release(msg.content.ptr);
    /* generate neighbor advertisement */
    ipv6_hdr_set_version(ipv6);
    ipv6->hl = NDP_HOP_LIMIT;
    /* this simulates a reply, so dst and src need to be switched */
    memcpy(&ipv6->src, dst, sizeof(ipv6->src));
    memcpy(&ipv6->dst, src, sizeof(ipv6->dst));
    nbr_adv->type = ICMPV6_NBR_ADV;
    /* checksum isn't checked by gnrc_ipv6_nib_handle_pkt() */
    nbr_adv->flags = adv_flags;
    memcpy(&nbr_adv->tgt, dst, sizeof(nbr_adv->tgt));
    tl2ao->type = NDP_OPT_TL2A;
    tl2ao->len = 1;
    memcpy(tl2ao + 1, _rem_l2, sizeof(_rem_l2));
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, (icmpv6_hdr_t *)nbr_adv,
                             sizeof(ndp_nbr_adv_t) + 8U);
}

static void test_get_next_hop_l2addr__link_local_after_handshake(uint8_t adv_flags)
{
    gnrc_ipv6_nib_nc_t nce;

    _simulate_ndp_handshake(&_loc_ll, &_rem_ll, adv_flags);
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_get_next_hop_l2addr(&_rem_ll,
                                                               _mock_netif,
                                                               NULL, &nce));
    TEST_ASSERT_MESSAGE((memcmp(&_rem_ll, &nce.ipv6, sizeof(_rem_ll)) == 0),
                        "_rem_ll != nce.ipv6");
    TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), nce.l2addr_len);
    TEST_ASSERT_MESSAGE((memcmp(&_rem_l2, &nce.l2addr, nce.l2addr_len) == 0),
                        "_rem_l2 != nce.l2addr");
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT_EQUAL_INT(_mock_netif->pid, gnrc_ipv6_nib_nc_get_iface(&nce));
    if (adv_flags & NDP_NBR_ADV_FLAGS_R) {
        TEST_ASSERT(gnrc_ipv6_nib_nc_is_router(&nce));
    }
    else {
        TEST_ASSERT(!gnrc_ipv6_nib_nc_is_router(&nce));
    }
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC,
                          gnrc_ipv6_nib_nc_get_ar_state(&nce));
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_get_next_hop_l2addr__link_local_after_handshake_iface(void)
{
    test_get_next_hop_l2addr__link_local_after_handshake(NDP_NBR_ADV_FLAGS_S);
}

static void test_get_next_hop_l2addr__link_local_after_handshake_iface_router(void)
{
    test_get_next_hop_l2addr__link_local_after_handshake(NDP_NBR_ADV_FLAGS_S |
                                                         NDP_NBR_ADV_FLAGS_R);
}

static void test_get_next_hop_l2addr__link_local_after_handshake_no_iface(void)
{
    gnrc_ipv6_nib_nc_t nce;

    _simulate_ndp_handshake(&_loc_ll, &_rem_ll, NDP_NBR_ADV_FLAGS_S);
    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH,
                          gnrc_ipv6_nib_get_next_hop_l2addr(&_rem_ll,
                                                            NULL,
                                                            NULL, &nce));
}

static void test_handle_pkt__unknown_type(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;

    ipv6_hdr_set_version(ipv6);
    ipv6->hl = NDP_HOP_LIMIT;
    memcpy(&ipv6->src, &_loc_ll, sizeof(ipv6->src));
    memcpy(&ipv6->dst, &_rem_ll, sizeof(ipv6->dst));
    icmpv6->type = ICMPV6_ECHO_REQ;
    icmpv6->code = 0;
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6,
                             sizeof(icmpv6_hdr_t));
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
}

static size_t _set_nbr_sol(const ipv6_addr_t *ipv6_src,
                           const ipv6_addr_t *ipv6_dst,
                           uint8_t ipv6_hl, uint8_t nbr_sol_code,
                           const ipv6_addr_t *nbr_sol_tgt,
                           const uint8_t *sl2ao_addr, size_t sl2ao_addr_len)
{
    size_t icmpv6_len = sizeof(ndp_nbr_sol_t);
    ndp_nbr_sol_t *nbr_sol = (ndp_nbr_sol_t *)icmpv6;

    ipv6_hdr_set_version(ipv6);
    ipv6->hl = ipv6_hl;
    memcpy(&ipv6->src, ipv6_src, sizeof(ipv6->src));
    memcpy(&ipv6->dst, ipv6_dst, sizeof(ipv6->dst));
    nbr_sol->type = ICMPV6_NBR_SOL;
    nbr_sol->code = nbr_sol_code;
    memcpy(&nbr_sol->tgt, nbr_sol_tgt, sizeof(nbr_sol->tgt));

    if ((sl2ao_addr != NULL) && (sl2ao_addr_len > 0)) {
        ndp_opt_t *sl2ao = (ndp_opt_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                                 sizeof(ndp_nbr_sol_t)];

        sl2ao->type = NDP_OPT_SL2A;
        sl2ao->len = ceil8(sizeof(ndp_opt_t) + sl2ao_addr_len) / 8;
        memcpy(sl2ao + 1, sl2ao_addr, sl2ao_addr_len);
        icmpv6_len += ceil8(sizeof(ndp_opt_t) + sl2ao_addr_len);
    }

    return icmpv6_len;
}

static void test_handle_pkt__nbr_sol__invalid_hl(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, 194U, 0U,
                                     &_loc_ll, _rem_l2, sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_code(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, NDP_HOP_LIMIT,
                                     201U, &_loc_ll, _rem_l2, sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_icmpv6_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;

    _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, NDP_HOP_LIMIT, 0U, &_loc_ll,
                 _rem_l2, sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6,
                             sizeof(ndp_nbr_sol_t) - 1);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_tgt(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, NDP_HOP_LIMIT,
                                     0U, &ipv6_addr_all_routers_site_local,
                                     _rem_l2, sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_opt_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, NDP_HOP_LIMIT,
                                     0U, &_loc_ll, _rem_l2, sizeof(_rem_l2));
    ndp_opt_t *opt = (ndp_opt_t *)&_buffer[icmpv6_len];

    opt->type = NDP_OPT_SL2A;
    opt->len = 0U;
    icmpv6_len += sizeof(ndp_opt_t);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_dst(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&ipv6_addr_unspecified, &_loc_ll,
                                     NDP_HOP_LIMIT, 0U, &_loc_ll, NULL, 0);

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_pkt_is_nbr_adv(gnrc_pktsnip_t *pkt, const ipv6_addr_t *dst,
                                const ipv6_addr_t *tgt,
                                const uint8_t *tgt_l2addr,
                                size_t tgt_l2addr_len)
{
    gnrc_pktsnip_t *options;
    gnrc_netif_hdr_t *netif_hdr;
    ipv6_hdr_t *ipv6_hdr;
    ndp_nbr_adv_t *nbr_adv;

    /* first snip is a netif header to _mock_netif */
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_NETIF, pkt->type);
    TEST_ASSERT(sizeof(gnrc_netif_hdr_t) <= pkt->size);
    netif_hdr = pkt->data;
    TEST_ASSERT_EQUAL_INT(_mock_netif->pid, netif_hdr->if_pid);
    /* second snip is an IPv6 header to dst */
    TEST_ASSERT_NOT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_IPV6, pkt->next->type);
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_hdr_t), pkt->next->size);
    ipv6_hdr = pkt->next->data;
    if ((tgt_l2addr != NULL) && (tgt_l2addr_len > 0)) {
        TEST_ASSERT(!ipv6_addr_is_multicast(&ipv6_hdr->dst));
    }
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(dst, &ipv6_hdr->dst),
                        "dst != ipv6_hdr->dst");
    TEST_ASSERT_EQUAL_INT(255, ipv6_hdr->hl);
    /* third snip is a valid solicited neighbor advertisement to tgt */
    TEST_ASSERT_NOT_NULL(pkt->next->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_ICMPV6, pkt->next->next->type);
    TEST_ASSERT_EQUAL_INT(sizeof(ndp_nbr_adv_t), pkt->next->next->size);
    nbr_adv = pkt->next->next->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_NBR_ADV, nbr_adv->type);
    TEST_ASSERT_EQUAL_INT(0, nbr_adv->code);
    TEST_ASSERT(!ipv6_addr_is_multicast(&nbr_adv->tgt));
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(tgt, &nbr_adv->tgt),
                        "tgt != nbr_adv->tgt");
    options = pkt->next->next->next;
    if ((tgt_l2addr != NULL) && (tgt_l2addr_len > 0)) {
        ndp_opt_t *tl2ao;

        TEST_ASSERT(nbr_adv->flags & NDP_NBR_ADV_FLAGS_S);

        /* fourth snip is a TL2AO for tgt_l2addr */
        TEST_ASSERT_NOT_NULL(options);
        TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, options->type);
        TEST_ASSERT_EQUAL_INT(ceil8(sizeof(ndp_opt_t) + tgt_l2addr_len),
                              options->size);
        tl2ao = options->data;
        TEST_ASSERT_EQUAL_INT(NDP_OPT_TL2A, tl2ao->type);
        TEST_ASSERT_EQUAL_INT(1, tl2ao->len);
        TEST_ASSERT_MESSAGE(memcmp(tl2ao + 1, tgt_l2addr, tgt_l2addr_len) == 0,
                            "tl2ao.l2addr != tgt_l2addr");
    }
    /* no further options */
    TEST_ASSERT_NULL(options->next);
}

static void test_handle_pkt__nbr_sol__invalid_sl2ao(void)
{
    msg_t msg;
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&ipv6_addr_unspecified, &_loc_sol_nodes,
                                     NDP_HOP_LIMIT, 0U, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */

    /* check if SLAAC generated neighbor advertisement */
    TEST_ASSERT_EQUAL_INT(1, msg_avail());
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    test_pkt_is_nbr_adv(msg.content.ptr, &ipv6_addr_all_nodes_link_local,
                        &_loc_ll, NULL, 0);
    gnrc_pktbuf_release(msg.content.ptr);

    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__tgt_not_assigned(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes,
                                     NDP_HOP_LIMIT, 0U, &_rem_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__ll_src(unsigned exp_nud_state,
                                             unsigned exp_ar_state)
{
    msg_t msg;
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes,
                                     NDP_HOP_LIMIT, 0U, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "Expected neighbor cache entry");
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(&_rem_ll, &nce.ipv6),
                        "_rem_ll != nce->ipv6");
    TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), nce.l2addr_len);
    TEST_ASSERT_MESSAGE(memcmp(_rem_l2, nce.l2addr, nce.l2addr_len) == 0,
                        "_rem_l2 != nce.l2addr");
    TEST_ASSERT_EQUAL_INT(exp_nud_state, gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_is_router(&nce));
    TEST_ASSERT_EQUAL_INT(_mock_netif->pid, gnrc_ipv6_nib_nc_get_iface(&nce));
    TEST_ASSERT_EQUAL_INT(exp_ar_state, gnrc_ipv6_nib_nc_get_ar_state(&nce));
    TEST_ASSERT_EQUAL_INT(1, msg_avail());
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    test_pkt_is_nbr_adv(msg.content.ptr, &_rem_ll, &_loc_ll, _loc_l2,
                        sizeof(_loc_l2));
    gnrc_pktbuf_release(msg.content.ptr);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_handle_pkt__nbr_sol__ll_src_empty_nc(void)
{
    test_handle_pkt__nbr_sol__ll_src(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE,
                                     GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC);
}

static void test_handle_pkt__nbr_sol__ll_src_unmanaged_nce(void)
{
    test_get_next_hop_l2addr__link_local_static_conf();
    /* unmanaged entry stays unmanaged */
    test_handle_pkt__nbr_sol__ll_src(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED,
                                     GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL);
}

static void test_handle_pkt__nbr_sol__ll_src_no_sl2ao(void)
{
    msg_t msg;
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes,
                                     NDP_HOP_LIMIT, 0U, &_loc_ll, NULL, 0);

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    TEST_ASSERT_EQUAL_INT(1, msg_avail());
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    test_pkt_is_nbr_adv(msg.content.ptr, &_rem_ll, &_loc_ll, _loc_l2,
                        sizeof(_loc_l2));
    gnrc_pktbuf_release(msg.content.ptr);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static size_t _set_nbr_adv(const ipv6_addr_t *ipv6_src,
                           const ipv6_addr_t *ipv6_dst,
                           uint8_t ipv6_hl, uint8_t nbr_adv_code,
                           uint8_t nbr_adv_flags,
                           const ipv6_addr_t *nbr_adv_tgt,
                           const uint8_t *tl2ao_addr, size_t tl2ao_addr_len)
{
    size_t icmpv6_len = sizeof(ndp_nbr_adv_t);
    ndp_nbr_adv_t *nbr_adv = (ndp_nbr_adv_t *)icmpv6;

    ipv6_hdr_set_version(ipv6);
    ipv6->hl = ipv6_hl;
    memcpy(&ipv6->src, ipv6_src, sizeof(ipv6->src));
    memcpy(&ipv6->dst, ipv6_dst, sizeof(ipv6->dst));
    nbr_adv->type = ICMPV6_NBR_ADV;
    nbr_adv->code = nbr_adv_code;
    nbr_adv->flags = nbr_adv_flags;
    memcpy(&nbr_adv->tgt, nbr_adv_tgt, sizeof(nbr_adv->tgt));

    if ((tl2ao_addr != NULL) || (tl2ao_addr_len > 0)) {
        ndp_opt_t *tl2ao = (ndp_opt_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                                 sizeof(ndp_nbr_adv_t)];

        tl2ao->type = NDP_OPT_TL2A;
        tl2ao->len = ceil8(sizeof(ndp_opt_t) + tl2ao_addr_len) / 8;
        memcpy(tl2ao + 1, tl2ao_addr, tl2ao_addr_len);
        icmpv6_len += ceil8(sizeof(ndp_opt_t) + tl2ao_addr_len);
    }

    return icmpv6_len;
}

static void test_handle_pkt__nbr_adv__invalid_hl(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_ll, 194U, 0U,
                                     NDP_NBR_ADV_FLAGS_S, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_code(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_ll, NDP_HOP_LIMIT, 201U,
                                     NDP_NBR_ADV_FLAGS_S, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_icmpv6_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;

    _set_nbr_adv(&_rem_ll, &_loc_ll, NDP_HOP_LIMIT, 0U, NDP_NBR_ADV_FLAGS_S,
                 &_loc_ll, _rem_l2,
                 sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6,
                             sizeof(ndp_nbr_adv_t) - 1);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_tgt(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_ll, NDP_HOP_LIMIT, 0U,
                                     NDP_NBR_ADV_FLAGS_S,
                                     &ipv6_addr_all_routers_site_local, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_flags(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &ipv6_addr_all_nodes_link_local,
                                     NDP_HOP_LIMIT, 0U, NDP_NBR_ADV_FLAGS_S,
                                     &_loc_ll, NULL, 0);

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_opt_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_ll, NDP_HOP_LIMIT, 0U,
                                     NDP_NBR_ADV_FLAGS_S, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));
    ndp_opt_t *opt = (ndp_opt_t *)&_buffer[icmpv6_len];

    opt->type = NDP_OPT_SL2A;
    opt->len = 0U;
    icmpv6_len += sizeof(ndp_opt_t);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__unspecified_src(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&ipv6_addr_unspecified, &_loc_ll,
                                     NDP_HOP_LIMIT, 0U, NDP_NBR_ADV_FLAGS_S,
                                     &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__unsolicited(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_sol_nodes, NDP_HOP_LIMIT,
                                     0U, NDP_NBR_ADV_FLAGS_S, &_loc_ll,
                                     _rem_l2, sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static size_t _set_rtr_sol(const ipv6_addr_t *ipv6_src,
                           const ipv6_addr_t *ipv6_dst,
                           uint8_t ipv6_hl, uint8_t rtr_sol_code,
                           const uint8_t *sl2ao_addr, size_t sl2ao_addr_len)
{
    size_t icmpv6_len = sizeof(ndp_rtr_sol_t);
    ndp_rtr_sol_t *rtr_sol = (ndp_rtr_sol_t *)icmpv6;

    ipv6_hdr_set_version(ipv6);
    ipv6->hl = ipv6_hl;
    memcpy(&ipv6->src, ipv6_src, sizeof(ipv6->src));
    memcpy(&ipv6->dst, ipv6_dst, sizeof(ipv6->dst));
    rtr_sol->type = ICMPV6_RTR_SOL;
    rtr_sol->code = rtr_sol_code;

    if ((sl2ao_addr != NULL) && (sl2ao_addr_len > 0)) {
        ndp_opt_t *sl2ao = (ndp_opt_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                                 sizeof(ndp_rtr_sol_t)];

        sl2ao->type = NDP_OPT_SL2A;
        sl2ao->len = ceil8(sizeof(ndp_opt_t) + sl2ao_addr_len) / 8;
        memcpy(sl2ao + 1, sl2ao_addr, sl2ao_addr_len);
        icmpv6_len += ceil8(sizeof(ndp_opt_t) + sl2ao_addr_len);
    }

    return icmpv6_len;
}

static void test_handle_pkt__rtr_sol(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_rtr_sol(&_rem_ll, &_loc_ll,
                                     NDP_HOP_LIMIT, 0U,
                                     _rem_l2, sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static size_t _set_rtr_adv(const ipv6_addr_t *ipv6_src,
                           uint8_t ipv6_hl, uint8_t rtr_adv_code,
                           bool set_rtr_adv_fields,
                           uint8_t rtr_adv_flags, uint32_t t_rtr_reachable_ms, uint16_t t_rtr_alive_s,
                           const uint8_t *sl2ao_addr, size_t sl2ao_addr_len,
                           uint16_t mtu,
                           const ipv6_addr_t *pfx, unsigned pfx_len,
                           uint8_t pfx_flags, uint32_t t_pfx_valid_s, uint32_t t_pfx_pref_s)
{
    size_t icmpv6_len = sizeof(ndp_rtr_adv_t);
    ndp_rtr_adv_t *rtr_adv = (ndp_rtr_adv_t *)icmpv6;

    ipv6_hdr_set_version(ipv6);
    ipv6->hl = ipv6_hl;
    memcpy(&ipv6->src, ipv6_src, sizeof(ipv6->src));
    memcpy(&ipv6->dst, &_loc_ll, sizeof(ipv6->dst));
    rtr_adv->type = ICMPV6_RTR_ADV;
    rtr_adv->code = rtr_adv_code;
    rtr_adv->flags = rtr_adv_flags;
    if (set_rtr_adv_fields) {
        rtr_adv->cur_hl = _CUR_HL,
        rtr_adv->ltime = byteorder_htons(t_rtr_alive_s);
        rtr_adv->reach_time = byteorder_htonl(t_rtr_reachable_ms);
        rtr_adv->retrans_timer = byteorder_htonl(_RETRANS_TIMER);
    }

    if (mtu > 0) {
        ndp_opt_mtu_t *mtuo = (ndp_opt_mtu_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                                        icmpv6_len];

        mtuo->type = NDP_OPT_MTU;
        mtuo->len = NDP_OPT_MTU_LEN;
        mtuo->mtu = byteorder_htonl(mtu);
        icmpv6_len += sizeof(ndp_opt_mtu_t);
    }
    if ((pfx != NULL) && (pfx_len > 0)) {
        ndp_opt_pi_t *pio = (ndp_opt_pi_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                                     icmpv6_len];

        pio->type = NDP_OPT_PI;
        pio->len = NDP_OPT_PI_LEN;
        pio->prefix_len = pfx_len;
        pio->flags = pfx_flags;
        pio->valid_ltime = byteorder_htonl(t_pfx_valid_s);
        pio->pref_ltime = byteorder_htonl(t_pfx_pref_s);
        ipv6_addr_init_prefix(&pio->prefix, pfx, pfx_len);
        icmpv6_len += sizeof(ndp_opt_pi_t);
    }
    if ((sl2ao_addr != NULL) && (sl2ao_addr_len > 0)) {
        ndp_opt_t *sl2ao = (ndp_opt_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                                 icmpv6_len];

        sl2ao->type = NDP_OPT_SL2A;
        sl2ao->len = ceil8(sizeof(ndp_opt_t) + sl2ao_addr_len) / 8;
        memcpy(sl2ao + 1, sl2ao_addr, sl2ao_addr_len);
        icmpv6_len += ceil8(sizeof(ndp_opt_t) + sl2ao_addr_len);
    }

    return icmpv6_len;
}

typedef struct {
    uint32_t reach_time_base;
    uint32_t reach_time;
    uint32_t retrans_timer;
    uint16_t mtu;
    uint8_t addr_count;
    uint8_t cur_hl;
} _netif_exp_t;

static uint8_t _netif_addr_count(const gnrc_netif_t *netif)
{
    unsigned count = 0U;

    for (int i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        if (netif->ipv6.addrs_flags[i] != 0) {
            count++;
        }
    }
    return count;
}

static inline void _get_netif_exp(const gnrc_netif_t *netif,
                                  _netif_exp_t *exp)
{
    exp->reach_time_base = netif->ipv6.reach_time_base;
    exp->reach_time = netif->ipv6.reach_time;
    exp->retrans_timer = netif->ipv6.retrans_time;
    exp->mtu = netif->ipv6.mtu;
    exp->addr_count = _netif_addr_count(netif);
    exp->cur_hl = netif->cur_hl;
}

#define TEST_ASSERT_NETIF_UNCHANGED(netif, exp) \
    TEST_ASSERT_EQUAL_INT(exp.reach_time_base, \
                          netif->ipv6.reach_time_base); \
    TEST_ASSERT_EQUAL_INT(exp.reach_time, netif->ipv6.reach_time); \
    TEST_ASSERT_EQUAL_INT(exp.retrans_timer, netif->ipv6.retrans_time); \
    TEST_ASSERT_EQUAL_INT(exp.mtu, netif->ipv6.mtu); \
    TEST_ASSERT_EQUAL_INT(exp.addr_count, _netif_addr_count(netif)); \
    TEST_ASSERT_EQUAL_INT(exp.cur_hl, netif->cur_hl)

static void test_handle_pkt__rtr_adv__invalid_src(void)
{
    gnrc_ipv6_nib_nc_t nce;
    gnrc_ipv6_nib_ft_t route;
    void *state = NULL;
    size_t icmpv6_len = _set_rtr_adv(&_rem_gb,
                                     NDP_HOP_LIMIT, 0U, true, 0U,
                                     _REACH_TIME, _RTR_LTIME,
                                     _loc_l2, sizeof(_loc_l2),
                                     32397U, &_loc_gb, _LOC_GB_PFX_LEN,
                                     NDP_OPT_PI_FLAGS_L | NDP_OPT_PI_FLAGS_A,
                                     _PIO_PFX_LTIME, _PIO_PFX_LTIME);
    _netif_exp_t exp_netif;

    _get_netif_exp(_mock_netif, &exp_netif);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    state = NULL;
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route),
                        "There is an unexpected forwarding entry");
    TEST_ASSERT_NETIF_UNCHANGED(_mock_netif, exp_netif);
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__rtr_adv__invalid_hl(void)
{
    gnrc_ipv6_nib_nc_t nce;
    gnrc_ipv6_nib_ft_t route;
    void *state = NULL;
    size_t icmpv6_len = _set_rtr_adv(&_rem_ll,
                                     194U, 0U, true, 0U,
                                     _REACH_TIME, _RTR_LTIME,
                                     _loc_l2, sizeof(_loc_l2),
                                     32397U, &_loc_gb, _LOC_GB_PFX_LEN,
                                     NDP_OPT_PI_FLAGS_L | NDP_OPT_PI_FLAGS_A,
                                     _PIO_PFX_LTIME, _PIO_PFX_LTIME);
    _netif_exp_t exp_netif;

    _get_netif_exp(_mock_netif, &exp_netif);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    state = NULL;
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route),
                        "There is an unexpected forwarding entry");
    TEST_ASSERT_NETIF_UNCHANGED(_mock_netif, exp_netif);
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__rtr_adv__invalid_code(void)
{
    gnrc_ipv6_nib_nc_t nce;
    gnrc_ipv6_nib_ft_t route;
    void *state = NULL;
    size_t icmpv6_len = _set_rtr_adv(&_rem_ll,
                                     NDP_HOP_LIMIT, 201U, true, 0U,
                                     _REACH_TIME, _RTR_LTIME,
                                     _loc_l2, sizeof(_loc_l2),
                                     32397U, &_loc_gb, _LOC_GB_PFX_LEN,
                                     NDP_OPT_PI_FLAGS_L | NDP_OPT_PI_FLAGS_A,
                                     _PIO_PFX_LTIME, _PIO_PFX_LTIME);
    _netif_exp_t exp_netif;

    _get_netif_exp(_mock_netif, &exp_netif);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    state = NULL;
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route),
                        "There is an unexpected forwarding entry");
    TEST_ASSERT_NETIF_UNCHANGED(_mock_netif, exp_netif);
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__rtr_adv__invalid_icmpv6_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    gnrc_ipv6_nib_ft_t route;
    void *state = NULL;
    _netif_exp_t exp_netif;

    _get_netif_exp(_mock_netif, &exp_netif);
    _set_rtr_adv(&_rem_ll, NDP_HOP_LIMIT, 201U, true, 0U,
                 _REACH_TIME, _RTR_LTIME,
                 _loc_l2, sizeof(_loc_l2),
                 32397U, &_loc_gb, _LOC_GB_PFX_LEN,
                 NDP_OPT_PI_FLAGS_L | NDP_OPT_PI_FLAGS_A,
                 _PIO_PFX_LTIME, _PIO_PFX_LTIME);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6,
                             sizeof(ndp_rtr_adv_t) - 1);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    state = NULL;
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route),
                        "There is an unexpected forwarding entry");
    TEST_ASSERT_NETIF_UNCHANGED(_mock_netif, exp_netif);
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__rtr_adv__invalid_opt_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    gnrc_ipv6_nib_ft_t route;
    void *state = NULL;
    size_t icmpv6_len = _set_rtr_adv(&_rem_ll,
                                     NDP_HOP_LIMIT, 201U, true, 0U,
                                     _REACH_TIME, _RTR_LTIME,
                                     _loc_l2, sizeof(_loc_l2),
                                     32397U, &_loc_gb, _LOC_GB_PFX_LEN,
                                     NDP_OPT_PI_FLAGS_L | NDP_OPT_PI_FLAGS_A,
                                     _PIO_PFX_LTIME, _PIO_PFX_LTIME);
    ndp_opt_t *opt = (ndp_opt_t *)&_buffer[icmpv6_len];
    _netif_exp_t exp_netif;

    _get_netif_exp(_mock_netif, &exp_netif);
    opt->type = NDP_OPT_SL2A;
    opt->len = 0U;
    icmpv6_len += sizeof(ndp_opt_t);

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    state = NULL;
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route),
                        "There is an unexpected forwarding entry");
    TEST_ASSERT_NETIF_UNCHANGED(_mock_netif, exp_netif);
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__rtr_adv__options_success(bool sl2ao, bool mtuo, bool pio,
                                                      uint8_t pio_flags, unsigned exp_addr_count,
                                                      _netif_exp_t exp_netif);

/**
* Check if SLAAC generated a neighbor solicitation
*/
static void test_handle_pkt__rtr_adv__slaac_triggers_dad(void)
{
    msg_t msg;
    gnrc_pktsnip_t *pkt;
    gnrc_netif_hdr_t *netif_hdr;
    ipv6_hdr_t *ipv6_hdr;
    ndp_nbr_adv_t *nbr_sol;

    TEST_ASSERT_EQUAL_INT(1, msg_avail());
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    pkt = msg.content.ptr;
    /* first snip is a netif header to _mock_netif */
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_NETIF, pkt->type);
    TEST_ASSERT(sizeof(gnrc_netif_hdr_t) <= pkt->size);
    netif_hdr = pkt->data;
    TEST_ASSERT_EQUAL_INT(_mock_netif->pid, netif_hdr->if_pid);
    /* second snip is an IPv6 header to solicited nodes of _loc_gb */
    TEST_ASSERT_NOT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_IPV6, pkt->next->type);
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_hdr_t), pkt->next->size);
    ipv6_hdr = pkt->next->data;
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(&ipv6_hdr->dst,
                            &_loc_sol_nodes),
                        "ipv6_hdr->dst != _loc_sol_nodes");
    TEST_ASSERT_EQUAL_INT(255, ipv6_hdr->hl);
    /* third snip is a valid solicited neighbor solicitation to
                 * _loc_gb */
    TEST_ASSERT_NOT_NULL(pkt->next->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_ICMPV6, pkt->next->next->type);
    TEST_ASSERT_EQUAL_INT(sizeof(ndp_nbr_sol_t), pkt->next->next->size);
    nbr_sol = pkt->next->next->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_NBR_SOL, nbr_sol->type);
    TEST_ASSERT_EQUAL_INT(0, nbr_sol->code);
    TEST_ASSERT(!ipv6_addr_is_multicast(&nbr_sol->tgt));
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(&_loc_gb, &nbr_sol->tgt),
                        "_loc_gb != nbr_sol->tgt");
    /* no further options */
    TEST_ASSERT_NULL(pkt->next->next->next);
    gnrc_pktbuf_release(pkt);
}

static void test_handle_pkt__rtr_adv__success(uint8_t rtr_adv_flags,
                                              bool set_rtr_adv_fields,
                                              bool sl2ao, bool mtuo,
                                              bool pio, uint8_t pio_flags)
{
    gnrc_ipv6_nib_ft_t route;
    void *state = NULL;
    size_t icmpv6_len = _set_rtr_adv(&_rem_ll, NDP_HOP_LIMIT, 0U,
                                     set_rtr_adv_fields, rtr_adv_flags,
                                     _REACH_TIME, _RTR_LTIME,
                                     (sl2ao) ? _rem_l2 : NULL, sizeof(_rem_l2),
                                     (mtuo) ? 32397U : 0U,
                                     (pio) ? &_loc_gb : NULL, _LOC_GB_PFX_LEN,
                                     pio_flags, _PIO_PFX_LTIME, _PIO_PFX_LTIME);
    const unsigned exp_addr_count = _netif_addr_count(_mock_netif);
    _netif_exp_t exp_netif;

    _get_netif_exp(_mock_netif, &exp_netif);
    TEST_ASSERT(gnrc_netif_ipv6_addr_idx(_mock_netif, &_loc_gb) < 0);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    if (set_rtr_adv_fields) {
        while (gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route)) {
            /* is default route */
            if (ipv6_addr_is_unspecified(&route.dst)) {
                break;
            }
        }
        TEST_ASSERT_MESSAGE(ipv6_addr_equal(&_rem_ll, &route.next_hop),
                            "default route is not via RA's source");
        TEST_ASSERT_EQUAL_INT(_mock_netif->pid, route.iface);
        TEST_ASSERT_EQUAL_INT(_REACH_TIME, _mock_netif->ipv6.reach_time_base);
        TEST_ASSERT_MESSAGE(((_REACH_TIME / 2) <= _mock_netif->ipv6.reach_time) &&
                            (_mock_netif->ipv6.reach_time <=
                             (_REACH_TIME + (_REACH_TIME / 2))),
                    "Randomized reachable time was out of expected range.");
        TEST_ASSERT_EQUAL_INT(_RETRANS_TIMER, _mock_netif->ipv6.retrans_time);
        TEST_ASSERT_EQUAL_INT(_CUR_HL, _mock_netif->cur_hl);
    }
    else {
        while (gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route)) {
            TEST_ASSERT_MESSAGE(!ipv6_addr_is_unspecified(&route.dst),
                                "There is a default route, "
                                "though RA's router lifetime was 0.");
        }
        TEST_ASSERT_EQUAL_INT(exp_netif.reach_time_base,
                              _mock_netif->ipv6.reach_time_base);
        TEST_ASSERT_EQUAL_INT(exp_netif.reach_time,
                              _mock_netif->ipv6.reach_time);
        TEST_ASSERT_EQUAL_INT(exp_netif.retrans_timer,
                              _mock_netif->ipv6.retrans_time);
        TEST_ASSERT_EQUAL_INT(exp_netif.cur_hl,
                              _mock_netif->cur_hl);
    }
    if (pio && (pio_flags & NDP_OPT_PI_FLAGS_A)) {
        test_handle_pkt__rtr_adv__slaac_triggers_dad();
    }
    test_handle_pkt__rtr_adv__options_success(sl2ao, mtuo, pio, pio_flags, exp_addr_count, exp_netif);

    {
        /*
         * > The Router Lifetime applies only to
                 the router's usefulness as a default router; it
                 does not apply to information contained in other
                 message fields or options.
         * - https://datatracker.ietf.org/doc/html/rfc4861#section-4.2
         * So send a RA with Router Lifetime value of zero,
         * to test that _handle_rtr_timeout follows this behavior.
         * If it doesn't, the following tests for RA options again will fail.
         */
        icmpv6_len = _set_rtr_adv(&_rem_ll, NDP_HOP_LIMIT, 0U,
                                  false, 0U,
                                  _REACH_TIME, _RTR_LTIME,
                                  NULL, sizeof(_rem_l2),
                                  0U,
                                  NULL, _LOC_GB_PFX_LEN,
                                  0U,
                                  _PIO_PFX_LTIME, _PIO_PFX_LTIME);
        ndp_rtr_adv_t *rtr_adv = (ndp_rtr_adv_t *)icmpv6;
        rtr_adv->ltime = byteorder_htons(0);
        gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    }
    test_handle_pkt__rtr_adv__options_success(sl2ao, mtuo, pio, pio_flags, exp_addr_count, exp_netif);
}

static void test_handle_pkt__rtr_adv__options_success(bool sl2ao, bool mtuo, bool pio,
                                                      uint8_t pio_flags,
                                                      const unsigned exp_addr_count,
                                                      _netif_exp_t exp_netif)
{
    gnrc_ipv6_nib_pl_t prefix;
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;

    if (sl2ao) {
        TEST_ASSERT_MESSAGE(gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                            "No neighbor cache entry found");
        TEST_ASSERT_MESSAGE((memcmp(&_rem_ll, &nce.ipv6, sizeof(_rem_ll)) == 0),
                            "_rem_ll != nce.ipv6");
        TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), nce.l2addr_len);
        TEST_ASSERT_MESSAGE((memcmp(&_rem_l2, &nce.l2addr, nce.l2addr_len) == 0),
                            "_rem_l2 != nce.l2addr");
        TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE,
                              gnrc_ipv6_nib_nc_get_nud_state(&nce));
        TEST_ASSERT_EQUAL_INT(_mock_netif->pid, gnrc_ipv6_nib_nc_get_iface(&nce));
        TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC,
                              gnrc_ipv6_nib_nc_get_ar_state(&nce));
    }
    else {
        TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                            "There is an unexpected neighbor cache entry");
    }
    if (mtuo) {
        TEST_ASSERT_EQUAL_INT(32397U, _mock_netif->ipv6.mtu);
    }
    else {
        TEST_ASSERT_EQUAL_INT(exp_netif.mtu, _mock_netif->ipv6.mtu);
    }
    state = NULL;
    if (pio) {
        if (pio_flags & NDP_OPT_PI_FLAGS_A) {
            TEST_ASSERT_MESSAGE(gnrc_netif_ipv6_addr_idx(_mock_netif,
                                                         &_loc_gb) >= 0,
                                "Address was not configured by PIO");
        }
        else {
            TEST_ASSERT_MESSAGE(gnrc_netif_ipv6_addr_idx(_mock_netif,
                                                         &_loc_gb) < 0,
                                "Address was configured by PIO, "
                                "but A flag was set");
        }
        if (pio_flags & (NDP_OPT_PI_FLAGS_A | NDP_OPT_PI_FLAGS_L)) {
            TEST_ASSERT_MESSAGE(gnrc_ipv6_nib_pl_iter(0, &state, &prefix),
                                "No prefix list entry found");
            TEST_ASSERT_MESSAGE(ipv6_addr_match_prefix(&_loc_gb,
                                        &prefix.pfx) >= _LOC_GB_PFX_LEN,
                                "Unexpected prefix configured");
            TEST_ASSERT_EQUAL_INT(_LOC_GB_PFX_LEN, prefix.pfx_len);
            TEST_ASSERT_EQUAL_INT(_mock_netif->pid, prefix.iface);
            TEST_ASSERT((_PIO_PFX_LTIME / MS_PER_SEC) < prefix.valid_until);
            TEST_ASSERT((_PIO_PFX_LTIME / MS_PER_SEC) < prefix.pref_until);
        }
    }
    if (!pio || !(pio_flags & (NDP_OPT_PI_FLAGS_A | NDP_OPT_PI_FLAGS_L))) {
        if (!pio) {
            TEST_ASSERT_EQUAL_INT(exp_addr_count,
                                  _netif_addr_count(_mock_netif));
        }
        TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_pl_iter(0, &state, &prefix),
                            "There is an unexpected prefix list entry");
    }
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__rtr_adv__success_all_zero(void)
{
    test_handle_pkt__rtr_adv__success(0U, false, false, false, false, 0U);
}

static void test_handle_pkt__rtr_adv__success_no_flags_no_opt(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, false, false, false, 0U);
}

static void test_handle_pkt__rtr_adv__success_no_opt(void)
{
    /* these flags only make sense with SLAAC, so don't further test them below
     * (except for PIO ;-)) */
    test_handle_pkt__rtr_adv__success(NDP_RTR_ADV_FLAGS_M | NDP_RTR_ADV_FLAGS_O,
                                      true, false, false, false, 0U);
}

static void test_handle_pkt__rtr_adv__success_sl2ao(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, true, false, false, 0U);
}

static void test_handle_pkt__rtr_adv__success_mtuo(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, false, true, false, 0U);
}

static void test_handle_pkt__rtr_adv__success_pio_00(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, false, false, true, 0U);
}

static void test_handle_pkt__rtr_adv__success_pio_L0(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, false, false, true,
                                      NDP_OPT_PI_FLAGS_L);
}

static void test_handle_pkt__rtr_adv__success_pio_0A(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, false, false, true,
                                      NDP_OPT_PI_FLAGS_A);
}

static void test_handle_pkt__rtr_adv__success_pio_LA(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, false, false, true,
                                      NDP_OPT_PI_FLAGS_L | NDP_OPT_PI_FLAGS_A);
}

static void test_handle_pkt__rtr_adv__success_sl2ao_mtuo_pio_00(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, true, true, true, 0U);
}

static void test_handle_pkt__rtr_adv__success_sl2ao_mtuo_pio_L0(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, true, true, true,
                                      NDP_OPT_PI_FLAGS_L);
}

static void test_handle_pkt__rtr_adv__success_sl2ao_mtuo_pio_0A(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, true, true, true,
                                      NDP_OPT_PI_FLAGS_A);
}

static void test_handle_pkt__rtr_adv__success_sl2ao_mtuo_pio_LA(void)
{
    test_handle_pkt__rtr_adv__success(0U, true, true, true, true,
                                      NDP_OPT_PI_FLAGS_L | NDP_OPT_PI_FLAGS_A);
}

static void test_change_rtr_adv_iface(void)
{
    TEST_ASSERT_MESSAGE(!(_mock_netif->flags & GNRC_NETIF_FLAGS_IPV6_RTR_ADV),
                        "RTR_ADV was unexpectedly set");
    gnrc_ipv6_nib_change_rtr_adv_iface(_mock_netif, true);
    TEST_ASSERT_MESSAGE(!(_mock_netif->flags & GNRC_NETIF_FLAGS_IPV6_RTR_ADV),
                        "RTR_ADV was unexpectedly changed");
    gnrc_ipv6_nib_change_rtr_adv_iface(_mock_netif, false);
    TEST_ASSERT_MESSAGE(!(_mock_netif->flags & GNRC_NETIF_FLAGS_IPV6_RTR_ADV),
                        "RTR_ADV was unexpectedly changed");
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_router_timeout(void)
{
    gnrc_ipv6_nib_ft_t route;
    gnrc_ipv6_nib_pl_t prefix;
    void *state = NULL;
    uint32_t t_rtr_alive_s = 5; /* [0, 9000]s, 0: not a default router */
    size_t icmpv6_len = _set_rtr_adv(&_rem_ll,
                                     NDP_HOP_LIMIT, 0U, true, 0U,
                                     _REACH_TIME, t_rtr_alive_s,
                                     _rem_l2, sizeof(_rem_l2),
                                     32397U, &_loc_gb, _LOC_GB_PFX_LEN,
                                     NDP_OPT_PI_FLAGS_L,
                                     _PIO_PFX_LTIME, _PIO_PFX_LTIME);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    /* check that prefix has been added */
    bool pfx = false, rtr = false;
    while (gnrc_ipv6_nib_pl_iter(_mock_netif->pid, &state, &prefix)) {
        pfx = true;
        TEST_ASSERT_MESSAGE(ipv6_addr_match_prefix(&prefix.pfx, &_loc_gb) >= _LOC_GB_PFX_LEN,
                            "Prefix is not the one from the RA");
    }
    if (!pfx) {
        TEST_ASSERT_MESSAGE(false, "Prefix not added");
    }
    /* check that router has been added */
    state = NULL;
    while (gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route)) {
        if (ipv6_addr_is_unspecified(&route.dst)) {
            rtr = true;
            TEST_ASSERT_MESSAGE(ipv6_addr_equal(&_rem_ll, &route.next_hop),
                                "Default route is not via RA source");
        }
    }
    if (!rtr) {
        TEST_ASSERT_MESSAGE(false, "Default route not added");
    }
    /* timeout router by RA with lifetime 0 */
    icmpv6_len = _set_rtr_adv(&_rem_ll,
                              NDP_HOP_LIMIT, 0U, true, 0U,
                              _REACH_TIME, 0,
                              _rem_l2, sizeof(_rem_l2),
                              32397U, &_loc_gb, _LOC_GB_PFX_LEN,
                              NDP_OPT_PI_FLAGS_L,
                              _PIO_PFX_LTIME, _PIO_PFX_LTIME);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    /* check that default router has been timed out */
    state = NULL;
    while (gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route)) {
        if (ipv6_addr_is_unspecified(&route.dst)) {
            TEST_ASSERT_MESSAGE(ipv6_addr_equal(&_rem_ll, &route.next_hop),
                                "Default route was not deleted");
        }
    }
}

static void test_handle_prefix_timeout(void)
{
    gnrc_ipv6_nib_ft_t route;
    gnrc_ipv6_nib_pl_t prefix;
    void *state = NULL;
    uint32_t t_rtr_alive_s = 0; /* [0, 9000]s, 0: not a default router */
    size_t icmpv6_len = _set_rtr_adv(&_rem_ll,
                                     NDP_HOP_LIMIT, 0U, true, 0U,
                                     _REACH_TIME, t_rtr_alive_s,
                                     _rem_l2, sizeof(_rem_l2),
                                     32397U, &_loc_gb, _LOC_GB_PFX_LEN,
                                     NDP_OPT_PI_FLAGS_L,
                                     _PIO_PFX_LTIME, _PIO_PFX_LTIME);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    /* check that prefix has been added */
    bool pfx = false;
    while (gnrc_ipv6_nib_pl_iter(_mock_netif->pid, &state, &prefix)) {
        pfx = true;
        TEST_ASSERT_MESSAGE(ipv6_addr_match_prefix(&prefix.pfx, &_loc_gb) >= _LOC_GB_PFX_LEN,
                            "Prefix is not the one from the RA");
    }
    if (!pfx) {
        TEST_ASSERT_MESSAGE(false, "Prefix not added");
    }
    /* check that router has not been added as default router */
    state = NULL;
    while (gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &route)) {
        if (ipv6_addr_is_unspecified(&route.dst)) {
            TEST_ASSERT_MESSAGE(false, "Router should not have been added as a default router");
        }
    }
    /* timeout router by RA with lifetime 0 */
    icmpv6_len = _set_rtr_adv(&_rem_ll,
                              NDP_HOP_LIMIT, 0U, true, 0U,
                              _REACH_TIME, 0,
                              _rem_l2, sizeof(_rem_l2),
                              32397U, &_loc_gb, _LOC_GB_PFX_LEN,
                              NDP_OPT_PI_FLAGS_L,
                              0, 0);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    /* check that prefix has been timed out */
    state = NULL;
    if (gnrc_ipv6_nib_pl_iter(_mock_netif->pid, &state, &prefix)) {
        TEST_ASSERT_MESSAGE(false, "Prefix has been timed out but is still in use");
    }
}

static Test *tests_gnrc_ipv6_nib(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* gnrc_ipv6_nib_init() and gnrc_ipv6_nib_init_iface() "tested" in
         * set-up (otherwise the following tests wouldn't work) */
        /* TODO: ENETUNREACH when non-link-local communication is implemented */
        new_TestFixture(test_get_next_hop_l2addr__link_local_EHOSTUNREACH_no_iface),
        new_TestFixture(test_get_next_hop_l2addr__link_local_EHOSTUNREACH_iface),
        new_TestFixture(test_get_next_hop_l2addr__global_EHOSTUNREACH_no_iface_off_link),
        new_TestFixture(test_get_next_hop_l2addr__global_EHOSTUNREACH_iface_off_link),
        new_TestFixture(test_get_next_hop_l2addr__global_EHOSTUNREACH_no_iface_on_link),
        new_TestFixture(test_get_next_hop_l2addr__global_EHOSTUNREACH_iface_on_link),
        new_TestFixture(test_get_next_hop_l2addr__ENETUNREACH),
        new_TestFixture(test_get_next_hop_l2addr__link_local_static_conf),
        new_TestFixture(test_get_next_hop_l2addr__link_local_after_handshake_iface),
        new_TestFixture(test_get_next_hop_l2addr__link_local_after_handshake_iface_router),
        new_TestFixture(test_get_next_hop_l2addr__link_local_after_handshake_no_iface),
        new_TestFixture(test_handle_pkt__unknown_type),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_hl),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_code),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_icmpv6_len),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_tgt),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_opt_len),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_dst),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_sl2ao),
        new_TestFixture(test_handle_pkt__nbr_sol__tgt_not_assigned),
        /* TODO add tests for unspecified source (involves SLAAC) */
        new_TestFixture(test_handle_pkt__nbr_sol__ll_src_empty_nc),
        new_TestFixture(test_handle_pkt__nbr_sol__ll_src_unmanaged_nce),
        new_TestFixture(test_handle_pkt__nbr_sol__ll_src_no_sl2ao),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_hl),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_code),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_icmpv6_len),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_tgt),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_flags),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_opt_len),
        new_TestFixture(test_handle_pkt__nbr_adv__unspecified_src),
        new_TestFixture(test_handle_pkt__nbr_adv__unsolicited),
        /* solicited tested in get_next_hop_l2addr */
        new_TestFixture(test_handle_pkt__rtr_sol),
        new_TestFixture(test_handle_pkt__rtr_adv__invalid_src),
        new_TestFixture(test_handle_pkt__rtr_adv__invalid_hl),
        new_TestFixture(test_handle_pkt__rtr_adv__invalid_code),
        new_TestFixture(test_handle_pkt__rtr_adv__invalid_icmpv6_len),
        new_TestFixture(test_handle_pkt__rtr_adv__invalid_opt_len),
        new_TestFixture(test_handle_pkt__rtr_adv__success_all_zero),
        new_TestFixture(test_handle_pkt__rtr_adv__success_no_flags_no_opt),
        new_TestFixture(test_handle_pkt__rtr_adv__success_no_opt),
        new_TestFixture(test_handle_pkt__rtr_adv__success_sl2ao),
        new_TestFixture(test_handle_pkt__rtr_adv__success_mtuo),
        new_TestFixture(test_handle_pkt__rtr_adv__success_pio_00),
        new_TestFixture(test_handle_pkt__rtr_adv__success_pio_L0),
        new_TestFixture(test_handle_pkt__rtr_adv__success_pio_0A),
        new_TestFixture(test_handle_pkt__rtr_adv__success_pio_LA),
        new_TestFixture(test_handle_pkt__rtr_adv__success_sl2ao_mtuo_pio_00),
        new_TestFixture(test_handle_pkt__rtr_adv__success_sl2ao_mtuo_pio_L0),
        new_TestFixture(test_handle_pkt__rtr_adv__success_sl2ao_mtuo_pio_0A),
        new_TestFixture(test_handle_pkt__rtr_adv__success_sl2ao_mtuo_pio_LA),
        /* gnrc_ipv6_nib_handle_timer_event not testable in this context since
         * we do not have access to the (internally defined) contexts required
         * for it */
        new_TestFixture(test_change_rtr_adv_iface),
        new_TestFixture(test_handle_router_timeout),
        new_TestFixture(test_handle_prefix_timeout),
    };

    EMB_UNIT_TESTCALLER(tests, _set_up, NULL, fixtures);

    return (Test *)&tests;
}

int main(void)
{
    _tests_init();

    TESTS_START();
    TESTS_RUN(tests_gnrc_ipv6_nib());
    TESTS_END();

    return 0;
}

int _mock_netif_get(gnrc_netapi_opt_t *opt)
{
    switch (opt->opt) {
        case NETOPT_ADDRESS:
            if (opt->data_len < sizeof(_loc_l2)) {
                return -EOVERFLOW;
            }
            memcpy(opt->data, _loc_l2, sizeof(_loc_l2));
            return sizeof(_loc_l2);
        case NETOPT_IS_WIRED:
            return 1;
        case NETOPT_MAX_PDU_SIZE: {
                uint16_t *val = opt->data;
                if (opt->data_len != sizeof(uint16_t)) {
                    return -EOVERFLOW;
                }
                *val = ETHERNET_DATA_LEN;
                return sizeof(uint16_t);
            }
        default:
            return -ENOTSUP;
    }
}

static inline size_t ceil8(size_t size)
{
    if (size % 8) {
        return ((size / 8) + 1) * 8;
    }
    else {
        return size;
    }
}
