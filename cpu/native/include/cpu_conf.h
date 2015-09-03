/**
 * Native CPU configuration
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */
#ifndef CPUCONF_H_
#define CPUCONF_H_

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
#define THREAD_EXTRA_STACKSIZE_PRINTF       (163840)
#define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT (163840)
/* for core/include/thread.h */
#define THREAD_STACKSIZE_MINIMUM            (163840)
/* native internal */
#define THREAD_STACKSIZE_MINIMUM            (163840)
#define NATIVE_ISR_STACKSIZE                (163840)

#else /* Linux etc. */
#define THREAD_STACKSIZE_DEFAULT            (8192)
#define THREAD_STACKSIZE_IDLE               (8192)
#define THREAD_EXTRA_STACKSIZE_PRINTF       (8192)
#define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT (8192)
/* for core/include/thread.h */
#define THREAD_STACKSIZE_MINIMUM            (8192)
/* native internal */
#define NATIVE_ISR_STACKSIZE                (8192)
#endif /* OS */
/** @} */

/**
 * @brief   Native internal Ethernet protocol number
 */
#define NATIVE_ETH_PROTO 0x1234

/**
 * @brief   Length of CPU ID for @ref cpu_id_get() in @ref periph/cpuid.h
 */
#ifndef CPUID_ID_LEN
#define CPUID_ID_LEN                    (4)
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPUCONF_H_ */
