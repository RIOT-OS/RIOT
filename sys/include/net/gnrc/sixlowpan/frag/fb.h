/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup net_gnrc_sixlowpan_frag_fb 6LoWPAN fragmentation buffer
 * @ingroup  net_gnrc_sixlowpan_frag
 * @brief   Buffer for asynchronous 6LoWPAN fragmentation
 * @{
 *
 * @file
 * @brief   Fragmentation buffer definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_FRAG_FB_H
#define NET_GNRC_SIXLOWPAN_FRAG_FB_H

#include <stdbool.h>
#include <stdint.h>

#include "msg.h"
#include "net/gnrc/pkt.h"
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_HINT
#include "net/gnrc/sixlowpan/frag/hint.h"
#endif /* MODULE_GNRC_SIXLOWPAN_FRAG_HINT */
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR)
#include "net/gnrc/sixlowpan/frag/sfr_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type for passing one 6LoWPAN fragment down the network stack
 */
#define GNRC_SIXLOWPAN_FRAG_FB_SND_MSG      (0x0225)

/**
 * @brief   6LoWPAN fragmentation buffer entry.
 */
typedef struct {
    gnrc_pktsnip_t *pkt;    /**< Pointer to the IPv6 packet to be fragmented */
    /**
     * @brief   Length of just the (uncompressed) IPv6 packet to be fragmented
     *
     * @note    With @ref net_gnrc_sixlowpan_frag_sfr this denotes the
     *          _compressed form_ of the datagram
     */
    uint16_t datagram_size;
    uint16_t tag;           /**< Tag used for the fragment */
    uint16_t offset;        /**< Offset of the Nth fragment from the beginning of the
                             *   payload datagram */
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR)
    /**
     * @brief   Extension for selective fragment recovery.
     */
    gnrc_sixlowpan_frag_sfr_fb_t sfr;
#endif  /* IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) */
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_HINT
    /**
     * @brief   Hint for the size (smaller than link-layer PDU) for the next
     *          fragment to sent
     */
    gnrc_sixlowpan_frag_hint_t hint;
#endif /* MODULE_GNRC_SIXLOWPAN_FRAG_HINT */
} gnrc_sixlowpan_frag_fb_t;

#ifdef TEST_SUITES
/**
 * @brief   Reset fragmentation buffer
 *
 * @note    Only available with test
 */
void gnrc_sixlowpan_frag_fb_reset(void);
#endif

/**
 * @brief   Allocates a fragmentation buffer entry
 *
 * @return  A fragmentation buffer entry if available
 * @return  NULL, otherwise
 */
gnrc_sixlowpan_frag_fb_t *gnrc_sixlowpan_frag_fb_get(void);

/**
 * @brief   Get an existing fragmentation buffer entry by a given tag
 *
 * @param[in] tag   Tag to search for.
 *
 * @return a fragmentation buffer entry if one with @p tag exists.
 * @return NULL, if no fragmentation buffer entry can be found.
 */
gnrc_sixlowpan_frag_fb_t *gnrc_sixlowpan_frag_fb_get_by_tag(uint16_t tag);

/**
 * @brief   Generate a new datagram tag for sending
 *
 * @return  A new datagram tag.
 */
uint16_t gnrc_sixlowpan_frag_fb_next_tag(void);

#if defined(TEST_SUITES) && !defined(DOXYGEN)
#include "sched.h"

/* can't include `net/sixlowpan.h` as this would create a cyclical include */
extern kernel_pid_t gnrc_sixlowpan_get_pid(void);
#endif

/**
 * @brief   Sends a message to pass a further fragment down the network stack
 *
 * @see GNRC_SIXLOWPAN_MSG_FRAG_SND
 *
 * @param[in] fbuf  A fragmentation buffer entry
 *]
 * @return  true, when the message was sent
 * @return  false when sending the message failed.
 */
static inline bool gnrc_sixlowpan_frag_fb_send(gnrc_sixlowpan_frag_fb_t *fbuf)
{
    msg_t msg;

    msg.content.ptr = fbuf;
    msg.type = GNRC_SIXLOWPAN_FRAG_FB_SND_MSG;
#ifdef TEST_SUITES
    return (msg_try_send(&msg, gnrc_sixlowpan_get_pid()) > 0);
#else
    return (msg_send_to_self(&msg) != 0);
#endif
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_SIXLOWPAN_FRAG_FB_H */
