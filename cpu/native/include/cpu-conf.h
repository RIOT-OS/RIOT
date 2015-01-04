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

/* TODO: tighten stack sizes */
#ifdef __MACH__ /* OSX */
#define KERNEL_CONF_STACKSIZE_DEFAULT       (163840)
#define KERNEL_CONF_STACKSIZE_IDLE          (163840)
#define KERNEL_CONF_STACKSIZE_PRINTF        (163840)
#define KERNEL_CONF_STACKSIZE_PRINTF_FLOAT  (163840)
/* for core/include/thread.h */
#define MINIMUM_STACK_SIZE              (163840)
/* undefine the TRANSCEIVER_STACK_SIZE (2048 or 512) defined in transceiver.h */
#ifdef TRANSCEIVER_STACK_SIZE
#undef TRANSCEIVER_STACK_SIZE
#endif
#define TRANSCEIVER_STACK_SIZE          (163840)
/* native internal */
#define MINIMUM_STACK_SIZE              (163840)
#define NATIVE_ISR_STACKSIZE            (163840)

#else /* Linux etc. */
#define KERNEL_CONF_STACKSIZE_DEFAULT       (8192)
#define KERNEL_CONF_STACKSIZE_IDLE          (8192)
#define KERNEL_CONF_STACKSIZE_PRINTF        (8192)
#define KERNEL_CONF_STACKSIZE_PRINTF_FLOAT  (8192)
/* for core/include/thread.h */
#define MINIMUM_STACK_SIZE              (8192)
/* undefine the TRANSCEIVER_STACK_SIZE (2048 or 512) defined in transceiver.h */
#ifdef TRANSCEIVER_STACK_SIZE
#undef TRANSCEIVER_STACK_SIZE
#endif
#define TRANSCEIVER_STACK_SIZE          (16384)
/* native internal */
#define NATIVE_ISR_STACKSIZE            (8192)
#endif /* OS */

#ifdef UART0_BUFSIZE
#undef UART0_BUFSIZE
#endif
#define UART0_BUFSIZE                   (128)

/* for nativenet */
#define NATIVE_ETH_PROTO 0x1234

/**
 * @brief   length of CPU ID for @ref cpu_id_get() in @ref periph/cpuid.h
 */
#ifndef CPUID_ID_LEN
#define CPUID_ID_LEN                    (4)
#endif

/**
 * @name CPU Flash configuration (file simulation)
 * @{
 */
#define FLASH_PAGE_SIZE         (1024)  /**< Page size of flash memory */
#define FLASH_NUM_PAGES         (256)   /**< Number of flash pages */
#define FLASH_START_ADDRESS     (0x0)   /**< Starting address to calculate end of flash */
#define FLASH_WRITE_ALIGN       (4)     /**< number of bytes must be written at once */
#define FLASH_WRITES_PER_WORD   (2)     /**< how often a word can overwritten without flash erase */
#define FLASH_ERASE_CYCLES      (20000) /**< Flash erase cycles described in datasheet */
#define FLASH_ERASED_BIT_VALUE  (0)     /**< Value of erased bits 0|1 */
/* @} */

#ifdef __cplusplus
}
#endif

#endif /* CPUCONF_H_ */
