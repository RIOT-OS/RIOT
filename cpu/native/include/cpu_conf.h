/*
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup cpu_native
 * @{
 */

/**
 * @file
 * @brief  Native CPU configuration
 * @author Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */
#ifndef CPU_CONF_H
#define CPU_CONF_H

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
#ifndef THREAD_STACKSIZE_DEFAULT
#if (__SIZEOF_POINTER__ == 8)
/**
 * @brief Default size of a thread stack on 64-bit platforms
 */
#define THREAD_STACKSIZE_DEFAULT            (16384)
#else
/**
 * @brief Default size of a thread stack
 */
#define THREAD_STACKSIZE_DEFAULT            (8192)
#endif
#endif
/**
 * @brief Default size of idle thread stack
 */
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE               (THREAD_STACKSIZE_DEFAULT)
#endif
/**
 * @brief Extra stack buffer capacity needed for `printf`
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF       (4096)
#endif
/**
 * @brief Extra stack buffer capacity needed for `printf` in floating-point operations
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT
#define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT (4096)
#endif
/* for core/include/thread.h */
/**
 * @brief Minimum thread size
 */
#ifndef THREAD_STACKSIZE_MINIMUM
#define THREAD_STACKSIZE_MINIMUM            (THREAD_STACKSIZE_DEFAULT)
#endif
/* native internal */
/**
 * @brief Size of stack used in ISR context
 */
#ifndef ISR_STACKSIZE
#define ISR_STACKSIZE                       (THREAD_STACKSIZE_DEFAULT)
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
#ifndef FLASHPAGE_SIZE
# define FLASHPAGE_SIZE                      (512)
#endif

/**
 * @brief Total number of emulated flash pages
 */
#ifndef FLASHPAGE_NUMOF
# define FLASHPAGE_NUMOF                     (32)
#endif

/**
 * @brief Flashpage alignment
 *
 * The address passed to @ref flashpage_write must be a multiple of this constant.
 */
#ifndef FLASHPAGE_WRITE_BLOCK_ALIGNMENT
# define FLASHPAGE_WRITE_BLOCK_ALIGNMENT     (8)
#endif
/**
 * @brief Flashpage block size
 *
 * When writing to flash, the data size must be a multiple of this constant.
 *
 * @see @ref flashpage_write
 */
#ifndef FLASHPAGE_WRITE_BLOCK_SIZE
# define FLASHPAGE_WRITE_BLOCK_SIZE          (16)
#endif

/**
 * @brief Value of bytes in erased flash
 *
 * The flash is set to this constant when you call @ref flashpage_erase.
 */
#ifndef FLASHPAGE_ERASE_STATE
# define FLASHPAGE_ERASE_STATE               (0x0)
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

#endif /* CPU_CONF_H */
/** @} */
