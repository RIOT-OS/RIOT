/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _CPU_CONF_H_
#define _CPU_CONF_H_

/**
 * @defgroup    cpu_mips32r2_commom MIPS32R2 Common
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       Common CPU definitions for mip32r2 compatable devices.
 *
 * @author      Neil Jones <neil.jones@imgtec.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Configuration of default stack sizes
 *
 * printf takes a pretty tortured route through the C lib
 * then via UHI syscall exception to end up at the UART
 * driver.
 *
 * When debugging timer code we get printfs on the idle threads
 * stack which can easily blow its limits.
 *
 * Note code must be compiled at -Os with these values, using -O0
 * you'll overflow these stacks.
 *
 * NO ISR stack is in use yet, interrupt use the current running stack
 * hence the big-ish default stack size.
 * @{
 */

#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF   (1024)
#endif

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (2048)
#endif

#ifndef THREAD_STACKSIZE_IDLE
#ifdef NDEBUG
#define THREAD_STACKSIZE_IDLE           (512)
#else
#define THREAD_STACKSIZE_IDLE           (512 + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif
#endif

#define ISR_STACKSIZE                   (0)
/** @} */


#ifdef __cplusplus
}
#endif

#endif
/** @} */
