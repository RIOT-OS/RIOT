/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_samd5x
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* SAME54 specific interrupt vector */
WEAK_DEFAULT void isr_pm                  ( void );
WEAK_DEFAULT void isr_mclk                ( void );
WEAK_DEFAULT void isr_oscctrl0            ( void );
WEAK_DEFAULT void isr_oscctrl1            ( void );
WEAK_DEFAULT void isr_oscctrl2            ( void );
WEAK_DEFAULT void isr_oscctrl3            ( void );
WEAK_DEFAULT void isr_oscctrl4            ( void );
WEAK_DEFAULT void isr_osc32kctrl          ( void );
WEAK_DEFAULT void isr_supc0               ( void );
WEAK_DEFAULT void isr_supc1               ( void );
WEAK_DEFAULT void isr_wdt                 ( void );
WEAK_DEFAULT void isr_rtc                 ( void );
WEAK_DEFAULT void isr_eic0                ( void );
WEAK_DEFAULT void isr_eic1                ( void );
WEAK_DEFAULT void isr_eic2                ( void );
WEAK_DEFAULT void isr_eic3                ( void );
WEAK_DEFAULT void isr_eic4                ( void );
WEAK_DEFAULT void isr_eic5                ( void );
WEAK_DEFAULT void isr_eic6                ( void );
WEAK_DEFAULT void isr_eic7                ( void );
WEAK_DEFAULT void isr_eic8                ( void );
WEAK_DEFAULT void isr_eic9                ( void );
WEAK_DEFAULT void isr_eic10               ( void );
WEAK_DEFAULT void isr_eic11               ( void );
WEAK_DEFAULT void isr_eic12               ( void );
WEAK_DEFAULT void isr_eic13               ( void );
WEAK_DEFAULT void isr_eic14               ( void );
WEAK_DEFAULT void isr_eic15               ( void );
WEAK_DEFAULT void irq_freqm               ( void );
WEAK_DEFAULT void isr_nvmctrl0            ( void );
WEAK_DEFAULT void isr_nvmctrl1            ( void );
WEAK_DEFAULT void isr_dmac0               ( void );
WEAK_DEFAULT void isr_dmac1               ( void );
WEAK_DEFAULT void isr_dmac2               ( void );
WEAK_DEFAULT void isr_dmac3               ( void );
WEAK_DEFAULT void isr_dmac4               ( void );
WEAK_DEFAULT void isr_evsys0              ( void );
WEAK_DEFAULT void isr_evsys1              ( void );
WEAK_DEFAULT void isr_evsys2              ( void );
WEAK_DEFAULT void isr_evsys3              ( void );
WEAK_DEFAULT void isr_evsys4              ( void );
WEAK_DEFAULT void isr_pac                 ( void );
WEAK_DEFAULT void irq_ramecc              ( void );
WEAK_DEFAULT void isr_sercom0_0           ( void );
WEAK_DEFAULT void isr_sercom0_1           ( void );
WEAK_DEFAULT void isr_sercom0_2           ( void );
WEAK_DEFAULT void isr_sercom0_3           ( void );
WEAK_DEFAULT void isr_sercom1_0           ( void );
WEAK_DEFAULT void isr_sercom1_1           ( void );
WEAK_DEFAULT void isr_sercom1_2           ( void );
WEAK_DEFAULT void isr_sercom1_3           ( void );
WEAK_DEFAULT void isr_sercom2_0           ( void );
WEAK_DEFAULT void isr_sercom2_1           ( void );
WEAK_DEFAULT void isr_sercom2_2           ( void );
WEAK_DEFAULT void isr_sercom2_3           ( void );
WEAK_DEFAULT void isr_sercom3_0           ( void );
WEAK_DEFAULT void isr_sercom3_1           ( void );
WEAK_DEFAULT void isr_sercom3_2           ( void );
WEAK_DEFAULT void isr_sercom3_3           ( void );
WEAK_DEFAULT void isr_sercom4_0           ( void );
WEAK_DEFAULT void isr_sercom4_1           ( void );
WEAK_DEFAULT void isr_sercom4_2           ( void );
WEAK_DEFAULT void isr_sercom4_3           ( void );
WEAK_DEFAULT void isr_sercom5_0           ( void );
WEAK_DEFAULT void isr_sercom5_1           ( void );
WEAK_DEFAULT void isr_sercom5_2           ( void );
WEAK_DEFAULT void isr_sercom5_3           ( void );
WEAK_DEFAULT void isr_sercom6_0           ( void );
WEAK_DEFAULT void isr_sercom6_1           ( void );
WEAK_DEFAULT void isr_sercom6_2           ( void );
WEAK_DEFAULT void isr_sercom6_3           ( void );
WEAK_DEFAULT void isr_sercom7_0           ( void );
WEAK_DEFAULT void isr_sercom7_1           ( void );
WEAK_DEFAULT void isr_sercom7_2           ( void );
WEAK_DEFAULT void isr_sercom7_3           ( void );
WEAK_DEFAULT void isr_can0                ( void );
WEAK_DEFAULT void isr_can1                ( void );
WEAK_DEFAULT void isr_usb0                ( void );
WEAK_DEFAULT void isr_usb1                ( void );
WEAK_DEFAULT void isr_usb2                ( void );
WEAK_DEFAULT void isr_usb3                ( void );
WEAK_DEFAULT void isr_gmac                ( void );
WEAK_DEFAULT void isr_tcc0_0              ( void );
WEAK_DEFAULT void isr_tcc0_1              ( void );
WEAK_DEFAULT void isr_tcc0_2              ( void );
WEAK_DEFAULT void isr_tcc0_3              ( void );
WEAK_DEFAULT void isr_tcc0_4              ( void );
WEAK_DEFAULT void isr_tcc0_5              ( void );
WEAK_DEFAULT void isr_tcc0_6              ( void );
WEAK_DEFAULT void isr_tcc1_0              ( void );
WEAK_DEFAULT void isr_tcc1_1              ( void );
WEAK_DEFAULT void isr_tcc1_2              ( void );
WEAK_DEFAULT void isr_tcc1_3              ( void );
WEAK_DEFAULT void isr_tcc1_4              ( void );
WEAK_DEFAULT void isr_tcc2_0              ( void );
WEAK_DEFAULT void isr_tcc2_1              ( void );
WEAK_DEFAULT void isr_tcc2_2              ( void );
WEAK_DEFAULT void isr_tcc2_3              ( void );
WEAK_DEFAULT void isr_tcc3_0              ( void );
WEAK_DEFAULT void isr_tcc3_1              ( void );
WEAK_DEFAULT void isr_tcc3_2              ( void );
WEAK_DEFAULT void isr_tcc4_0              ( void );
WEAK_DEFAULT void isr_tcc4_1              ( void );
WEAK_DEFAULT void isr_tcc4_2              ( void );
WEAK_DEFAULT void isr_tc0                 ( void );
WEAK_DEFAULT void isr_tc1                 ( void );
WEAK_DEFAULT void isr_tc2                 ( void );
WEAK_DEFAULT void isr_tc3                 ( void );
WEAK_DEFAULT void isr_tc4                 ( void );
WEAK_DEFAULT void isr_tc5                 ( void );
WEAK_DEFAULT void isr_tc6                 ( void );
WEAK_DEFAULT void isr_tc7                 ( void );
WEAK_DEFAULT void isr_pdec0               ( void );
WEAK_DEFAULT void isr_pdec1               ( void );
WEAK_DEFAULT void isr_pdec2               ( void );
WEAK_DEFAULT void isr_adc0_0              ( void );
WEAK_DEFAULT void isr_adc0_1              ( void );
WEAK_DEFAULT void isr_adc1_0              ( void );
WEAK_DEFAULT void isr_adc1_1              ( void );
WEAK_DEFAULT void isr_ac                  ( void );
WEAK_DEFAULT void isr_dac0                ( void );
WEAK_DEFAULT void isr_dac1                ( void );
WEAK_DEFAULT void isr_dac2                ( void );
WEAK_DEFAULT void isr_dac3                ( void );
WEAK_DEFAULT void isr_dac4                ( void );
WEAK_DEFAULT void isr_i2s                 ( void );
WEAK_DEFAULT void isr_pcc                 ( void );
WEAK_DEFAULT void isr_aes                 ( void );
WEAK_DEFAULT void isr_trng                ( void );
WEAK_DEFAULT void isr_icm                 ( void );
WEAK_DEFAULT void isr_pukcc               ( void );
WEAK_DEFAULT void isq_qspi                ( void );
WEAK_DEFAULT void isr_sdhc0               ( void );
WEAK_DEFAULT void isr_sdhc1               ( void );

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
  isr_pm,                   /*  0 Power Manager */
  isr_mclk,                 /*  1 Main Clock */
  isr_oscctrl0,             /*  2 Oscillators Control IRQ 0 */
  isr_oscctrl1,             /*  3 Oscillators Control IRQ 1 */
  isr_oscctrl2,             /*  4 Oscillators Control IRQ 2 */
  isr_oscctrl3,             /*  5 Oscillators Control IRQ 3 */
  isr_oscctrl4,             /*  6 Oscillators Control IRQ 4 */
  isr_osc32kctrl,           /*  7 32kHz Oscillators Control */
  isr_supc0,                /*  8 Supply Controller IRQ 0 */
  isr_supc1,                /*  9 Supply Controller IRQ 1 */
  isr_wdt,                  /* 10 Watchdog Timer */
  isr_rtc,                  /* 11 Real-Time Counter */
  isr_eic0,                 /* 12 External Interrupt Controller IRQ 0 */
  isr_eic1,                 /* 13 External Interrupt Controller IRQ 1 */
  isr_eic2,                 /* 14 External Interrupt Controller IRQ 2 */
  isr_eic3,                 /* 15 External Interrupt Controller IRQ 3 */
  isr_eic4,                 /* 16 External Interrupt Controller IRQ 4 */
  isr_eic5,                 /* 17 External Interrupt Controller IRQ 5 */
  isr_eic6,                 /* 18 External Interrupt Controller IRQ 6 */
  isr_eic7,                 /* 19 External Interrupt Controller IRQ 7 */
  isr_eic8,                 /* 20 External Interrupt Controller IRQ 8 */
  isr_eic9,                 /* 21 External Interrupt Controller IRQ 9 */
  isr_eic10,                /* 22 External Interrupt Controller IRQ 10 */
  isr_eic11,                /* 23 External Interrupt Controller IRQ 11 */
  isr_eic12,                /* 24 External Interrupt Controller IRQ 12 */
  isr_eic13,                /* 25 External Interrupt Controller IRQ 13 */
  isr_eic14,                /* 26 External Interrupt Controller IRQ 14 */
  isr_eic15,                /* 27 External Interrupt Controller IRQ 15 */
  irq_freqm,                /* 28 Frequency Meter */
  isr_nvmctrl0,             /* 29 Non-Volatile Memory Controller IRQ 0 */
  isr_nvmctrl1,             /* 30 Non-Volatile Memory Controller IRQ 1 */
  isr_dmac0,                /* 31 Direct Memory Access Controller IRQ 0 */
  isr_dmac1,                /* 32 Direct Memory Access Controller IRQ 1 */
  isr_dmac2,                /* 33 Direct Memory Access Controller IRQ 2 */
  isr_dmac3,                /* 34 Direct Memory Access Controller IRQ 3 */
  isr_dmac4,                /* 35 Direct Memory Access Controller IRQ 4 */
  isr_evsys0,               /* 36 Event System Interface IRQ 0 */
  isr_evsys1,               /* 37 Event System Interface IRQ 1 */
  isr_evsys2,               /* 38 Event System Interface IRQ 2 */
  isr_evsys3,               /* 39 Event System Interface IRQ 3 */
  isr_evsys4,               /* 40 Event System Interface IRQ 4 */
  isr_pac,                  /* 41 Peripheral Access Controller */
  dummy_handler,
  dummy_handler,
  dummy_handler,
  irq_ramecc,                /* 45 RAM ECC */
  isr_sercom0_0,             /* 46 Serial Communication Interface 0 IRQ 0 */
  isr_sercom0_1,             /* 47 Serial Communication Interface 0 IRQ 1 */
  isr_sercom0_2,             /* 48 Serial Communication Interface 0 IRQ 2 */
  isr_sercom0_3,             /* 49 Serial Communication Interface 0 IRQ 3 */
  isr_sercom1_0,             /* 50 Serial Communication Interface 1 IRQ 0 */
  isr_sercom1_1,             /* 51 Serial Communication Interface 1 IRQ 1 */
  isr_sercom1_2,             /* 52 Serial Communication Interface 1 IRQ 2 */
  isr_sercom1_3,             /* 53 Serial Communication Interface 1 IRQ 3 */
  isr_sercom2_0,             /* 54 Serial Communication Interface 2 IRQ 0 */
  isr_sercom2_1,             /* 55 Serial Communication Interface 2 IRQ 1 */
  isr_sercom2_2,             /* 56 Serial Communication Interface 2 IRQ 2 */
  isr_sercom2_3,             /* 57 Serial Communication Interface 2 IRQ 3 */
  isr_sercom3_0,             /* 58 Serial Communication Interface 3 IRQ 0 */
  isr_sercom3_1,             /* 59 Serial Communication Interface 3 IRQ 1 */
  isr_sercom3_2,             /* 60 Serial Communication Interface 3 IRQ 2 */
  isr_sercom3_3,             /* 61 Serial Communication Interface 3 IRQ 3 */
  isr_sercom4_0,             /* 62 Serial Communication Interface 4 IRQ 0 */
  isr_sercom4_1,             /* 63 Serial Communication Interface 4 IRQ 1 */
  isr_sercom4_2,             /* 64 Serial Communication Interface 4 IRQ 2 */
  isr_sercom4_3,             /* 65 Serial Communication Interface 4 IRQ 3 */
  isr_sercom5_0,             /* 66 Serial Communication Interface 5 IRQ 0 */
  isr_sercom5_1,             /* 67 Serial Communication Interface 5 IRQ 1 */
  isr_sercom5_2,             /* 68 Serial Communication Interface 5 IRQ 2 */
  isr_sercom5_3,             /* 69 Serial Communication Interface 5 IRQ 3 */
  isr_sercom6_0,             /* 70 Serial Communication Interface 6 IRQ 0 */
  isr_sercom6_1,             /* 71 Serial Communication Interface 6 IRQ 1 */
  isr_sercom6_2,             /* 72 Serial Communication Interface 6 IRQ 2 */
  isr_sercom6_3,             /* 73 Serial Communication Interface 6 IRQ 3 */
  isr_sercom7_0,             /* 74 Serial Communication Interface 7 IRQ 0 */
  isr_sercom7_1,             /* 75 Serial Communication Interface 7 IRQ 1 */
  isr_sercom7_2,             /* 76 Serial Communication Interface 7 IRQ 2 */
  isr_sercom7_3,             /* 77 Serial Communication Interface 7 IRQ 3 */
  isr_can0,                  /* 78 Control Area Network 0 */
  isr_can1,                  /* 79 Control Area Network 1 */
  isr_usb0,                  /* 80 Universal Serial Bus IRQ 0 */
  isr_usb1,                  /* 81 Universal Serial Bus IRQ 1 */
  isr_usb2,                  /* 82 Universal Serial Bus IRQ 2 */
  isr_usb3,                  /* 83 Universal Serial Bus IRQ 3 */
  isr_gmac,                  /* 84 Ethernet MAC */
  isr_tcc0_0,                /* 85 Timer Counter Control 0 IRQ 0 */
  isr_tcc0_1,                /* 86 Timer Counter Control 0 IRQ 1 */
  isr_tcc0_2,                /* 87 Timer Counter Control 0 IRQ 2 */
  isr_tcc0_3,                /* 88 Timer Counter Control 0 IRQ 3 */
  isr_tcc0_4,                /* 89 Timer Counter Control 0 IRQ 4 */
  isr_tcc0_5,                /* 90 Timer Counter Control 0 IRQ 5 */
  isr_tcc0_6,                /* 91 Timer Counter Control 0 IRQ 6 */
  isr_tcc1_0,                /* 92 Timer Counter Control 1 IRQ 0 */
  isr_tcc1_1,                /* 93 Timer Counter Control 1 IRQ 1 */
  isr_tcc1_2,                /* 94 Timer Counter Control 1 IRQ 2 */
  isr_tcc1_3,                /* 95 Timer Counter Control 1 IRQ 3 */
  isr_tcc1_4,                /* 96 Timer Counter Control 1 IRQ 4 */
  isr_tcc2_0,                /* 97 Timer Counter Control 2 IRQ 0 */
  isr_tcc2_1,                /* 98 Timer Counter Control 2 IRQ 1 */
  isr_tcc2_2,                /* 99 Timer Counter Control 2 IRQ 2 */
  isr_tcc2_3,                /* 100 Timer Counter Control 2 IRQ 3 */
  isr_tcc3_0,                /* 101 Timer Counter Control 3 IRQ 0 */
  isr_tcc3_1,                /* 102 Timer Counter Control 3 IRQ 1 */
  isr_tcc3_2,                /* 103 Timer Counter Control 3 IRQ 2 */
  isr_tcc4_0,                /* 104 Timer Counter Control 4 IRQ 0 */
  isr_tcc4_1,                /* 105 Timer Counter Control 4 IRQ 1 */
  isr_tcc4_2,                /* 106 Timer Counter Control 4 IRQ 2 */
  isr_tc0,                   /* 107 Basic Timer Counter 0 */
  isr_tc1,                   /* 108 Basic Timer Counter 1 */
  isr_tc2,                   /* 109 Basic Timer Counter 2 */
  isr_tc3,                   /* 110 Basic Timer Counter 3 */
  isr_tc4,                   /* 111 Basic Timer Counter 4 */
  isr_tc5,                   /* 112 Basic Timer Counter 5 */
  isr_tc6,                   /* 113 Basic Timer Counter 6 */
  isr_tc7,                   /* 114 Basic Timer Counter 7 */
  isr_pdec0,                 /* 115 Quadrature Decodeur IRQ 0 */
  isr_pdec1,                 /* 116 Quadrature Decodeur IRQ 1 */
  isr_pdec2,                 /* 117 Quadrature Decodeur IRQ 2 */
  isr_adc0_0,                /* 118 Analog Digital Converter 0 IRQ 0 */
  isr_adc0_1,                /* 119 Analog Digital Converter 0 IRQ 1 */
  isr_adc1_0,                /* 120 Analog Digital Converter 1 IRQ 0 */
  isr_adc1_1,                /* 121 Analog Digital Converter 1 IRQ 1 */
  isr_ac,                    /* 122 Analog Comparators */
  isr_dac0,                  /* 123 Digital-to-Analog Converter IRQ 0 */
  isr_dac1,                  /* 124 Digital-to-Analog Converter IRQ 1 */
  isr_dac2,                  /* 125 Digital-to-Analog Converter IRQ 2 */
  isr_dac3,                  /* 126 Digital-to-Analog Converter IRQ 3 */
  isr_dac4,                  /* 127 Digital-to-Analog Converter IRQ 4 */
  isr_i2s,                   /* 128 Inter-IC Sound Interface */
  isr_pcc,                   /* 129 Parallel Capture Controller */
  isr_aes,                   /* 130 Advanced Encryption Standard */
  isr_trng,                  /* 131 True Random Generator */
  isr_icm,                   /* 132 Integrity Check Monitor */
  isr_pukcc,                 /* 133 PUblic-Key Cryptography Controller */
  isq_qspi,                  /* 134 Quad SPI interface */
  isr_sdhc0,                 /* 135 SD/MMC Host Controller 0 */
  isr_sdhc1,                 /* 136 SD/MMC Host Controller 1 */
};
