/**
 * Native CPU configuration
 *
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup cpu_native
 * @{
 * @file
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @}
 */
#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CPU specific default stack sizes
 *
 * TODO: tighten stack sizes
 *
 * @{
 */
#ifdef __MACH__ /* OSX */
#define THREAD_STACKSIZE_DEFAULT            (163840)
#define THREAD_STACKSIZE_IDLE               (163840)
#define THREAD_EXTRA_STACKSIZE_PRINTF       (81920)
#define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT (81920)
/* for core/include/thread.h */
#define THREAD_STACKSIZE_MINIMUM            (163840)
/* native internal */
#define THREAD_STACKSIZE_MINIMUM            (163840)
#define ISR_STACKSIZE                       (163840)

#else /* Linux etc. */
#define THREAD_STACKSIZE_DEFAULT            (8192)
#define THREAD_STACKSIZE_IDLE               (8192)
#define THREAD_EXTRA_STACKSIZE_PRINTF       (4096)
#define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT (4096)
/* for core/include/thread.h */
#define THREAD_STACKSIZE_MINIMUM            (8192)
/* native internal */
#define ISR_STACKSIZE                       (8192)
#endif /* OS */
/** @} */

/**
 * @brief   Native internal Ethernet protocol number
 */
#define NATIVE_ETH_PROTO 0x1234

#if (defined(CONFIG_GNRC_PKTBUF_SIZE)) && (CONFIG_GNRC_PKTBUF_SIZE < 2048)
#   undef  CONFIG_GNRC_PKTBUF_SIZE
#   define CONFIG_GNRC_PKTBUF_SIZE     (2048)
#endif

/**
 * @brief   Native Flash emulation
 *          Use unusual parameters to trigger edge cases
 * @{
 */
#ifndef FLASHPAGE_SIZE
#define FLASHPAGE_SIZE                      (512)
#endif
#ifndef FLASHPAGE_NUMOF
#define FLASHPAGE_NUMOF                     (32)
#endif
#ifndef FLASHPAGE_WRITE_BLOCK_ALIGNMENT
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT     (8)
#endif
#ifndef FLASHPAGE_WRITE_BLOCK_SIZE
#define FLASHPAGE_WRITE_BLOCK_SIZE          (16)
#endif
#ifndef FLASHPAGE_ERASE_STATE
#define FLASHPAGE_ERASE_STATE               (0x0)
#endif

extern char _native_flash[FLASHPAGE_SIZE * FLASHPAGE_NUMOF];

#define CPU_FLASH_BASE ((uintptr_t)_native_flash)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
