/*
 * Copyright (C) 2013  INRIA.
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ieee802154 IEEE802.15.4
 * @ingroup     net
 * @brief       IEEE802.15.4 adapaption layer
 * @{
 *
 * @file
 * @brief       IEEE 802.14.4 framing data structs and prototypes
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef IEEE802154_IEEE802154_H_
#define IEEE802154_IEEE802154_H_

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* maximum 802.15.4 header length */
#define IEEE_802154_MAX_HDR_LEN         (23)
/* ...and FCS*/
#define IEEE_802154_FCS_LEN             (2)

/**
 * @brief   IEEE802.15.4 FCF bitfield definitions
 * @{
 */
#define IEEE_802154_FCF_TYPE_MASK       (0x0700)
#define IEEE_802154_FCF_TYPE_POS        (8U)
#define IEEE_802154_FCF_TYPE_BEACON     (0x0000)
#define IEEE_802154_FCF_TYPE_DATA       (0x0100)
#define IEEE_802154_FCF_TYPE_ACK        (0x0200)
#define IEEE_802154_FCF_TYPE_MAC_CMD    (0x0300)

#define IEEE_802154_FCF_SEC_EN          (0x0800)
#define IEEE_802154_FCF_SEC_EN_POS      (11U)

#define IEEE_802154_FCF_FRAME_PEND      (0x1000)
#define IEEE_802154_FCF_FRAME_PEND_POS  (12U)

#define IEEE_802154_FCF_ACK_REQ         (0x2000)
#define IEEE_802154_FCF_ACK_REQ_POS     (13U)

#define IEEE_802154_FCF_PANID_COMP      (0x4000)
#define IEEE_802154_FCF_PANID_COMP_POS  (14U)

#define IEEE_802154_FCF_DST_ADDR_MASK   (0x000a)
#define IEEE_802154_FCF_DST_ADDR_POS    (2U)
#define IEEE_802154_FCF_DST_ADDR_EMPTY  (0x0000)
#define IEEE_802154_FCF_DST_ADDR_SHORT  (0x0008)
#define IEEE_802154_FCF_DST_ADDR_LONG   (0x000a)

#define IEEE_802154_FCF_FRAME_VER_MASK  (0x0030)
#define IEEE_802154_FCF_FRAME_VER_POS   (4U)

#define IEEE_802154_FCF_SRC_ADDR_MASK   (0x00a0)
#define IEEE_802154_FCF_SRC_ADDR_POS    (6U)
#define IEEE_802154_FCF_SRC_ADDR_EMPTY  (0x0000)
#define IEEE_802154_FCF_SRC_ADDR_SHORT  (0x0080)
#define IEEE_802154_FCF_SRC_ADDR_LONG   (0x00a0)
/** @} */

/**
 * @brief IEEE802.15.4 multi-cast addresses
 * @{
 */
#define IEEE_802154_SHORT_MCAST_ADDR    (0xffff)
#define IEEE_802154_LONG_MCAST_ADDR     {0xff, 0xff, 0xff, 0xff, \
                                          0xff, 0xff, 0xff, 0xff}
/** @} */

/**
 * @brief The default PAN id to use if not specified otherwise by upper layers
 */
#define IEEE_802154_DEFAULT_PAN_ID      (1)

/**
 * @brief   Transform 16-bit number from network order (big-endian) to
 *          little-endian byte order (as used by IEEE 802.15.4).
 *
 * @NOTE    TODO: remove and use macros from /core/include/byteorder.h
 */
#define NTOLES(a)   (((a) >> 8) | (((a) & 0x00ff) << 8))

/**
 * @brief   Transform 16-bit number from little-endian byte order to network
 *          order (big-endian).
 *
 * @NOTE    TODO: remove and use macros from /core/include/byteorder.h
 */
#define LETONS(a)   NTOLES(a)

/**
 * @brief   Transform 16-bit number from host byte order to little-endian byte
 *          order (as used by IEEE 802.15.4).
 *
 * @NOTE    TODO: remove and use macros from /core/include/byteorder.h
 */
#define HTOLES(a)   a

/**
 * @brief   Transform 16-bit number from little-endian byte order to host byte
 *          order.
 *
 * @NOTE    TODO: remove and use macros from /core/include/byteorder.h
 */
#define LETOHS(a)   HTOLES(a)

/**
 * @brief   IEEE802.15.4 frame descriptor
 */
typedef struct __attribute__((packed)) {
    uint16_t fcf;               /**< frame control field */
    uint8_t seq_nr;             /**< sequence number */
    uint16_t dest_pan_id;       /**< destination PAN id */
    uint8_t dest_addr[8];       /**< destination address */
    uint16_t src_pan_id;        /**< source PAN id */
    uint8_t src_addr[8];        /**< source address */
    uint8_t *payload;           /**< payload pointer */
    uint8_t payload_len;        /**< size of payload in byte */
} ieee802154_frame_t;

/**
 * Structure to represent an IEEE 802.15.4 packet for the transceiver.
 */
typedef struct __attribute__(( packed )) {
    /* @{ */
    uint8_t processing;         /** < internal processing state */
    uint8_t length;             /** < the length of the frame of the frame including fcs*/
    ieee802154_frame_t frame;   /** < the ieee802154 frame */
    int8_t rssi;                /** < the rssi value */
    uint8_t crc;                /** < 1 if crc was successfull, 0 otherwise */
    uint8_t lqi;                /** < the link quality indicator */
    /* @} */
} ieee802154_packet_t;

/**
 * @brief   Convert the given frame data-structure to raw binary data
 *
 * @note    The function asserts that the given buffer is large enough to hold
 *          the raw header
 *
 * @param[in]  frame            frame to convert
 * @param[out] buf              memory to save raw frame to
 *
 * @return                      the actual size of the raw header in byte
 */
uint8_t ieee802154_frame_init(ieee802154_frame_t *frame, uint8_t *buf);

/**
 * @brief   Populate the given frame data-structure by extracting its members
 *          from the given data buffer
 *
 * @note    The function asserts that the buffer is of sufficient size!
 *
 * @param[out] frame            frame struct to populate
 * @param[in]  buf              raw frame in binary form
 * @param[in]  len              the size of the ieee802154 packet including
 *                              payload and checksum
 *
 * @return                      the raw size of the header in byte
 */
uint8_t ieee802154_frame_read(uint8_t *buf, ieee802154_frame_t *frame, uint8_t len);

/**
 * @brief   Get the actual length of the IEEE802.15.4 frame in byte
 *
 * @param[in] frame             frame to analyze
 *
 * @return                      the header's raw size in byte
 */
uint8_t ieee802154_frame_get_hdr_len(ieee802154_frame_t *frame);

/**
 * @brief   Calculate the checksum for the given IEEE802.15.4 frame
 *
 * @param[in] frame             the raw frame (header + payload)
 * @param[in] frame_len         the size of the raw frame
 *
 * @return                      the checksum of the given frame
 */
uint16_t ieee802154_frame_get_fcs(const uint8_t *frame, uint8_t frame_len);

/**
 * @brief   Print the given IEEE802.15.4 frame header
 *
 * @param[in] frame             IEEE802.15.4 frame header to print
 */
void ieee802154_frame_print_fcf_frame(ieee802154_frame_t *frame);

#ifdef __cplusplus
}
#endif

#endif /* IEEE802154_IEEE802154_H_ */
/** @} */
