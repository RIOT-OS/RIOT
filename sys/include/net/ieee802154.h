/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ieee802154 IEEE802.15.4
 * @ingroup     net
 * @brief       IEEE802.15.4 header definitions and utility functions
 * @{
 *
 * @file
 * @brief       IEEE 802.14.4 header definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef IEEE802154_H_
#define IEEE802154_H_

#include <stdlib.h>

#include "net/eui64.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IEEE802.15.4 FCF field definitions
 * @{
 */
#define IEEE802154_MAX_HDR_LEN              (23U)
#define IEEE802154_FCF_LEN                  (2U)
#define IEEE802154_FCS_LEN                  (2U)

#define IEEE802154_FCF_TYPE_MASK            (0x07)
#define IEEE802154_FCF_TYPE_BEACON          (0x00)
#define IEEE802154_FCF_TYPE_DATA            (0x01)
#define IEEE802154_FCF_TYPE_ACK             (0x02)
#define IEEE802154_FCF_TYPE_MACCMD          (0x03)

#define IEEE802154_FCF_SECURITY_EN          (0x08)
#define IEEE802154_FCF_FRAME_PEND           (0x10)
#define IEEE802154_FCF_ACK_REQ              (0x20)
#define IEEE802154_FCF_PAN_COMP             (0x40)

#define IEEE802154_FCF_DST_ADDR_MASK        (0x0c)
#define IEEE802154_FCF_DST_ADDR_VOID        (0x00)
#define IEEE802154_FCF_DST_ADDR_SHORT       (0x08)
#define IEEE802154_FCF_DST_ADDR_LONG        (0x0c)

#define IEEE802154_FCF_VERS_V0              (0x00)
#define IEEE802154_FCF_VERS_V1              (0x10)

#define IEEE802154_FCF_SRC_ADDR_MASK        (0xc0)
#define IEEE802154_FCF_SRC_ADDR_VOID        (0x00)
#define IEEE802154_FCF_SRC_ADDR_SHORT       (0x80)
#define IEEE802154_FCF_SRC_ADDR_LONG        (0xc0)
/** @} */

/**
 * @brief   Generates an IPv6 interface identifier from an IEEE 802.15.4 address.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-6">
 *          RFC 4944, section 6
 *      </a>
 * @see <a href="https://tools.ietf.org/html/rfc6282#section-3.2.2">
 *          RFC 6282, section 3.2.2
 *      </a>
 *
 * @param[out] eui64    The resulting EUI-64.
 * @param[in] addr      An IEEE 802.15.4 address.
 * @param[in] addr_len  The length of @p addr. Must be 2 (short address),
 *                      4 (PAN ID + short address), or 8 (long address).
 *
 * @return Copy of @p eui64 on success.
 * @return NULL, if @p addr_len was of illegal length.
 */
static inline eui64_t *ieee802154_get_iid(eui64_t *eui64, uint8_t *addr,
                                             size_t addr_len)
{
    int i = 0;

    eui64->uint8[0] = eui64->uint8[1] = 0;

    switch (addr_len) {
        case 8:
            eui64->uint8[0] = addr[i++] ^ 0x02;
            eui64->uint8[1] = addr[i++];
            eui64->uint8[2] = addr[i++];
            eui64->uint8[3] = addr[i++];
            eui64->uint8[4] = addr[i++];
            eui64->uint8[5] = addr[i++];
            eui64->uint8[6] = addr[i++];
            eui64->uint8[7] = addr[i++];
            break;

        case 4:
            eui64->uint8[0] = addr[i++] ^ 0x02;
            eui64->uint8[1] = addr[i++];

        case 2:
            eui64->uint8[2] = 0;
            eui64->uint8[3] = 0xff;
            eui64->uint8[4] = 0xfe;
            eui64->uint8[5] = 0;
            eui64->uint8[6] = addr[i++];
            eui64->uint8[7] = addr[i++];
            break;

        default:
            return NULL;
    }

    return eui64;
}


#ifdef __cplusplus
}
#endif

#endif /* IEEE802154_H_ */
/** @} */
