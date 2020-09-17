/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup pkg_libschc_gnrc   libschc_gnrc: Glue-code for libSCHC and GNRC
 * @ingroup pkg_libschc
 * @ingroup net_gnrc_schc
 * @{
 *
 * @file
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef LIBSCHC_GNRC_H
#define LIBSCHC_GNRC_H

#include <stdint.h>

#include "net/gnrc/netif.h"
#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialization of the libSCHC thread
 *
 * If SCHC was already initialized, it will just return the PID of the SCHC thread.
 *
 * @return  The PID of the libSCHC thread, on success.
 * @return  -EINVAL, if @ref GNRC_SCHC_PRIO was greater than or equal to @ref SCHED_PRIO_LEVELS.
 * @return  -EOVERFLOW, if there are too many threads running already in general
 */
kernel_pid_t libschc_gnrc_init(void);

/**
 * @brief   Initialize a GNRC network interface for libSCHC.
 *
 * @param[in] netif The network interface to initialize for libSCHC.
 */
void libschc_gnrc_netif_init(gnrc_netif_t *netif);

#ifdef __cplusplus
}
#endif

#endif /* LIBSCHC_GNRC_H */
/** @} */
