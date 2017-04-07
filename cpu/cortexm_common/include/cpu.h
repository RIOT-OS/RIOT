/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *               2017      Inria
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
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
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
* @brief   Offset to reset handler on VTOR
*/
#define VTOR_RESET_HANDLER  (0x4) /** One pointer after the beginning */

#if defined(FLASHPAGE_SIZE) && defined(FLASHPAGE_NUMOF) && defined (FW_SLOT_PAGES)
/**
 * @brief  Defines for slot management
 */
#define BOOTLOADER_SPACE    (0x4000)
#define FW_SLOT_SIZE        FLASHPAGE_SIZE * FW_SLOT_PAGES
#define FW_SLOT_1           FLASH_BASE + BOOTLOADER_SPACE
#define FW_SLOT_1_END       FW_SLOT_1 + FW_SLOT_SIZE
#define FW_SLOT_2           FW_SLOT_1_END
#define FW_SLOT_2_END       FW_SLOT_2 + FW_SLOT_SIZE
#endif

/**
* @brief     _estack pointer needed to reset PSP position for
*            cortexm_branch_address
*/
extern uint32_t _estack;

/**
 * @brief   Initialization of the CPU
 */
void cpu_init(void);

/**
 * @brief   Initialize Cortex-M specific core parts of the CPU
 */
void cortexm_init(void);

#if defined(FW_SLOT_1) && defined(FW_SLOT_2)
/**
 * @brief Get FW internal address for a given slot
 *
 * @param[in] slot    FW slot
 *
 * @return            FW slot address
 */
static inline uint32_t get_slot_address(uint8_t slot)
{
    switch (slot) {
        case 1:
            return FW_SLOT_1;
            break;

        case 2:
            return FW_SLOT_2;
            break;
    }

    return 0;
}
#endif

#if defined(FW_SLOT_1_PAGE) && defined(FW_SLOT_2_PAGE)
/**
 * @brief Get internal page for a given slot
 *
 * @param[in] slot    FW slot
 *
 * @return            FW slot page
 */
static inline uint32_t get_slot_page(uint8_t slot)
{
    switch (slot) {
        case 1:
            return FW_SLOT_1_PAGE;
            break;

        case 2:
            return FW_SLOT_2_PAGE;
            break;
    }

    return 0;
}
#endif

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
 *  @brief   Branch the execution to a specified address, usually used to
 *           jump to another RIOT instance.
 */
static inline void cortexm_branch_address(uint32_t destination_address)
{
    /* Disable IRQ */
    __disable_irq();

    /* Move PSP to the end of the stack */
    __set_PSP((uint32_t)&_estack);

    /* Load the destination address */
    __asm("LDR R0, [%[dest]]"::[dest]"r"(destination_address));
    /* Make sure the Thumb State bit is set. */
    __asm("ORR R0, #1");
    /* Branch execution */
    __asm("BX R0");
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
