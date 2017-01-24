/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_lwip    lwIP
 * @ingroup     pkg
 * @brief       A lightweight TCP/IP stack
 * @see         http://savannah.nongnu.org/projects/lwip/
 *
 * lwIP is a lightweight TCP/IP stack primarily for usage with Ethernet.
 * It can be used with the the @ref conn.
 *
 * @{
 *
 * @file
 * @brief   lwIP bootstrap definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef LWIP_H
#define LWIP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initializes lwIP stack.
 *
 * This initializes lwIP, i.e. all netdevs are added to as interfaces to the
 * stack and the stack's thread is started.
 */
void lwip_bootstrap(void);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_H */
/** @} */
