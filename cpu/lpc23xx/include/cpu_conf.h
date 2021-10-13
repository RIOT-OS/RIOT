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
 * @ingroup     cpu_lpc23xx
 *
 * @{
 */

/**
 * @file
 * @brief       LPC23XX CPU configuration
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
￼ * @brief   This arch uses the inlined irq API.
￼ */
#define IRQ_API_INLINED     (1)

/**
 * @name Kernel configuration
 * @{
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF        (512)

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (1024)
#endif

#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE      (160)
#endif
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

/**
 * @brief   Stack size used for the undefined instruction interrupt stack
 * @{
 */
#define UND_STACKSIZE                   (4)
/** @} */

/**
 * @brief   Stack size used for the abort interrupt stack
 * @{
 */
#define ABT_STACKSIZE                   (4)
/** @} */

/**
 * @brief   Stack size used for the interrupt (ISR) stack
 * @{
 */
#ifndef ISR_STACKSIZE
#define ISR_STACKSIZE                   (400)
#endif
/** @} */

/**
 * @brief   Stack size used for the fast interrupt (FIQ) stack
 * @{
 */
#define FIQ_STACKSIZE                   (64)
/** @} */

/**
 * @brief   Stack size used for the supervisor mode (SVC) stack
 * @{
 */
#define SVC_STACKSIZE                   (400)
/** @} */

/**
 * @brief   Stack size used for the user mode/kernel init stack
 * @{
 */
#define USR_STACKSIZE                   (4096)
/** @} */

/**
 * @brief   The CPU has 4 blocks of SRAM at different addresses.
 *          (primary RAM, USB RAM, Ethernet RAM & Backup RAM)
 */
#define NUM_HEAPS (4)

/**
 * @brief   Memory marked with this attribute is retained during deep sleep
 *          and initialized with 0 on cold boot.
 */
#define BACKUP_RAM      __attribute__((section(".backup.bss")))

/**
 * @brief   Memory marked with this attribute is retained during deep sleep
 *          and initialized with user provided data on cold boot.
 */
#define BACKUP_RAM_DATA __attribute__((section(".backup.data")))

/**
 * @brief   lpc23xx has non-uniform pages
 */
#define PERIPH_FLASHPAGE_CUSTOM_PAGESIZES

/**
 * @brief   FLASH base address
 */
#define CPU_FLASH_BASE          (0x0U)

/**
 * @brief   Flash page configuration
 * @{
 */
#define PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_ADDR
#define PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_PAGE

#define FLASHPAGE_NUMOF                 (27)
#define FLASHPAGE_WRITE_BLOCK_SIZE      (256)
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT (256)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_CONF_H */
