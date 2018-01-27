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
#include "board.h"
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

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
WEAK_DEFAULT void isr_rfcorerxtx(void);
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

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_gpioa,              /* 16 GPIO Port A */
    isr_gpiob,              /* 17 GPIO Port B */
    isr_gpioc,              /* 18 GPIO Port C */
    isr_gpiod,              /* 19 GPIO Port D */
    (0UL),                  /* 20 none */
    isr_uart0,              /* 21 UART0 Rx and Tx */
    isr_uart1,              /* 22 UART1 Rx and Tx */
    isr_ssi0,               /* 23 SSI0 Rx and Tx */
    isr_i2c,                /* 24 I2C Master and Slave */
    (0UL),                  /* 25 Reserved */
    (0UL),                  /* 26 Reserved */
    (0UL),                  /* 27 Reserved */
    (0UL),                  /* 28 Reserved */
    (0UL),                  /* 29 Reserved */
    isr_adc,                /* 30 ADC Sequence 0 */
    (0UL),                  /* 31 Reserved */
    (0UL),                  /* 32 Reserved */
    (0UL),                  /* 33 Reserved */
    isr_watchdog,           /* 34 Watchdog timer, timer 0 */
    isr_timer0_chan0,       /* 35 Timer 0 subtimer A */
    isr_timer0_chan1,       /* 36 Timer 0 subtimer B */
    isr_timer1_chan0,       /* 37 Timer 1 subtimer A */
    isr_timer1_chan1,       /* 38 Timer 1 subtimer B */
    isr_timer2_chan0,       /* 39 Timer 2 subtimer A */
    isr_timer2_chan1,       /* 40 Timer 2 subtimer B */
    isr_comp,               /* 41 Analog Comparator 0 */
    isr_rfcorerxtx,         /* 42 RFCore Rx/Tx */
    isr_rfcoreerr,          /* 43 RFCore Error */
    isr_icepick,            /* 44 IcePick */
    isr_flash,              /* 45 FLASH Control */
    isr_aes,                /* 46 AES */
    isr_pka,                /* 47 PKA */
    isr_sleepmode,          /* 48 Sleep Timer */
    isr_mactimer,           /* 49 MacTimer */
    isr_ssi1,               /* 50 SSI1 Rx and Tx */
    isr_timer3_chan0,       /* 51 Timer 3 subtimer A */
    isr_timer3_chan1,       /* 52 Timer 3 subtimer B */
    (0UL),                  /* 53 Reserved */
    (0UL),                  /* 54 Reserved */
    (0UL),                  /* 55 Reserved */
    (0UL),                  /* 56 Reserved */
    (0UL),                  /* 57 Reserved */
    (0UL),                  /* 58 Reserved */
    (0UL),                  /* 59 Reserved */
    isr_usb,                /* 60 USB 2538 */
    (0UL),                  /* 61 Reserved */
    isr_dma,                /* 62 uDMA */
    isr_dmaerr,             /* 63 uDMA Error */
};

#if UPDATE_CCA
extern cortexm_base_t cortex_vector_base;
/**
 * @brief Flash Customer Configuration Area (CCA)
 *
 * Defines bootloader backdoor configuration, boot image validity and base address, and flash page lock bits.
 */
__attribute__((section(".flashcca"), used))
const uint32_t cca[] = {
    /* Bootloader Backdoor Configuration: */
    0xe0ffffff | (CCA_BACKDOOR_ENABLE << 28) | (CCA_BACKDOOR_ACTIVE_LEVEL << 27) | (CCA_BACKDOOR_PORT_A_PIN << 24),
    0x00000000,                     /**< Image Valid */
    (uint32_t)&cortex_vector_base,  /**< Application Entry Point */

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
