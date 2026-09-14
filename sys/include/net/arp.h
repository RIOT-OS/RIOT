/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_arp    Address resolution protocol (ARP)
 * @ingroup     net_ipv4
 * @brief       ARP definitions
 * @{
 *
 * @file
 * @brief   ARP definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Hardware types
 * @anchor  net_arp_hwtype
 * @see     [IANA ARP parameters]
 *          (https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml#table-arp-parameters-2)
 * @{
 */
#define ARP_HWTYPE_ETHERNET (1U)        /**< Ethernet */
#define ARP_HWTYPE_SERIAL   (20U)       /**< Serial Line */
#define ARP_HWTYPE_EUI64    (27U)       /**< EUI-64 */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
