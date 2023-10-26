/*
 * Copyright (C) 2020 Bart Moons <bamoons.moons@ugent.be>
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_schc   gnrc_schc: SCHC
 * @ingroup     net_gnrc
 * @brief       GNRC's SCHC implementation
 * @see         [RFC 8724](https://tools.ietf.org/html/rfc8724)
 * @see         [RFC 8824](https://tools.ietf.org/html/rfc8924)
 * @see         [RFC 9011](https://tools.ietf.org/html/rfc9011)
 * @experimental
 * @{
 *
 * @file
 * @brief       SCHC definitions for GNRC
 *
 * @author      Bart Moons <bamoons.moons@ugent.be>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_SCHC_H
#define NET_GNRC_SCHC_H

#include <stdbool.h>

#include "kernel_defines.h"
#include "log.h"
#include "net/gnrc/netif.h"
#include "sched.h"

#if IS_USED(MODULE_LIBSCHC_GNRC)
#include "libschc/gnrc.h"
#endif

#include "net/gnrc/schc/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialization of the SCHC thread
 *
 * If SCHC was already initialized, it will just return the PID of the SCHC thread.
 *
 * @return  The PID of the SCHC thread, on success.
 * @return  -EINVAL, if @ref GNRC_SCHC_PRIO was greater than or equal to @ref SCHED_PRIO_LEVELS.
 * @return  -EOVERFLOW, if there are too many threads running already in general
 */
kernel_pid_t gnrc_schc_init(void);

/**
 * @brief   Initialize a network interface for SCHC.
 *
 * @param[in] netif The network interface to initialize for SCHC.
 */
static inline void gnrc_schc_netif_init(gnrc_netif_t *netif)
{
#if IS_USED(MODULE_LIBSCHC_GNRC)
    libschc_gnrc_netif_init(netif);
#else
    (void)netif;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SCHC_H */
/** @} */
