/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * Basic definitions and forwards for x86 boards.
 *
 * @defgroup x86 i586
 * @ingroup cpu
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU_X86_CPU_H_
#define CPU_X86_CPU_H_

#include "attributes.h"
#include "irq.h"
#include "ucontext.h"
#include "cpu_conf.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief x86 has architecture specific atomic_cas in x86_atomic.c
 * @{
 */
#define ARCH_HAS_ATOMIC_COMPARE_AND_SWAP 1
/** @} */

/**
 * @brief Disable interrupts
 */
static inline void __attribute__((always_inline)) dINT(void)
{
    asm volatile ("cli");
}

/**
 * @brief Enable interrupts
 */
static inline void __attribute__((always_inline)) eINT(void)
{
    asm volatile ("sti");
}

/**
 * @brief   Disable interrupts and halt forever.
 *
 * This function is the last resort in case of an unrecoverable error.
 * No message will be printed, no nothing.
 */
static inline void __attribute__((always_inline, noreturn)) x86_hlt(void)
{
    while (1) {
        asm volatile ("cli");
        asm volatile ("hlt");
    }
}

/**
 * @brief   Initialize subsystems and run kernel.
 *
 * Called by the board specific startup code.
 * @li The .bss has to be cleared before.
 * @li The stack has to be set up, probably somewhere in the low memory.
 * @li The A20 line has to be activated, because all the code is beyond 1MB.
 * @li Paging must be disabled.
 * @li The SS, DS, and CS must span the whole 4GB of RAM.
 * @li 32 bit protected mode must be entered.
 * @li Interrupts must be disabled.
 */
void x86_startup(void) NORETURN;

/**
 * @brief   Setup board specific hardware and such.
 *
 * Called by x86_startup() after all generic subsystems are activated,
 * and before kernel_init() gets called.
 * Interrupts are disabled before, and are expected to be disabled after calling this function.
 * Interrupts may be enabled during the course of this function call.
 *
 * Probably most of the board specific initialization should be done in auto_init():
 * @li You must not spawn thread_create() in this function.
 * @li The hwtimer is not set up properly at this point of executation.
 */
void x86_init_board(void);

/**
 * @brief   Returns a range of usable physical memory.
 * @param[out]   start   Start address of the physical memory region.
 * @param[out]   len     Total length of the physical memory region.
 *                       One page are 0x1000 bytes.
 * @param        cnt     Reentrant pointer. Must be zero on first call.
 *                       The value is entirely opaque.
 * @returns   true iff there was a memory region set in start and len.
 *            false iff there are no more memory regions.
 *            The function must not get called again in the latter case.
 *
 * Called by x86_startup() before x86_init_board() gets called.
 *
 * The memory management is one of the earliest subsystems to be set up.
 * The uart is already configured and usable.
 * The interrupt handling is already set up and you may call x86_interrupt_handler_set().
 *
 * The function may return unaligned memory.
 * In this case the begin of the region gets rounded up the the next page,
 * and the end gets rounded down.
 * The function may supply low memory regions, which will be ignored.
 * The regions are expected to contain memory that lies inside the elf sections.
 */
bool x86_get_memory_region(uint64_t *start, uint64_t *len, unsigned long *cnt);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
