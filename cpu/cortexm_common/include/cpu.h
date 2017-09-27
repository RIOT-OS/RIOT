/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_cortexm_common ARM Cortex-M common
 * @ingroup     cpu
 * @brief       Common implementations and headers for Cortex-M family based
 *              micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the Cortex-M common module
 *
 * When ever you want to do something hardware related, that is accessing MCUs
 * registers, just include this file. It will then make sure that the MCU
 * specific headers are included.
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @todo        remove include irq.h once core was adjusted
 */

#ifndef CPU_H
#define CPU_H

#include <stdio.h>

#include "irq.h"
#include "sched.h"
#include "thread.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Interrupt stack canary value
 *
 * @note 0xe7fe is the ARM Thumb machine code equivalent of asm("bl #-2\n") or
 * 'while (1);', i.e. an infinite loop.
 * @internal
 */
#define STACK_CANARY_WORD   (0xE7FEE7FEu)

/**
 * @brief   Initialization of the CPU
 */
void cpu_init(void);

/**
 * @brief   Initialize Cortex-M specific core parts of the CPU
 */
void cortexm_init(void);

/**
 * @brief   Prints the current content of the link register (lr)
 */
static inline void cpu_print_last_instruction(void)
{
    uint32_t *lr_ptr;
    __asm__ __volatile__("mov %0, lr" : "=r"(lr_ptr));
    printf("%p\n", (void*) lr_ptr);
}

/**
 * @brief   Put the CPU into the 'wait for event' sleep mode
 *
 * This function is meant to be used for short periods of time, where it is not
 * feasible to switch to the idle thread and back.
 */
static inline void cortexm_sleep_until_event(void)
{
    __WFE();
}

/**
 * @brief   Put the CPU into (deep) sleep mode, using the `WFI` instruction
 *
 * @param[in] deep      !=0 for deep sleep, 0 for light sleep
 */
static inline void cortexm_sleep(int deep)
{
    if (deep) {
        SCB->SCR |=  (SCB_SCR_SLEEPDEEP_Msk);
    }
    else {
        SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
    }

    /* ensure that all memory accesses have completed and trigger sleeping */
    __disable_irq();
    __DSB();
    __WFI();
    __enable_irq();
}

/**
 * @brief   Trigger a conditional context scheduler run / context switch
 *
 * This function is supposed to be called in the end of each ISR.
 */
static inline void cortexm_isr_end(void)
{
    if (sched_context_switch_request) {
        thread_yield();
    }
}

/**
 * @brief   Jumps to another image in flash
 *
 * This function is supposed to be called by a bootloader application.
 *
 * @param[in]   image_address   address in flash of other image
 */
static inline void cpu_jump_to_image(uint32_t image_address)
{
    /* Disable IRQ */
    __disable_irq();

    /* set PSP */
    __set_PSP(*(uint32_t*)image_address);

    /* skip stack pointer */
    image_address += 4;

    /* load the images reset_vector address */
    uint32_t destination_address = *(uint32_t*)image_address;

    /* Make sure the Thumb State bit is set. */
    destination_address |= 0x1;

    /* Branch execution */
    __asm("BX %0" :: "r" (destination_address));
}

/* The following register is only present for Cortex-M0+, -M3, -M4 and -M7 CPUs */
#if defined(CPU_ARCH_CORTEX_M0PLUS) || defined(CPU_ARCH_CORTEX_M3) || \
    defined(CPU_ARCH_CORTEX_M4) || defined(CPU_ARCH_CORTEX_M4F) || \
    defined(CPU_ARCH_CORTEX_M7)
static inline unsigned cpu_get_image_baseaddr(void)
{
    return (unsigned)SCB->VTOR;
}
#endif

#if defined(SLOT0_SIZE) && defined(SLOT1_SIZE)
/**
 * @brief Get FW internal address for a given slot
 *
 * @param[in] slot    FW slot
 *
 * @return            FW slot address
 */
static inline uint32_t cpu_get_slot_address(uint8_t slot)
{
    switch (slot) {
        case 1:
            return SLOT0_SIZE;
            break;

        case 2:
            return SLOT0_SIZE + SLOT1_SIZE;
            break;
    }

    return 0;
}
#endif

#if defined(SLOT0_SIZE) && defined(SLOT2_SIZE)
/**
 * @brief Get internal page for a given slot
 *
 * @param[in] slot    FW slot
 *
 * @return            FW slot page
 */
static inline uint32_t cpu_get_slot_page(uint8_t slot)
{
    switch (slot) {
        case 1:
            return SLOT0_SIZE / FLASHPAGE_SIZE;
            break;

        case 2:
            return (SLOT0_SIZE + SLOT1_SIZE) / FLASHPAGE_SIZE;
            break;
    }

    return 0;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
