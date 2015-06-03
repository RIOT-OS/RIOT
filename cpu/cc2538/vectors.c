/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Ian Martin <ian@locicontrols.com>
 */

#include <stdint.h>
#include "cpu.h"
#include "vectors_cortexm.h"

/* get the start of the ISR stack as defined in the linkerscript */
extern uint32_t _estack;

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* CC2538 specific interrupt vectors */
WEAK_DEFAULT void isr_gpioa(void);
WEAK_DEFAULT void isr_gpiob(void);
WEAK_DEFAULT void isr_gpioc(void);
WEAK_DEFAULT void isr_gpiod(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_ssi0(void);
WEAK_DEFAULT void isr_i2c(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_watchdog(void);
WEAK_DEFAULT void isr_timer0_chan0(void);
WEAK_DEFAULT void isr_timer0_chan1(void);
WEAK_DEFAULT void isr_timer1_chan0(void);
WEAK_DEFAULT void isr_timer1_chan1(void);
WEAK_DEFAULT void isr_timer2_chan0(void);
WEAK_DEFAULT void isr_timer2_chan1(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_rfcoretx(void);
WEAK_DEFAULT void isr_rfcoreerr(void);
WEAK_DEFAULT void isr_icepick(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_pka(void);
WEAK_DEFAULT void isr_sleepmode(void);
WEAK_DEFAULT void isr_mactimer(void);
WEAK_DEFAULT void isr_ssi1(void);
WEAK_DEFAULT void isr_timer3_chan0(void);
WEAK_DEFAULT void isr_timer3_chan1(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_dma(void);
WEAK_DEFAULT void isr_dmaerr(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M3 handlers */
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
    /* CC2538 specific peripheral handlers */
    (void*) isr_gpioa,              /* 16 GPIO Port A */
    (void*) isr_gpiob,              /* 17 GPIO Port B */
    (void*) isr_gpioc,              /* 18 GPIO Port C */
    (void*) isr_gpiod,              /* 19 GPIO Port D */
    (void*) (0UL),                  /* 20 none */
    (void*) isr_uart0,              /* 21 UART0 Rx and Tx */
    (void*) isr_uart1,              /* 22 UART1 Rx and Tx */
    (void*) isr_ssi0,               /* 23 SSI0 Rx and Tx */
    (void*) isr_i2c,                /* 24 I2C Master and Slave */
    (void*) (0UL),                  /* 25 Reserved */
    (void*) (0UL),                  /* 26 Reserved */
    (void*) (0UL),                  /* 27 Reserved */
    (void*) (0UL),                  /* 28 Reserved */
    (void*) (0UL),                  /* 29 Reserved */
    (void*) isr_adc,                /* 30 ADC Sequence 0 */
    (void*) (0UL),                  /* 31 Reserved */
    (void*) (0UL),                  /* 32 Reserved */
    (void*) (0UL),                  /* 33 Reserved */
    (void*) isr_watchdog,           /* 34 Watchdog timer, timer 0 */
    (void*) isr_timer0_chan0,       /* 35 Timer 0 subtimer A */
    (void*) isr_timer0_chan1,       /* 36 Timer 0 subtimer B */
    (void*) isr_timer1_chan0,       /* 37 Timer 1 subtimer A */
    (void*) isr_timer1_chan1,       /* 38 Timer 1 subtimer B */
    (void*) isr_timer2_chan0,       /* 39 Timer 2 subtimer A */
    (void*) isr_timer2_chan1,       /* 40 Timer 2 subtimer B */
    (void*) isr_comp,               /* 41 Analog Comparator 0 */
    (void*) isr_rfcoretx,           /* 42 RFCore Rx/Tx */
    (void*) isr_rfcoreerr,          /* 43 RFCore Error */
    (void*) isr_icepick,            /* 44 IcePick */
    (void*) isr_flash,              /* 45 FLASH Control */
    (void*) isr_aes,                /* 46 AES */
    (void*) isr_pka,                /* 47 PKA */
    (void*) isr_sleepmode,          /* 48 Sleep Timer */
    (void*) isr_mactimer,           /* 49 MacTimer */
    (void*) isr_ssi1,               /* 50 SSI1 Rx and Tx */
    (void*) isr_timer3_chan0,       /* 51 Timer 3 subtimer A */
    (void*) isr_timer3_chan1,       /* 52 Timer 3 subtimer B */
    (void*) (0UL),                  /* 53 Reserved */
    (void*) (0UL),                  /* 54 Reserved */
    (void*) (0UL),                  /* 55 Reserved */
    (void*) (0UL),                  /* 56 Reserved */
    (void*) (0UL),                  /* 57 Reserved */
    (void*) (0UL),                  /* 58 Reserved */
    (void*) (0UL),                  /* 59 Reserved */
    (void*) isr_usb,                /* 60 USB 2538 */
    (void*) (0UL),                  /* 61 Reserved */
    (void*) isr_dma,                /* 62 uDMA */
    (void*) isr_dmaerr,             /* 63 uDMA Error */
};

#if UPDATE_CCA
/**
 * @brief Flash Customer Configuration Area (CCA)
 *
 * Defines bootloader backdoor configuration, boot image validity and base address, and flash page lock bits.
 */
__attribute__((section(".flashcca"), used))
const uint32_t cca[] = {
    /* Bootloader Backdoor Configuration: */
    0xe0ffffff | (CCA_BACKDOOR_ENABLE << 28) | (CCA_BACKDOOR_ACTIVE_LEVEL << 27) | (CCA_BACKDOOR_PORT_A_PIN << 24),
    0x00000000,                  /**< Image Valid */
    (uintptr_t)interrupt_vector, /**< Application Entry Point */

    /* Unlock all pages and debug: */
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
#endif

/** @} */
