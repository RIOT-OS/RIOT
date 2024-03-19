/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2017 RWTH Aachen, Josua Arndt
 *               2018 Matthew Blue
 *               2021-2023 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_avr8_common
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "cpu.h"
#include "cpu_clock.h"
#include "board.h"
#include "irq.h"
#include "periph/init.h"
#include "panic.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef RST
/* In ATxmega there is a special register to get reset cause */
#define MCUSR RST.STATUS
#else
#ifndef MCUSR
/* In older ATmegas the MCUSR register was still named MCUCSR. Current avrlibc
 * versions provide the MCUSR macro for those as well, but adding a fallback
 * here doesn't hurt*/
#define MCUSR MCUCSR
#endif /* !MCUSR */
#endif /* RST */

/*
* Since atmega MCUs do not feature a software reset, the watchdog timer
* is being used. It will be set to the shortest time and then force a
* reset. Therefore the MCUSR register needs to be reset as fast as
* possible.
* Which means in the bootloader or in the following init0 if no bootloader is used.
* Bootloader resets watchdog and pass MCUSR in r2 (e.g. Optiboot) in order to pass
* information about the reset cause to the application.
* When no Bootloader is used the watchdog will be disabled in the init0 section.
* When a software reset was triggered, r3 will contain 0xAA.
* In order to prevent changes to the values from the .init section, MCUSR and r3
* are saved in the .init0 section
*/
uint8_t mcusr_mirror __attribute__((section(".noinit")));
uint8_t soft_rst __attribute__((section(".noinit")));
#if (AVR8_STATE_IRQ_USE_SRAM)
uint8_t avr8_state_irq_count_sram = 0;
#endif
#if (AVR8_STATE_UART_USE_SRAM)
uint8_t avr8_state_uart_sram = 0;
#endif

void get_mcusr(void) __attribute__((naked, section(".init0"), used));

void get_mcusr(void)
{
    /* save soft reset flag set in reset routine */
    __asm__ __volatile__("mov %0, r3\n" : "=r" (soft_rst) :);
#ifdef BOOTLOADER_CLEARS_WATCHDOG_AND_PASSES_MCUSR
    /* save the reset flags passed from the bootloader */
    __asm__ __volatile__("mov %0, r2\n" : "=r" (mcusr_mirror) :);
#else
    /* save the reset flags */
    mcusr_mirror = MCUSR;
    MCUSR = 0;
    wdt_disable();
#endif
}

void cpu_init(void)
{
#ifdef PRUSB
    /* disable usb interrupt */
    PRR1 |= 1<<PRUSB;
#endif

    avr8_reset_cause();

    wdt_reset();   /* should not be nececessary as done in bootloader */
    wdt_disable(); /* but when used without bootloader this is needed */

    avr8_clk_init();

    /* Initialize stdio before periph_init() to allow use of DEBUG() there */
#ifdef MODULE_AVR_LIBC_EXTRA
    avr8_stdio_init();
#endif
    /* Initialize peripherals for which modules are included in the makefile.*/
    /* spi_init */
    /* rtc_init */
    /* hwrng_init */
    periph_init();

#ifdef CPU_ATXMEGA
    /* Enable Multilevel Interrupt Controller */
    PMIC.CTRL |= PMIC_HILVLEN_bm
              |  PMIC_MEDLVLEN_bm
              |  PMIC_LOLVLEN_bm;
#endif

    /* Set global resources initial state */
    avr8_state_uart = 0;
    avr8_state_irq_count = 0;

    irq_enable();
}

struct __freelist {
    size_t size;
    struct __freelist *next;
};

extern struct __freelist *__flp;
extern char *__malloc_heap_start;
extern char *__malloc_heap_end;
extern char *__brkval;

void heap_stats(void)
{
    int heap_size = __malloc_heap_end - __malloc_heap_start;
    int free = __malloc_heap_end - __brkval;
    struct __freelist *fp;
    for (fp = __flp; fp; fp = fp->next) {
        free += fp->size;
    }
    printf("heap: %d (used %d, free %d) [bytes]\n",
           heap_size, heap_size - free, free);
}
