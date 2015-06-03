/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f3
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "board.h"

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
    while (1) {asm ("nop");}
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
    while (1) {asm ("nop");}
}

void isr_bus_fault(void)
{
    while (1) {asm ("nop");}
}

void isr_usage_fault(void)
{
    while (1) {asm ("nop");}
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));
/* STM32F3 specific interrupt vector */
void isr_wwdg(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_pvd(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tamp_stamp(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc_wkup(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_flash(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_rcc(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_exti(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_channel1(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_channel2(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_channel3(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_channel4(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_channel5(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_channel6(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_channel7(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_adc1_2(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usb_hp_can_tx(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usb_lp_can_rx0(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_can_rx1(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_can_sce(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_brk_tim15(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_up_tim16(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_trg_com_tim17(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_cc(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim2(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim3(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim4(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1_ev(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1_er(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c2_ev(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c2_er(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_spi1(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_spi2(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usart1(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usart2(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usart3(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc_alarm(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usbwakeup(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_brk(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_up(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_trg_com(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_cc(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_adc3(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_spi3(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_uart4(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_uart5(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim6_dac(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim7(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_channel1(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_channel2(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_channel3(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_channel4(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_channel5(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_adc4(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_comp1_2_3(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_comp4_5_6(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_comp7(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usb_hp(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usb_lp(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usbwakeup_rmp(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_fpu(void)            __attribute__ ((weak, alias("dummy_handler")));


/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler,          /*  */
    (void*) isr_nmi,                /*  */
    (void*) isr_hard_fault,         /*  */
    (void*) isr_mem_manage,         /*  */
    (void*) isr_bus_fault,          /*  */
    (void*) isr_usage_fault,        /*  */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_svc,                /*  */
    (void*) isr_debug_mon,          /*  */
    (void*) (0UL),                  /* reserved */
    (void*) isr_pendsv,             /*  */
    (void*) isr_systick,            /*  */
    /* STMF3 specific peripheral handlers */
    (void*) isr_wwdg,               /*  */
    (void*) isr_pvd,                /*  */
    (void*) isr_tamp_stamp,         /*  */
    (void*) isr_rtc_wkup,           /*  */
    (void*) isr_flash,              /*  */
    (void*) isr_rcc,                /*  */
    (void*) isr_exti,               /*  */
    (void*) isr_exti,               /*  */
    (void*) isr_exti,               /*  */
    (void*) isr_exti,               /*  */
    (void*) isr_exti,               /*  */
    (void*) isr_dma1_channel1,      /*  */
    (void*) isr_dma1_channel2,      /*  */
    (void*) isr_dma1_channel3,      /*  */
    (void*) isr_dma1_channel4,      /*  */
    (void*) isr_dma1_channel5,      /*  */
    (void*) isr_dma1_channel6,      /*  */
    (void*) isr_dma1_channel7,      /*  */
    (void*) isr_adc1_2,             /*  */
    (void*) isr_usb_hp_can_tx,      /*  */
    (void*) isr_usb_lp_can_rx0,     /*  */
    (void*) isr_can_rx1,            /*  */
    (void*) isr_can_sce,            /*  */
    (void*) isr_exti,               /*  */
    (void*) isr_tim1_brk_tim15,     /*  */
    (void*) isr_tim1_up_tim16,      /*  */
    (void*) isr_tim1_trg_com_tim17, /*  */
    (void*) isr_tim1_cc,            /*  */
    (void*) isr_tim2,               /*  */
    (void*) isr_tim3,               /*  */
    (void*) isr_tim4,               /*  */
    (void*) isr_i2c1_ev,            /*  */
    (void*) isr_i2c1_er,            /*  */
    (void*) isr_i2c2_ev,            /*  */
    (void*) isr_i2c2_er,            /*  */
    (void*) isr_spi1,               /*  */
    (void*) isr_spi2,               /*  */
    (void*) isr_usart1,             /*  */
    (void*) isr_usart2,             /*  */
    (void*) isr_usart3,             /*  */
    (void*) isr_exti,               /*  */
    (void*) isr_rtc_alarm,          /*  */
    (void*) isr_usbwakeup,          /*  */
    (void*) isr_tim8_brk,           /*  */
    (void*) isr_tim8_up,            /*  */
    (void*) isr_tim8_trg_com,       /*  */
    (void*) isr_tim8_cc,            /*  */
    (void*) isr_adc3,               /*  */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_spi3,               /*  */
    (void*) isr_uart4,              /*  */
    (void*) isr_uart5,              /*  */
    (void*) isr_tim6_dac,           /*  */
    (void*) isr_tim7,               /*  */
    (void*) isr_dma2_channel1,      /*  */
    (void*) isr_dma2_channel2,      /*  */
    (void*) isr_dma2_channel3,      /*  */
    (void*) isr_dma2_channel4,      /*  */
    (void*) isr_dma2_channel5,      /*  */
    (void*) isr_adc4,               /*  */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_comp1_2_3,          /*  */
    (void*) isr_comp4_5_6,          /*  */
    (void*) isr_comp7,              /*  */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_usb_hp,             /*  */
    (void*) isr_usb_lp,             /*  */
    (void*) isr_usbwakeup_rmp,      /*  */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_fpu,                /*  */
};
