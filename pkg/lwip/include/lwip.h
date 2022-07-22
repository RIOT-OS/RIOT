/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lwip
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

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief event queue for netdev events
 */
extern event_queue_t lwip_event_queue;

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
