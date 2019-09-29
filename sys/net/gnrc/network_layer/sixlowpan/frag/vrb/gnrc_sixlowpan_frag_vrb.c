/*
 * Copyright (C) 2019 Freie Universität Berlin
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

#include "net/ieee802154.h"
#include "xtimer.h"

#include "net/gnrc/sixlowpan/frag/vrb.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static gnrc_sixlowpan_frag_vrb_t _vrb[GNRC_SIXLOWPAN_FRAG_VRB_SIZE];
static char l2addr_str[3 * IEEE802154_LONG_ADDRESS_LEN];

#if !defined(MODULE_GNRC_SIXLOWPAN_FRAG) && defined(TEST_SUITES)
/* mock for e.g. testing */
uint16_t tag = 0;

uint16_t gnrc_sixlowpan_frag_next_tag(void)
{
    return tag++;
}
#endif  /* !defined(MODULE_GNRC_SIXLOWPAN_FRAG) && defined(TEST_SUITES) */

gnrc_sixlowpan_frag_vrb_t *gnrc_sixlowpan_frag_vrb_add(
        const gnrc_sixlowpan_frag_rb_base_t *base,
        gnrc_netif_t *out_netif, const uint8_t *out_dst, size_t out_dst_len)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = NULL;

    assert(base != NULL);
    assert(out_netif != NULL);
    assert(out_dst != NULL);
    assert(out_dst_len > 0);
    for (unsigned i = 0; i < GNRC_SIXLOWPAN_FRAG_VRB_SIZE; i++) {
        gnrc_sixlowpan_frag_vrb_t *ptr = &_vrb[i];

        if (gnrc_sixlowpan_frag_vrb_entry_empty(ptr) ||
            (memcmp(&ptr->super, base, sizeof(ptr->super)) == 0)) {
            vrbe = ptr;
            if (gnrc_sixlowpan_frag_vrb_entry_empty(vrbe)) {
                vrbe->super = *base;
                vrbe->out_netif = out_netif;
                memcpy(vrbe->out_dst, out_dst, out_dst_len);
                vrbe->out_tag = gnrc_sixlowpan_frag_next_tag();
                vrbe->out_dst_len = out_dst_len;
                DEBUG("6lo vrb: creating entry (%s, ",
                      gnrc_netif_addr_to_str(vrbe->super.src,
                                             vrbe->super.src_len,
                                             l2addr_str));
                DEBUG("%s, %u, %u) => ",
                      gnrc_netif_addr_to_str(vrbe->super.dst,
                                             vrbe->super.dst_len,
                                             l2addr_str),
                      (unsigned)vrbe->super.datagram_size, vrbe->super.tag);
                DEBUG("(%s, %u)\n",
                      gnrc_netif_addr_to_str(vrbe->out_dst,
                                             vrbe->out_dst_len,
                                             l2addr_str), vrbe->out_tag);
            }
            break;
        }
    }
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_STATS
    if (vrbe == NULL) {
        gnrc_sixlowpan_frag_stats_get()->vrb_full++;
    }
#endif
    return vrbe;
}

gnrc_sixlowpan_frag_vrb_t *gnrc_sixlowpan_frag_vrb_get(
        const uint8_t *src, size_t src_len,
        const uint8_t *dst, size_t dst_len,
        size_t datagram_size, unsigned src_tag)
{
    DEBUG("6lo vrb: trying to get entry for (%s, ",
          gnrc_netif_addr_to_str(src, src_len, l2addr_str));
    DEBUG("%s, %u, %u)\n",
          gnrc_netif_addr_to_str(dst, dst_len, l2addr_str),
          (unsigned)datagram_size, src_tag);
    for (unsigned i = 0; i < GNRC_SIXLOWPAN_FRAG_VRB_SIZE; i++) {
        gnrc_sixlowpan_frag_vrb_t *vrbe = &_vrb[i];

        if ((vrbe->super.datagram_size == datagram_size) &&
            (vrbe->super.tag == src_tag) &&
            (vrbe->super.src_len == src_len) &&
            (vrbe->super.dst_len == dst_len) &&
            (memcmp(vrbe->super.src, src, src_len) == 0) &&
            (memcmp(vrbe->super.dst, dst, dst_len) == 0)) {
            DEBUG("6lo vrb: got VRB to (%s, %u)\n",
                  gnrc_netif_addr_to_str(vrbe->out_dst,
                                         vrbe->out_dst_len,
                                         l2addr_str), vrbe->out_tag);
            return vrbe;
        }
    }
    DEBUG("6lo vrb: no entry found\n");
    return NULL;
}

void gnrc_sixlowpan_frag_vrb_gc(void)
{
    uint32_t now_usec = xtimer_now_usec();

    for (unsigned i = 0; i < GNRC_SIXLOWPAN_FRAG_VRB_SIZE; i++) {
        if (!gnrc_sixlowpan_frag_vrb_entry_empty(&_vrb[i]) &&
            (now_usec - _vrb[i].super.arrival) > GNRC_SIXLOWPAN_FRAG_VRB_TIMEOUT_US) {
            DEBUG("6lo vrb: entry (%s, ",
                  gnrc_netif_addr_to_str(_vrb[i].super.src,
                                         _vrb[i].super.src_len,
                                         l2addr_str));
            DEBUG("%s, %u, %u) timed out\n",
                  gnrc_netif_addr_to_str(_vrb[i].super.dst,
                                         _vrb[i].super.dst_len,
                                         l2addr_str),
                  (unsigned)_vrb[i].super.datagram_size, _vrb[i].super.tag);
            gnrc_sixlowpan_frag_vrb_rm(&_vrb[i]);
        }
    }
}

#ifdef TEST_SUITES
void gnrc_sixlowpan_frag_vrb_reset(void)
{
    memset(_vrb, 0, sizeof(_vrb));
}
#endif

/** @} */
