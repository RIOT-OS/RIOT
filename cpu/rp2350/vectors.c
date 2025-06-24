/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Interrupt vector table for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "RP2350.h"
#include "cpu_conf.h"
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) { dummy_handler_default(); }

/* rp2350 specific interrupt vector */
WEAK_DEFAULT void isr_timer0_0(void);
WEAK_DEFAULT void isr_timer0_1(void);
WEAK_DEFAULT void isr_timer0_2(void);
WEAK_DEFAULT void isr_timer0_3(void);
WEAK_DEFAULT void isr_timer1_0(void);
WEAK_DEFAULT void isr_timer1_1(void);
WEAK_DEFAULT void isr_timer1_2(void);
WEAK_DEFAULT void isr_timer1_3(void);
WEAK_DEFAULT void isr_pwm_wrap_0(void);
WEAK_DEFAULT void isr_pwm_wrap_1(void);
WEAK_DEFAULT void isr_dma_0(void);
WEAK_DEFAULT void isr_dma_1(void);
WEAK_DEFAULT void isr_dma_2(void);
WEAK_DEFAULT void isr_dma_3(void);
WEAK_DEFAULT void isr_usbctrl(void);
WEAK_DEFAULT void isr_pio0_0(void);
WEAK_DEFAULT void isr_pio0_1(void);
WEAK_DEFAULT void isr_pio1_0(void);
WEAK_DEFAULT void isr_pio1_1(void);
WEAK_DEFAULT void isr_pio2_0(void);
WEAK_DEFAULT void isr_pio2_1(void);
WEAK_DEFAULT void isr_io_bank0(void);
WEAK_DEFAULT void isr_io_bank0_ns(void);
WEAK_DEFAULT void isr_io_qspi(void);
WEAK_DEFAULT void isr_io_qspi_ns(void);
WEAK_DEFAULT void isr_sio_fifo(void);
WEAK_DEFAULT void isr_sio_bell(void);
WEAK_DEFAULT void isr_sio_fifo_ns(void);
WEAK_DEFAULT void isr_sio_bell_ns(void);
WEAK_DEFAULT void isr_sio_mtimecmp(void);
WEAK_DEFAULT void isr_clocks(void);
WEAK_DEFAULT void isr_spi0(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_adc_fifo(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_otp(void);
WEAK_DEFAULT void isr_trng(void);
WEAK_DEFAULT void isr_proc0_cti(void);
WEAK_DEFAULT void isr_proc1_cti(void);
WEAK_DEFAULT void isr_pll_sys(void);
WEAK_DEFAULT void isr_pll_usb(void);
WEAK_DEFAULT void isr_powman_pow(void);
WEAK_DEFAULT void isr_powman_timer(void);
WEAK_DEFAULT void isr_spareirq_0(void);
WEAK_DEFAULT void isr_spareirq_1(void);
WEAK_DEFAULT void isr_spareirq_2(void);
WEAK_DEFAULT void isr_spareirq_3(void);
WEAK_DEFAULT void isr_spareirq_4(void);
WEAK_DEFAULT void isr_spareirq_5(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1)
const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    (void*)isr_timer0_0,     /* 0 TIMER0_IRQ_0 */
    (void*)isr_timer0_1,     /* 1 TIMER0_IRQ_1 */
    (void*)isr_timer0_2,     /* 2 TIMER0_IRQ_2 */
    (void*)isr_timer0_3,     /* 3 TIMER0_IRQ_3 */
    (void*)isr_timer1_0,     /* 4 TIMER1_IRQ_0 */
    (void*)isr_timer1_1,     /* 5 TIMER1_IRQ_1 */
    (void*)isr_timer1_2,     /* 6 TIMER1_IRQ_2 */
    (void*)isr_timer1_3,     /* 7 TIMER1_IRQ_3 */
    (void*)isr_pwm_wrap_0,   /* 8 PWM_IRQ_WRAP_0 */
    (void*)isr_pwm_wrap_1,   /* 9 PWM_IRQ_WRAP_1 */
    (void*)isr_dma_0,        /* 10 DMA_IRQ_0 */
    (void*)isr_dma_1,        /* 11 DMA_IRQ_1 */
    (void*)isr_dma_2,        /* 12 DMA_IRQ_2 */
    (void*)isr_dma_3,        /* 13 DMA_IRQ_3 */
    (void*)isr_usbctrl,      /* 14 USBCTRL_IRQ */
    (void*)isr_pio0_0,       /* 15 PIO0_IRQ_0 */
    (void*)isr_pio0_1,       /* 16 PIO0_IRQ_1 */
    (void*)isr_pio1_0,       /* 17 PIO1_IRQ_0 */
    (void*)isr_pio1_1,       /* 18 PIO1_IRQ_1 */
    (void*)isr_pio2_0,       /* 19 PIO2_IRQ_0 */
    (void*)isr_pio2_1,       /* 20 PIO2_IRQ_1 */
    (void*)isr_io_bank0,     /* 21 IO_IRQ_BANK0 */
    (void*)isr_io_bank0_ns,  /* 22 IO_IRQ_BANK0_NS */
    (void*)isr_io_qspi,      /* 23 IO_IRQ_QSPI */
    (void*)isr_io_qspi_ns,   /* 24 IO_IRQ_QSPI_NS */
    (void*)isr_sio_fifo,     /* 25 SIO_IRQ_FIFO */
    (void*)isr_sio_bell,     /* 26 SIO_IRQ_BELL */
    (void*)isr_sio_fifo_ns,  /* 27 SIO_IRQ_FIFO_NS */
    (void*)isr_sio_bell_ns,  /* 28 SIO_IRQ_BELL_NS */
    (void*)isr_sio_mtimecmp, /* 29 SIO_IRQ_MTIMECMP */
    (void*)isr_clocks,       /* 30 CLOCKS_IRQ */
    (void*)isr_spi0,         /* 31 SPI0_IRQ */
    (void*)isr_spi1,         /* 32 SPI1_IRQ */
    (void*)isr_uart0,        /* 33 UART0_IRQ */
    (void*)isr_uart1,        /* 34 UART1_IRQ */
    (void*)isr_adc_fifo,     /* 35 ADC_IRQ_FIFO */
    (void*)isr_i2c0,         /* 36 I2C0_IRQ */
    (void*)isr_i2c1,         /* 37 I2C1_IRQ */
    (void*)isr_otp,          /* 38 OTP_IRQ */
    (void*)isr_trng,         /* 39 TRNG_IRQ */
    (void*)isr_proc0_cti,    /* 40 PROC0_IRQ_CTI */
    (void*)isr_proc1_cti,    /* 41 PROC1_IRQ_CTI */
    (void*)isr_pll_sys,      /* 42 PLL_SYS_IRQ */
    (void*)isr_pll_usb,      /* 43 PLL_USB_IRQ */
    (void*)isr_powman_pow,   /* 44 POWMAN_IRQ_POW */
    (void*)isr_powman_timer, /* 45 POWMAN_IRQ_TIMER */
    (void*)isr_spareirq_0,   /* 46 SPAREIRQ_IRQ_0 */
    (void*)isr_spareirq_1,   /* 47 SPAREIRQ_IRQ_1 */
    (void*)isr_spareirq_2,   /* 48 SPAREIRQ_IRQ_2 */
    (void*)isr_spareirq_3,   /* 49 SPAREIRQ_IRQ_3 */
    (void*)isr_spareirq_4,   /* 50 SPAREIRQ_IRQ_4 */
    (void*)isr_spareirq_5,   /* 51 SPAREIRQ_IRQ_5 */
};

/** @} */
