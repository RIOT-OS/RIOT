/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "panic.h"


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
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

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
    uint32_t *dst;
    uint32_t *src = &_etext;

    /* load data section from flash to ram */
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
    core_panic(PANIC_DUMMY_HANDLER, "DUMMY HANDLER");
}

void isr_nmi(void)
{
    while (1) {asm ("nop");}
}

void isr_mem_manage(void)
{
    while (1) {asm ("nop");}
}

void isr_debug_mon(void)
{
    while (1) {asm ("nop");}
}

void isr_hard_fault(void)
{
    core_panic(PANIC_HARD_FAULT, "HARD FAULT");
}

void isr_bus_fault(void)
{
    core_panic(PANIC_BUS_FAULT, "BUS FAULT");
}

void isr_usage_fault(void)
{
    core_panic(PANIC_USAGE_FAULT, "USAGE FAULT");
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));

/* stm32f1 specific interrupt vector */
void isr_wwdg(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_pvd(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tamper(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_flash(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_rcc(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_exti0(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_exti1(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_exti2(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_exti3(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_exti4(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch1(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch2(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch3(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch4(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch5(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch6(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch7(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_adc1_2(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_usb_hp_can1_tx(void)     __attribute__ ((weak, alias("dummy_handler")));
void isr_usb_lp_can1_rx0(void)    __attribute__ ((weak, alias("dummy_handler")));
void isr_can1_rx1(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_can1_sce(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_exti9_5(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_brk(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_up(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_trg_com(void)   __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_cc(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_tim2(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_tim3(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_tim4(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1_ev(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1_er(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c2_ev(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c2_er(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_spi1(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_spi2(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_usart1(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_usart2(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_usart3(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_exti15_10(void)      __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc_alarm(void)      __attribute__ ((weak, alias("dummy_handler")));
void isr_usb_wakeup(void)     __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_brk(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_up(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_trg_com(void)   __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_cc(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_adc3(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_fsmc(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_sdio(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_tim5(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_spi3(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_uart4(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_uart5(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_tim6(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_tim7(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_ch1(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_ch2(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_ch3(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_ch4_5(void)     __attribute__ ((weak, alias("dummy_handler")));


/* interrupt vector table */
__attribute__ ((section(".vectors"))) __attribute__((used))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M handlers */
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
    /* STM specific peripheral handlers */
    (void*) isr_wwdg,
    (void*) isr_pvd,
    (void*) isr_tamper,
    (void*) isr_rtc,
    (void*) isr_flash,
    (void*) isr_rcc,
    (void*) isr_exti0,
    (void*) isr_exti1,
    (void*) isr_exti2,
    (void*) isr_exti3,
    (void*) isr_exti4,
    (void*) isr_dma1_ch1,
    (void*) isr_dma1_ch2,
    (void*) isr_dma1_ch3,
    (void*) isr_dma1_ch4,
    (void*) isr_dma1_ch5,
    (void*) isr_dma1_ch6,
    (void*) isr_dma1_ch7,
    (void*) isr_adc1_2,
    (void*) isr_usb_hp_can1_tx,
    (void*) isr_usb_lp_can1_rx0,
    (void*) isr_can1_rx1,
    (void*) isr_can1_sce,
    (void*) isr_exti9_5,
    (void*) isr_tim1_brk,
    (void*) isr_tim1_up,
    (void*) isr_tim1_trg_com,
    (void*) isr_tim1_cc,
    (void*) isr_tim2,
    (void*) isr_tim3,
    (void*) isr_tim4,
    (void*) isr_i2c1_ev,
    (void*) isr_i2c1_er,
    (void*) isr_i2c2_ev,
    (void*) isr_i2c2_er,
    (void*) isr_spi1,
    (void*) isr_spi2,
    (void*) isr_usart1,
    (void*) isr_usart2,
    (void*) isr_usart3,
    (void*) isr_exti15_10,
    (void*) isr_rtc_alarm,
    (void*) isr_usb_wakeup,
    (void*) isr_tim8_brk,
    (void*) isr_tim8_up,
    (void*) isr_tim8_trg_com,
    (void*) isr_tim8_cc,
    (void*) isr_adc3,
    (void*) isr_fsmc,
    (void*) isr_sdio,
    (void*) isr_tim5,
    (void*) isr_spi3,
    (void*) isr_uart4,
    (void*) isr_uart5,
    (void*) isr_tim6,
    (void*) isr_tim7,
    (void*) isr_dma2_ch1,
    (void*) isr_dma2_ch2,
    (void*) isr_dma2_ch3,
    (void*) isr_dma2_ch4_5,
};
