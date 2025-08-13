/*
 * Copyright (C) 2016 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_ppphdr Point-to-Point Protocol Header
 * @ingroup     net_ppp
 * @brief       PPP header abstraction type and helper functions
 * @{
 *
 * @file
 * @brief   General definitions for PPP header and their helper functions
 *
 * @author  José Ignacio Alamos
 */

#include <inttypes.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Header of a PPP packet
 * @details A PPP packet is transmitted as a payload of an HDLC packet. PPP packets only carry information about control protocol
 * of a PPP stack (Link Control Protocol, IP Control Protocol, etc). IP packets encapsulated in HDLC frame are not
 * considered PPP packet.
 *
 * The format of PPP header plus payload is:
 *
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Code      |  Identifier   |            Length             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    Payload ...
 * +-+-+-+-+
 *
 *
 * @see <a href="https://tools.ietf.org/html/rfc1661#section-5">
 *          RFC 1661, section 5
 *      </a>
 */
/*  PPP pkt header struct */
typedef struct __attribute__((packed)){
    uint8_t code;               /**< Code of PPP packet*/
    uint8_t id;                 /**< Identifier PPP of packet*/
    network_uint16_t length;    /**< Length of PPP packet including payload*/
} ppp_hdr_t;

#ifdef __cplusplus
}
#endif

/** @} */
