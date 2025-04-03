/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_SIXLOWPAN_SFR_H
#define NET_SIXLOWPAN_SFR_H
/**
 * @defgroup    net_sixlowpan_sfr   6LoWPAN selective fragment recovery
 * @ingroup     net_sixlowpan
 * @brief       Provides dispatches for 6LoWPAN selective fragment recovery and
 *              helper functions
 * @{
 *
 * @file
 * @brief   6LoWPAN selective fragment recovery dispatch type and helper
 *          function definitions.
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "bitfield.h"
#include "byteorder.h"
#include "net/sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mask for generic dispatch of both selective fragment recovery
 *          headers
 *
 * @see SIXLOWPAN_SFR_GEN_DISP
 * @see SIXLOWPAN_SFR_DISP_MASK
 * @see SIXLOWPAN_SFR_RFRAG_DISP
 * @see SIXLOWPAN_SFR_ACK_DISP
 */
#define SIXLOWPAN_SFR_GEN_DISP_MASK     (0xfc)
#define SIXLOWPAN_SFR_GEN_DISP          (0xe8)      /**< generic dispatch for both SFR headers */
#define SIXLOWPAN_SFR_ECN               (0x01U)     /**< explicit congestion notification flag */
#define SIXLOWPAN_SFR_ACK_REQ           (0x80U)     /**< Acknowledgment request flag (for 8 MSB) */
#define SIXLOWPAN_SFR_SEQ_MASK          (0x7cU)     /**< Sequence number mask (for 8 MSB) */
#define SIXLOWPAN_SFR_SEQ_POS           (2U)        /**< Sequence number position (for 8 MSB) */
#define SIXLOWPAN_SFR_SEQ_MAX           (0x1fU)     /**< Maximum value for sequence number */
#define SIXLOWPAN_SFR_FRAG_SIZE_MASK    (0x03ffU)   /**< Fragment size mask */
#define SIXLOWPAN_SFR_FRAG_SIZE_MAX     (0x03ffU)   /**< Maximum value for fragment size */
#define SIXLOWPAN_SFR_ACK_BITMAP_SIZE   (32U)       /**< Acknowledgment bitmap size in bits */

/**
 * @brief   Generic type for selective fragment recovery headers
 *
 * @see [RFC 8931, section 5](https://tools.ietf.org/html/rfc8931#section-5)
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief   Dispatch and explicit congestion notification (ECN) flag
     *
     * In network byte-order the 7 most significant bits are the dispatch (7
     * bits) and the least significant bit is the ECN flag:
     *
     *     +-+-+-+-+-+-+-+-+
     *     |  Dispatch   |E|
     *     +-+-+-+-+-+-+-+-+
     *
     * For @ref sixlowpan_sfr_ack_t the ECN flag represents the ECN echo flag.
     *
     * This module provides helper functions to set, get, check these fields
     * accordingly:
     *
     * - sixlowpan_sfr_is()
     * - sixlowpan_sfr_set_ecn()
     * - sixlowpan_sfr_clear_ecn()
     * - sixlowpan_sfr_ecn()
     * - sixlowpan_sfr_rfrag_set_disp()
     * - sixlowpan_sfr_rfrag_is()
     * - sixlowpan_sfr_ack_set_disp()
     * - sixlowpan_sfr_ack_is()
     */
    uint8_t disp_ecn;
    uint8_t tag;            /**< Datagram tag */
} sixlowpan_sfr_t;

/**
 * @brief   Recoverable fragment header
 *
 * @see [RFC 8931, section 5.1](https://tools.ietf.org/html/rfc8931#section-5.1)
 */
typedef struct __attribute__((packed)) {
    sixlowpan_sfr_t base;   /**< generic part */
    /**
     * @brief   Acknowledgment request flag, sequence number, and fragment size
     *
     * In network byte-order the most significant bit is the acknowledgment
     * request flag, the second to sixth most significant bits are the sequence
     * number (5 bits), and the 10 least significant bits are the fragment size
     * (10 bits):
     *
     *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *     |X| sequence|   fragment_size   |
     *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *
     * This module provides helper functions to set, get, check these fields
     * accordingly:
     *
     * - sixlowpan_sfr_rfrag_set_ack_req()
     * - sixlowpan_sfr_rfrag_clear_ack_req()
     * - sixlowpan_sfr_rfrag_ack_req()
     * - sixlowpan_sfr_rfrag_set_seq()
     * - sixlowpan_sfr_rfrag_get_seq()
     * - sixlowpan_sfr_rfrag_set_frag_size()
     * - sixlowpan_sfr_rfrag_get_frag_size()
     */
    network_uint16_t ar_seq_fs;

    /**
     * @brief   Fragment offset
     *
     * This module provides helper functions to set, get, check these fields
     * accordingly:
     *
     * - sixlowpan_sfr_rfrag_set_offset()
     * - sixlowpan_sfr_rfrag_get_offset()
     */
    network_uint16_t offset;
} sixlowpan_sfr_rfrag_t;

/**
 * @brief   Recoverable fragment (RFRAG) acknowledgment header
 *
 * @see [RFC 8931, section 5.2](https://tools.ietf.org/html/rfc8931#section-5.2)
 */
typedef struct __attribute__((packed)) {
    sixlowpan_sfr_t base;   /**< generic part */
    /**
     * @brief   RFRAG acknowledgment bitmap
     */
    BITFIELD(bitmap, SIXLOWPAN_SFR_ACK_BITMAP_SIZE);
} sixlowpan_sfr_ack_t;

/**
 * @brief   Check if given header is a 6LoWPAN selective fragment recovery
 *          header (either RFRAG or RFRAG ACK)
 *
 * @see @ref SIXLOWPAN_SFR_RFRAG_DISP
 *
 * @param[in] hdr       The 6LoWPAN selective fragment recovery header to check
 *
 * return   true, if @p hdr is a 6LoWPAN selective fragment recovery header
 * return   false, if @p hdr is not a 6LoWPAN selective fragment recovery header
 */
static inline bool sixlowpan_sfr_is(sixlowpan_sfr_t *hdr)
{
    return ((hdr->disp_ecn & SIXLOWPAN_SFR_GEN_DISP_MASK) == SIXLOWPAN_SFR_GEN_DISP);
}

/**
 * @brief   Set the ECN flag in a 6LoWPAN selective fragment recovery header
 *
 * @param[in,out] hdr   A 6LoWPAN selective fragment recovery header
 */
static inline void sixlowpan_sfr_set_ecn(sixlowpan_sfr_t *hdr)
{
    hdr->disp_ecn |= SIXLOWPAN_SFR_ECN;
}

/**
 * @brief   Clear the ECN flag in a 6LoWPAN recoverable fragment header
 *
 * @param[in,out] hdr   A 6LoWPAN selective fragment recovery header
 */
static inline void sixlowpan_sfr_clear_ecn(sixlowpan_sfr_t *hdr)
{
    hdr->disp_ecn &= ~SIXLOWPAN_SFR_ECN;
}

/**
 * @brief   Check if the ECN flag in a 6LoWPAN recoverable fragment header is
 *          set
 *
 * @param[in] hdr   A 6LoWPAN selective fragment recovery header
 *
 * return   true, if ECN flag in @p hdr is set
 * return   false, if ECN flag in @p hdr is cleared
 */
static inline bool sixlowpan_sfr_ecn(const sixlowpan_sfr_t *hdr)
{
    return (hdr->disp_ecn & SIXLOWPAN_SFR_ECN);
}

/**
 * @brief   Initialize 6LoWPAN recoverable fragment dispatch
 *
 * @see @ref SIXLOWPAN_SFR_RFRAG_DISP
 *
 * @param[in,out] hdr   A 6LoWPAN selective fragment recovery header
 */
static inline void sixlowpan_sfr_rfrag_set_disp(sixlowpan_sfr_t *hdr)
{
    hdr->disp_ecn &= ~SIXLOWPAN_SFR_DISP_MASK;
    hdr->disp_ecn |= SIXLOWPAN_SFR_RFRAG_DISP;
}

/**
 * @brief   Check if given header is a 6LoWPAN recoverable fragment header
 *
 * @see @ref SIXLOWPAN_SFR_RFRAG_DISP
 *
 * @param[in] hdr       The 6LoWPAN selective fragment recovery header to check
 *
 * return   true, if @p hdr is a 6LoWPAN recoverable fragment header
 * return   false, if @p hdr is not a 6LoWPAN recoverable fragment header
 */
static inline bool sixlowpan_sfr_rfrag_is(const sixlowpan_sfr_t *hdr)
{
    return ((hdr->disp_ecn & SIXLOWPAN_SFR_DISP_MASK) == SIXLOWPAN_SFR_RFRAG_DISP);
}

/**
 * @brief   Set the Acknowledgment request flag in a 6LoWPAN recoverable
 *          fragment header
 *
 * @param[in,out] hdr   A 6LoWPAN recoverable fragment header
 */
static inline void sixlowpan_sfr_rfrag_set_ack_req(sixlowpan_sfr_rfrag_t *hdr)
{
    hdr->ar_seq_fs.u8[0] |= SIXLOWPAN_SFR_ACK_REQ;
}

/**
 * @brief   Clear the Acknowledgment request flag in a 6LoWPAN recoverable
 *          fragment header
 *
 * @param[in,out] hdr   A 6LoWPAN recoverable fragment header
 */
static inline void sixlowpan_sfr_rfrag_clear_ack_req(sixlowpan_sfr_rfrag_t *hdr)
{
    hdr->ar_seq_fs.u8[0] &= ~SIXLOWPAN_SFR_ACK_REQ;
}

/**
 * @brief   Check if the Acknowledgment request flag in a 6LoWPAN recoverable
 *          fragment header is set
 *
 * @param[in] hdr   A 6LoWPAN recoverable fragment header
 *
 * return   true, if Acknowledgment request flag in @p hdr is set
 * return   false, if Acknowledgment request flag in @p hdr is cleared
 */
static inline bool sixlowpan_sfr_rfrag_ack_req(sixlowpan_sfr_rfrag_t *hdr)
{
    return (hdr->ar_seq_fs.u8[0] & SIXLOWPAN_SFR_ACK_REQ);
}

/**
 * @brief   Set sequence number in a 6LoWPAN recoverable fragment header
 *
 * @pre     `seq <= SIXLOWPAN_SFR_SEQ_MAX`
 *
 * @param[in,out] hdr   A 6LoWPAN recoverable fragment header
 * @param[in] seq       The sequence number to set. Must be lesser or equal
 *                      to @ref SIXLOWPAN_SFR_SEQ_MAX.
 */
static inline void sixlowpan_sfr_rfrag_set_seq(sixlowpan_sfr_rfrag_t *hdr,
                                               uint8_t seq)
{
    assert(seq <= SIXLOWPAN_SFR_SEQ_MAX);
    hdr->ar_seq_fs.u8[0] &= ~SIXLOWPAN_SFR_SEQ_MASK;
    hdr->ar_seq_fs.u8[0] |= (seq << SIXLOWPAN_SFR_SEQ_POS);
}

/**
 * @brief   Get sequence number from a 6LoWPAN recoverable fragment header
 *
 * @param[in] hdr   A 6LoWPAN recoverable fragment header
 *
 * @return  The sequence number of the 6LoWPAN recoverable fragment
 */
static inline uint8_t sixlowpan_sfr_rfrag_get_seq(const sixlowpan_sfr_rfrag_t *hdr)
{
    return ((hdr->ar_seq_fs.u8[0] & SIXLOWPAN_SFR_SEQ_MASK) >>
            SIXLOWPAN_SFR_SEQ_POS);
}

/**
 * @brief   Set fragment size in a 6LoWPAN recoverable fragment header
 *
 * @pre     `frag_size <= SIXLOWPAN_SFR_FRAG_SIZE_MAX`
 *
 * @param[in,out] hdr   A 6LoWPAN recoverable fragment header
 * @param[in] frag_size The fragment size to set. Must be lesser or equal to
 *                      @ref SIXLOWPAN_SFR_FRAG_SIZE_MAX.
 */
static inline void sixlowpan_sfr_rfrag_set_frag_size(sixlowpan_sfr_rfrag_t *hdr,
                                                     uint16_t frag_size)
{
    assert(frag_size <= SIXLOWPAN_SFR_FRAG_SIZE_MAX);
    hdr->ar_seq_fs.u16 &= ~htons(SIXLOWPAN_SFR_FRAG_SIZE_MASK);
    hdr->ar_seq_fs.u16 |= htons(frag_size);
}

/**
 * @brief   Get fragment size from a 6LoWPAN recoverable fragment header
 *
 * @param[in] hdr   A 6LoWPAN recoverable fragment header
 *
 * @return  The fragment size of the 6LoWPAN recoverable fragment
 */
static inline uint16_t sixlowpan_sfr_rfrag_get_frag_size(const sixlowpan_sfr_rfrag_t *hdr)
{
    return (byteorder_ntohs(hdr->ar_seq_fs) & SIXLOWPAN_SFR_FRAG_SIZE_MASK);
}

/**
 * @brief   Set fragment offset in a 6LoWPAN recoverable fragment header
 *
 * @param[in,out] hdr   A 6LoWPAN recoverable fragment header
 * @param[in] offset    The fragment offset to set.
 */
static inline void sixlowpan_sfr_rfrag_set_offset(sixlowpan_sfr_rfrag_t *hdr,
                                                  uint16_t offset)
{
    hdr->offset = byteorder_htons(offset);
}

/**
 * @brief   Get fragment offset from a 6LoWPAN recoverable fragment header
 *
 * @param[in] hdr   A 6LoWPAN recoverable fragment header
 *
 * @return  The fragment offset of the 6LoWPAN recoverable fragment
 */
static inline uint16_t sixlowpan_sfr_rfrag_get_offset(const sixlowpan_sfr_rfrag_t *hdr)
{
    return byteorder_ntohs(hdr->offset);
}

/**
 * @brief   Initialize 6LoWPAN RFRAG acknowledgment dispatch
 *
 * @see @ref SIXLOWPAN_SFR_ACK_DISP
 *
 * @param[in,out] hdr   A 6LoWPAN selective fragment recovery header
 */
static inline void sixlowpan_sfr_ack_set_disp(sixlowpan_sfr_t *hdr)
{
    hdr->disp_ecn &= ~SIXLOWPAN_SFR_DISP_MASK;
    hdr->disp_ecn |= SIXLOWPAN_SFR_ACK_DISP;
}

/**
 * @brief   Check if given header is a 6LoWPAN RFRAG acknowledgment header
 *
 * @see @ref SIXLOWPAN_SFR_ACK_DISP
 *
 * @param[in] hdr       The 6LoWPAN selective fragment recovery header to check
 *
 * return   true, if @p hdr is a 6LoWPAN RFRAG acknowledgment header
 * return   false, if @p hdr is not a 6LoWPAN RFRAG acknowledgment header
 */
static inline bool sixlowpan_sfr_ack_is(const sixlowpan_sfr_t *hdr)
{
    return ((hdr->disp_ecn & SIXLOWPAN_SFR_DISP_MASK) == SIXLOWPAN_SFR_ACK_DISP);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_SIXLOWPAN_SFR_H */
