/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup net_gnrc_sixlowpan_frag_sfr_congure Congestion control for 6LoWPAN SFR
 * @ingroup net_gnrc_sixlowpan_frag_sfr
 *
 * @brief Congestion control for 6LoWPAN SFR using the @ref sys_congure
 *
 * When included, this module enables congestion control for 6LoWPAN Selective Fragment Recovery
 * (SFR). The flavor of congestion control can be selected using the following sub-modules:
 *
 * - @ref net_gnrc_sixlowpan_frag_sfr_congure_sfr (the default)
 * - @ref net_gnrc_sixlowpan_frag_sfr_congure_quic
 * - @ref net_gnrc_sixlowpan_frag_sfr_congure_reno
 * - @ref net_gnrc_sixlowpan_frag_sfr_congure_abe
 * @{
 *
 * @file
 * @brief   Congure definitions for @ref net_gnrc_sixlowpan_frag_sfr
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_H
#define NET_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_H

#include <stdbool.h>
#include <stdlib.h>

#include "congure.h"
#include "modules.h"
#include "net/gnrc/sixlowpan/frag/sfr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The user-defined window unit for congure is one fragment with SFR
 */
#define GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_UNIT    (1U)

#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE) || DOXYGEN
/**
 * @brief   Retrieve CongURE state object from a pool of free objects
 *
 * Needs to be defined in for a each CongURE implementation `congure_x` e.g. as
 * a sub-module `gnrc_sixlowpan_frag_sfr_congure_x` and call the respective
 * `congure_x_snd_setup` function when a free object is available for that
 * object. As such, congure_snd_t::driver == NULL can be used as an identifier
 * if a state object is free.
 *
 * The pool of objects has to have an initial size of at least
 * @ref CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE.
 *
 * The window unit is @ref GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_UNIT.
 *
 * @return  A CongURE state object on success
 * @return  NULL, if no free CongURE state object is available (including when
 *          when module `gnrc_sixlowpan_frag_sfr_congure` is not included).
 */
congure_snd_t *gnrc_sixlowpan_frag_sfr_congure_snd_get(void);
#else
static inline congure_snd_t *gnrc_sixlowpan_frag_sfr_congure_snd_get(void)
{
    return NULL;
}
#endif

/**
 * @brief   Frees the CongURE state object
 *
 * This makes a CongURE state object retrievable with
 * @ref gnrc_sixlowpan_frag_sfr_congure_snd_get again.
 *
 * @pre CongURE object is not NULL when called with module
 *      `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @param[in] c     A CongURE state object
 *
 * @note    Does not do anything without the module
 *          `gnrc_sixlowpan_frag_sfr_congure`
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_free(congure_snd_t *c)
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    c->driver = NULL;
#else
    (void)c;
#endif
}

/**
 * @brief   Frees the CongURE state object of a fragmentation buffer and set's
 *          it to `NULL`
 *
 * @param[in] fb    A fragmentation buffer entry
 *
 * @note    Does not do anything without the module
 *          `gnrc_sixlowpan_frag_sfr_congure`
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_destroy(gnrc_sixlowpan_frag_fb_t *fb)
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    gnrc_sixlowpan_frag_sfr_congure_snd_free(fb->sfr.congure);
    fb->sfr.congure = NULL;
#else
    (void)fb;
#endif
}

/**
 * @brief   Initializes a CongURE state object in a fragmentation buffer entry
 *
 * @param[in] fb    A fragmentation buffer entry
 *
 * @note    Does not do anything without the module
 *          `gnrc_sixlowpan_frag_sfr_congure`
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_init(gnrc_sixlowpan_frag_fb_t *fb)
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    fb->sfr.congure->driver->init(fb->sfr.congure, fb);
#else
    (void)fb;
#endif
}

/**
 * @brief   Retrieve CongURE state object when not retrieved and initialize it
 *          for a fragmentation buffer entry
 *
 * @param[in] fb    A fragmentation buffer entry
 *
 * @note    Does not do anything without the module
 *          `gnrc_sixlowpan_frag_sfr_congure`
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_setup(gnrc_sixlowpan_frag_fb_t *fb)
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    if (fb->sfr.congure == NULL) {
        fb->sfr.congure = gnrc_sixlowpan_frag_sfr_congure_snd_get();
        assert(fb->sfr.congure);
        gnrc_sixlowpan_frag_sfr_congure_snd_init(fb);
        if (fb->sfr.congure->cwnd > CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE) {
            fb->sfr.congure->cwnd = CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE;
        }
    }
#else
    (void)fb;
#endif
}

/**
 * @brief   Checks if given fragmentation buffer entry is within congestion
 *          window
 *
 * @pre     CongURE object of the fragmentation buffer entry is initialized when
 *          called with module `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @param[in] fb    A fragmentation buffer entry
 *
 * @note    Without the module `gnrc_sixlowpan_frag_sfr_congure` the
 *          fragmentation buffer entry is checked against
 *          @ref CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE
 *
 * @retval  true    When @p fb is in congestion window
 * @retval  false   When @p fb is not in congestion window
 */
static inline bool gnrc_sixlowpan_frag_sfr_congure_snd_in_cwnd(gnrc_sixlowpan_frag_fb_t *fb)
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    return fb->sfr.frags_sent < fb->sfr.congure->cwnd;
#else
    return fb->sfr.frags_sent < CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE;
#endif
}

/**
 * @brief   Checks if given fragmentation buffer entry would still be within
 *          congestion window after next send
 *
 * @pre     CongURE object of the fragmentation buffer entry is initialized when
 *          called with module `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @note    Without the module `gnrc_sixlowpan_frag_sfr_congure` the
 *          fragmentation buffer entry is checked against
 *          @ref CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE
 *
 * @retval  true    When @p fb can still send one fragment under the congestion
 *                  window constraint.
 * @retval  false   When @p fb can not still send one fragment under the
 *                  congestion window constraint.
 */
static inline bool gnrc_sixlowpan_frag_sfr_congure_snd_next_in_cwnd(gnrc_sixlowpan_frag_fb_t *fb)
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    return (fb->sfr.frags_sent + GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_UNIT)
        < fb->sfr.congure->cwnd;
#else
    return (fb->sfr.frags_sent + GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_UNIT)
        < CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE;
#endif
}

/**
 * @brief   Checks if inter-frame gap is provided
 *
 * Either because @ref CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US is greater
 * 0 or module `gnrc_sixlowpan_frag_sfr_congure` is provided
 *
 * @retval  true    When an inter-frame gap can be provided
 * @retval  false   When the inter-frame gap is supposed to be 0.
 */
static inline bool gnrc_sixlowpan_frag_sfr_congure_snd_has_inter_frame_gap(void)
{
    return (CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US > 0) ||
           IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE);
}

/**
 * @brief   Returns inter-frame gap if provided by CongURE implementation
 *
 * When module `gnrc_sixlowpan_frag_sfr_congure` is provided it will provide
 * congure_snd_driver_t::inter_message_interval() of the CongURE state object of
 * the provided fragmentation buffer with @ref
 * CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US as a lower bound.
 * If congure_snd_driver_t::inter_message_interval returns -1, @p fb is NULL, or
 * without the module `gnrc_sixlowpan_frag_sfr_congure` it will return
 * @ref CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US
 *
 * @pre     CongURE object of the fragmentation buffer entry is initialized when
 *          `fb` is `NULL` and when called with module
 *          `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @param[in]   fb A fragmentation buffer. May be NULL.
 *
 * @return  The inter-frame gap for the given fragmentation buffer, but
 */
static inline uint32_t gnrc_sixlowpan_frag_sfr_congure_snd_inter_frame_gap(
        gnrc_sixlowpan_frag_fb_t *fb
    )
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    if (fb != NULL) {
        congure_snd_t *c = fb->sfr.congure;
        int32_t res = c->driver->inter_msg_interval(
            c, GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_UNIT
        );

        if ((res >= 0) &&
            ((unsigned)res >= CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US)) {
            return (uint32_t)res;
        }
    }
#else
    (void)fb;
#endif
    return CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US;
}

/**
 * @brief   Report to CongURE that a fragment was sent.
 *
 * Calls congure_snd_driver_t::report_msg_sent for the CongURE state object of
 * @p fb with `msg_size` @ref GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_UNIT.
 *
 * @pre     CongURE object of the fragmentation buffer entry is initialized when
 *          called with module `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @param[in]   fb A fragmentation buffer.
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_report_frag_sent(
        gnrc_sixlowpan_frag_fb_t *fb
    )
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    congure_snd_t *c = fb->sfr.congure;

    c->driver->report_msg_sent(c, GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_UNIT);
#else
    (void)fb;
#endif
}

/**
 * @brief   Report to CongURE that a fragment as discarded.
 *
 * Calls congure_snd_driver_t::report_msg_discarded for the CongURE state object
 * of @p fb with `msg_size` @ref GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_UNIT.
 *
 * @pre     CongURE object of the fragmentation buffer entry is initialized when
 *          called with module `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @param[in]   fb A fragmentation buffer.
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_report_frag_discard(
        gnrc_sixlowpan_frag_fb_t *fb
    )
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    congure_snd_t *c = fb->sfr.congure;

    c->driver->report_msg_discarded(c, GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_UNIT);
#else
    (void)fb;
#endif
}

/**
 * @brief   Report to CongURE that the ACK for a fragment timed out.
 *
 * Calls congure_snd_driver_t::report_msgs_timeout for the CongURE state object
 * of @p fb with gnrc_sixlowpan_frag_sfr_fb_t::window of @p fb as `msgs`.
 *
 * @pre     CongURE object of the fragmentation buffer entry is initialized when
 *          called with module `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @param[in]   fb      A fragmentation buffer.
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_report_frags_timeout(
        gnrc_sixlowpan_frag_fb_t *fb
    )
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    congure_snd_t *c = fb->sfr.congure;

    c->driver->report_msgs_timeout(c, (congure_snd_msg_t *)(&fb->sfr.window));
#else
    (void)fb;
#endif
}

/**
 * @brief   Report to CongURE that a number of fragments are known to be lost.
 *
 * Calls congure_snd_driver_t::report_msgs_lost for the CongURE state object
 * of @p fb with @p frags.
 *
 * @pre     CongURE object of the fragmentation buffer entry is initialized when
 *          called with module `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @param[in]   fb      A fragmentation buffer.
 * @param[in]   frags   A collection of messages that are known to be lost.
 *                      The list may be changed by the function.
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_report_frags_lost(
        gnrc_sixlowpan_frag_fb_t *fb, congure_snd_msg_t *frags
    )
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    congure_snd_t *c = fb->sfr.congure;

    c->driver->report_msgs_lost(c, frags);
#else
    (void)fb;
    (void)frags;
#endif
}

/**
 * @brief   Report to CongURE that a number of fragments are known to be lost.
 *
 * Calls congure_snd_driver_t::report_msgs_acked for the CongURE state object
 * of @p fb with @p frag and @p ack.
 *
 * @pre     CongURE object of the fragmentation buffer entry is initialized when
 *          called with module `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @param[in]   fb      A fragmentation buffer.
 * @param[in]   frag    The ACK'd fragment.
 * @param[in]   ack     The received ACK.
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_report_frag_acked(
        gnrc_sixlowpan_frag_fb_t *fb,
        congure_snd_msg_t *frag,
        congure_snd_ack_t *ack
    )
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    congure_snd_t *c = fb->sfr.congure;

    c->driver->report_msg_acked(c, frag, ack);
    if (c->cwnd > CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE) {
        c->cwnd = CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE;
    }
#else
    (void)fb;
    (void)frag;
    (void)ack;
#endif
}

/**
 * @brief   Report to CongURE that ECN bit was set in an ACK.
 *
 * Calls congure_snd_driver_t::report_ecn_ce for the CongURE state object
 * of @p fb with @p time.
 *
 * @pre     CongURE object of the fragmentation buffer entry is initialized when
 *          called with module `gnrc_sixlowpan_frag_sfr_congure` used.
 *
 * @param[in]   fb      A fragmentation buffer.
 * @param[in]   time    Timestamp in milliseconds of the earliest fragment
 *                      for which the notified congestion occurred was sent.
 */
static inline void gnrc_sixlowpan_frag_sfr_congure_snd_report_ecn(
        gnrc_sixlowpan_frag_fb_t *fb, uint32_t time
    )
{
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
    congure_snd_t *c = fb->sfr.congure;

    c->driver->report_ecn_ce(c, (ztimer_now_t)time);
#else
    (void)fb;
    (void)time;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE_H */
/** @} */
