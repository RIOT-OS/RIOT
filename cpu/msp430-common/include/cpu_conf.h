/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CPUCONF_H_
#define CPUCONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Kernel configuration
 * @{
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF    (256)
#define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT (THREAD_EXTRA_STACKSIZE_PRINTF)

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (256)
#endif

#define THREAD_STACKSIZE_IDLE      (96)
#define MSP430_ISR_STACK_SIZE           (256)

#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (32)
#endif

#ifndef NG_PKTBUF_SIZE
#define NG_PKTBUF_SIZE                  (2560)    /* TODO: Make this value
                                                   * overall MTU dependent */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPUCONF_H_ */
