/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc3200
 * @{
 *
 * @file
 * @brief           Interrupt vector definitions
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 *
 * @{
 */

#include "cpu.h"
#include "vectors_cortexm.h"
#include <stdint.h>

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void)
{
    dummy_handler_default();
}

/* CC3200 specific interrupt vectors */
WEAK_DEFAULT void isr_gpio_porta0(void);
WEAK_DEFAULT void isr_gpio_porta1(void);
WEAK_DEFAULT void isr_gpio_porta2(void);
WEAK_DEFAULT void isr_gpio_porta3(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_adc0_seq0(void);
WEAK_DEFAULT void isr_adc0_seq1(void);
WEAK_DEFAULT void isr_adc0_seq2(void);
WEAK_DEFAULT void isr_adc0_seq3(void);
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_timer0a(void);
WEAK_DEFAULT void isr_timer0b(void);
WEAK_DEFAULT void isr_timer1a(void);
WEAK_DEFAULT void isr_timer1b(void);
WEAK_DEFAULT void isr_timer2a(void);
WEAK_DEFAULT void isr_timer2b(void);
WEAK_DEFAULT void isr_timer3a(void);
WEAK_DEFAULT void isr_timer3b(void);
WEAK_DEFAULT void isr_flashctl(void);
WEAK_DEFAULT void isr_udma_sw(void);
WEAK_DEFAULT void isr_udma_error(void);
WEAK_DEFAULT void isr_sha(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_des(void);
WEAK_DEFAULT void isr_sdhost(void);
WEAK_DEFAULT void isr_i2s(void);
WEAK_DEFAULT void isr_camera(void);
WEAK_DEFAULT void isr_shared_spi(void);
WEAK_DEFAULT void isr_generic_spi(void);
WEAK_DEFAULT void isr_link_spi(void);
WEAK_DEFAULT void isr_nwp(void);
WEAK_DEFAULT void isr_prcm(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1)
const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [0] = isr_gpio_porta0,      /* 16 GPIO Port A */
    [1] = isr_gpio_porta1,      /* 17 GPIO Port B */
    [2] = isr_gpio_porta2,      /* 18 GPIO Port C */
    [3] = isr_gpio_porta3,      /* 19 GPIO Port D */
                                /* 20 Reserved */
    [5] = isr_uart0,            /* 21 UART0 Rx and Tx */
    [6] = isr_uart1,            /* 22 UART1 Rx and Tx */
                                /* 23 Reserved */
    [8] = isr_i2c0,             /* 24 I2C0 Master and Slave */
                                /* 25 - 29 Reserved */
    [14] = isr_adc0_seq0,       /* 30 ADC 0 Sequence 0 */
    [15] = isr_adc0_seq1,       /* 31 ADC 0 Sequence 1 */
    [16] = isr_adc0_seq2,       /* 32 ADC 0 Sequence 2 */
    [17] = isr_adc0_seq3,       /* 33 ADC 0 Sequence 3 */
    [18] = isr_wdt,             /* 34 Watchdog timer */
    [19] = isr_timer0a,         /* 35 Timer 0 subtimer A */
    [20] = isr_timer0b,         /* 36 Timer 0 subtimer B */
    [21] = isr_timer1a,         /* 37 Timer 1 subtimer A */
    [22] = isr_timer1b,         /* 38 Timer 1 subtimer B */
    [23] = isr_timer2a,         /* 39 Timer 2 subtimer A */
    [24] = isr_timer2b,         /* 40 Timer 2 subtimer B */
                                /* 41 - 44 Reserved */
    [29] = isr_flashctl,        /* 45 Flash */
                                /* 46 - 50 Reserved */
    [35] = isr_timer3a,         /* 51 Timer 3 subtimer B */
    [36] = isr_timer3b,         /* 52 Timer 3 subtimer B */
                                /* 53 - 61 Reserved */
    [46] = isr_udma_sw,         /* 62 uDMA Software Transfer */
    [47] = isr_udma_error,      /* 63 uDMA Error */
                                /* 64 - 163 Reserved */
    [148] = isr_sha,            /* 164 SHA */
                                /* 165 - 166 Reserved */
    [151] = isr_aes,            /* 167 AES */
                                /* 168 Reserved */
    [153] = isr_des,            /* 169 DES */
                                /* Reserved */
    [159] = isr_sdhost,         /* 175 SDHost */
                                /* 176 Reserved */
    [161] = isr_i2s,            /* 177 I2S */
                                /* 178 Reserved */
    [163] = isr_camera,         /* 179 Camera */
                                /* 181 - 186 Reserved */
    [171] = isr_nwp,            /* 187 NWP to APPS Interrupt */
    [172] = isr_prcm,           /* 188 Power, Reset and Clock module */
                                /* 189 - 190 Reserved */
    [175] = isr_shared_spi,     /* 191 Shared SPI interrupt (for SFLASH) */
    [176] = isr_generic_spi,    /* 192 SPI */
    [177] = isr_link_spi,       /* 193 Link SPI (APPS to NWP) */
};
