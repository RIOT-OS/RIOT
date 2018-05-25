/*
 * Copyright (C) 2015 José Ignacio Alamos <jialaos@uc.cl>
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_hdlc_hdr    HDLC header
 * @ingroup     net_hdlc
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

/**
 * @brief HDLC types
 */
typedef enum {
    HDLC_TYPE_RECEIVE_READY     = 0,        /**< ready receive type */
    HDLC_TYPE_RECEIVE_NOT_READY = (1 << 1), /**< receive not ready */
    HDLC_TYPE_REJECT            = (1 << 0), /**< reject */
    HDLC_TYPE_SELECTIVE_REJECT  = (3 << 0), /**< selective reject */
} hdlc_type_t;

/**
 * @brief HDLC Frame types
 */
typedef enum {
    HDLC_FRAME_TYPE_INFORMATION = 0, /**< represents i(nformation) frame */
    HDLC_FRAME_TYPE_SUPERVISORY = 1, /**< represents s(upervisory) frame*/
    HDLC_FRAME_TYPE_UNNUMBERED = 3,  /**< represents u(numbered) frame */
} hdlc_frame_type_t;

/**
 * @brief HDLC Supervisory Frames
 */
typedef struct __attribute__((packed)) {
    uint8_t id : 2;
    hdlc_type_t type : 2;
    uint8_t poll_final : 1;
    uint8_t sequence_no : 3;
} hdlc_control_s_frame_t;

/**
 * @brief HDLC Information Frames
 */
typedef struct __attribute__((packed)) {
    uint8_t id : 1;
    uint8_t send_sequence_no : 3;
    uint8_t poll_final : 1;
    uint8_t sequence_no : 3;
} hdlc_control_i_frame_t;

/**
 * @brief HDLC Unnumbered Frames
 */
typedef struct __attribute__((packed)) {
    uint8_t id : 2;
    hdlc_type_t type : 2;
    uint8_t poll_final : 1;
    hdlc_type_t type_x : 3;
} hdlc_control_u_frame_t;

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
    uint8_t address;                /**< Address field oh HDLC header */
    union{
        hdlc_control_s_frame_t s;
        hdlc_control_i_frame_t i;
        hdlc_control_u_frame_t u;
        uint8_t frame;              /**< Control field of HDLC header */
    }control;
} hdlc_hdr_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_HDLC_HDR_H */
/** @} */
