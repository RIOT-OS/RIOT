/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ethertype Ether types
 * @ingroup     net
 * @brief       Ether types
 * @see         <a href="http://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml#ieee-802-numbers-1">
 *                  IANA, ETHER TYPES
 *              </a>
 * @note        Last Updated: 2015-02-04
 * @{
 *
 * @file
 * @brief       Ether type definitions
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef ETHERTYPE_H_
#define ETHERTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* expand at will ;-) */
#define ETHERTYPE_RESERVED      (0x0000)    /**< Reserved */
#define ETHERTYPE_IPV4          (0x0800)    /**< Internet protocol version 4 */
#define ETHERTYPE_ARP           (0x0806)    /**< Address resolution protocol */
#define ETHERTYPE_IPV6          (0x86dd)    /**< Internet protocol version 6 */
#define ETHERTYPE_UNKNOWN       (0xffff)    /**< Reserved (no protocol specified) */

#ifdef __cplusplus
}
#endif

#endif /* ETHERTYPE_H_ */
/**
 * @}
 */
