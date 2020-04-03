/*
 * Copyright(C) 2017, 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @defgroup     cpu_mips_pic32mz PIC32MZ
 * @ingroup      cpu
 * @{
 *
 * @file
 * @brief       CPU definitions for Microchip PIC32MZ devices.
 *
 * @author      Neil Jones <neil.jones@imgtec.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef CPU_MODEL_P32MZ2048EFG100
#include "vendor/p32mz2048efg100.h"
#else
#error "No CPU headers for the defined CPU_MODEL found"
#endif

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
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
