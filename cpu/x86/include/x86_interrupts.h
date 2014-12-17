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
 * The entry points for all interrupts on x86 boards.
 *
 * @ingroup     x86
 * @defgroup    x86-irq   Interrupt handling for i586 boards
 * @{
 * @file
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__INTERRUPTS__H__
#define CPU__X86__INTERRUPTS__H__

#include "ucontext.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize interrupts.
 *
 * This function is called during initialization by x86_startup().
 * You must not call this function on your own accord.
 */
void x86_init_interrupts(void);

#define X86_MAX_INTERRUPTS (0x30)

/**
 * @brief   The exceptions in an x86 system.
 *
 * http://www.sandpile.org/x86/except.htm
 */
enum x86_interrupt_numbers {
    X86_INT_DE = 0x00, /**< divide error */
    X86_INT_DB = 0x01, /**< debug (thrown after each instruction if X86_TF is activated) */
    X86_INT_BP = 0x03, /**< breakpoint (`int3`) */
    X86_INT_OF = 0x04, /**< overflow (`into`) */
    X86_INT_BR = 0x05, /**< boundary range exceeded (`bound`) */
    X86_INT_UD = 0x06, /**< undefined opcode */
    X86_INT_NM = 0x07, /**< device not available (raised on FPU accessing if CR0.TS is set) */
    X86_INT_DF = 0x08, /**< double fault (exceptions during exception handler invocation) */
    X86_INT_GP = 0x0d, /**< general protection */
    X86_INT_PF = 0x0e, /**< page fault */
    X86_INT_MF = 0x10, /**< math fault */
    X86_INT_AC = 0x11, /**< alignment checking */
    X86_INT_MC = 0x12, /**< machine check */
};

/**
 * @brief   Bits in the EFLAGS register.
 *
 * http://www.sandpile.org/x86/flags.htm
 */
enum x86_eflags {
    X86_CF     = 1 << 0,  /**< carry */
    X86_PF     = 1 << 2,  /**< parity */
    X86_AF     = 1 << 4,  /**< adjust */
    X86_ZF     = 1 << 6,  /**< zero */
    X86_SF     = 1 << 7,  /**< signed */
    X86_TF     = 1 << 8,  /**< singled step (throw #DB after each instruction) */
    X86_IF     = 1 << 9,  /**< interrupts enabled */
    X86_DF     = 1 << 10, /**< direction (0 = movs increses addresses, 1 = movs decreases addresses) */
    X86_OF     = 1 << 11, /**< overflow */
    X86_IOPL_0 = 0 << 12, /**< priority level 0 (?) */
    X86_IOPL_1 = 1 << 12, /**< priority level 1 (?) */
    X86_IOPL_2 = 2 << 12, /**< priority level 2 (?) */
    X86_IOPL_3 = 3 << 12, /**< priority level 3 (?) */
    X86_NT     = 1 << 14, /**< nested task */
    X86_RF     = 1 << 16, /**< resume */
    X86_VM     = 1 << 17, /**< virtual 8086 mode */
    X86_AC     = 1 << 18, /**< alignment check */
    X86_VIF    = 1 << 19, /**< virtual interrupts enabled */
    X86_VIP    = 1 << 20, /**< virtual interrupts pendigng  */
    X86_ID     = 1 << 21, /**< able to use CPUID */
};

/**
 * @brief   Datum for the Interrupt Descriptor Table Register.
 */
struct idtr_t {
    uint16_t limit; /**< `sizeof (struct idt_desc) * count` */
    void *base; /**< physical address of the start of the IDT */
} __attribute__((packed));

/**
 * @brief   One entry in the IDT
 */
struct idt_desc {
    uint16_t offset_1; /**< offset bits 0..15 */
    uint16_t selector; /**< a code segment selector in GDT or LDT */
    uint8_t zero;      /**< unused, set to 0 */
    uint8_t type_attr; /**< type and attributes, see below */
    uint16_t offset_2; /**< offset bits 16..31 */
} __attribute__((packed));

/**
 * @brief   Callback on interrupt.
 * @param   intr_num     Number of interrupt -- not the number of the IRQ
 * @param   orig_ctx     (Changable) register set of the calling thread
 * @param   error_code   Related error code (if applies, otherwise 0)
 */
typedef void (*x86_intr_handler_t)(uint8_t intr_num, struct x86_pushad *orig_ctx, unsigned long error_code);

/**
 * @brief   Set callback function for interrupt.
 * @param   num       Number of interrupt. Must be less than X86_MAX_INTERRUPTS.
 * @param   handler   Function to call, or `NULL` to use default interrupt handler.
 *
 * This function is meant to be called by other subsystems (x86_pic.c and x86_threading.c).
 * Any upper level system should use x86_pic_set_handler() or even higher level functions.
 */
void x86_interrupt_handler_set(unsigned num, x86_intr_handler_t handler);

/**
 * @brief   Disable interrupts and return previous EFLAGS.
 */
static inline unsigned long __attribute__((always_inline)) x86_pushf_cli(void)
{
    unsigned long result;
    asm volatile("pushf; cli; pop %0" : "=g"(result));
    return result;
}

/**
 * @brief   Set EFLAGS.
 */
static inline void __attribute__((always_inline)) x86_restore_flags(unsigned long stored_value)
{
    asm volatile("push %0; popf" :: "g"(stored_value));
}

/**
 * @brief   Print saved general purposed registers.
 */
void x86_print_registers(struct x86_pushad *orig_ctx, unsigned long error_code);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
