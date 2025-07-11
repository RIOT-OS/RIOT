/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2025 Ahmad Mahmod <ahmad.nawras.mahmod@gmail.com -- mahmod@unistra.fr>
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Cenk Gündoğan <cnkgndgn@gmail.com>
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 * @author Ahmad Mahmod <ahmad.nawras.mahmod@gmail.com -- mahmod@unistra.fr>
 */

#include <assert.h>
#include <string.h>
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/ipv6/ext/rh.h"
#include "net/gnrc/rpl/srh.h"
#include "net/gnrc/rpl/sr_table.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"
#define ENABLE_DEBUG                0
#include "debug.h"

#define GNRC_RPL_SRH_PADDING(X)     ((X & 0xF0) >> 4)
#define GNRC_RPL_SRH_COMPRE(X)      (X & 0x0F)
#define GNRC_RPL_SRH_COMPRI(X)      ((X & 0xF0) >> 4)
#define MIN(X, Y) (((X) < (Y))?(X):(Y))
#define IPV6_ADDR_LEN 16
#define MAX_COMPR_INDEX 15

static char addr_str[IPV6_ADDR_MAX_STR_LEN];
const ipv6_addr_t link_local_prefix = IPV6_ADDR_LINK_LOCAL_PREFIX;
/* checks if multiple addresses within the source routing header exist on my
 * interfaces */
static void *_contains_multiple_of_my_addr(const ipv6_addr_t *dst,
                                           const gnrc_rpl_srh_t *rh,
                                           unsigned num_addr,
                                           unsigned compri_addr_len)
{
    ipv6_addr_t addr;
    uint8_t *addr_vec = (uint8_t *)(rh + 1);
    bool found = false;
    uint8_t pref_elided = GNRC_RPL_SRH_COMPRI(rh->compr);
    uint8_t addr_len = compri_addr_len;
    uint8_t found_pos = 0;

    memcpy(&addr, dst, pref_elided);
    for (unsigned i = 0; i < num_addr; i++) {
        uint8_t *addr_vec_ptr = &addr_vec[i * compri_addr_len];

        if (i == num_addr - 1) {
            pref_elided = GNRC_RPL_SRH_COMPRE(rh->compr);
            addr_len = sizeof(ipv6_addr_t) - pref_elided;
        }
        memcpy(&addr.u8[pref_elided], addr_vec_ptr, addr_len);
        if (gnrc_netif_get_by_ipv6_addr(&addr) != NULL) {
            if (found && ((i - found_pos) > 1)) {
                DEBUG("RPL SRH: found multiple addresses that belong to me - "
                      "discard\n");
                return addr_vec_ptr;
            }
            found_pos = i;
            found = true;
        }
    }
    return NULL;
}

int gnrc_rpl_srh_process(ipv6_hdr_t *ipv6, gnrc_rpl_srh_t *rh, void **err_ptr)
{
    ipv6_addr_t addr;
    uint8_t *addr_vec = (uint8_t *)(rh + 1), *current_address;
    uint8_t num_addr;
    uint8_t current_pos, pref_elided, addr_len, compri_addr_len;
    const uint8_t new_seg_left = rh->seg_left - 1;

    if ((rh->len * 8) < (GNRC_RPL_SRH_PADDING(rh->pad_resv) +
                         (16 - GNRC_RPL_SRH_COMPRE(rh->compr)))) {
        DEBUG("RPL SRH: inconsistent header received\n");
        *err_ptr = &rh->len;
        return GNRC_IPV6_EXT_RH_ERROR;
    }

    assert(rh->seg_left > 0);
    num_addr = (((rh->len * 8) - GNRC_RPL_SRH_PADDING(rh->pad_resv) -
                 (16 - GNRC_RPL_SRH_COMPRE(rh->compr))) /
                (16 - GNRC_RPL_SRH_COMPRI(rh->compr))) + 1;

    DEBUG("RPL SRH: %u addresses in the routing header\n", (unsigned)num_addr);

    if (rh->seg_left > num_addr) {
        DEBUG("RPL SRH: number of segments left > number of addresses - "
              "discard\n");
        *err_ptr = &rh->seg_left;
        return GNRC_IPV6_EXT_RH_ERROR;
    }

    current_pos = num_addr - new_seg_left;
    pref_elided = (new_seg_left)
                ? GNRC_RPL_SRH_COMPRI(rh->compr)
                : GNRC_RPL_SRH_COMPRE(rh->compr);
    compri_addr_len = sizeof(ipv6_addr_t) - GNRC_RPL_SRH_COMPRI(rh->compr);
    addr_len = sizeof(ipv6_addr_t) - pref_elided;
    memcpy(&addr, &ipv6->dst, pref_elided);
    current_address = &addr_vec[(current_pos - 1) * compri_addr_len];
    memcpy(&addr.u8[pref_elided], current_address, addr_len);

    if (ipv6_addr_is_multicast(&ipv6->dst)) {
        DEBUG("RPL SRH: found a multicast destination address - discard\n");
        return GNRC_IPV6_EXT_RH_ERROR;
    }
    if (ipv6_addr_is_multicast(&addr)) {
        DEBUG("RPL SRH: found a multicast address in next address - discard\n");
        return GNRC_IPV6_EXT_RH_ERROR;
    }

    /* check if multiple addresses of my interface exist */
    if ((*err_ptr = _contains_multiple_of_my_addr(&ipv6->dst, rh, num_addr,
                                                  compri_addr_len))) {
        return GNRC_IPV6_EXT_RH_ERROR;
    }
    rh->seg_left = new_seg_left;
    memcpy(current_address, &ipv6->dst.u8[pref_elided], addr_len);

    DEBUG("RPL SRH: Next hop: %s at position %d\n",
          ipv6_addr_to_str(addr_str, &addr, sizeof(addr_str)), current_pos);

    memcpy(&ipv6->dst, &addr, sizeof(ipv6->dst));
    /*Set the link-local address prefix (Prefix = 64 bit (8 Bytes))*/
    memcpy(&ipv6->dst, &link_local_prefix, 8);
    return GNRC_IPV6_EXT_RH_FORWARDED;
}

static uint8_t _ipv6_addr_matching_bytes(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    if ((a == NULL) || (b == NULL)) {
        return 0;
    }

    if (a == b) {
        return sizeof(ipv6_addr_t);
    }

    for (size_t i = 0; i < sizeof(ipv6_addr_t); i++) {
        if (((uint8_t *)a)[i] != ((uint8_t *)b)[i]) {
            return i;
        }
    }
    return 16;
}

static uint8_t _compute_compression_index(ipv6_addr_t *route_list, size_t route_length,
                                          ipv6_addr_t *dest_addr)
{
    uint8_t compri = 15;
    for (uint8_t i = 0; i < route_length; i++) {
        compri = MIN(compri, _ipv6_addr_matching_bytes(&route_list[i], dest_addr));
    }

    return compri;
}

gnrc_pktsnip_t *gnrc_rpl_srh_insert(gnrc_pktsnip_t *pkt, ipv6_hdr_t *ipv6_hdr)
{
    if (pkt == NULL || ipv6_hdr == NULL) {
        DEBUG("RPL SRH: Invalid parameters: pkt or ipv6_hdr is NULL.\n");
        return NULL;
    }
    gnrc_pktsnip_t *srh_snip;
    gnrc_rpl_srh_t *srh;
    ipv6_addr_t *dest_addr = &ipv6_hdr->dst;
    ipv6_addr_t route_list[GNRC_SR_MAX_ROUTE_SIZE];

    uint8_t ext_len = 0;
    uint8_t compri = 15, compre = 15; /* ComprI and ComprE fields of the RPL Source Routing Header. */
    uint8_t padding = 0;

    /* Get the route to destination */
    size_t route_length = 0;
    if (gnrc_sr_get_full_route(dest_addr, route_list, &route_length) < 0) {
        DEBUG("RPL SRH: Failed to get a route to destination %s.\n",
              ipv6_addr_to_str(addr_str, dest_addr, sizeof(addr_str)));
        return NULL;
    }
    DEBUG("RPL SRH: A source route to %s has been found.\n",
          ipv6_addr_to_str(addr_str, dest_addr, sizeof(addr_str)));

    /* Find compre and compri, supposing compre=compri */
    compri = _compute_compression_index(route_list, route_length, dest_addr);
    compre = compri;

    if (compri > MAX_COMPR_INDEX) {
        DEBUG("RPL SRH: Invalid compression.\n");
        return NULL;
    }

    /* Find SRH ext length */
    ext_len = sizeof(gnrc_rpl_srh_t) +
              (route_length - 2) * (IPV6_ADDR_LEN - compri) +
              (IPV6_ADDR_LEN - compre);

    padding = ext_len % 8 == 0 ? 0 : (8 - (ext_len % 8));
    ext_len += padding;

    /* Init the SRH */
    srh_snip = gnrc_pktbuf_add(pkt->next, NULL, ext_len, GNRC_NETTYPE_IPV6_EXT);
    if (srh_snip == NULL) {
        DEBUG("RPL SRH: No space in PKT Buffer.\n");
        return NULL;
    }
    srh = (gnrc_rpl_srh_t *)srh_snip->data;

    srh->nh = ipv6_hdr->nh;
    srh->len = (ext_len - 8) / 8;
    srh->type = IPV6_EXT_RH_TYPE_RPL_SRH;
    srh->seg_left = route_length - 1;
    srh->compr = (compri << 4) + compre;
    srh->pad_resv = padding << 4;
    srh->resv = 0;

    DEBUG(
        "RPL SRH: A new SRH has been created: Len=%d, No. of Segments=%d, Compressed Bytes=%d, Padding Size=%d.\n", \
        srh->len, srh->seg_left, (srh->compr & 0x0F), ((srh->pad_resv & 0xF0) >> 4));


    uint8_t *addr_vector = (uint8_t *)(srh + 1);
    for (int8_t i = route_length - 2; i >= 0; i--) {
        memcpy(addr_vector, ((uint8_t *)&route_list[i]) + compri, IPV6_ADDR_LEN - compri);
        addr_vector += (IPV6_ADDR_LEN - compri);
    }

    /* Update the new dst to the next-to-root node*/
    dest_addr = &route_list[(route_length - 1)];

    if (dest_addr != NULL) {
        memcpy(((uint8_t *)&ipv6_hdr->dst) + 0, ((uint8_t *)&link_local_prefix), 8);
        /* Set the link-local address prefix */
        memcpy(((uint8_t *)&ipv6_hdr->dst) + 8, ((uint8_t *)dest_addr) + 8, 8);
    }

    else {
        DEBUG("RPL SRH: Invalid destination address.\n");
        return NULL;
    }

    /* Update ipv6 packet */
    ipv6_hdr->len = byteorder_htons(byteorder_ntohs(ipv6_hdr->len) + ext_len);
    ipv6_hdr->nh = PROTNUM_IPV6_EXT_RH;
    pkt->next = srh_snip;

    return pkt;

}
/** @} */
