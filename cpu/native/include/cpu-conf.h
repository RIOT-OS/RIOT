/**
 * Native CPU configuration
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */
#ifndef CPUCONF_H_
#define CPUCONF_H_

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
#define CPUID_ID_LEN                    (4)

#endif /* CPUCONF_H_ */
