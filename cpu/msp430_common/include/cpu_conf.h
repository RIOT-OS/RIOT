/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_common
 * @{
 *
 * @file
 * @brief       Common CPU definitions fpr MSP430
 *
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Configure the internal flash memory
 * @{
 */
#define FLASHPAGE_SIZE      (512)

#if defined (CPU_MODEL_MSP430F1611)
#define CPU_FLASH_BASE      (0x4000)
#define FLASHPAGE_NUMOF     (96)        /* 48K */
#elif defined (CPU_MODEL_MSP430F1612)
#define CPU_FLASH_BASE      (0x2600)
#define FLASHPAGE_NUMOF     (110)       /* 56K */
#elif defined (CPU_MODEL_MSP430F2617)
#define CPU_FLASH_BASE      (0x3100)
#define FLASHPAGE_NUMOF     (128)       /* we can currently only access 52K */
#elif defined (CPU_MODEL_CC430F6137)
#define CPU_FLASH_BASE      (0x8000)
#define FLASHPAGE_NUMOF     (64)        /* 32K */
#endif
/** @} */

/**
 * @name    Kernel configuration
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

#endif /* CPU_CONF_H */
/** @} */
