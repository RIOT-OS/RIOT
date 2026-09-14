/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_ethernet Ethernet
 * @ingroup     net
 * @brief       Provides Ethernet header and helper functions
 * @{
 *
 * @file
 * @brief       Definitions for Ethernet
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

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

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
