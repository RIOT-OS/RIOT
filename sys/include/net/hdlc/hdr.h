/*
 * Copyright (C) 2015 José Ignacio Alamos <jialaos@uc.cl>
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     net_hdlc   HDLC header
 * @brief       HDLC header architecture
 *
 * @{
 *
 * @file
 * @brief       Definitions for High Level Data Link Control header
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @author      Max van Kessel
 */
#ifndef NET_HDLC_HDR_H
#define NET_HDLC_HDR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HDLC frame type mask */
#define HDLC_FRAME_TYPE_MASK  (0x03)

/* HDLC type shift */
#define HDLC_TYPE_SHIFT       (2)

/* HDLC type mask */
#define HDLC_TYPE_MASK        (0x03)

/* HDLC (recv) sequence number shift */
#define HDLC_SEQNO_SHIFT      (5)

/* HDLC sequence number mask */
#define HDLC_SEQNO_MASK       (0x03)

/**
 * @brief HDLC types
 */
typedef enum {
    HDLC_TYPE_RECEIVE_READY     = 0x0, /**< ready receive type */
    HDLC_TYPE_REJECT            = 0x1, /**< reject */
    HDLC_TYPE_RECEIVE_NOT_READY = 0x2, /**< receive not ready */
    HDLC_TYPE_SELECTIVE_REJECT  = 0x3, /**< selective reject */
} hdlc_type_t;

/**
 * @brief HDLC Frame types
 */
typedef enum {
    HDLC_FRAME_TYPE_INFORMATION = 0, /**< represents i(nformation) frame */
    HDLC_FRAME_TYPE_SUPERVISORY = 1, /**< represents s(upervisory) frame*/
    HDLC_FRAME_TYPE_UNNUMBERED  = 3, /**< represents u(numbered) frame */
} hdlc_frame_type_t;

/**
 * @brief Data type to represent an HDLC header.
 *
 *  +----------+----------+----------+
 *  |   Flag   | Address  | Control  |
 *  | 01111110 | 11111111 | 00000011 |
 *  +----------+----------+----------+
 *  +--------------------------------+
 *  |              Data              |
 *  |              8 x n             |
 *  +--------------------------------+
 *  +----------+----------+-----------------
 *  |   FCS    |   Flag   | Inter-frame Fill
 *  |16/32 bits| 01111110 | or next Address
 *  +----------+----------+-----------------
 *
 */
typedef struct __attribute__((packed)) {
    uint8_t address; /**< Address field oh HDLC header */
    uint8_t control; /**< Control field of HDLC header */
} hdlc_hdr_t;

/**
 * @brief Get type of message
 * @note Only valid for supervisory and unnumbered frames
 *
 * @param[in] hdr Header to get message type from
 *
 * @return message type
 */
static inline hdlc_type_t hdlc_get_type(hdlc_hdr_t *hdr)
{
    return (hdlc_type_t)((hdr->control >> HDLC_TYPE_SHIFT) & HDLC_TYPE_MASK);
}

/**
 * @brief Get type of frame
 *
 * @param[in] hdr Header to get type from
 *
 * @return type of frame
 */
static inline hdlc_frame_type_t hdlc_get_frame_type(hdlc_hdr_t *hdr)
{
    if (hdr->control & 0x01) {
        return (hdlc_frame_type_t)hdr->control & HDLC_FRAME_TYPE_MASK;
    }
    return HDLC_FRAME_TYPE_INFORMATION;
}

/**
 * @brief Get received sequence number
 *
 * @param[in] hdr Header to get seqno from
 *
 * @return sequence number
 */
static inline uint8_t hdlc_get_recv_seqno(hdlc_hdr_t *hdr)
{
    return (uint8_t)((hdr->control >> HDLC_SEQNO_SHIFT) & HDLC_SEQNO_MASK);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_HDLC_HDR_H */
/** @} */
