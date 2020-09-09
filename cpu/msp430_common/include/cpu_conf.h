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
 * @brief       Common CPU definitions for MSP430
 *
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   This arch uses the inlined IRQ API.
 */
#define IRQ_API_INLINED     (1)

/**
 * @name   Configure the internal flash memory
 * @{
 */
#define FLASHPAGE_SIZE      (512U)

#if defined (CPU_MODEL_MSP430F1611)
#define CPU_FLASH_BASE      (0x4000)
#define FLASHPAGE_NUMOF     (96U)        /* 48K */
#elif defined (CPU_MODEL_MSP430F1612)
#define CPU_FLASH_BASE      (0x2600)    /* first sector is only 256 byte, skip it*/
#define FLASHPAGE_NUMOF     (109U)      /* 54.5K */
#elif defined (CPU_MODEL_MSP430F2617)
#define CPU_FLASH_BASE      (0x3200)    /* first sector is only 256 byte, skip it*/
#define FLASHPAGE_NUMOF     (103U)      /* we can currently only access 51.5K */
#elif defined (CPU_MODEL_CC430F6137)
#define CPU_FLASH_BASE      (0x8000)
#define FLASHPAGE_NUMOF     (64U)        /* 32K */
#endif

/* The minimum block size which can be written is 1B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_RAW_BLOCKSIZE    (1U)
/* Writing should be always 2 byte aligned */
#define FLASHPAGE_RAW_ALIGNMENT    (2U)
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
#   define THREAD_STACKSIZE_DEFAULT         (384)
#endif

#ifndef THREAD_STACKSIZE_IDLE
#   define THREAD_STACKSIZE_IDLE            (96)
#endif

#ifndef ISR_STACKSIZE
#   define ISR_STACKSIZE                    (256)
#endif

#ifndef CONFIG_GNRC_PKTBUF_SIZE
/* TODO: Make this value overall MTU dependent */
#   define CONFIG_GNRC_PKTBUF_SIZE                 (2560)
#endif

#ifndef GNRC_IPV6_STACK_SIZE
#   define GNRC_IPV6_STACK_SIZE             (512)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
