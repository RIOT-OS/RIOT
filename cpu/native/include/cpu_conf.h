/*
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup cpu_native
 * @{
 */

/**
 * @file
 * @brief  Native CPU configuration
 * @author Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - CPU-specific default stack sizes */
/**
 * @brief CPU-specific default stack sizes
 *
 * @todo TODO: tighten stack sizes
 *
 * @{
 */
#if !defined(THREAD_STACKSIZE_DEFAULT) || defined(DOXYGEN)
/**
 * @brief Default size of a thread stack
 */
#  if (__SIZEOF_POINTER__ == 8)
#    define THREAD_STACKSIZE_DEFAULT            (16384)
#  else
#    define THREAD_STACKSIZE_DEFAULT            (8192)
#  endif
#endif
/**
 * @brief Default size of idle thread stack
 */
#if !defined(THREAD_STACKSIZE_IDLE) || defined(DOXYGEN)
#  define THREAD_STACKSIZE_IDLE               (THREAD_STACKSIZE_DEFAULT)
#endif
/**
 * @brief Extra stack buffer capacity needed for `printf`
 */
#if !defined(THREAD_EXTRA_STACKSIZE_PRINTF) || defined(DOXYGEN)
#  define THREAD_EXTRA_STACKSIZE_PRINTF       (4096)
#endif
/**
 * @brief Extra stack buffer capacity needed for `printf` in floating-point operations
 */
#if !defined(THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT) || defined(DOXYGEN)
#  define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT (4096)
#endif
/* for core/include/thread.h */
/**
 * @brief Minimum thread size
 */
#if !defined(THREAD_STACKSIZE_MINIMUM) || defined(DOXYGEN)
#  define THREAD_STACKSIZE_MINIMUM            (THREAD_STACKSIZE_DEFAULT)
#endif
/* native internal */
/**
 * @brief Size of stack used in ISR context
 */
#if !defined(ISR_STACKSIZE) || defined(DOXYGEN)
#  define ISR_STACKSIZE                       (THREAD_STACKSIZE_DEFAULT)
#endif
/** @} */

/* MARK: - Networking constants */
/**
 * @name Networking constants
 * @{
 */
/**
 * @brief   Native internal Ethernet protocol number
 */
#define NATIVE_ETH_PROTO 0x1234

#if (defined(CONFIG_GNRC_PKTBUF_SIZE)) && (CONFIG_GNRC_PKTBUF_SIZE < 2048)
#   undef  CONFIG_GNRC_PKTBUF_SIZE
#   define CONFIG_GNRC_PKTBUF_SIZE     (2048)
#endif
/** @} */

/* MARK: - Native flash emulation */
/**
 * @name Native flash emulation
 * Use unusual parameters to trigger edge cases
 * @{
 */
/**
 * @brief Size of a single emulated flash page
 */
#if !defined(FLASHPAGE_SIZE) || defined(DOXYGEN)
#  define FLASHPAGE_SIZE                      (512)
#endif

/**
 * @brief Total number of emulated flash pages
 */
#if !defined(FLASHPAGE_NUMOF) || defined(DOXYGEN)
#  define FLASHPAGE_NUMOF                     (32)
#endif

/**
 * @brief Flashpage alignment
 *
 * The address passed to @ref flashpage_write must be a multiple of this constant.
 */
#if !defined(FLASHPAGE_WRITE_BLOCK_ALIGNMENT) || defined(DOXYGEN)
#  define FLASHPAGE_WRITE_BLOCK_ALIGNMENT     (8)
#endif
/**
 * @brief Flashpage block size
 *
 * When writing to flash, the data size must be a multiple of this constant.
 *
 * @see @ref flashpage_write
 */
#if !defined(FLASHPAGE_WRITE_BLOCK_SIZE) || defined(DOXYGEN)
#  define FLASHPAGE_WRITE_BLOCK_SIZE          (16)
#endif

/**
 * @brief Value of bytes in erased flash
 *
 * The flash is set to this constant when you call @ref flashpage_erase.
 */
#if !defined(FLASHPAGE_ERASE_STATE) || defined(DOXYGEN)
#  define FLASHPAGE_ERASE_STATE               (0x0)
#endif

/**
 * @brief Emulated flash buffer
 */
extern char _native_flash[FLASHPAGE_SIZE * FLASHPAGE_NUMOF];

/**
 * @brief Base buffer pointer to emulated flash
 */
#define CPU_FLASH_BASE ((uintptr_t)_native_flash)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
