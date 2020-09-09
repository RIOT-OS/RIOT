/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa#gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file        vectors.c
 * @brief       Interrupt vector definitions
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 */

#include <stdint.h>
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* LM4F120 specific interrupt vectors */
WEAK_DEFAULT void isr_gpio_porta(void);
WEAK_DEFAULT void isr_gpio_portb(void);
WEAK_DEFAULT void isr_gpio_portc(void);
WEAK_DEFAULT void isr_gpio_portd(void);
WEAK_DEFAULT void isr_gpio_porte(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_ssi0(void);
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
WEAK_DEFAULT void isr_comp0(void);
WEAK_DEFAULT void isr_comp1(void);
WEAK_DEFAULT void isr_sysctl(void);
WEAK_DEFAULT void isr_flashctl(void);
WEAK_DEFAULT void isr_gpio_portf(void);
WEAK_DEFAULT void isr_uart2(void);
WEAK_DEFAULT void isr_ssi1(void);
WEAK_DEFAULT void isr_timer3a(void);
WEAK_DEFAULT void isr_timer3b(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_can0(void);
WEAK_DEFAULT void isr_hibernate(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_udma_sw(void);
WEAK_DEFAULT void isr_udma_error(void);
WEAK_DEFAULT void isr_adc1_seq0(void);
WEAK_DEFAULT void isr_adc1_seq1(void);
WEAK_DEFAULT void isr_adc1_seq2(void);
WEAK_DEFAULT void isr_adc1_seq3(void);
WEAK_DEFAULT void isr_ssi2(void);
WEAK_DEFAULT void isr_ssi3(void);
WEAK_DEFAULT void isr_uart3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_uart6(void);
WEAK_DEFAULT void isr_uart7(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_i2c4(void);
WEAK_DEFAULT void isr_timer4a(void);
WEAK_DEFAULT void isr_timer4b(void);
WEAK_DEFAULT void isr_timer5a(void);
WEAK_DEFAULT void isr_timer5b(void);
WEAK_DEFAULT void isr_wtimer0a(void);
WEAK_DEFAULT void isr_wtimer0b(void);
WEAK_DEFAULT void isr_wtimer1a(void);
WEAK_DEFAULT void isr_wtimer1b(void);
WEAK_DEFAULT void isr_wtimer2a(void);
WEAK_DEFAULT void isr_wtimer2b(void);
WEAK_DEFAULT void isr_wtimer3a(void);
WEAK_DEFAULT void isr_wtimer3b(void);
WEAK_DEFAULT void isr_wtimer4a(void);
WEAK_DEFAULT void isr_wtimer4b(void);
WEAK_DEFAULT void isr_wtimer5a(void);
WEAK_DEFAULT void isr_wtimer5b(void);
WEAK_DEFAULT void isr_sysex(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_gpio_porta,        /* GPIO Port A                      16 */
    isr_gpio_portb,        /* GPIO Port B                      17 */
    isr_gpio_portc,        /* GPIO Port C                      18 */
    isr_gpio_portd,        /* GPIO Port D                      19 */
    isr_gpio_porte,        /* GPIO Port E                      20 */
    isr_uart0,             /* UART 0                           21 */
    isr_uart1,             /* UART 1                           22 */
    isr_ssi0,              /* SSI 0                            23 */
    isr_i2c0,              /* I2C 0                            24 */
    (0UL),                 /* Reserved                         25 */
    (0UL),                 /* Reserved                         26 */
    (0UL),                 /* Reserved                         27 */
    (0UL),                 /* Reserved                         28 */
    (0UL),                 /* Reserved                         29 */
    isr_adc0_seq0,         /* ADC 0 Seq 0                      30 */
    isr_adc0_seq1,         /* ADC 0 Seq 1                      31 */
    isr_adc0_seq2,         /* ADC 0 Seq 2                      32 */
    isr_adc0_seq3,         /* ADC 0 Seq 3                      33 */
    isr_wdt,               /* WDT 0 and 1                      34 */
    isr_timer0a,           /* 16/32 bit timer 0 A              35 */
    isr_timer0b,           /* 16/32 bit timer 0 B              36 */
    isr_timer1a,           /* 16/32 bit timer 1 A              37 */
    isr_timer1b,           /* 16/32 bit timer 1 B              38 */
    isr_timer2a,           /* 16/32 bit timer 2 A              39 */
    isr_timer2b,           /* 16/32 bit timer 2 B              40 */
    isr_comp0,             /* Analog comparator 0              41 */
    isr_comp1,             /* Analog comparator 1              42 */
    (0UL),                 /* Reserved                         43 */
    isr_sysctl,            /* System control                   44 */
    isr_flashctl,          /* Flash + EEPROM control           45 */
    isr_gpio_portf,        /* GPIO Port F                      46 */
    (0UL),                 /* Reserved                         47 */
    (0UL),                 /* Reserved                         48 */
    isr_uart2,             /* UART 2                           49 */
    isr_ssi1,              /* SSI 1                            50 */
    isr_timer3a,           /* 16/32 bit timer 3 A              51 */
    isr_timer3b,           /* 16/32 bit timer 3 B              52 */
    isr_i2c1,              /* I2C 1                            53 */
    (0UL),                 /* Reserved                         54 */
    isr_can0,              /* CAN 0                            55 */
    (0UL),                 /* Reserved                         56 */
    (0UL),                 /* Reserved                         57 */
    (0UL),                 /* Reserved                         58 */
    isr_hibernate,         /* Hibernation module               59 */
    isr_usb,               /* USB                              60 */
    (0UL),                 /* Reserved                         61 */
    isr_udma_sw,           /* UDMA SW                          62 */
    isr_udma_error,        /* UDMA Error                       63 */
    isr_adc1_seq0,         /* ADC 1 Seq 0                      64 */
    isr_adc1_seq1,         /* ADC 1 Seq 1                      65 */
    isr_adc1_seq2,         /* ADC 1 Seq 2                      66 */
    isr_adc1_seq3,         /* ADC 1 Seq 3                      67 */
    (0UL),                 /* Reserved                         68 */
    (0UL),                 /* Reserved                         69 */
    (0UL),                 /* Reserved                         70 */
    (0UL),                 /* Reserved                         71 */
    (0UL),                 /* Reserved                         72 */
    isr_ssi2,              /* SSI 2                            73 */
    isr_ssi3,              /* SSI 3                            74 */
    isr_uart3,             /* UART 3                           75 */
    isr_uart4,             /* UART 4                           76 */
    isr_uart5,             /* UART 5                           77 */
    isr_uart6,             /* UART 6                           78 */
    isr_uart7,             /* UART 7                           79 */
    (0UL),                 /* Reserved                         80 */
    (0UL),                 /* Reserved                         81 */
    (0UL),                 /* Reserved                         82 */
    (0UL),                 /* Reserved                         83 */
    isr_i2c2,              /* I2C 2                            84 */
    isr_i2c4,              /* I2C 4                            85 */
    isr_timer4a,           /* 16/32 bit timer 4 A              86 */
    isr_timer4b,           /* 16/32 bit timer 4 B              87 */
    (0UL),                 /* Reserved                         88 */
    (0UL),                 /* Reserved                         89 */
    (0UL),                 /* Reserved                         90 */
    (0UL),                 /* Reserved                         91 */
    (0UL),                 /* Reserved                         92 */
    (0UL),                 /* Reserved                         93 */
    (0UL),                 /* Reserved                         94 */
    (0UL),                 /* Reserved                         95 */
    (0UL),                 /* Reserved                         96 */
    (0UL),                 /* Reserved                         97 */
    (0UL),                 /* Reserved                         98 */
    (0UL),                 /* Reserved                         99 */
    (0UL),                 /* Reserved                         100 */
    (0UL),                 /* Reserved                         101 */
    (0UL),                 /* Reserved                         102 */
    (0UL),                 /* Reserved                         103 */
    (0UL),                 /* Reserved                         104 */
    (0UL),                 /* Reserved                         105 */
    (0UL),                 /* Reserved                         106 */
    (0UL),                 /* Reserved                         107 */
    isr_timer5a,           /* 16/32 bit timer 5 A              108 */
    isr_timer5b,           /* 16/32 bit timer 5 B              109 */
    isr_wtimer0a,          /* 32/64 bit timer 0 A              110 */
    isr_wtimer0b,          /* 32/64 bit timer 0 B              111 */
    isr_wtimer1a,          /* 32/64 bit timer 1 A              112 */
    isr_wtimer1b,          /* 32/64 bit timer 1 B              113 */
    isr_wtimer2a,          /* 32/64 bit timer 2 A              114 */
    isr_wtimer2b,          /* 32/64 bit timer 2 B              115 */
    isr_wtimer3a,          /* 32/64 bit timer 3 A              116 */
    isr_wtimer3b,          /* 32/64 bit timer 3 B              117 */
    isr_wtimer4a,          /* 32/64 bit timer 4 A              118 */
    isr_wtimer4b,          /* 32/64 bit timer 4 B              119 */
    isr_wtimer5a,          /* 32/64 bit timer 5 A              120 */
    isr_wtimer5b,          /* 32/64 bit timer 5 B              121 */
    isr_sysex,             /* System Exception                 122 */
    (0UL),                 /* Reserved                         123 */
    (0UL),                 /* Reserved                         124 */
    (0UL),                 /* Reserved                         125 */
    (0UL),                 /* Reserved                         126 */
    (0UL),                 /* Reserved                         127 */
    (0UL),                 /* Reserved                         128 */
    (0UL),                 /* Reserved                         129 */
    (0UL),                 /* Reserved                         130 */
    (0UL),                 /* Reserved                         131 */
    (0UL),                 /* Reserved                         132 */
    (0UL),                 /* Reserved                         133 */
    (0UL),                 /* Reserved                         134 */
    (0UL),                 /* Reserved                         135 */
    (0UL),                 /* Reserved                         136 */
    (0UL),                 /* Reserved                         137 */
    (0UL),                 /* Reserved                         138 */
    (0UL),                 /* Reserved                         139 */
    (0UL),                 /* Reserved                         140 */
    (0UL),                 /* Reserved                         141 */
    (0UL),                 /* Reserved                         142 */
    (0UL),                 /* Reserved                         143 */
    (0UL),                 /* Reserved                         144 */
    (0UL),                 /* Reserved                         145 */
    (0UL),                 /* Reserved                         146 */
    (0UL),                 /* Reserved                         147 */
    (0UL),                 /* Reserved                         148 */
    (0UL),                 /* Reserved                         149 */
    (0UL),                 /* Reserved                         150 */
    (0UL),                 /* Reserved                         151 */
    (0UL),                 /* Reserved                         152 */
    (0UL),                 /* Reserved                         153 */
    (0UL)                  /* Reserved                         154 */
};
/** @} */
