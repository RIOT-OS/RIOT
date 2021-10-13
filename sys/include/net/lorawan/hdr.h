/*
 * Copyright (C) 2016 Fundación Inria Chile
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lorawan_hdr    LoRaWAN header
 * @ingroup     net_lorawan
 * @brief       LoRaWAN header types and helper functions
 * @{
 *
 * @file
 * @brief   LoRaWAN header type and helper function definitions
 *
 * @author  Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */

#ifndef NET_LORAWAN_HDR_H
#define NET_LORAWAN_HDR_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "byteorder.h"
#include "net/loramac.h"

#define LORAWAN_HDR_MTYPE_MASK (0xe0)           /**< MType mask */
#define LORAWAN_HDR_MTYPE_POS  (5U)             /**< MType position */
#define LORAWAN_HDR_MAJOR_MASK (0x03)           /**< Major version mask */
#define LORAWAN_HDR_MAJOR_POS  (0U)             /**< Major version position */
#define LORAWAN_HDR_ADR_MASK   (0x80)           /**< ADR mask */
#define LORAWAN_HDR_ADR_POS    (7U)             /**< ADR position */
#define LORAWAN_HDR_ADR_ACK_MASK (0x40)         /**< ADR ACK bit mask */
#define LORAWAN_HDR_ADR_ACK_POS  (6U)           /**< ADR ACK bit position */
#define LORAWAN_HDR_ACK_MASK     (0x20)         /**< ACK bit mask */
#define LORAWAN_HDR_ACK_POS      (5U)           /**< ACK bit position */
#define LORAWAN_HDR_FRAME_PENDING_MASK (0x10)   /**< Frame pending bit mask */
#define LORAWAN_HDR_FRAME_PENDING_POS  (4U)     /**< Frame pending bit position */
#define LORAWAN_HDR_FOPTS_LEN_MASK      (0x0F)  /**< Frame options mask */
#define LORAWAN_HDR_FOPTS_LEN_POS       (0U)    /**< Frame options position */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Data type to represent a LoRaWAN packet header
 *
 *          This definition includes MHDR and FHDR in the same structure.
 *          The structure of the header is as follows:
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
 * @see <a href="https://lora-alliance.org/sites/default/files/2018-07/lorawan1.0.3.pdf">
 *          LoRaWAN spcefication, section 4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief message type and major version
     *
     * The message type are the 3 most significant bytes and the
     * major version are the 2 less significant bytes. There are 3 bytes
     * in the middle reserved for future usage.
     * This module provides helper functions to set and get:
     * * lorawan_hdr_set_mtype()
     * * lorawan_hdr_get_mtype()
     * * lorawan_hdr_set_maj()
     * * lorawan_hdr_get_maj()
     */
    uint8_t mt_maj;
    le_uint32_t addr;                           /**< 32 bit LoRaWAN address */
    uint8_t fctrl;                              /**< frame control */
    le_uint16_t fcnt;                           /**< frame counter */
} lorawan_hdr_t;

/**
 * @brief Join request packet representation
 */
typedef struct __attribute__((packed)) {
    uint8_t mt_maj;                             /**< mtype and major version holder */
    le_uint64_t app_eui;                        /**< application EUI */
    le_uint64_t dev_eui;                        /**< device EUI */
    le_uint16_t dev_nonce;                      /**< device nonce */
    le_uint32_t mic;                            /**< message integrity code */
} lorawan_join_request_t;

/**
 * @brief Join accept packet representation
 */
typedef struct __attribute__((packed)) {
    uint8_t mt_maj;                             /**< mtype and major version holder */
    uint8_t app_nonce[LORAMAC_APP_NONCE_LEN];   /**< application nonce */
    uint8_t net_id[LORAMAC_NETWORK_ID_LEN];     /**< network id */
    uint8_t dev_addr[LORAMAC_DEVADDR_LEN];      /**< device address */
    uint8_t dl_settings;                        /**< downlink settings */
    uint8_t rx_delay;                           /**< first reception window delay */
} lorawan_join_accept_t;

/**
 * @brief Set LoRaWAN header MType
 *
 * @param[in] hdr LoRaWAN hdr
 * @param[in] mtype desired MType
 */
static inline void lorawan_hdr_set_mtype(lorawan_hdr_t *hdr, uint8_t mtype)
{
    hdr->mt_maj &= ~LORAWAN_HDR_MTYPE_MASK;
    hdr->mt_maj |= (mtype << LORAWAN_HDR_MTYPE_POS) & LORAWAN_HDR_MTYPE_MASK;
}

/**
 * @brief Get LoRaWAN header MType
 *
 * @param[in] hdr LoRaWAN hdr
 *
 * @return mtype of the packet
 */
static inline uint8_t lorawan_hdr_get_mtype(lorawan_hdr_t *hdr)
{
    return ((hdr->mt_maj & LORAWAN_HDR_MTYPE_MASK) >> LORAWAN_HDR_MTYPE_POS);
}

/**
 * @brief Set LoRaWAN major version
 *
 * @param[in] hdr LoRaWAN header
 * @param[in] maj major version
 */
static inline void lorawan_hdr_set_maj(lorawan_hdr_t *hdr, uint8_t maj)
{
    hdr->mt_maj &= ~LORAWAN_HDR_MAJOR_MASK;
    hdr->mt_maj |= (maj << LORAWAN_HDR_MAJOR_POS) & LORAWAN_HDR_MAJOR_MASK;
}

/**
 * @brief Get LoRaWAN major version
 *
 * @param[in] hdr LoRaWAN header
 *
 * @return major version of the packet
 */
static inline uint8_t lorawan_hdr_get_maj(lorawan_hdr_t *hdr)
{
    return (hdr->mt_maj & LORAWAN_HDR_MAJOR_MASK) >> LORAWAN_HDR_MAJOR_POS;
}

/**
 * @brief Set LoRaWAN header Adaptive Data Rate bit
 *
 * @param[in] hdr LoRaWAN header
 * @param[in] adr value of the ADR bit
 */
static inline void lorawan_hdr_set_adr(lorawan_hdr_t *hdr, bool adr)
{
    hdr->fctrl &= ~LORAWAN_HDR_ADR_MASK;
    hdr->fctrl |= (adr << LORAWAN_HDR_ADR_POS) & LORAWAN_HDR_ADR_MASK;
}

/**
 * @brief Get LoRaWAN header Adaptive Data Rate bit
 *
 * @param[in] hdr LoRaWAN header
 *
 * @return the value of the ADR bit
 */
static inline bool lorawan_hdr_get_adr(lorawan_hdr_t *hdr)
{
    return (hdr->fctrl & LORAWAN_HDR_ADR_MASK) >> LORAWAN_HDR_ADR_POS;
}

/**
 * @brief Set LoRaWAN header ADR ACK request bit
 *
 * @param[in] hdr LoRaWAN header
 * @param[in] adr_ack_req value of the ADR ACK request bit
 */
static inline void lorawan_hdr_set_adr_ack_req(lorawan_hdr_t *hdr, bool adr_ack_req)
{
    hdr->fctrl &= ~LORAWAN_HDR_ADR_ACK_MASK;
    hdr->fctrl |= (adr_ack_req << LORAWAN_HDR_ADR_ACK_POS) & LORAWAN_HDR_ADR_ACK_MASK;
}

/**
 * @brief Get LoRaWAN header ADR ACK request bit
 *
 * @param[in] hdr LoRaWAN header
 *
 * @return the value of ADR ACK request bit
 */
static inline bool lorawan_hdr_get_adr_ack_req(lorawan_hdr_t *hdr)
{
    return (hdr->fctrl & LORAWAN_HDR_ADR_ACK_MASK) >> LORAWAN_HDR_ADR_ACK_POS;
}

/**
 * @brief Set LoRaWAN header ACK bit
 *
 * @param[in] hdr LoRaWAN header
 * @param[in] ack value of the ACK bit
 */
static inline void lorawan_hdr_set_ack(lorawan_hdr_t *hdr, bool ack)
{
    hdr->fctrl &= ~LORAWAN_HDR_ACK_MASK;
    hdr->fctrl |= (ack << LORAWAN_HDR_ACK_POS) & LORAWAN_HDR_ACK_MASK;
}

/**
 * @brief Get LoRaWAN header ACK bit
 *
 * @param[in] hdr LoRaWAN header
 *
 * @return value of the ACK bit
 */
static inline bool lorawan_hdr_get_ack(lorawan_hdr_t *hdr)
{
    return (hdr->fctrl & LORAWAN_HDR_ACK_MASK) >> LORAWAN_HDR_ACK_POS;
}

/**
 * @brief Set LoRaWAN header frame pending bit
 *
 * @param[in] hdr LoRaWAN header
 * @param[in] frame_pending value of the frame pending bit
 */
static inline void lorawan_hdr_set_frame_pending(lorawan_hdr_t *hdr, bool frame_pending)
{
    hdr->fctrl &= ~LORAWAN_HDR_FRAME_PENDING_MASK;
    hdr->fctrl |= (frame_pending << LORAWAN_HDR_FRAME_PENDING_POS) & LORAWAN_HDR_FRAME_PENDING_MASK;
}

/**
 * @brief Get LoRaWAN header frame pending bit
 *
 * @param[in] hdr LoRaWAN header
 *
 * @return value of the frame pending bit
 */
static inline bool lorawan_hdr_get_frame_pending(lorawan_hdr_t *hdr)
{
    return (hdr->fctrl & LORAWAN_HDR_FRAME_PENDING_MASK) >> LORAWAN_HDR_FRAME_PENDING_POS;
}

/**
 * @brief Set LoRaWAN header FOpts length
 *
 * @param[in] hdr LoRaWAN header
 * @param[in] len length of the piggybacked FOpts
 */
static inline void lorawan_hdr_set_frame_opts_len(lorawan_hdr_t *hdr, uint8_t len)
{
    hdr->fctrl &= ~LORAWAN_HDR_FOPTS_LEN_MASK;
    hdr->fctrl |= (len << LORAWAN_HDR_FOPTS_LEN_POS) & LORAWAN_HDR_FOPTS_LEN_MASK;
}

/**
 * @brief Get LoRaWAN header FOps length
 *
 * @param[in] hdr LoRaWAN header
 *
 * @return length of piggybacked FOpts
 */
static inline uint8_t lorawan_hdr_get_frame_opts_len(lorawan_hdr_t *hdr)
{
    return (hdr->fctrl & LORAWAN_HDR_FOPTS_LEN_MASK) >> LORAWAN_HDR_FOPTS_LEN_POS;
}

#ifdef __cplusplus
}
#endif

#endif /* NET_LORAWAN_HDR_H */
/** @} */
