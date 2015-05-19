/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_ieee802154 IEEE802.15.4
 * @ingroup     net
 * @brief       IEEE802.15.4 header definitions and utility functions
 * @{
 *
 * @file
 * @brief       IEEE 802.14.4 header definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NG_IEEE802154_H_
#define NG_IEEE802154_H_

#include <stdint.h>
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IEEE802.15.4 FCF field definitions
 * @{
 */
#define NG_IEEE802154_MAX_HDR_LEN           (23U)
#define NG_IEEE802154_FCF_LEN               (2U)
#define NG_IEEE802154_FCS_LEN               (2U)

#define NG_IEEE802154_FCF_TYPE_MASK         (0x07)
#define NG_IEEE802154_FCF_TYPE_BEACON       (0x00)
#define NG_IEEE802154_FCF_TYPE_DATA         (0x01)
#define NG_IEEE802154_FCF_TYPE_ACK          (0x02)
#define NG_IEEE802154_FCF_TYPE_MACCMD       (0x03)

#define NG_IEEE802154_FCF_SECURITY_EN       (0x08)
#define NG_IEEE802154_FCF_FRAME_PEND        (0x10)
#define NG_IEEE802154_FCF_ACK_REQ           (0x20)
#define NG_IEEE802154_FCF_PAN_COMP          (0x40)

#define NG_IEEE802154_FCF_DST_ADDR_MASK     (0x0c)
#define NG_IEEE802154_FCF_DST_ADDR_VOID     (0x00)
#define NG_IEEE802154_FCF_DST_ADDR_SHORT    (0x08)
#define NG_IEEE802154_FCF_DST_ADDR_LONG     (0x0c)

#define NG_IEEE802154_FCF_VERS_V0           (0x00)
#define NG_IEEE802154_FCF_VERS_V1           (0x10)

#define NG_IEEE802154_FCF_SRC_ADDR_MASK     (0xc0)
#define NG_IEEE802154_FCF_SRC_ADDR_VOID     (0x00)
#define NG_IEEE802154_FCF_SRC_ADDR_SHORT    (0x80)
#define NG_IEEE802154_FCF_SRC_ADDR_LONG     (0xc0)
/** @} */

/**
 * @brief Data type to represent an EUI-64.
 */
typedef union {
    le_uint64_t uint64;     /**< represented as 64 bit value */
    uint8_t uint8[8];       /**< split into 8 8-bit words. */
    le_uint16_t uint16[4];  /**< split into 4 16-bit words. */
} eui64_t ;

#ifdef __cplusplus
}
#endif

#endif /* NG_IEEE802154_H_ */
/** @} */
