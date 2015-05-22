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
 * @ingroup     conf
 * @ingroup     lpc2387
 *
 * @{
 */

/**
 * @file
 * @brief       LPC2387 CPUconfiguration
 *
 * @author      baar
 * @version     $Revision$
 *
 * @note        $Id$
 */

/**
 * @name Stdlib configuration
 * @{
 */
#define __FOPEN_MAX__       4
#define __FILENAME_MAX__    12
/** @} */

/**
 * @name Kernel configuration
 * @{
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT  (4096)
#define THREAD_EXTRA_STACKSIZE_PRINTF        (2048)

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (512)
#endif

#define THREAD_STACKSIZE_IDLE      (160)
/** @} */

/**
 * @name Compiler specifics
 * @{
 */
#define CC_CONF_INLINE                  inline
#define CC_CONF_USED                    __attribute__((used))
#define CC_CONF_NONNULL(...)            __attribute__((nonnull(__VA_ARGS__)))
#define CC_CONF_WARN_UNUSED_RESULT      __attribute__((warn_unused_result))
/** @} */

#define TRANSCEIVER_BUFFER_SIZE (10)
#define RX_BUF_SIZE  (10)

#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (128)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPUCONF_H_ */
