/*
 * SPDX-FileCopyrightText: 2017 Ken Rabold
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_riscv_common
 * @{
 *
 * @file
 * @brief           Implementation of the kernels irq interface
 *
 * @author          Ken Rabold
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <stdint.h>

#include "irq.h"
#include "vendor/riscv_csr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Bit mask for the MCAUSE register
 */
#define CPU_CSR_MCAUSE_CAUSE_MSK        (0x0fffu)

extern volatile int riscv_in_isr;

/**
 * @brief Enable all maskable interrupts
 */
static inline __attribute__((always_inline)) unsigned int irq_enable(void)
{
    /* Enable all interrupts */
    unsigned state;

    __asm__ volatile (
        "csrrs %[dest], mstatus, %[mask]"
        :[dest]    "=r" (state)
        :[mask]    "i" (MSTATUS_MIE)
        : "memory"
        );
    return state;
}

/**
 * @brief Disable all maskable interrupts
 */
static inline __attribute__((always_inline)) unsigned int irq_disable(void)
{

    unsigned int state;

    __asm__ volatile (
        "csrrc %[dest], mstatus, %[mask]"
        :[dest]    "=r" (state)
        :[mask]    "i" (MSTATUS_MIE)
        : "memory"
        );

    return state;
}

/**
 * @brief Restore the state of the IRQ flags
 */
static inline __attribute__((always_inline)) void irq_restore(
    unsigned int state)
{
    /* Restore all interrupts to given state */
    __asm__ volatile (
        "csrw mstatus, %[state]"
        : /* no outputs */
        :[state]   "r" (state)
        : "memory"
        );
}

/**
 * @brief See if the current context is inside an ISR
 */
static inline __attribute__((always_inline)) bool irq_is_in(void)
{
    return riscv_in_isr;
}

static inline __attribute__((always_inline)) bool irq_is_enabled(void)
{
    unsigned state;

    __asm__ volatile (
        "csrr %[dest], mstatus"
        :[dest]    "=r" (state)
        : /* no inputs */
        : "memory"
        );
    return (state & MSTATUS_MIE);
}

#ifdef __cplusplus
}
#endif

/** @} */
