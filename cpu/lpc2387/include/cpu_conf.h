/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup     cpu_lpc2387
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
 * @name Pthread configuration
 * @{
 */
/* The idle stack of '160' is not enough to do the 'msg_receive'.
 * It currently used '164' bytes. */
#define CONFIG_PTHREAD_REAPER_BASE_STACKSIZE (2*THREAD_STACKSIZE_IDLE)
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

/**
 * @brief   Attribute for memory sections required by SRAM PUF
 */
#define PUF_SRAM_ATTRIBUTES __attribute__((used, section(".noinit")))

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_CONF_H */
