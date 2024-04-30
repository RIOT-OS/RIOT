/*
 * Copyright (C) 2023 HAW Hamburg. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @ingroup     cpu_arm7tdmi_gba
 *
 * @{
 */

/**
 * @file
 * @brief       ARM7TDMI_GBA CPU configuration
 *
 * @author
 */

/**
 * @brief   This arch uses the inlined irq API.
 */
#define IRQ_API_INLINED (1)

/**
 * @name Kernel configuration
 * @{
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF (512)

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT (1024)
#endif

#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE (160)
#endif
/** @} */

/**
 * @brief   Stack size used for the undefined instruction interrupt stack
 */
#define UND_STACKSIZE (64)

/**
 * @brief   Stack size used for the abort interrupt stack
 */
#define ABT_STACKSIZE (64)

/**
 * @brief   Stack size used for the interrupt (ISR) stack
 */
#ifndef ISR_STACKSIZE
#define ISR_STACKSIZE (400)
#endif

/**
 * @brief   Stack size used for the fast interrupt (FIQ) stack
 */
#define FIQ_STACKSIZE (64)

/**
 * @brief   Stack size used for the supervisor mode (SVC) stack
 */
#define SVC_STACKSIZE (400)

/**
 * @brief   Stack size used for the user mode/kernel init stack
 */
#define USR_STACKSIZE (4096)

/**
 * @brief   The CPU has 2 blocks of SRAM at different addresses.
 *          (primary on-chip RAM and external working EWRAM)
 */
#define NUM_HEAPS (2)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
