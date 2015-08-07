/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ethernet Ethernet
 * @ingroup     net
 * @brief       Ethernet implementation
 * @{
 *
 * @file
 * @brief       Definitions for Ethernet
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef ETHERNET_H_
#define ETHERNET_H_

#include <stdint.h>

#include "net/ethernet/hdr.h"
#include "net/eui64.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETHERNET_DATA_LEN       (1500)  /**< maximum number of bytes in payload */
#define ETHERNET_FCS_LEN        (4)     /**< number of bytes in the FCS
                                         *  (frame check sequence) */

/**
 * @brief maximum number of bytes in an ethernet frame (without FCS)
 */
#define ETHERNET_FRAME_LEN      (ETHERNET_DATA_LEN + sizeof(ethernet_hdr_t))
#define ETHERNET_MIN_LEN        (64)    /**< minimum number of bytes in an
                                         * ethernet frame (with FCF) */

/**
 * @brief maximum number of bytes in an ethernet frame (with FCF)
 */
#define ETHERNET_MAX_LEN        (ETHERNET_FRAME_LEN + ETHERNET_FCS_LEN)

/**
 * @brief   Generates an IPv6 interface identifier from a 48-bit MAC address.
 *
 * @see <a href="https://tools.ietf.org/html/rfc2464#section-4">
 *          RFC 2464, section 4
 *      </a>
 *
 * @param[out] eui64    The resulting EUI-64.
 * @param[in] mac       A 48-bit MAC address. Is expected to be at least
 *                      @ref ETHERNET_ADDR_LEN long.
 */
static inline void ethernet_get_iid(eui64_t *eui64, uint8_t *mac)
{
    eui64->uint8[0] = mac[0] ^ 0x02;
    eui64->uint8[1] = mac[1];
    eui64->uint8[2] = mac[2];
    eui64->uint8[3] = 0xff;
    eui64->uint8[4] = 0xfe;
    eui64->uint8[5] = mac[3];
    eui64->uint8[6] = mac[4];
    eui64->uint8[7] = mac[5];
}

#ifdef __cplusplus
}
#endif

#endif /* ETHERNET_H_ */
/**
 * @}
 */
