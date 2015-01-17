/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */


#include <stdint.h>

#include "board.h"
#include "kinetis_sim.h"
#include "k20_gpio.h"

#include "MK20DZ10.h"
#include "cpu.h"

#define ENABLE_DEBUG (1)
#include "debug.h"
#include "periph/uart.h"

/**
 * memory markers as defined in the linker script
 */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/**
 * @brief functions for initializing the board, std-lib and kernel
 */
extern void cpu_init(void);
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

void panic_delay(uint32_t delay)
{
    for (uint32_t j = 0; j < delay; j++) {
        for (uint32_t i = 0; i < 0xF; i++) {
            asm("nop");
        }
    }
}

__attribute__((noreturn)) void panic_blink(void)
{
    while(1) {
        LED_GREEN_ON;
        panic_delay(100000);

        LED_GREEN_OFF;
        panic_delay(100000);

        LED_GREEN_ON;
        panic_delay(100000);

        LED_GREEN_OFF;
        panic_delay(1000000);
    }
}

/**
 * @brief This function is the entry point after a system reset
 *
 * After a system reset, the following steps are necessary and carried out:
 * 1. load data section from flash to ram
 * 2. overwrite uninitialized data section (BSS) with zeros
 * 3. initialize the newlib
 * 4. initialize the board (sync clock, setup std-IO)
 * 5. initialize and start RIOTs kernel
 */
void reset_handler(void)
{
    /* Disable Watchdog */
    WDOG_UNLOCK = 0xc520;
    WDOG_UNLOCK = 0xd928;
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;

    /* load data section from flash to ram */
    uint32_t *dst;
    uint32_t *src = &_etext;

    for (dst = &_srelocate; dst < &_erelocate; ) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    for (dst = &_szero; dst < &_ezero; ) {
        *(dst++) = 0;
    }

    /* initialize the board and startup the kernel */
    board_init();
    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array();
    /* startup the kernel */
    kernel_init();
}


/**
 * @brief Default handler is called in case no interrupt handler was defined
 */
void dummy_handler(void)
{
    DEBUG("Dummy interrupt handler called! Stalling...");
    panic_blink();
}

void isr_nmi(void)
{
    DEBUG("Non maskable interrupt called! Stalling...");
    panic_blink();
}

void isr_mem_manage(void)
{
    DEBUG("Mem manage interrupt called! Stalling...");
    panic_blink();
}

void isr_debug_mon(void)
{
    DEBUG("Debug monitor interrupt called! Stalling...");
    panic_blink();
}

void isr_hard_fault(void)
{
    DEBUG("HARD FAULT interrupt called! Stalling...");
    panic_blink();
}

void isr_bus_fault(void)
{
    DEBUG("BUS FAULT interrupt called! Stalling...");
    panic_blink();
}

void isr_usage_fault(void)
{
    DEBUG("Usage fault interrupt handler called! Stalling...");
    panic_blink();
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));

/* K20 specific interrupt vector */
void isr_dma0(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_dma3(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_dma_error(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_dma(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_flash_cmd_complete(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_flash_read_collision(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_mode_low_voltage(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_llwu(void) __attribute__ ((weak, alias("dummy_handler"))); // low leakage wakeup
void isr_wdog(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c0(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_spi0(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_i2s0(void) __attribute__ ((weak, alias("dummy_handler"))); // transmit
void isr_i2s1(void) __attribute__ ((weak, alias("dummy_handler"))); // receive
void isr_uart0_lon_status(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_uart0_status(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_uart0_error(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_uart1_lon_status(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_uart1_status(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_uart1_error(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_uart2_lon_status(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_uart2_status(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_uart2_error(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_adc0(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_cmp0(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_cmp1(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_ftm0(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_ftm1(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_cmt(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc_alarm(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc_seconds(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_timer_pit0(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_timer_pit1(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_timer_pit2(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_timer_pit3(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_timer_pdb(void) __attribute__ ((weak, alias("dummy_handler"))); // pdb timer
void isr_usb_otg(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_usb_chrg(void) __attribute__ ((weak, alias("dummy_handler"))); // charger detect
void isr_tsi(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_mcg(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_lptmr(void) __attribute__ ((weak, alias("dummy_handler"))); // low power timer
void isr_port_a(void) __attribute__ ((weak, alias("dummy_handler"))); // pin detect port a
void isr_port_b(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_port_c(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_port_d(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_port_e(void) __attribute__ ((weak, alias("dummy_handler")));
void isr_swi(void) __attribute__ ((weak, alias("dummy_handler")));


__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler,          /* entry point of the program */
    (void*) isr_nmi,                /* non maskable interrupt handler */
    (void*) isr_hard_fault,         /* if you end up here its not good */
    (void*) isr_mem_manage,         /* memory controller interrupt */
    (void*) isr_bus_fault,          /* also not good to end up here */
    (void*) isr_usage_fault,        /* autsch */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt */
    (void*) isr_debug_mon,          /* debug interrupt */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, used for task switching in RIOT */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* K20 specific peripheral handlers */
    (void*) isr_dma0,
    (void*) isr_dma1,
    (void*) isr_dma2,
    (void*) isr_dma3,
    (void*) isr_dma_error,
    (void*) isr_dma,
    (void*) isr_flash_cmd_complete,
    (void*) isr_flash_read_collision,
    (void*) isr_mode_low_voltage,
    (void*) isr_llwu, // low leakage wakeup
    (void*) isr_wdog,
    (void*) isr_i2c0,
    (void*) isr_spi0,
    (void*) isr_i2s0, // transmit
    (void*) isr_i2s1, // receive
    (void*) isr_uart0_lon_status,
    (void*) isr_uart0_status,
    (void*) isr_uart0_error,
    (void*) isr_uart1_status,
    (void*) isr_uart1_error,
    (void*) isr_uart2_status,
    (void*) isr_uart2_error,
    (void*) isr_adc0,
    (void*) isr_cmp0,
    (void*) isr_cmp1,
    (void*) isr_ftm0,
    (void*) isr_ftm1,
    (void*) isr_cmt,
    (void*) isr_rtc_alarm,
    (void*) isr_rtc_seconds,
    (void*) isr_timer_pit0,
    (void*) isr_timer_pit1,
    (void*) isr_timer_pit2,
    (void*) isr_timer_pit3,
    (void*) isr_timer_pdb, // pdb timer
    (void*) isr_usb_otg,
    (void*) isr_usb_chrg, // charger detect
    (void*) isr_tsi,
    (void*) isr_mcg,
    (void*) isr_lptmr, // low power timer
    (void*) isr_port_a, // pin detect port a
    (void*) isr_port_b,
    (void*) isr_port_c,
    (void*) isr_port_d,
    (void*) isr_port_e,
    (void*) isr_swi
};
