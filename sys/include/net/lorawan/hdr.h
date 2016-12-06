/*
 * Copyright (C) 2016 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lw_hdr    LoRaWAN header
 * @ingroup     net_lw
 * @brief       LoRaWAN header types and helper functions
 * @{
 *
 * @file
 * @brief   LoRaWAN header type and helper function definitions
 *
 * @author  José Ignacio Alamos <jose.alamos@inria.cl>
 */

#ifndef LW_HDR_H_
#define LW_HDR_H_

#include <stdint.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Data type to represent a LoRaWAN packet header
 *
 * @details This definition includes MHDR and FHDR in the same structure. The structure of the header is as follows:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.unparsed}
 *                      1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * |Mtype| RFU |Maj|           LoRaWAN Address                   ...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * ...             | Frame Control |         Frame Counter         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @see <a href="https://www.lora-alliance.org/portals/0/specs/LoRaWAN%20Specification%201R0.pdf">
 *          LoRaWAN spcefication, section 4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief message type and major version
     *
     * @details the message type are the 3 most significant bytes and the
     * major version are the 2 less significant bytes. There are 3 bytes
     * in the middle reserved for future usage.
     * This module provides helper functions to set and get:
     * * lw_hdr_set_mtype()
     * * lw_hdr_get_mtype()
     * * lw_hdr_set_maj()
     * * lw_hdr_get_maj()
     */
    uint8_t mt_maj;
    le_uint32_t addr; /**< 32 bit LoRaWAN address */
    uint8_t fctrl; /**< frame control */
    le_uint16_t fcnt; /**< frame counter */
} lw_hdr_t;

#include <stdio.h>
static inline void lw_hdr_set_mtype(lw_hdr_t *hdr, uint8_t mtype)
{
    hdr->mt_maj &= 0x1f;
    hdr->mt_maj |= mtype << 5;
}

static inline uint8_t lw_hdr_get_mtype(lw_hdr_t *hdr)
{
    return ((hdr->mt_maj & 0xe0) >> 5);
}

static inline void lw_hdr_set_maj(lw_hdr_t *hdr, uint8_t maj)
{
    hdr->mt_maj &= 0xfc;
    hdr->mt_maj |= maj & 0x03;
}

static inline uint8_t lw_hdr_get_maj(lw_hdr_t *hdr)
{
    return (hdr->mt_maj & 0x03);
}
#endif
