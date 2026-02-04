/*
 * SPDX-FileCopyrightText: 2020 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Trigger reset to the bootloader stored in the internal boot ROM
 *              memory.
 *
 *              This will start the DFU/UART/SPI bootloader.
 *              See application note AN2606 for which options are available on
 *              your individual MCU.
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"
#include "sched.h"

/* repurpose the sched_context_switch_request variable to signal
 * jump to bootloader after reset */
#define MAGIC               sched_context_switch_request
#define BOOTLOADER_MAGIC    0xB007AFFE

/* called by reset_handler_default() before memory is initialized */
void pre_startup(void)
{
    if (MAGIC != BOOTLOADER_MAGIC) {
        return;
    }

    /* clear magic */
    MAGIC = 0;

    /* enable SYSCFG clock */
#if defined(RCC_APB2ENR_SYSCFGEN)
    RCC->APB2ENR   = RCC_APB2ENR_SYSCFGEN;
#elif defined(RCC_APB2ENR_SYSCFGCOMPEN)
    RCC->APB2ENR   = RCC_APB2ENR_SYSCFGCOMPEN
#endif

    /* remap ROM at zero */
#if defined(SYSCFG_MEMRMP_MEM_MODE_0)
    SYSCFG->MEMRMP = SYSCFG_MEMRMP_MEM_MODE_0;
#elif defined(SYSCFG_CFGR1_MEM_MODE_0)
    SYSCFG->CFGR1  = SYSCFG_CFGR1_MEM_MODE_0;
#endif

    /* jump to the bootloader */
    __asm__ volatile(
        "ldr r0, [%[btldr]] \n"
        "mov sp, r0 \n"
        "ldr r0, [%[btldr], #4] \n"
        "mov pc, r0 \n"
        : /* no outputs */
        : [btldr] "r" ((uintptr_t)STM32_BOOTLOADER_ADDR)
        : "r0", "memory"
    );
}

void __attribute__((weak)) usb_board_reset_in_bootloader(void)
{
    irq_disable();
    MAGIC = BOOTLOADER_MAGIC;
    NVIC_SystemReset();
}
