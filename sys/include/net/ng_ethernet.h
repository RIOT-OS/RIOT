/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_ethernet Ethernet
 * @ingroup     net
 * @brief       Ethernet implementation
 * @{
 *
 * @file
 * @brief       Definitions for Ethernet
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef NG_ETHERNET_H_
#define NG_ETHERNET_H_

#include "net/ng_ethernet/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NG_ETHERNET_DATA_LEN    (1500)  /**< maximum number of bytes in payload */
#define NG_ETHERNET_FCS_LEN     (4)     /**< number of bytes in the FCS
                                         *  (frame check sequence) */

/**
 * @brief maximum number of bytes in an ethernet frame (without FCS)
 */
#define NG_ETHERNET_FRAME_LEN   (NG_ETHERNET_DATA_LEN + \
                                 sizeof(ng_ethernet_hdr_t))
#define NG_ETHERNET_MIN_LEN     (64)    /**< minimum number of bytes in an
                                         * ethernet frame (with FCF) */

/**
 * @brief maximum number of bytes in an ethernet frame (with FCF)
 */
#define NG_ETHERNET_MAX_LEN     (NG_ETHERNET_FRAME_LEN + \
                                 NG_ETHERNET_FCS_LEN)

#ifdef __cplusplus
}
#endif

#endif /* NG_ETHERNET_H_ */
/**
 * @}
 */
