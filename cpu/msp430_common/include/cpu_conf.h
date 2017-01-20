/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CPUCONF_H
#define CPUCONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Kernel configuration
 * @{
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#   define THREAD_EXTRA_STACKSIZE_PRINTF    (256)
#endif

#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#   define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT (THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifndef THREAD_STACKSIZE_DEFAULT
#   define THREAD_STACKSIZE_DEFAULT         (256)
#endif

#ifndef THREAD_STACKSIZE_IDLE
#   define THREAD_STACKSIZE_IDLE            (96)
#endif

#ifndef ISR_STACKSIZE
#   define ISR_STACKSIZE                    (256)
#endif

#ifndef GNRC_PKTBUF_SIZE
/* TODO: Make this value overall MTU dependent */
#   define GNRC_PKTBUF_SIZE                 (2560)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPUCONF_H */
