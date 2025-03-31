/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_GNRC_SIXLOWPAN_CONFIG_H
#define NET_GNRC_SIXLOWPAN_CONFIG_H
/**
 * @defgroup    net_gnrc_sixlowpan_config GNRC 6LoWPAN compile configurations
 * @ingroup     net_gnrc_sixlowpan
 * @ingroup     net_gnrc_conf
 * @brief
 * @{
 *
 * @file
 * @brief   Configuration macros for @ref net_gnrc_sixlowpan
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "modules.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the 6LoWPAN thread.
 *
 * @note    The message queue was previously allocated on the stack.
 *          The default number of messages is 2³.
 *          Given sizeof(msg_t) == 8, the stack size is reduced by 64 bytes.
 */
#ifndef GNRC_SIXLOWPAN_STACK_SIZE
#define GNRC_SIXLOWPAN_STACK_SIZE           ((THREAD_STACKSIZE_DEFAULT) - 64)
#endif

/**
 * @brief   Default priority for the 6LoWPAN thread.
 */
#ifndef GNRC_SIXLOWPAN_PRIO
#define GNRC_SIXLOWPAN_PRIO                 (THREAD_PRIORITY_MAIN - 4)
#endif

/**
 * @brief   Default message queue size to use for the 6LoWPAN thread (as
 *          exponent of 2^n).
 *
 * As the queue size ALWAYS needs to be power of two, this option represents the
 * exponent of 2^n, which will be used as the size of the queue.
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_SIXLOWPAN_MSG_QUEUE_SIZE_EXP   (3U)
#endif

/**
 * @brief   Number of datagrams that can be fragmented simultaneously
 *
 * This determines the number of @ref gnrc_sixlowpan_frag_fb_t instances
 * available.
 *
 * @note    Only applicable with
 *          [gnrc_sixlowpan_frag_fb](@ref net_gnrc_sixlowpan_frag_fb) module
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR)
#define CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE         (4U)
#else   /* defined(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) */
#define CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE         (1U)
#endif  /* defined(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) */
#endif

/**
 * @brief   Size of the reassembly buffer
 *
 * @note    Only applicable with
 *          [gnrc_sixlowpan_frag_rb](@ref net_gnrc_sixlowpan_frag_rb) module
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE
#define CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE       (4U)
#endif

/**
 * @brief   Timeout for reassembly buffer entries in microseconds
 *
 * @note    Only applicable with
 *          [gnrc_sixlowpan_frag_rb](@ref net_gnrc_sixlowpan_frag_rb) module
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US
#define CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US (3U * US_PER_SEC)
#endif

/**
 * @brief   Do not override oldest datagram when reassembly buffer is full
 *
 * @note    Only applicable with
 *          [gnrc_sixlowpan_frag_rb](@ref net_gnrc_sixlowpan_frag_rb) module
 *
 * When not set, it will cause the reassembly buffer to override the oldest
 * entry when a fragment for a new datagram is received. When set, only the
 * oldest entry that is older than @ref
 * CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US will be overwritten (they will
 * still timeout normally if reassembly buffer is not full).
 */
#ifdef DOXYGEN
#define CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DO_NOT_OVERRIDE
#endif

/**
 * @brief   Deletion timer for reassembly buffer entries in microseconds
 *
 * @note    Only applicable with
 *          [gnrc_sixlowpan_frag_rb](@ref net_gnrc_sixlowpan_frag_rb) module
 *
 * Time to pass between completion of a datagram and the deletion of its
 * reassembly buffer entry. If this value is 0, the entry is dropped
 * immediately. Use this value to prevent re-creation of a reassembly buffer
 * entry on late arriving link-layer duplicates.
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER
#define CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER              (0U)
#endif

/**
 * @brief   Registration lifetime in minutes for the address registration option
 *
 * This value should be adapted to the devices power-lifecycle so that it is
 * greater than the time the device spends sleeping.
 *
 * @see [RFC 6775, section 5.8.1](https://tools.ietf.org/html/rfc6775#section-5.8.1)
 *
 * @note    Only applicable with [gnrc_ipv6_nib](@ref net_gnrc_ipv6_nib) and
 *          [gnrc_sixlowpan_nd](@ref net_gnrc_sixlowpan_nd) modules. The first
 *          provides automatic sending of neighbor solicitations, the latter
 *          provides capabilities to build the address registration option as a
 *          @ref gnrc_pktsnip_t
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_ND_AR_LTIME
#define CONFIG_GNRC_SIXLOWPAN_ND_AR_LTIME          (15U)
#endif

/**
 * @brief   Size of the virtual reassembly buffer
 *
 * @see     https://tools.ietf.org/html/draft-ietf-lwig-6lowpan-virtual-reassembly-01
 *
 * @note    Only applicable with
 *          [gnrc_sixlowpan_frag_vrb](@ref net_gnrc_sixlowpan_frag_vrb) module,
 *          but has also a direct influence on the number of available
 *          gnrc_sixlowpan_frag_rb_int_t entries.
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE
#define CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE        (16U)
#endif  /* CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE */

/**
 * @brief   Timeout for a VRB entry in microseconds
 *
 * @see     https://tools.ietf.org/html/draft-ietf-lwig-6lowpan-virtual-reassembly-01
 *
 * @note    Only applicable with
 *          [gnrc_sixlowpan_frag_vrb](@ref net_gnrc_sixlowpan_frag_vrb) module.
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_TIMEOUT_US
#define CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_TIMEOUT_US  (CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US)
#endif  /* CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_TIMEOUT_US */

/**
 * @name Selective fragment recovery configuration
 * @see  [RFC 8931, section 7.1]
 *       (https://tools.ietf.org/html/rfc8931#section-7.1)
 * @note Only applicable with gnrc_sixlowpan_frag_sfr module
 * @{
 */
/**
 * @brief   Default minimum value for fragment size (MinFragmentSize)
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_MIN_FRAG_SIZE
#define CONFIG_GNRC_SIXLOWPAN_SFR_MIN_FRAG_SIZE     96U
#endif

/**
 * @brief   Default maximum value for fragment size (MaxFragmentSize)
 *
 * It must be lower than the minimum MTU along the path. A large value augments
 * the chances of buffer bloat and transmission loss. The value must be less
 * than 512 if the unit is defined for the PHY layer is the octet.
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_MAX_FRAG_SIZE
#define CONFIG_GNRC_SIXLOWPAN_SFR_MAX_FRAG_SIZE     112U
#endif

/**
 * @brief   Default value for fragment size that the sender should use to start
 *          with (OptFragmentSize)
 *
 * @pre     Must be inclusively between
 *          @ref CONFIG_GNRC_SIXLOWPAN_SFR_MIN_FRAG_SIZE and
 *          @ref CONFIG_GNRC_SIXLOWPAN_SFR_MAX_FRAG_SIZE
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_OPT_FRAG_SIZE
#define CONFIG_GNRC_SIXLOWPAN_SFR_OPT_FRAG_SIZE     CONFIG_GNRC_SIXLOWPAN_SFR_MAX_FRAG_SIZE
#endif

/**
 * @brief   Indicates whether the sender should react to Explicit Congestion Notification (UseECN)
 *
 * When the sender reacts to Explicit Congestion Notification (ECN) its window
 * size will vary between @ref CONFIG_GNRC_SIXLOWPAN_SFR_MIN_WIN_SIZE and @ref
 * CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE.
 */
#ifdef CONFIG_GNRC_SIXLOWPAN_SFR_USE_ECN
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_CONGURE)
#define CONFIG_GNRC_SIXLOWPAN_SFR_USE_ECN           1U
#else
#define CONFIG_GNRC_SIXLOWPAN_SFR_USE_ECN           0U
#endif
#endif

/**
 * @brief   Default minimum value of window size that the sender can use
 *          (MinWindowSize)
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_MIN_WIN_SIZE
#define CONFIG_GNRC_SIXLOWPAN_SFR_MIN_WIN_SIZE      1U
#endif

/**
 * @brief   Default maximum value of window size that the sender can use
 *          (MaxWindowSize)
 *
 * @warning **Must** be lesser than 32.
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE
#define CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE      16U
#endif

/**
 * @brief   Default value of window size that the sender should start with
 *          (OptWindowSize)
 *
 * @pre     Must be inclusively between
 *          @ref CONFIG_GNRC_SIXLOWPAN_SFR_MIN_WIN_SIZE and
 *          @ref CONFIG_GNRC_SIXLOWPAN_SFR_MAX_WIN_SIZE
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE
#define CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE      16U
#endif

/**
 * @brief   Default minimum amount of time between transmissions in microseconds
 *          (InterFrameGap)
 *
 * All packets to a same destination, and in particular fragments, may be
 * subject to receive while transmitting and hidden terminal collisions with the
 * next or the previous transmission as the fragments progress along a same
 * path. The InterFrameGap protects the propagation of to one transmission
 * before the next one is triggered and creates a duty cycle that controls the
 * ratio of air and memory in intermediate nodes that a particular datagram will
 * use.
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US
#define CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US    100U
#endif

/**
 * @brief   Minimum RFRAG-ACK timeout in msec before a node takes a next action
 *          (MinARQTimeOut)
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_MIN_ARQ_TIMEOUT_MS
#define CONFIG_GNRC_SIXLOWPAN_SFR_MIN_ARQ_TIMEOUT_MS    350U
#endif

/**
 * @brief   Maximum RFRAG-ACK timeout in msec before a node takes a next action
 *          (MaxARQTimeOut)
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_MAX_ARQ_TIMEOUT_MS
#define CONFIG_GNRC_SIXLOWPAN_SFR_MAX_ARQ_TIMEOUT_MS    700U
#endif

/**
 * @brief   Default RFRAG-ACK timeout in msec before a node takes a next action
 *          (OptARQTimeOut)
 *
 * @pre     Must be inclusively between
 *          @ref CONFIG_GNRC_SIXLOWPAN_SFR_MIN_ARQ_TIMEOUT_MS and
 *          @ref CONFIG_GNRC_SIXLOWPAN_SFR_MAX_ARQ_TIMEOUT_MS
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_OPT_ARQ_TIMEOUT_MS
#define CONFIG_GNRC_SIXLOWPAN_SFR_OPT_ARQ_TIMEOUT_MS \
            CONFIG_GNRC_SIXLOWPAN_SFR_MAX_ARQ_TIMEOUT_MS
#endif

/**
 * @brief   The maximum number of retries for a particular fragment
 *          (MaxFragRetries)
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES
#define CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES          2U
#endif

/**
 * @brief   The maximum number of retries from scratch for a particular
 *          datagram (MaxDatagramRetries)
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_DG_RETRIES
#define CONFIG_GNRC_SIXLOWPAN_SFR_DG_RETRIES            0U
#endif

/**
 * @brief   The numerator for the factor for when to mark ECN on incoming `netif`
 *          queue state
 *
 * When `gnrc_sixlowpan_frag_sfr_ecn_if_in` is compiled in, nodes will set
 * the ECN bit of an RFRAG header when the message queue of the incoming `netif`
 * is filled by
 * @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_IN_NUM / @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_IN_DEN
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_IN_NUM
#define CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_IN_NUM         1U
#endif

/**
 * @brief   The denominator for the factor for when to mark ECN on incoming `netif`
 *          queue state
 *
 * When `gnrc_sixlowpan_frag_sfr_ecn_if_in` is compiled in, nodes will set the
 * ECN bit of an RFRAG header when the message queue of the incoming `netif` is
 * filled by
 * @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_IN_NUM / @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_IN_DEN
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_IN_DEN
#define CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_IN_DEN         2U
#endif

/**
 * @brief   The numerator for the factor for when to mark ECN on the outgoing `netif`'s
 *          output queue state
 *
 * When `gnrc_sixlowpan_frag_sfr_ecn_if_out` is compiled in, nodes will set
 * the ECN bit of an RFRAG header when the output queue of the outgoing `netif`
 * is filled by
 * @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_OUT_NUM / @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_OUT_DEN
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_OUT_NUM
#define CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_OUT_NUM        1U
#endif

/**
 * @brief   The denominator for the factor for when to mark ECN on the outgoing `netif`
 *          output queue state
 *
 * When `gnrc_sixlowpan_frag_sfr_ecn_if_out` is compiled in, nodes will set the
 * ECN bit of an RFRAG header when the output queue of the outgoing `netif` is
 * filled by
 * @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_OUT_NUM / @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_OUT_DEN
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_OUT_DEN
#define CONFIG_GNRC_SIXLOWPAN_SFR_ECN_IF_OUT_DEN        2U
#endif

/**
 * @brief   The numerator for the factor for when to mark ECN on frame queue
 *          state
 *
 * When `gnrc_sixlowpan_frag_sfr_ecn_fqueue` is compiled in, nodes will set the
 * ECN bit of an RFRAG header when the frame queue for SFR is filled by
 * @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_FQUEUE_NUM / @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_FQUEUE_DEN
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_ECN_FQUEUE_NUM
#define CONFIG_GNRC_SIXLOWPAN_SFR_ECN_FQUEUE_NUM        1U
#endif

/**
 * @brief   The denominator for the factor for when to mark ECN on frame queue
 *          state
 *
 * When `gnrc_sixlowpan_frag_sfr_ecn_fqueue` is compiled in, nodes will set the
 * ECN bit of an RFRAG header when the frame queue for SFR is filled by
 * @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_FQUEUE_NUM / @ref CONFIG_GNRC_SIXLOWPAN_SFR_ECN_FQUEUE_DEN
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_ECN_FQUEUE_DEN
#define CONFIG_GNRC_SIXLOWPAN_SFR_ECN_FQUEUE_DEN        2U
#endif

/**
 * @brief   Deactivate automatic handling of ARQ timer
 *
 * This requires an external source (e.g. a test application) to call
 * @ref gnrc_sixlowpan_frag_sfr_arq_timeout() for
 * @ref net_gnrc_sixlowpan_frag_sfr to still work properly.
 */
#ifndef CONFIG_GNRC_SIXLOWPAN_SFR_MOCK_ARQ_TIMER
#define CONFIG_GNRC_SIXLOWPAN_SFR_MOCK_ARQ_TIMER        0U
#endif
/** @} */

/**
 * @brief   Message queue size to use for the 6LoWPAN thread.
 */
#ifndef GNRC_SIXLOWPAN_MSG_QUEUE_SIZE
#define GNRC_SIXLOWPAN_MSG_QUEUE_SIZE    (1 << CONFIG_GNRC_SIXLOWPAN_MSG_QUEUE_SIZE_EXP)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_SIXLOWPAN_CONFIG_H */
