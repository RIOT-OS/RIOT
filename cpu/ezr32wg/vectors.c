/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_ezr32wg
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
#include "vectors_cortexm.h"

/* get the start of the ISR stack as defined in the linkerscript */
extern uint32_t _estack;

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    __asm__("b     dummy_handler_default");
}

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* Silicon Labs specific interrupt vector */
WEAK_DEFAULT void isr_dma(void);
WEAK_DEFAULT void isr_gpio_even(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_usart0_rx(void);
WEAK_DEFAULT void isr_usart0_tx(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_acmp0(void);
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_dac0(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_gpio_odd(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_timer2(void);
WEAK_DEFAULT void isr_timer3(void);
WEAK_DEFAULT void isr_usart1_rx(void);
WEAK_DEFAULT void isr_usart1_tx(void);
WEAK_DEFAULT void isr_lesense(void);
WEAK_DEFAULT void isr_usart2_rx(void);
WEAK_DEFAULT void isr_usart2_tx(void);
WEAK_DEFAULT void isr_uart0_rx(void);
WEAK_DEFAULT void isr_uart0_tx(void);
WEAK_DEFAULT void isr_uart1_rx(void);
WEAK_DEFAULT void isr_uart1_tx(void);
WEAK_DEFAULT void isr_leuart0(void);
WEAK_DEFAULT void isr_leuart1(void);
WEAK_DEFAULT void isr_letimer0(void);
WEAK_DEFAULT void isr_pcnt0(void);
WEAK_DEFAULT void isr_pcnt1(void);
WEAK_DEFAULT void isr_pcnt2(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_burtc(void);
WEAK_DEFAULT void isr_cmu(void);
WEAK_DEFAULT void isr_vcmp(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_msc(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_emu(void);
WEAK_DEFAULT void isr_fpueh(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) mem_manage_default,     /* memory manage exception */
    (void*) bus_fault_default,      /* bus fault exception */
    (void*) usage_fault_default,    /* usage fault exception */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) debug_mon_default,      /* debug monitor exception */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* Silicon Labs specific peripheral handlers */
    (void*) isr_dma,                /* 0 - DMA       */
    (void*) isr_gpio_even,          /* 1 - GPIO_EVEN       */
    (void*) isr_timer0,             /* 2 - TIMER0       */
    (void*) isr_usart0_rx,          /* 3 - USART0_RX       */
    (void*) isr_usart0_tx,          /* 4 - USART0_TX       */
    (void*) isr_usb,                /* 5 - USB       */
    (void*) isr_acmp0,              /* 6 - ACMP0       */
    (void*) isr_adc0,               /* 7 - ADC0       */
    (void*) isr_dac0,               /* 8 - DAC0       */
    (void*) isr_i2c0,               /* 9 - I2C0       */
    (void*) isr_i2c1,               /* 10 - I2C1       */
    (void*) isr_gpio_odd,           /* 11 - GPIO_ODD       */
    (void*) isr_timer1,             /* 12 - TIMER1       */
    (void*) isr_timer2,             /* 13 - TIMER2       */
    (void*) isr_timer3,             /* 14 - TIMER3       */
    (void*) isr_usart1_rx,          /* 15 - USART1_RX       */
    (void*) isr_usart1_tx,          /* 16 - USART1_TX       */
    (void*) isr_lesense,            /* 17 - LESENSE       */
    (void*) isr_usart2_rx,          /* 18 - USART2_RX       */
    (void*) isr_usart2_tx,          /* 19 - USART2_TX       */
    (void*) isr_uart0_rx,           /* 20 - UART0_RX       */
    (void*) isr_uart0_tx,           /* 21 - UART0_TX       */
    (void*) isr_uart1_rx,           /* 22 - UART1_RX       */
    (void*) isr_uart1_tx,           /* 23 - UART1_TX       */
    (void*) isr_leuart0,            /* 24 - LEUART0       */
    (void*) isr_leuart1,            /* 25 - LEUART1       */
    (void*) isr_letimer0,           /* 26 - LETIMER0       */
    (void*) isr_pcnt0,              /* 27 - PCNT0       */
    (void*) isr_pcnt1,              /* 28 - PCNT1       */
    (void*) isr_pcnt2,              /* 29 - PCNT2       */
    (void*) isr_rtc,                /* 30 - RTC       */
    (void*) isr_burtc,              /* 31 - BURTC       */
    (void*) isr_cmu,                /* 32 - CMU       */
    (void*) isr_vcmp,               /* 33 - VCMP       */
    (void*) isr_lcd,                /* 34 - LCD       */
    (void*) isr_msc,                /* 35 - MSC       */
    (void*) isr_aes,                /* 36 - AES       */
    (void*) (0UL),                  /* 37 - Reserved */
    (void*) isr_emu,                /* 38 - EMU       */
    (void*) isr_fpueh,              /* 39 - FPUEH       */
};
